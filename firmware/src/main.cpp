/*
 * nRF24L01+PA+LNA Interference Jammer for Logitech Lightspeed
 * ============================================================
 * PlatformIO / Arduino build for nice!nano (nRF52840)
 *
 * Board: nice_nano (1:1 GPIO mapping — Arduino pin N = nRF52840 GPIO N)
 *   P0.x = x, P1.x = 32+x
 *
 * ===================== WIRING SCHEMATIC =====================
 *
 *   E01-ML01DP5 (back side facing you, SMA antenna at top)
 *
 *        +-----------------------+
 *        |  [SMA antenna conn]   |
 *        |                       |
 *        |   EBYTE E01-ML01DP5   |
 *        |                       |
 *        |  VCC            GND   |
 *        |   o              o    |
 *        |  CSN            CE    |
 *        |   o              o    |
 *        | MOSI            SCK   |
 *        |   o              o    |
 *        |  IRQ           MISO   |
 *        |   o              o    |
 *        +-----------------------+
 *            |              |
 *            |              |
 *   Wires:   |              |
 *            v              v
 *
 *        E01-ML01DP5          nice!nano
 *        ----------           ---------
 *        VCC  (top-left)  --> VCC (3.3V)
 *        GND  (top-right) --> GND
 *        CSN  (2nd-left)  --> pad 100
 *        CE   (2nd-right) --> pad 024
 *        MOSI (3rd-left)  --> pad 106
 *        SCK  (3rd-right) --> pad 113
 *        IRQ  (4th-left)  --> NOT CONNECTED
 *        MISO (4th-right) --> pad 111
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

// GPIO numbers = Arduino pin numbers on nice_nano board (1:1 mapping)
#define CE_PIN   24   // P0.24 — nice!nano pad 024
#define CSN_PIN  32   // P1.00 — nice!nano pad 100
#define MOSI_PIN 38   // P1.06 — nice!nano pad 106
#define MISO_PIN 43   // P1.11 — nice!nano pad 111
#define SCK_PIN  45   // P1.13 — nice!nano pad 113

RF24 radio(CE_PIN, CSN_PIN);

void setup() {
    Serial.begin(115200);
    // Wait up to 5 s for USB serial to enumerate
    while (!Serial && millis() < 5000);

    Serial.println("nRF24L01 Interference Jammer initializing...");

    // Remap SPI to the correct nice!nano pads before begin()
    // Adafruit nRF52 BSP: SPI.setPins(MISO, SCK, MOSI)
    SPI.setPins(MISO_PIN, SCK_PIN, MOSI_PIN);
    SPI.begin();

    if (!radio.begin(&SPI)) {
        Serial.println("ERROR: Radio hardware not responding!");
        while (1);
    }
    Serial.println("  Radio OK");

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);
    Serial.println("  PA level: MAX  |  Data rate: 1 Mbps");

    // startConstCarrier() sets CONT_WAVE + PLL_LOCK bits in RF_SETUP (reg 0x06)
    // and asserts CE — equivalent to radio._reg_write(0x06, 0x97) in the Python version.
    // First arg is PA level, second is the starting channel (0 = 2400 MHz).
    radio.startConstCarrier(RF24_PA_MAX, 0);
    Serial.println("  Continuous carrier wave mode: ENABLED");

    Serial.println();
    Serial.println("=== INTERFERENCE ACTIVE ===");
    Serial.println("Sweeping 2.400 - 2.525 GHz (all 126 channels)");
    Serial.println("Expected result: Logitech headset audio dropouts/static");
}

void loop() {
    static uint32_t sweep_count = 0;

    for (uint8_t ch = 0; ch < 126; ch++) {
        radio.setChannel(ch);
        delayMicroseconds(200);  // 200 µs × 126 channels = 25.2 ms per sweep
    }

    sweep_count++;
    if (sweep_count % 100 == 0) {
        Serial.print("Sweep #");
        Serial.print(sweep_count);
        Serial.println(": 2.4-2.5 GHz flooded...");
    }
}
