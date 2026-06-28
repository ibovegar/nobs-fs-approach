# Bill of Materials (Shopping List)

This is the full shopping list for building one Nobs Approach panel: every part you need, from the
electronics to the screws. The "Manufacturer / Part Number" column lists the exact parts we used,
linked to their Mouser listing, so you can search for them or find equivalents.

A few friendly notes before you start buying:

- **Exact parts aren't mandatory.** Most components can be swapped for similar parts you like or
  already have. The part numbers here are just what's known to work.
- **The enclosure and lever parts are 3D printed**: you print them yourself from the files in the
  `models/` folder, or have a print service make them.
- Quantities are for **one** panel.

## Core Electronics & Switches

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **1.1** | 1 | Arduino Nano ESP32 (with headers) | [Arduino / ABX00083](https://no.mouser.com/ProductDetail/Arduino/ABX00083) | Main microcontroller (ESP32-S3, USB HID, VID 0x303A / PID 0x80F8) |
| **1.2** | 1 | SPDT Toggle Switch, Locking Lever, On-On (2-position), 15 A @ 125 VAC | [NKK Switches / S2AL](https://no.mouser.com/ProductDetail/NKK-Switches/S2AL?qs=EIJ16CYJdOsFNpXA4HNUoQ%3D%3D) | Gear lever, under `models/gear_lever.step` |
| **1.3** | 1 | SPDT Toggle Switch, Momentary (On)-Off-(On), 15 A @ 125 VAC | [Apem / 637H-2](https://no.mouser.com/ProductDetail/Apem/637H-2?qs=sGAEpiMZZMtFyPk3yBMYYM4hYjBerRHjpwkY%2FkVx7u8%3D) | Flaps lever, under `models/flaps_lever_top.step` + `flaps_lever_bottom.step` |
| **1.4** | 1 | SPDT Push-Pull Switch, On-On (2-position), Marine Grade, 10 A @ 12 VDC | [Cole Hersee / M-630](https://no.mouser.com/ProductDetail/Cole-Hersee/M-630?qs=lYH8mLnNY3r10KRvj4ytOg%3D%3D) | Parking brake lever, under `models/gear_lever.step` (shared shape) |
| **1.5** | 1 | Stripboard, 50 × 80 mm | [BusBoard Prototype Systems / ST1](https://no.mouser.com/ProductDetail/BusBoard-Prototype-Systems/ST1) | Controller board carrier for the Arduino, pin headers, and terminal block |
| **1.6** | 2 | Female Pin Header Socket, 26-position, 2.54 mm pitch | [3M Electronic Solutions Division / 929850-01-26-RA](https://no.mouser.com/ProductDetail/3M-Electronic-Solutions-Division/929850-01-26-RA) | Sockets soldered to the stripboard to seat the Arduino Nano ESP32 |
| **1.7** | 1 | Fixed Terminal Block, 6-position, 2.54 mm pitch | [GCT / TBC05-06-1-G-G](https://no.mouser.com/ProductDetail/GCT/TBC05-06-1-G-G) | Screw-down wiring for the 6 lever signal terminals (Flaps/Gear/Brake Pin 1 + Pin 3); the lever ground returns bus directly to the stripboard's GND rail |
| **1.8** | 1 | 3 mm Green LED, Diffused, 2.2 V | [Lumex / SSL-LX30FT4GD](https://no.mouser.com/ProductDetail/Lumex/SSL-LX30FT4GD?qs=7jmq9uQgtYG4HR2h%252BlgbMw%3D%3D) | Status indicator (blinks while booting/waiting for USB enumeration, steady once connected) — wired to D4 |
| **1.9** | 1 | 120 Ω Resistor, 1/4 W, ±1%, Metal Film | [KOA Speer / MFS1/4DCT52R1200F](https://no.mouser.com/ProductDetail/KOA-Speer/MFS1-4DCT52R1200F?qs=ddCg%252BR5cWn20oJHZLuYO%2FQ%3D%3D) | Current-limiting resistor for the status LED |

## Structural & Enclosure Hardware

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **2.1** | 1 | Enclosure Top | 3D Printed (`models/enclosure_top.step`) | Main upper housing shell |
| **2.2** | 1 | Enclosure Bottom | 3D Printed (`models/enclosure_bottom.step`) | Main lower housing base, carries the controller board |
| **2.3** | 1 | Mounting Plate | 3D Printed (`models/nobs_approach_mounting_plate.step`) | Carries the 3 levers |
| **2.4** | 1 | Front Plate | 3D Printed (`models/nobs_approach_frontpanel.step`) | Visual faceplate with lever labels |
| **2.5** | 1 | Flaps Lever, Top | 3D Printed (`models/flaps_lever_top.step`) | Upper half of the Flaps lever assembly |
| **2.6** | 1 | Flaps Lever, Bottom | 3D Printed (`models/flaps_lever_bottom.step`) | Lower half of the Flaps lever assembly |
| **2.7** | 2 | Gear / Parking Brake Lever | 3D Printed (`models/gear_lever.step`) | Shared lever shape, printed once for Gear and once for Parking Brake |
| **2.8** | 10 | M4 Heat-Set Insert | [SI (PennEngineering) / IUTB-M4](https://no.mouser.com/ProductDetail/SI/IUTB-M4) | Threaded anchors moulded into the enclosure top/bottom |
| **2.9** | 10 | M3 Heat-Set Insert | [SI (PennEngineering) / IUTB-M3](https://no.mouser.com/ProductDetail/SI/IUTB-M3) | Threaded anchors moulded into the mounting plate and front plate |
| **2.10** | 4 | M4 × 0.7, 30 mm Socket-Head Screw | — | Clamps the enclosure top and bottom halves together |
| **2.11** | 6 | M4 × 0.7, 10 mm Socket-Head Screw | — | Fastens the mounting plate to the rear of the enclosure bottom |
| **2.12** | 4 | M3 × 0.5, 8 mm Pan-Head Screw | — | Fastens the controller board to its standoffs in the enclosure bottom |
| **2.13** | 6 | M3 × 0.5, 8 mm Countersunk Screw | — | Fastens the front plate to the mounting plate, and the assembly to the enclosure |

## Connectors & Wiring

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **3.1** | 1 | USB-C to USB-A Cable, 1.5 m to 2 m, data-sync capable | — | Connects the Arduino Nano ESP32 to your PC |
| **3.2** | 1 | Heat Shrink Tubing Assortment Pack, 1.5 mm to 3.5 mm diameters | — | Insulates soldered connections on the lever terminals |

> **A note on the fastener counts above:** `enclosure_top.step`/`enclosure_bottom.step` in this
> project are geometrically identical to the panel project's, and the mounting-plate/front-plate
> *joining* holes (the ones that bolt the plates together and to the enclosure) match too — only
> the switch/lever cutouts differ, which is expected since this panel has 3 levers instead of 8
> switches. So items 2.8–2.13 are carried over directly from the panel BOM. Still worth a quick
> visual check against the printed parts before ordering, since this wasn't measured by hand.

## Project Costs

Rough cost estimate for a single panel, in USD. These are ballpark figures only, actual prices
vary significantly by supplier, region, and quantity, and **exclude shipping and taxes**. Several
items (heat-set inserts, screws) are sold in multi-build packs, so the per-build cost is lower if
you build more than one or already have them on hand.

| Category | Items | Estimated Cost |
| :--- | :--- | ---: |
| Microcontroller | Arduino Nano ESP32 (genuine; clones are cheaper) | $20 – $28 |
| Levers (switches) | NKK S2AL toggle + Apem 637H-2 + Cole Hersee M-630 | $30 – $55 |
| Board & connectors | Stripboard, 2 × pin headers, terminal block, status LED + resistor | $10 – $15 |
| Fasteners | Heat-set inserts (20) and M3/M4 screws (20) | $10 – $18 |
| Enclosure (3D printed) | Filament for top, bottom, mounting plate, front plate, and 3 levers | $5 – $15 |
| Connectors & wiring | USB-C cable, heat-shrink tubing pack | $8 – $15 |
| **Total** | | **≈ $83 – $146** |

> **Notes:**
> * The **enclosure** assumes you print the parts yourself (filament cost only). Ordering them
>   from a print service will add roughly $20 – $50.
> * This estimate doesn't include hookup wire, since it's usually bought once in bulk and reused
>   across builds.
> * Bulk/pack purchasing of the inserts, screws, and heat-shrink tubing spreads their cost across
>   multiple builds, lowering the effective per-panel total.
