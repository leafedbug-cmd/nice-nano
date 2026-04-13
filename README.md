# nice-nano

Local repo for the connected `nice!nano` (`nRF52840-nicenano`).

## Board

- Model: `nice!nano`
- MCU: `nRF52840`
- UF2 label: `NICENANO`
- Detected bootloader: `UF2 Bootloader 0.6.0`

## Current state

The board was detected in UF2 bootloader mode and exposed:

- USB serial: `COM12`
- UF2 drive: `K:`
- Board-ID: `nRF52840-nicenano`

The flashed image does not appear to be ZMK or CircuitPython. Embedded strings suggest an Adafruit nRF52/Bluefruit-style firmware image.

## Repo layout

- `firmware/`: firmware files, notes, and extracted artifacts
- `notes/`: quick investigation notes

## Flashing

When the board is in bootloader mode and mounted as `NICENANO`, copy a UF2 file to the mounted drive, for example:

```powershell
Copy-Item .\firmware\app.uf2 K:\
```

## Next steps

- Drop known-good UF2 builds into `firmware/`
- Add source or links for the firmware you want on the board
- Capture serial logs if you want to identify the current app behavior further
