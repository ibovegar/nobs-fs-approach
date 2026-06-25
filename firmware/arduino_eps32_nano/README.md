# Nobs Approach: Arduino Nano ESP32 Firmware

This is the program (the "firmware") that runs on an **Arduino Nano ESP32** and turns it into a USB
game controller for Microsoft Flight Simulator. Once loaded, the board shows up to your PC and the
Nobs app as **"Nobs Approach"** with **6 buttons**: the 3 levers (Flaps, Gear, Parking Brake), two
buttons each (one per position). Wiring is in
[`docs/arduino-esp-32-wiring.md`](../../docs/arduino-esp-32-wiring.md).

All three levers are **ON-OFF-ON** (3-position): one outer position presses the first button, the
other presses the second, and the center position presses neither.

> 👍 Everything you need is in this folder. You don't edit any Arduino files. Keep
> [`build_opt.h`](./build_opt.h) next to the sketch: the IDE uses it automatically, and the build
> fails on purpose without it. Tested with the Arduino ESP32 board package `2.0.18-arduino.5`.

## One-time setup

1. Install the **Arduino IDE** (free, from arduino.cc).
2. Add the board: **Tools → Board → Boards Manager…**, search **Arduino ESP32 Boards**, click
   **Install** (big download). Then pick **Tools → Board → Arduino ESP32 Boards → Arduino Nano ESP32**.
3. In **Tools**, set **USB Mode → `Normal mode (TinyUSB)`** and **Pin Numbering → `By Arduino pin (default)`**.

> If the board install fails ("platform not installed"), close the IDE, reopen it **as
> administrator**, and try again.

## Flashing the first time

1. Open [`arduino_eps32_nano.ino`](./arduino_eps32_nano.ino) in the Arduino IDE.
2. Plug in the board with a **data** USB cable (not charge-only), straight into the PC.
3. Click **Upload** (the round arrow).

On a fresh board this just works. 🎉 When it's done, the board reconnects as **"Nobs Approach"** and
the app picks it up automatically.

## Re-flashing (after firmware is already on it)

This is **different**, and expected. Once the firmware is running, the board reports a custom USB
ID and no longer auto-enters upload mode, so a normal Upload fails with:

```
No DFU capable USB device available — exit status 74
```

Put the board into **update mode** by hand first:

1. Press the **RESET** button **twice, quickly** (like a mouse double-click, ~0.3–0.5 s apart).
2. You're in update mode when the **green LED breathes slowly in and out and stays there**.
   A quick red→blue→green flash that stops is just a normal reset; try the double-tap again.
3. With the green breathing, click **Upload** right away.
4. When it finishes, press **RESET once** to start the firmware.

> **If upload still can't find the board:** it's almost always the **cable or USB port**; swap to
> a known-good data cable and a different port (this is the most common cause). The double-tap
> timing is the other one; keep trying the rhythm.

## Check it works

- **Nobs app:** the approach panel is detected automatically; move the levers and watch them react.
- **Windows:** press `Win`, type **Set up USB game controllers**, open the device's **Properties**,
  and move the levers to see the 6 buttons react.

## Changing the board's name / ID

The name and product ID aren't compiled in; they're stored on the board, so the same firmware can
become any Nobs profile. Out of the box it's **"Nobs Approach"** (`303A` / `80F8`). The configuration
app changes it over the board's serial port. See
[`docs/board-identity.md`](../../docs/board-identity.md) for how that works and the full profile list.
