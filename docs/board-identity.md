# How the Board Names Itself (and How to Rename It)

This page explains, in plain language, how a Nobs board tells your PC who it is, and how you can
change that without re-installing the firmware. You don't need to understand any of this to use the
approach panel; it's here for the curious and for anyone setting up more than one box.

## The short version

- Every Nobs box appears to your PC as a USB game controller with a **name** (like "Nobs Approach")
  and an **ID number** (called a *Product ID*, or **PID**).
- That name and ID are **not baked into the firmware**. The board remembers them in its own memory,
  so the *same* firmware can be set up as a panel, an autopilot, or an approach box.
- Out of the box, this board is **"Nobs Approach"**. You can change it later with the configuration
  app, no re-flashing needed.

## Why each box needs its own name and ID

If you own more than one Nobs box, this matters. Microsoft Flight Simulator keeps a separate set of
button bindings for each controller, but it tells controllers apart by their **ID number**.

If two boxes shared the same ID, the simulator would treat them as the same device and **mix up
their button assignments**. Giving each box its own ID (and a clear name) keeps their controls
neatly separated, so plugging in a second or third box never disturbs the first.

Here are the standard profiles. Each product owns a small **block of IDs** so you can have several
of the same type (see below):

| If the box is a… | It should be named… | ID block |
| :--------------- | :------------------ | :------- |
| Panel            | Nobs Panel          | `80F0`–`80F3` |
| Autopilot        | Nobs Autopilot      | `80F4`–`80F7` |
| Approach         | Nobs Approach       | `80F8`–`80FB` |

(They all share the same *vendor* ID, `303A`; that part never changes.)

## Running several of the same type

You can use more than one of the same module, say two approach panels, at the same time. The
catch is they must **not** look identical to the PC, or the simulator mixes up their bindings. So
each one gets its own ID and name from its product's block:

| | Name | ID |
| :-- | :-- | :-- |
| 1st approach | Nobs Approach | `80F8` |
| 2nd approach | Nobs Approach 2 | `80F9` |
| 3rd approach | Nobs Approach 3 | `80FA` |

The first one keeps the plain name and the block's first ID (its out-of-box default). For each
extra one, give it the next ID and a numbered name with the rename step below: e.g. send
`SET_ID:80F9:Nobs Approach 2` to the second one. In the Nobs app, open **Devices** and press **+**
on that product to start tracking the extra module. The simulator then sees "Nobs Approach" and
"Nobs Approach 2" as two separate controllers with independent bindings.

## How to rename a board

The configuration app does this for you. Behind the scenes it just sends the board one short line of
text over its connection, for example:

```
SET_ID:80F8:Nobs Approach
```

That reads as: *"Set your ID to `80F8` and your name to `Nobs Approach`."* The board then:

1. **Saves** the new name and ID into its own memory.
2. **Confirms** it received them.
3. **Restarts itself**, and comes back wearing the new name.

That restart is normal and only takes a second. The reason it's needed: a board can only announce
its identity to the PC at the moment it powers on, so it has to reboot for a new name to take effect.

> 💡 To check what a board is currently called, the app can ask it with `GET_ID`, and the board
> replies with its stored name and ID.

## If Windows still shows the old name

Windows likes to remember names, so occasionally after a rename it'll keep showing the **old** one
in its game-controller list or in the simulator. If that happens, nudge it to take a fresh look:

1. Leave the board plugged in.
2. Right-click the **Start** button → open **Device Manager**.
3. Open the **Human Interface Devices** group.
4. Right-click your controller → **Uninstall device**.
5. Unplug the USB cable, wait a couple of seconds, and plug it back in.

Windows will then read the board's name again from scratch and show the updated one.

## Related pages

- [Which wire goes where](./arduino-esp-32-wiring.md): the button and pin map.
- [Loading the firmware](../firmware/arduino_eps32_nano/README.md): step-by-step setup, including
  the more technical details of how the identity is stored.
