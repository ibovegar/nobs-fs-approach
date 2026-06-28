// Nobs Approach — Arduino Nano ESP32 (ESP32-S3) USB HID gamepad firmware.
//
// A switch-only sibling of the Nobs Panel build, ported to the ESP32-S3's native
// USB. Exposes 6 buttons in the exact order the nobs-fs app expects:
//   buttons 0..5 → 3 levers × 2 outer terminals each
//                  (Flaps Pin3, Flaps Pin1, Gear Pin1, Gear Pin3, Brake Pin1, Brake Pin3)
// Flaps is wired Pin1→Button2/Pin3→Button1 (inverted from the other two levers) so the
// lever's physical up/down sense matches the sim without re-wiring it.
// All three levers are ON-OFF-ON (3-position): the center position grounds neither
// terminal, so both of that lever's buttons read released.
// See docs/arduino-esp-32-wiring.md for the full button + pin table.
//
// There are no encoders, so this sketch has no quadrature decoding, no acceleration,
// and no EEPROM acceleration store. Every input is a plain switch reported as a held
// button level.
//
// The HID report also doubles as a Feature report (GET_REPORT(Feature)) so a host can
// read the resting lever positions the instant it opens the device, and loop() resends
// the Input report on a 250 ms heartbeat so a host that opens the device later (missing
// the one-shot initial report) still re-syncs without the user touching a lever.
//
// ── Arduino IDE Tools settings (required) ─────────────────────────────────────
//   Board:        "Arduino Nano ESP32"
//   USB Mode:     "Normal mode (TinyUSB)"     ← the default; needed for custom HID
//   Pin Numbers:  "By Arduino pin (default)"   so D0/A0… match the wiring doc
// No other Tools changes and NO edits to the installed Arduino core are needed — the
// whole USB-identity fix lives in this repo (this sketch + build_opt.h beside it).
//
// ── USB identity: dynamic, stored in NVS (Preferences) ────────────────────────
// The device enumerates with VID 0x303A (Espressif's vendor ID) and a PID + product
// name that are *configurable at runtime*. Each Nobs box gets its own PID so MSFS
// can't mirror/overwrite control bindings across boxes. Out of the box it defaults to
// PID 0x80F8 / "Nobs Approach" (matches the `approach` entry in the app's device
// registry, src/panel/panel.ts).
//
// The PID and name live in flash via the native Preferences library (NVS). On boot
// the firmware reads them *before* USB.begin() and applies them to the descriptor.
// The configuration/diagnostic app can change them over the USB CDC serial port:
//
//   "SET_ID:<pidHex>:<name>\n"  store PID (hex, e.g. 80F8) + product name to NVS,
//                               then soft-reboot so the new identity takes effect
//   "GET_ID\n"                  reply "ID:<pidHex>:<name>" with the stored values
//
// After SET_ID the firmware replies "OK:<pidHex>:<name>", flushes, and calls
// ESP.restart(); the host re-enumerates the device under its new profile. (Identity
// can't change live mid-session because the USB stack is brought up once at boot.)
//
// The firmware also pushes its identity unprompted, as "BOOT:<pidHex>:<name>\n", the
// moment a host opens the CDC port (DTR asserted) — so the nobs-fs app learns which
// box it's talking to as soon as it connects, with no GET_ID round trip required. This
// fires again on every reconnect (including the re-enumeration after a SET_ID reboot).
//
// This NVS-at-boot scheme works because the Nano ESP32 normally forces "USB CDC On
// Boot" and "DFU On Boot" on, which would make the core bring USB up at boot (before
// setup()), freezing the descriptor with the stock 0x2341/0x0070 identity. The
// build_opt.h file next to this sketch overrides both to 0 (it is appended to every
// compile, including the core's), so the boot-time USB.begin() is skipped and our
// runtime identity wins. With CDC-on-boot off, the core's `Serial` is the hardware
// UART, so the app's config port is our own USB CDC interface (USBSerial below).
//
// Re-flashing: the running firmware reports a custom PID (and, with DFU-on-boot off, no
// DFU runtime interface), so the IDE can't auto-enter the bootloader. Double-tap the
// RESET button (RGB LED pulses) before uploading. See this folder's README.md.

