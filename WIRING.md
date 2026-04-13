# nRF24L01+PA+LNA to nice!nano Wiring Schematic

## Pin Connections

| nRF24L01 Pin | nice!nano Pad | GPIO | Arduino Pin | Wire Color | Function |
|---|---|---|---|---|---|
| VCC | VCC | - | - | **BLUE** | 3.3V Power |
| GND | GND | - | - | **BROWN** | Ground |
| CE | 024 | P0.24 | 24 | **GREEN** | Chip Enable |
| CSN | 100 | P1.00 | 32 | **WHITE** | Chip Select (Active Low) |
| MOSI | 010 | P0.10 | 10 | **WHITE/ORANGE** | SPI Data In |
| MISO | 111 | P1.11 | 43 | **PURPLE** | SPI Data Out |
| SCK | SCK (113) | P1.13 | 45 | **BLUE** | SPI Clock |
| IRQ | - | - | - | *(not connected)* | Interrupt (optional) |

## Physical Layout

```
                        nice!nano
                    ┌─────────────────┐
                    │ USB  GND    VCC │
                    │              │  │
          CE ───────┤ 24      043  │  │
         CSN ───────┤ 32  nRF52840 │  │
        MOSI ───────┤ 10      045  │  │
        MISO ───────┤ 43           │  │
         SCK ───────┤ 45      010  │  │
        GND ────────┤ GND     100  │  │
        VCC ────────┤ VCC     024  │  │
                    └─────────────────┘


                   nRF24L01+PA+LNA Module
                    ┌─────────────────┐
                    │ 1 2 3 4 5 6 7 8 │  (pin numbering, viewed from top)
                    │ ┌───────────────┤
                    ├─┤ GND  VCC  CE  │
                    │ │ CSN  MOSI MISO│
                    │ │ SCK  IRQ      │
                    │ └───────────────┤
                    └─────────────────┘
```

## Wire Colors & Pinout (Top View of nRF24L01)

```
    Pin 1: GND      ─── BROWN
    Pin 2: VCC      ─── BLUE
    Pin 3: CE       ─── GREEN
    Pin 4: CSN      ─── WHITE
    Pin 5: MOSI     ─── WHITE/ORANGE
    Pin 6: MISO     ─── PURPLE
    Pin 7: SCK      ─── BLUE
    Pin 8: IRQ      ─── (NOT CONNECTED)
```

## SPI Protocol

- **MOSI** (White/Orange): Master Out, Slave In — data FROM nice!nano TO nRF24
- **MISO** (Purple): Master In, Slave Out — data FROM nRF24 TO nice!nano
- **SCK** (Blue): Serial Clock — synchronization signal
- **CSN** (White): Chip Select — pulled LOW to enable SPI communication
- **CE** (Green): Chip Enable — asserted HIGH during TX/RX operations

## Key Notes

1. **All signals are 3.3V** (nRF52840 native voltage)
2. **Decoupling Capacitor**: Add 10µF ceramic cap between VCC and GND on nRF24 module (recommended for PA+LNA variant)
3. **Antenna**: Module includes integrated antenna + PA/LNA for extended range
4. **IRQ not used**: The interference jammer runs in a continuous loop; IRQ interrupt is not needed
5. **Faraday Cage**: All RF testing must occur inside shielded enclosure to prevent real-world interference

## Testing Procedure

1. Double-tap nRF52840 reset button to enter bootloader (LED blinks)
2. Flash firmware: `pio run -e nice_nano --target upload`
3. Monitor output: `pio device monitor`
4. Expected output: "=== INTERFERENCE ACTIVE ===" + sweep counters
5. Verify LED status (if connected) or listen for target device degradation

## Troubleshooting

| Issue | Cause | Fix |
|---|---|---|
| "Radio hardware not responding" | SPI wiring incorrect | Check CE, CSN, SCK, MOSI, MISO connections |
| Intermittent failures | Insufficient power delivery | Add 10µF cap to VCC/GND; check power supply |
| No interference effect | Wrong frequency band | Verify radio.startConstCarrier() in firmware |
| Serial monitor not working | USB enumeration timeout | Hold RST longer before flashing |

---

**Generated for**: University of Arkansas RF Testing Lab  
**Hardware**: nice!nano (nRF52840) + nRF24L01+PA+LNA module  
**Faraday Cage**: Required for all testing
