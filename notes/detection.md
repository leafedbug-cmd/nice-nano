# Detection Notes

Initial local detection from Windows:

- Runtime USB VID/PID seen earlier: `VID_239A&PID_8029`
- Bootloader USB VID/PID: `VID_239A&PID_00B3`
- Mounted volume label: `NICENANO`
- `INFO_UF2.TXT` reported:
  - `Model: nice!nano`
  - `Board-ID: nRF52840-nicenano`
  - `SoftDevice: S140 version 6.1.1`

Quick string scan of `CURRENT.UF2`:

- No `ZMK`
- No `CircuitPython`
- No `MicroPython`
- Found `Adafruit`
- Found `#Feather nRF52840 Express`

Interpretation:

The hardware is a `nice!nano`, but the currently flashed image looks like an Adafruit nRF52/Bluefruit-style firmware build rather than a standard ZMK image.