#include "USB.h"
#include "USBHID.h"
#include "USBCDC.h"
#include <Preferences.h>

#if ARDUINO_USB_MODE
#error "Set Tools > USB Mode to 'Normal mode (TinyUSB)'. Custom HID needs the TinyUSB stack."
#endif
#if ARDUINO_USB_CDC_ON_BOOT
#error "build_opt.h is not taking effect: ARDUINO_USB_CDC_ON_BOOT must be 0. Make sure build_opt.h sits next to this .ino (it disables CDC/DFU-on-boot so the runtime USB identity works)."
#endif

// ── Custom HID gamepad: 6 buttons, no axes ────────────────────────────────────
// 6 1-bit buttons + 2 bits of padding pack into a flat 1-byte report. The same 6 bits
// are also declared as a Feature report so a host can pull the current lever positions
// on demand (GET_REPORT(Feature)) the moment it opens the device — see _onGetFeature.
// The Feature item re-declares its own Report Count (the Input padding above changed
// the global count) and gets its own 2 padding bits so it, too, is a whole byte.
static const uint8_t HID_REPORT_DESCRIPTOR[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (Button 1)
  0x29, 0x06,        //   Usage Maximum (Button 6)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x06,        //   Report Count (6)
  0x81, 0x02,        //   Input (Data,Var,Abs)
  0x95, 0x02,        //   Report Count (2)   — padding to a whole byte
  0x81, 0x03,        //   Input (Const,Var,Abs)
  0x19, 0x01,        //   Usage Minimum (Button 1)   — re-declared for the Feature item
  0x29, 0x06,        //   Usage Maximum (Button 6)
  0x95, 0x06,        //   Report Count (6)           — re-declared (padding above changed it)
  0xB1, 0x02,        //   Feature (Data,Var,Abs)     — same 6 bits, host-readable on demand
  0x95, 0x02,        //   Report Count (2)           — padding so the Feature item is a whole byte too
  0xB1, 0x03,        //   Feature (Const,Var,Abs)
  0xC0               // End Collection
};

class NobsGamepad : public USBHIDDevice {
public:
  NobsGamepad() { HID.addDevice(this, sizeof(HID_REPORT_DESCRIPTOR)); }

  void begin() { HID.begin(); }

  // Set/clear one button bit in the pending report (mirrors Joystick.setButton).
  void setButton(uint8_t index, uint8_t value) {
    if (index >= 6) return;
    uint8_t mask = 1 << index;
    if (value) report[0] |= mask;
    else       report[0] &= ~mask;
  }

  // One USB report carrying every button change since the last call (mirrors
  // Joystick.sendState). Only transmitted when something actually changed — HID
  // hosts track button state edge-to-edge, so re-sending identical reports just
  // adds USB traffic and risks blocking the loop.
  void sendState() {
    if (report[0] == lastSent) return;
    if (HID.SendReport(0, report, sizeof(report))) {
      lastSent = report[0];
    }
  }

  // Re-publish the current report even when nothing changed. The initial report is
  // only emitted once (on the first change after boot), so a host that opens the
  // device later misses it and can't know the resting lever positions until the user
  // moves something. loop() calls this on a slow heartbeat so any host re-syncs the
  // true state within the heartbeat interval.
  void resend() {
    if (HID.SendReport(0, report, sizeof(report))) {
      lastSent = report[0];
    }
  }

