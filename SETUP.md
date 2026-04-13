# Setup: nRF24L01+PA+LNA Interference Firmware

## Step 1: Wiring the nRF24L01+PA+LNA to nice!nano

| nRF24L01 Pin | nice!nano Pad # | nRF52840 Pin |
|---|---|---|
| VCC | **VCC** (3.3V regulated) | — |
| GND | **GND** | — |
| CE (Chip Enable) | **024** | P0.24 |
| CSN (Chip Select) | **100** | P1.00 |
| SCK (SPI Clock) | **113** (between 111 & 115) | P1.13 |
| MOSI (SPI Data In) | **010** | P0.10 |
| MISO (SPI Data Out) | **111** | P1.11 |
| IRQ (optional) | Not connected | — |

> **Note:** The nice!nano silkscreen shows 3-digit nRF52840 pad numbers (e.g. 024 = Port 0, Pin 24). The SPI pins (SCK/MOSI/MISO) are handled automatically by `board.SPI()` in CircuitPython.

> **Important:** The nRF24L01+PA+LNA module has an onboard voltage regulator. Power it from the nice!nano's **3.3V pin**, NOT from 5V USB.

---

## Step 2: Download CircuitPython for nice!nano v2

1. Go to **https://circuitpython.org/board/nice_nano_v2/**
2. Download the **latest stable UF2 file** (e.g., `circuitpython-nice_nano_v2-en_US-9.1.0.uf2`)

---

## Step 3: Flash CircuitPython to nice!nano

1. **Enter bootloader mode:** Double-tap the **RST** (reset) button on the nice!nano
2. The board mounts as a **`NICENANO`** USB drive
3. **Drag the CircuitPython `.uf2` file** onto the `NICENANO` drive
4. Board reboots automatically — it now mounts as **`CIRCUITPY`**

> You should see a `code.py` file and a `lib/` folder on the CIRCUITPY drive.

---

## Step 4: Install the nRF24L01 Library

1. Download the **Adafruit CircuitPython Bundle** from:  
   https://github.com/adafruit/circuitpython_nrf24l01

2. **Option A (Quick):** Download the ZIP, extract, and copy the **`circuitpython_nrf24l01/`** folder to `CIRCUITPY/lib/`

3. **Option B (Full Bundle):** Download the full Adafruit bundle, copy **`lib/circuitpython_nrf24l01/`** to `CIRCUITPY/lib/`

After copying, your drive structure should look like:
```
CIRCUITPY/
├── code.py
└── lib/
    └── circuitpython_nrf24l01/
        ├── __init__.py
        └── rf24.py
```

---

## Step 5: Copy code.py to the Board

From this repo, copy **`firmware/code.py`** to the **`CIRCUITPY/`** drive (overwrite the default `code.py`).

---

## Step 6: Power On and Test

1. **Plug the nice!nano into USB-C power** (or use a separate USB power adapter for more current stability)
2. The `code.py` script runs automatically
3. **Put on the Logitech G Pro X wireless headset**
4. You should hear **static, dropouts, or complete loss of audio** as the nRF24L01+PA+LNA floods all 2.4 GHz channels

### Expected Behavior
- **Before power:** Headset audio works normally
- **During power:** Logitech Lightspeed protocol cannot sync → audio artifacts
- **After power:** Headset recovers normal audio within 1-2 seconds

---

## Troubleshooting

| Issue | Cause | Solution |
|---|---|---|
| No audio interference | nRF24L01 not transmitting | Check SPI wiring (SCK, MOSI, MISO), CE/CSN pins |
| `ImportError: No module named 'circuitpython_nrf24l01'` | Library not installed | Copy `circuitpython_nrf24l01/` folder to `CIRCUITPY/lib/` |
| Board won't enter bootloader | RST button not working | Try pressing and holding RST for 1 second, release |
| CIRCUITPY drive not appearing | USB cable issue | Use a quality USB-C cable with data lines |

---

## Tuning / Experimentation

### Increase Sweep Speed
Edit `firmware/code.py` and reduce the dwell time:
```python
time.sleep(0.0001)  # 100 µs instead of 200 µs
```
Faster sweep = better coverage of hopping patterns.

### Focus on Lightspeed Channels
Logitech Lightspeed typically uses the **lower 2.4 GHz range** (channels 0–80 roughly). Try:
```python
for channel in range(0, 80):  # Only sweep 2.400-2.480 GHz
    radio.channel = channel
    time.sleep(0.0002)
```
This creates a tighter, faster loop.

### Monitor via Serial Console
Connect to the board via serial (COM port on Windows, `/dev/ttyACM0` on Linux):
- Baud: 115200
- You'll see debug messages like `Sweep #100` every 100 cycles

---

## References

- **nRF24L01 Datasheet:** https://cdn.sparkfun.com/assets/learn_tutorials/3/1/nRF24L01_Wireless_Transceiver_Module.pdf
- **nice!nano v2 Schematic/Pinout:** https://github.com/jpconstantineau/nice-nano/wiki
- **CircuitPython nRF24L01 Library:** https://github.com/adafruit/circuitpython_nrf24l01
- **Logitech Lightspeed Protocol (public research):** Academic papers on 2.4 GHz FHSS security

---

## Notes for Lab Report

When documenting this exercise:
- Record the **before/after audio** (phone recording of headset audio)
- Note the **sweep rate** (channels per second) achievable with CircuitPython
- Compare with **focused channel ranges** vs. **full 2.4 GHz sweep**
- Document **power consumption** if measuring
- Discuss why **frequency hopping** alone is insufficient against wideband jamming in a confined RF environment

---

**Good luck with the lab!**
