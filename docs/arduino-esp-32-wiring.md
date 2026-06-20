# Wiring Map — Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Nano ESP32**.

This button box has **8 toggle switches**: SW1–SW6 are **ON-ON** (2-position) and SW7–SW8 are
**ON-OFF-ON** (3-position, with the center position doing nothing). Every switch has three
terminals: the two outer terminals (pin 1 and pin 3) each go to a signal pin, and the center
terminal (pin 2) goes to **GND**.

**How to read a row:** find the switch (e.g. "SW1"), look at which terminal it is ("Pin 1"), then
run a wire from that terminal to the pin listed in the board's column. Pin names like `D13` or
`A0` are printed on the board itself. Every switch's center terminal (pin 2) connects to **GND**
(ground), see the last row.

The "Virtual HID Button" column is just for reference: it's the button number the sim sees when
you flip that switch into that position. You don't wire anything for it.

> 💡 Each switch terminal has one wire to its signal pin (from this table) and the center terminal
> wires to **GND**. No resistors or extra parts needed — the firmware handles that.

| Component Group | Component Label | Hardware Connection | Arduino Nano ESP32 Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **SW1** (ON-ON) | Switch 1 | Pin 1 Terminal | **A0** | Button 1 | Position 1 |
| | Switch 1 | Pin 3 Terminal | **A1** | Button 2 | Position 2 |
| **SW2** (ON-ON) | Switch 2 | Pin 1 Terminal | **A2** | Button 3 | Position 1 |
| | Switch 2 | Pin 3 Terminal | **A3** | Button 4 | Position 2 |
| **SW3** (ON-ON) | Switch 3 | Pin 1 Terminal | **A4** | Button 5 | Position 1 |
| | Switch 3 | Pin 3 Terminal | **A5** | Button 6 | Position 2 |
| **SW4** (ON-ON) | Switch 4 | Pin 1 Terminal | **A6** | Button 7 | Position 1 |
| | Switch 4 | Pin 3 Terminal | **A7** | Button 8 | Position 2 |
| **SW5** (ON-ON) | Switch 5 | Pin 1 Terminal | **D12** | Button 9 | Position 1 |
| | Switch 5 | Pin 3 Terminal | **D11** | Button 10 | Position 2 |
| **SW6** (ON-ON) | Switch 6 | Pin 1 Terminal | **D10** | Button 11 | Position 1 |
| | Switch 6 | Pin 3 Terminal | **D9** | Button 12 | Position 2 |
| **SW7** (ON-OFF-ON) | Switch 7 | Pin 1 Terminal | **D8** | Button 13 | Position 1 |
| | Switch 7 | Pin 3 Terminal | **D7** | Button 14 | Position 2 |
| **SW8** (ON-OFF-ON) | Switch 8 | Pin 1 Terminal | **D6** | Button 15 | Position 1 |
| | Switch 8 | Pin 3 Terminal | **D5** | Button 16 | Position 2 |
| **Ground Loop** | All Switches | Pin 2 / Center Terminals | **GND** | *None* | System Ground |