  // USBHIDDevice: hand the descriptor to the core during enumeration.
  uint16_t _onGetDescriptor(uint8_t* buffer) override {
    memcpy(buffer, HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    return sizeof(HID_REPORT_DESCRIPTOR);
  }

  // Answer GET_REPORT(Feature) with the current button state. This lets a host read
  // the resting lever positions the instant it opens the device, instead of waiting
  // for the heartbeat or a user actuation. (The ESP32 USB stack routes feature
  // get-reports here but STALLs input get-reports, so on-demand state is exposed as a
  // Feature report mirroring the Input report's 6 button bits.)
  uint16_t _onGetFeature(uint8_t report_id, uint8_t* buffer, uint16_t len) override {
    (void)report_id; // single unnumbered report
    if (len < sizeof(report)) return 0;
    memcpy(buffer, report, sizeof(report));
    return sizeof(report);
  }

private:
  USBHID  HID;
  uint8_t report[1] = { 0 };
  uint8_t lastSent  = 0xFF; // force the first real report out
};

NobsGamepad Joystick;

// USB CDC config port. With CDC-on-boot disabled (via build_opt.h) the core's `Serial`
// is the hardware UART, so we expose our own USB CDC interface for the config app.
// Like USBHID, USBCDC registers its interface in its constructor (static init), so it
// is in place before USB.begin() builds the composite descriptor in setup().
USBCDC USBSerial;

// ── Dynamic USB identity (NVS via Preferences) ────────────────────────────────
// VID is fixed (Espressif's vendor ID); PID + product name are stored in flash and
// host-configurable. Defaults are the approach profile; an unconfigured chip uses them.
// NB: named NOBS_USB_VID, not USB_VID — the core's pins_arduino.h #defines USB_VID.
const uint16_t NOBS_USB_VID    = 0x303A;
const uint16_t DEFAULT_PID     = 0x80F8;
const char*    DEFAULT_NAME    = "Nobs Approach";
const char*    NVS_NAMESPACE   = "nobs";
const char*    NVS_KEY_PID     = "pid";
const char*    NVS_KEY_NAME    = "name";

Preferences prefs;
uint16_t     usbPid;     // active PID (kept alive for the whole session)
String       usbName;    // active product name (c_str() must outlive USB.begin())

void loadIdentity() {
  prefs.begin(NVS_NAMESPACE, true); // read-only
  usbPid  = prefs.getUShort(NVS_KEY_PID, DEFAULT_PID);
  usbName = prefs.getString(NVS_KEY_NAME, DEFAULT_NAME);
  prefs.end();
}

void storeIdentity(uint16_t pid, const String& name) {
  prefs.begin(NVS_NAMESPACE, false); // read-write
  prefs.putUShort(NVS_KEY_PID, pid);
  prefs.putString(NVS_KEY_NAME, name);
  prefs.end();
}

// ── USB string descriptors: name the interfaces after the product ─────────────
// The core hardcodes the HID/CDC *interface* strings to "TinyUSB HID"/"TinyUSB CDC".
// For a composite device (HID + CDC) Windows and MSFS show that interface string as
// the controller's name — so without this the box shows up as "TinyUSB HID" even
// though iProduct is "Nobs Approach". tud_descriptor_string_cb is weak in the core,
// so defining it here replaces it (no core edit needed) and points every interface
// string at the product name. Index map: 0=language, 1=manufacturer, 2=product,
// 3=serial (from the chip MAC, stable per board), 4+=interface strings → product.
extern "C" const uint16_t* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;
  static uint16_t desc[40];

  if (index == 0) {
    desc[1] = 0x0409; // English (United States)
    desc[0] = (uint16_t)((3 << 8) | (2 * 1 + 2));
    return desc;
  }
  // Beyond our real strings (1..3 + a handful of interface strings). Returning NULL
  // mirrors the core and lets Windows' MS-OS probe at index 0xEE fail cleanly.
  if (index >= 16) return NULL;

  char serial[16];
  const char* str;
  switch (index) {
    case 1: str = "Arduino"; break;       // manufacturer (matches USB.manufacturerName)
    case 2: str = usbName.c_str(); break; // product (iProduct)
    case 3:                               // serial — stable per board
      snprintf(serial, sizeof(serial), "%012llX", (unsigned long long)ESP.getEfuseMac());
      str = serial;
      break;
    default: str = usbName.c_str(); break; // HID/CDC/config interface strings
  }

  uint8_t chr_count = strlen(str);
  if (chr_count > 38) chr_count = 38; // desc holds 39 UTF-16 chars + the header word
  for (uint8_t i = 0; i < chr_count; i++) desc[1 + i] = str[i];
  desc[0] = (uint16_t)((3 << 8) | (2 * chr_count + 2));
  return desc;
}

