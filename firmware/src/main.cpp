/*
 * DUAL-RADIO 2.4 GHz Interference Jammer — MAX POWER
 * ====================================================
 * PlatformIO / Arduino build for nice!nano (nRF52840)
 *
 * Uses TWO radios simultaneously on independent sweep paths:
 *   1. External nRF24L01+PA+LNA (E01-ML01DP5) — +20dBm TX flood via SPI
 *      Sweeps channels 0→80 in main loop, ~200 sweeps/sec
 *   2. Internal nRF52840 RADIO — +8dBm constant carrier via timer ISR
 *      Sweeps channels 80→0 independently, ~60 sweeps/sec
 *
 * At any instant, TWO channels are being jammed simultaneously.
 * The radios sweep in opposite directions for maximum coverage.
 *
 * Board: nice_nano (1:1 GPIO mapping — Arduino pin N = nRF52840 GPIO N)
 *   P0.x = x, P1.x = 32+x
 *
 * ===================== WIRING SCHEMATIC =====================
 *
 *   nice!nano v2 (component side facing you, USB-C at top)
 *   Pad naming: P0.xx = 0xx, P1.xx = 1xx
 *
 *                  +----------+
 *                  |  USB-C   |
 *            +-----+----------+-----+
 *            |                      |
 *            |  B+              B-  |
 *            |   o              o   |
 *            |                      |
 *      006   |   o              o   |  RAW
 *      008   |   o              o   |  GND  ──┐
 *      GND   |   o              o   |  RST    │
 *      GND   |   o              o   |  VCC ─┐ │
 *      017   |   o              o   |  031  │ │
 *      020   |   o              o   |  029  │ │
 *      022   |   o              o   |  002  │ │
 *  CE> 024   |   o              o   |  115  │ │
 * CSN> 100   |   o              o   |  113  │ │ <SCK
 *      011   |   o              o   |  111  │ │ <MISO
 *      104   |   o              o   |  010  │ │
 *MOSI> 106   |   o              o   |  009  │ │
 *            |                      |       │ │
 *            +----------------------+       │ │
 *                                           │ │
 *   E01-ML01DP5 (back side facing you,      │ │
 *   SMA antenna at top)                     │ │
 *                                           │ │
 *        +-----------------------+          │ │
 *        |  [SMA antenna conn]   |          │ │
 *        |                       |          │ │
 *        |   EBYTE E01-ML01DP5   |          │ │
 *        |                       |          │ │
 *        |  VCC  red          GND black  |          │ │
 *        |   o              o    |  
 *        |  CSN  orange       CE white   
 *        |   o              o    |
 *        | MOSI   yellow      SCK grey  |
 *        |   o              o    |
 *        |  IRQ    green       MISO purple  |
 *        |   o              o    |
 *        +-----------------------+
 *
 *        CONNECTIONS
 *        ──────────────────────────────────
 *        E01-ML01DP5          nice!nano
 *        ──────────────────────────────────
 *        VCC  (top-left)  --> VCC (3.3V)
 *        GND  (top-right) --> GND
 *        CSN  (2nd-left)  --> pad 100
 *        CE   (2nd-right) --> pad 024
 *        MOSI (3rd-left)  --> pad 106
 *        SCK  (3rd-right) --> pad 113
 *        IRQ  (4th-left)  --> NOT CONNECTED
 *        MISO (4th-right) --> pad 111
 *        ──────────────────────────────────
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "nrf.h"

// GPIO numbers = Arduino pin numbers on nice_nano board (1:1 mapping)
#define CE_PIN   24   // P0.24
#define CSN_PIN  32   // P1.00
#define MOSI_PIN 38   // P1.06
#define MISO_PIN 43   // P1.11
#define SCK_PIN  45   // P1.13

// Channel range: 0-80 = 2400-2480 MHz
// Covers BLE (Bolt), Lightspeed, WiFi 2.4 GHz (Sonos)
#define CH_MIN 0
#define CH_MAX 80

RF24 radio(CE_PIN, CSN_PIN);
static bool internal_radio_ok = false;

// ================================================================
// Internal nRF52840 RADIO — timer-driven independent sweep
// ================================================================

static volatile uint8_t int_radio_ch = CH_MAX;

extern "C" void TIMER1_IRQHandler() {
    if (NRF_TIMER1->EVENTS_COMPARE[0]) {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;

        // Disable radio, switch channel, re-enable
        NRF_RADIO->TASKS_DISABLE = 1;
        while (NRF_RADIO->EVENTS_DISABLED == 0);
        NRF_RADIO->EVENTS_DISABLED = 0;

        // Sweep reverse: 80 -> 0 -> 80 -> ...
        if (int_radio_ch == 0)
            int_radio_ch = CH_MAX;
        else
            int_radio_ch--;

        NRF_RADIO->FREQUENCY = int_radio_ch;
        NRF_RADIO->EVENTS_READY = 0;
        NRF_RADIO->TASKS_TXEN = 1;
        // PLL re-locks (~130µs) during next timer interval
    }
}

static void internal_radio_init() {
    // Disable SoftDevice — it owns RADIO exclusively
    uint32_t err = sd_softdevice_disable();
    if (err != 0) {
        Serial.print("  [INT] sd_softdevice_disable err=");
        Serial.println(err);
        return;
    }

    // Restart HFCLK (was managed by SoftDevice)
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    // Re-enable USB so serial keeps working
    NVIC_EnableIRQ(USBD_IRQn);

    // ---- Configure RADIO ----
    NRF_RADIO->POWER = 1;
    NRF_RADIO->TXPOWER = 0x08;   // +8dBm (nRF52840 max)
    NRF_RADIO->MODE = 1;         // 2 Mbps Nordic mode
    NRF_RADIO->FREQUENCY = CH_MAX;

    // Enable constant carrier wave (TEST register at offset 0x540)
    *(volatile uint32_t *)((uint32_t)NRF_RADIO + 0x540) = 0x03;

    // Start transmitter
    NRF_RADIO->EVENTS_READY = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    while (NRF_RADIO->EVENTS_READY == 0);
    NRF_RADIO->EVENTS_READY = 0;

    // ---- TIMER1: drives channel hopping at 200µs intervals ----
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->MODE = 0;          // Timer mode
    NRF_TIMER1->BITMODE = 0;       // 16-bit
    NRF_TIMER1->PRESCALER = 4;     // 16 MHz / 16 = 1 MHz (1µs ticks)
    NRF_TIMER1->CC[0] = 500;       // Fire every 500µs — longer dwell = more range
    NRF_TIMER1->SHORTS = 1;        // COMPARE0 → CLEAR (auto-reload)
    NRF_TIMER1->INTENSET = (1 << 16);  // COMPARE[0] interrupt
    NVIC_SetPriority(TIMER1_IRQn, 3);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_START = 1;

    internal_radio_ok = true;
}

// ================================================================
// Setup
// ================================================================

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    // LED off until nRF24 is confirmed working
    pinMode(PIN_LED1, OUTPUT);
    digitalWrite(PIN_LED1, LOW);

    Serial.println();
    Serial.println("=== DUAL-RADIO JAMMER — MAX POWER ===");
    Serial.println();

    // ---- External nRF24L01+PA+LNA ----
    SPI.setPins(MISO_PIN, SCK_PIN, MOSI_PIN);
    SPI.begin();

    bool ok = false;
    for (int attempt = 1; attempt <= 10; attempt++) {
        delay(100 * attempt);
        if (radio.begin(&SPI)) {
            ok = true;
            Serial.print("  [EXT] nRF24L01+PA+LNA OK (attempt ");
            Serial.print(attempt);
            Serial.println(")");
            digitalWrite(PIN_LED1, HIGH);  // LED on = nRF24 alive
            break;
        }
        Serial.print("  [EXT] attempt ");
        Serial.print(attempt);
        Serial.println(" failed...");
    }
    if (!ok) {
        Serial.println("  [EXT] FATAL: nRF24L01 not responding!");
        while (1);
    }

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_DISABLED);
    radio.setAutoAck(false);
    radio.setRetries(0, 0);
    radio.setPayloadSize(32);  // 32 bytes = ~152µs airtime = max energy per hop

    const uint8_t addr[5] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    radio.openWritingPipe(addr);
    radio.stopListening();

    Serial.println("  [EXT] +20dBm | 2Mbps | 32-byte payload | CRC off");

    // ---- Internal nRF52840 RADIO ----
    internal_radio_init();
    if (internal_radio_ok) {
        Serial.println("  [INT] +8dBm  | constant carrier | timer-driven");
    } else {
        Serial.println("  [INT] unavailable — external only");
    }

    Serial.println();
    Serial.println("========================================");
    Serial.println("  JAMMING ACTIVE — 2.400-2.480 GHz");
    if (internal_radio_ok) {
        Serial.println("  [EXT] TX flood sweep  0->80 (3x32B/ch)");
        Serial.println("  [INT] Carrier sweep  80->0  (500us dwell)");
    } else {
        Serial.println("  [EXT] TX flood sweep 0->80");
    }
    Serial.println("========================================");
}

// ================================================================
// Main loop — external nRF24L01+PA+LNA TX flood
// ================================================================

static uint8_t junk = 0xFF;

void loop() {
    static uint32_t sweep_count = 0;

    // Fast TX flood: 1-byte packets, proper channel sync.
    // writeFast queues to FIFO, txStandBy ensures TX completes
    // before we switch channels — no FIFO desync.
    for (uint8_t ch = CH_MIN; ch <= CH_MAX; ch++) {
        radio.setChannel(ch);
        radio.writeFast(&junk, 1, true);   // 1-byte multicast, no ACK
        radio.txStandBy();                 // complete TX before channel change
    }

    sweep_count++;
    if (sweep_count % 500 == 0) {
        Serial.print("[EXT] sweeps: ");
        Serial.println(sweep_count);
    }
}
