# Wiring Map: Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Nano ESP32**.

This box has **3 levers** (Flaps, Gear, and Parking Brake), and they are all **ON-OFF-ON**
(3-position, with the center position doing nothing). Each lever has three terminals: the two outer
terminals (pin 1 and pin 3) each go to a signal pin, and the center terminal (pin 2) goes to
**GND**.

**How to read a row:** find the lever (e.g. "Flaps"), look at which terminal it is ("Pin 1"), then
run a wire from that terminal to the pin listed in the board's column. Pin names like `A0` or `D2`
are printed on the board itself. Every lever's center terminal (pin 2) connects to **GND**
(ground), see the last row.

The "Virtual HID Button" column is just for reference: it's the button number the sim sees when you
move that lever into that position. You don't wire anything for it.

> 💡 Each lever terminal has one wire to its signal pin (from this table) and the center terminal
> wires to **GND**. No resistors or extra parts needed; the firmware handles that.

| Component Group | Component Label | Hardware Connection | Arduino Nano ESP32 Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Flaps** (ON-OFF-ON) | Flaps lever | Pin 1 Terminal | **A0** | Button 1 | Position 1 |
| | Flaps lever | Pin 3 Terminal | **A1** | Button 2 | Position 2 |
| **Gear** (ON-OFF-ON) | Gear lever | Pin 1 Terminal | **A2** | Button 3 | Position 1 |
| | Gear lever | Pin 3 Terminal | **A3** | Button 4 | Position 2 |
| **Parking Brake** (ON-OFF-ON) | Parking brake lever | Pin 1 Terminal | **D2** | Button 5 | Position 1 |
| | Parking brake lever | Pin 3 Terminal | **D3** | Button 6 | Position 2 |
| **Ground Loop** | All levers | Pin 2 / Center Terminals | **GND** | *None* | System Ground |