// ── Pin assignments (Arduino Nano ESP32 labels — see arduino-esp-32-wiring.md) ─
// In HID button order: button i is driven by leverPin[i]. Each lever contributes two
// consecutive buttons (its Pin 1 and Pin 3 terminals); the center terminal is GND.
//
//   index  HID button  lever / terminal      pin
//   0      Button 1     Flaps Pin 3           A7
//   1      Button 2     Flaps Pin 1           A6
//   2      Button 3     Gear Pin 1            A4
//   3      Button 4     Gear Pin 3            A5
//   4      Button 5     Parking brake Pin 1   D3
//   5      Button 6     Parking brake Pin 3   D2
const uint8_t leverPin[6] = { A7, A6, A4, A5, D3, D2 };

// NOTE: every pin above is driven with INPUT_PULLUP, so the closed-to-GND wiring
// reads LOW = pressed. All of these GPIOs support the ESP32-S3 internal pull-up;
// if a future revision moves a signal onto a pull-up-less pad, add an external
// 10 kΩ pull-up to 3V3 there.

// ── Status LED (D4) ────────────────────────────────────────────────────────────
// Blinks while booting/waiting for USB enumeration, steady once enumerated. D4 also
// doubles as DSR (a USB CDC modem-control signal), but this sketch never drives DSR,
// so the pin is free for plain GPIO use.
const uint8_t STATUS_LED_PIN = D4;

// ── Host serial config ───────────────────────────────────────────────────────
// Line protocol over the USB CDC port (see header). SET_ID persists a new identity
// to NVS and reboots; GET_ID reports the current one; BOOT is pushed unprompted on
// connect (see onCdcEvent below) and needs no host request.
char    cmdBuf[64];
uint8_t cmdLen = 0;

void printIdentity(const char* prefix) {
  char pidHex[5];
  snprintf(pidHex, sizeof(pidHex), "%04X", usbPid);
  USBSerial.print(prefix);
  USBSerial.print(':');
  USBSerial.print(pidHex);
  USBSerial.print(':');
  USBSerial.println(usbName);
}

// ── Unprompted identity push on connect ───────────────────────────────────────
// The CDC line-state event fires whenever the host opens/closes the port (DTR), which
// on Windows tracks the nobs-fs app actually connecting — not just USB enumeration.
// The event runs on the USB/event task, so it only sets a flag here; loop() does the
// actual send. announceIdentity starts true so the very first loop() iteration (in case
// a host is already attached, e.g. after a SET_ID reboot) also pushes one.
volatile bool announceIdentity = true;

void onCdcEvent(void* arg, esp_event_base_t base, int32_t id, void* eventData) {
  (void)arg;
  (void)base;
  if (id != ARDUINO_USB_CDC_LINE_STATE_EVENT) return;
  auto* data = (arduino_usb_cdc_event_data_t*)eventData;
  if (data->line_state.dtr) announceIdentity = true;
}

// ── USB enumeration tracking (for the status LED) ─────────────────────────────
// This device is read purely over HID for normal use — the CDC port (and its DTR
// signal above) is only ever opened for one-off config, so it can't tell us whether
// the flight-sim app is actually using the device. The best available signal is USB
// enumeration itself: ARDUINO_USB_STARTED_EVENT fires once the host finishes
// recognizing the composite device (tud_mount_cb), ARDUINO_USB_STOPPED_EVENT if it's
// unplugged or the bus drops (tud_umount_cb). So "connected" here means "enumerated
// by a host", not "the app has it open" — there's no such signal for a plain HID
// gamepad with no application-level handshake.
volatile bool hostConnected = false;

void onUsbEvent(void* arg, esp_event_base_t base, int32_t id, void* eventData) {
  (void)arg;
  (void)base;
  (void)eventData;
  if (id == ARDUINO_USB_STARTED_EVENT) hostConnected = true;
  else if (id == ARDUINO_USB_STOPPED_EVENT) hostConnected = false;
}

