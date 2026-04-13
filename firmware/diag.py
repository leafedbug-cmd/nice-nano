"""SPI diagnostic for nRF24L01 — tests raw communication"""
import board
import busio
import digitalio
import time

print("=== nRF24L01 SPI Diagnostic ===\n")

# Setup pins
ce = digitalio.DigitalInOut(board.P0_24)   # pad 024
ce.direction = digitalio.Direction.OUTPUT
ce.value = False

csn = digitalio.DigitalInOut(board.P1_00)  # pad 100
csn.direction = digitalio.Direction.OUTPUT
csn.value = True  # active low, so idle high

print("CE  = P0.24 (pad 024) -> LOW")
print("CSN = P1.00 (pad 100) -> HIGH (idle)")

# Setup SPI
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
while not spi.try_lock():
    pass
spi.configure(baudrate=1000000)  # 1 MHz, slow for reliability
print("SPI locked at 1 MHz")
print(f"  SCK  = {board.SCK}")
print(f"  MOSI = {board.MOSI}")
print(f"  MISO = {board.MISO}")

def spi_transfer(tx):
    """Send bytes, return received bytes"""
    rx = bytearray(len(tx))
    csn.value = False
    time.sleep(0.001)
    spi.write_readinto(tx, rx)
    csn.value = True
    time.sleep(0.001)
    return rx

# Test 1: Read STATUS register (command 0xFF = NOP, returns status in first byte)
print("\n--- Test 1: Read STATUS (NOP command) ---")
for i in range(3):
    rx = spi_transfer(bytearray([0xFF]))
    status = rx[0]
    print(f"  Attempt {i+1}: STATUS = 0x{status:02X} (binary: {status:08b})")
    if status == 0x00 or status == 0xFF:
        print("    BAD: 0x00 or 0xFF means no SPI response")
    else:
        print("    GOOD: got a real response")

# Test 2: Read CONFIG register (address 0x00)
print("\n--- Test 2: Read CONFIG register ---")
rx = spi_transfer(bytearray([0x00, 0x00]))
print(f"  STATUS=0x{rx[0]:02X}, CONFIG=0x{rx[1]:02X}")

# Test 3: Write then read CONFIG register
print("\n--- Test 3: Write 0x0E to CONFIG, then read back ---")
spi_transfer(bytearray([0x20, 0x0E]))  # write command = 0x20 | addr
time.sleep(0.01)
rx = spi_transfer(bytearray([0x00, 0x00]))
print(f"  STATUS=0x{rx[0]:02X}, CONFIG=0x{rx[1]:02X}")
if rx[1] == 0x0E:
    print("  PASS: CONFIG readback matches!")
else:
    print(f"  FAIL: expected 0x0E, got 0x{rx[1]:02X}")

# Test 4: Read all registers 0x00-0x09
print("\n--- Test 4: Register dump (0x00-0x09) ---")
for addr in range(10):
    rx = spi_transfer(bytearray([addr, 0x00]))
    print(f"  Reg 0x{addr:02X} = 0x{rx[1]:02X}")

spi.unlock()
print("\n=== Diagnostic complete ===")
