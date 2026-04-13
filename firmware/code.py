"""
nRF24L01+PA+LNA Interference Jammer for Logitech Lightspeed
=============================================================

Firmware for nice!nano (nRF52840) driving an nRF24L01+PA+LNA module.
Sweeps all 126 channels in continuous carrier wave (CW) mode,
causing interference with 2.4 GHz frequency-hopping devices.

Wiring:
  nRF24L01    nice!nano pad
  VCC    -->  VCC (3.3V)
  GND    -->  GND
  CE     -->  024 (P0.24)
  CSN    -->  100 (P1.00)
  SCK    -->  SCK (P1.13)
  MOSI   -->  010 (P0.10)
  MISO   -->  111 (P1.11)
  IRQ    --> (not connected)

"""

import board
import busio
import digitalio
import time

# Try to import the nRF24L01 library
try:
    from circuitpython_nrf24l01.rf24 import RF24
except ImportError:
    print("ERROR: circuitpython_nrf24l01 library not found!")
    print("Download from: https://github.com/adafruit/circuitpython_nrf24l01")
    print("Copy circuitpython_nrf24l01/ folder to CIRCUITPY/lib/")
    import sys
    sys.exit(1)

# Initialize SPI bus
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)

# Control pins: CE (Chip Enable) and CSN (Chip Select NOT)
# nice!nano uses raw nRF52840 pad names, not Pro Micro D-numbers
ce_pin = digitalio.DigitalInOut(board.P0_24)   # pad 024
csn_pin = digitalio.DigitalInOut(board.P1_00)  # pad 100

# Initialize RF24 radio
radio = RF24(spi, csn_pin, ce_pin)

print("nRF24L01 Interference Jammer initializing...")

# Power on the radio
radio.power = True
print("  Radio powered on")

# Configure data rate and PA level
# For nRF24L01+PA+LNA:
#   - 1 Mbps (data_rate = 1)
#   - Maximum PA level (pa_level = 3 = +20 dBm equivalent with amplifier)
radio.data_rate = 1  # 1 Mbps
radio.pa_level = 3   # Maximum power for PA+LNA version
print("  Data rate: 1 Mbps")
print("  PA level: Maximum (+20 dBm equivalent)")

# Enable continuous carrier wave (CW) mode
# This requires direct register writes via the SPI interface
# RF_SETUP register address: 0x06
# Bit 7: CONT_WAVE (1 = enable continuous carrier)
# Bit 4: PLL_LOCK (1 = enable PLL lock required for CW mode)
# Bit 3: Reserved
# Bit 2: RF_DR_HIGH
# Bit 1: RF_DR_LOW (01 = 1 Mbps)
# Bits 0: RF_PWR[1:0] = 11 (max power)
#
# Final value: 0b10010111 = 0x97
#
# Note: The circuitpython_nrf24l01 library may not expose CONT_WAVE directly,
# so we use raw SPI register write if available, or skip if library handles it.

RF_SETUP_REG = 0x06
CW_MODE_VALUE = 0x97  # CONT_WAVE | PLL_LOCK | 1Mbps | Max Power

try:
    # Attempt raw register write (if library supports it)
    radio._reg_write(RF_SETUP_REG, CW_MODE_VALUE)
    print("  Continuous carrier wave mode: ENABLED")
except AttributeError:
    # If _reg_write not available, try lower-level SPI access
    try:
        ce_pin.value = False  # De-assert CE before register write
        time.sleep(0.01)
        csn_pin.value = False  # Assert CSN (active low)
        spi.write(bytes([RF_SETUP_REG | 0x20]))  # Write command: address | 0x20
        spi.write(bytes([CW_MODE_VALUE]))
        csn_pin.value = True   # De-assert CSN
        ce_pin.value = True    # Re-assert CE
        time.sleep(0.01)
        print("  Continuous carrier wave mode: ENABLED (via raw SPI)")
    except Exception as e:
        print(f"  WARNING: Could not enable CW mode: {e}")
        print("  Proceeding with standard TX mode (less effective)")

# Assert CE (Chip Enable) to begin transmission
ce_pin.value = True
print("  CE asserted - transmission starting")

print("\n=== INTERFERENCE ACTIVE ===")
print("Sweeping 2.400 - 2.525 GHz (all 126 channels)")
print("Expected result: Logitech headset audio dropouts/static")
print("Press Ctrl+C to stop (on REPL)\n")

# Main loop: sweep all 126 channels
sweep_count = 0
try:
    while True:
        for channel in range(126):
            radio.channel = channel  # Tunes RF frequency = 2400 + channel MHz
            # Dwell time on each channel
            # 200µs per channel * 126 channels = 25.2ms full sweep
            # Reducing this time increases hopping speed (up to ~50µs Python minimum)
            time.sleep(0.0002)  # 200 microseconds

        sweep_count += 1
        # Print sweep progress every 100 sweeps
        if sweep_count % 100 == 0:
            print(f"Sweep #{sweep_count}: 2.4-2.5 GHz flooded...")

except KeyboardInterrupt:
    print("\nStopping interference...")
    ce_pin.value = False  # De-assert CE to stop transmission
    radio.power = False   # Power down radio
    print("Radio powered off. Interference stopped.")