// Blink while booting/waiting for USB enumeration; steady once enumerated
// (hostConnected, set from the USB started/stopped events above). Non-blocking so it
// never stalls loop().
void updateStatusLed() {
  if (hostConnected) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    return;
  }
  static uint32_t lastToggle = 0;
  static bool     ledOn      = false;
  uint32_t now = millis();
  if (now - lastToggle >= 150) {
    lastToggle = now;
    ledOn = !ledOn;
    digitalWrite(STATUS_LED_PIN, ledOn);
  }
}

void applyConfigCommand(const char* s) {
  if (strcmp(s, "GET_ID") == 0) {
    printIdentity("ID");
    return;
  }
  if (strncmp(s, "SET_ID:", 7) != 0) return;

  const char* p     = s + 7;          // "<pidHex>:<name>"
  const char* colon = strchr(p, ':');
  if (!colon || colon == p) return;   // need a PID and a separator

  char pidStr[8] = { 0 };
  size_t n = colon - p;
  if (n >= sizeof(pidStr)) return;
  memcpy(pidStr, p, n);
  uint16_t pid = (uint16_t)strtol(pidStr, nullptr, 16);

  String name = colon + 1;            // everything after the second ':'
  name.trim();
  if (name.length() == 0) return;

  storeIdentity(pid, name);
  // Reflect the just-stored values, confirm, then reboot so USB re-enumerates.
  usbPid  = pid;
  usbName = name;
  printIdentity("OK");
  USBSerial.flush();
  delay(50);                          // let the CDC packet drain before reset
  ESP.restart();
}

void handleSerialConfig() {
  while (USBSerial.available()) {
    char c = USBSerial.read();
    if (c == '\n' || c == '\r') {
      cmdBuf[cmdLen] = '\0';
      if (cmdLen > 0) applyConfigCommand(cmdBuf);
      cmdLen = 0;
    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = c;
    } else {
      cmdLen = 0; // overlong line (no newline) — drop it and resync
    }
  }
}

void setup() {
  for (uint8_t i = 0; i < 6; i++) {
    pinMode(leverPin[i], INPUT_PULLUP);
  }
  pinMode(STATUS_LED_PIN, OUTPUT);

  // Load the saved identity (PID + name) from NVS, falling back to the approach
  // defaults on a fresh chip.
  loadIdentity();

  // USB identity — set BEFORE USB.begin() so the descriptor is built with the stored
  // VID/PID/name. This works because build_opt.h disabled CDC/DFU-on-boot, so USB has
  // NOT been started yet.
  USB.VID(NOBS_USB_VID);
  USB.PID(usbPid);
  USB.productName(usbName.c_str());
  USB.manufacturerName("Arduino");

  Joystick.begin();        // create the HID report semaphores/mutex
  USBSerial.onEvent(ARDUINO_USB_CDC_LINE_STATE_EVENT, onCdcEvent); // detect host connect
  USB.onEvent(onUsbEvent); // detect USB enumeration, for the status LED
  USBSerial.begin(115200); // USB CDC config port; do NOT wait on it (would stall)
  USB.begin();             // build + start the composite USB device (HID + CDC)
}

void loop() {
  handleSerialConfig(); // apply any pending SET_ID/GET_ID command
  updateStatusLed();    // blink while waiting for USB enumeration, steady once enumerated

  // Push our identity the moment a host opens the CDC port, so the nobs-fs app learns
  // which box it's talking to without sending GET_ID first.
  if (announceIdentity) {
    announceIdentity = false;
    printIdentity("BOOT");
  }

  // Each lever terminal is reported as a held level (pressed = LOW with INPUT_PULLUP).
  for (uint8_t i = 0; i < 6; i++) {
    Joystick.setButton(i, digitalRead(leverPin[i]) == LOW ? 1 : 0);
  }

  // One USB report with all the updates above (sent only when changed).
  Joystick.sendState();

  // Heartbeat: re-publish the current state periodically so a host that opens the
  // device after boot (missing the one initial report) picks up the resting lever
  // positions without the user having to move anything. 250 ms is imperceptible yet
  // negligible USB traffic (1 byte, ~4×/s).
  static uint32_t lastBeat = 0;
  uint32_t now = millis();
  if (now - lastBeat >= 250) {
    lastBeat = now;
    Joystick.resend();
  }
}
