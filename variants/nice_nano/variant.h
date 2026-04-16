/*
 * variant.h — nice!nano v2 (nRF52840)
 *
 * 1:1 GPIO mapping: Arduino pin N == nRF52840 GPIO N
 *   P0.x  = pin x      (0-31)
 *   P1.x  = pin 32+x   (32-47)
 */

#ifndef _VARIANT_NICE_NANO_
#define _VARIANT_NICE_NANO_

/** Master clock frequency */
#define VARIANT_MCK (64000000ul)

#define USE_LFRC    // nice!nano v2 has no 32 kHz crystal; use internal RC

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C" {
#endif

// Number of pins defined in PinDescription array
#define PINS_COUNT           (48)
#define NUM_DIGITAL_PINS     (48)
#define NUM_ANALOG_INPUTS    (6)
#define NUM_ANALOG_OUTPUTS   (0)

// LEDs  (active HIGH)
#define PIN_LED1             (15)   // P0.15 — blue LED on nice!nano
#define LED_BUILTIN          PIN_LED1
#define LED_STATE_ON         1

/*
 * Buttons — no user button on nice!nano; set to RST pad
 */
#define PIN_BUTTON1          (18)   // P0.18 (RESET)

/*
 * Analog pins  (directly using GPIO numbers)
 */
#define PIN_A0               (2)    // P0.02 / AIN0
#define PIN_A1               (3)    // P0.03 / AIN1
#define PIN_A2               (4)    // P0.04 / AIN2 (battery divider)
#define PIN_A3               (5)    // P0.05 / AIN3
#define PIN_A4               (28)   // P0.28 / AIN4
#define PIN_A5               (29)   // P0.29 / AIN5

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;

#define ADC_RESOLUTION 14

#define PIN_AREF             (2)
#define PIN_VBAT             (4)    // P0.04 — battery voltage via ADC

static const uint8_t AREF = PIN_AREF;

/*
 * Serial interfaces
 */
#define PIN_SERIAL1_RX       (8)    // P0.08
#define PIN_SERIAL1_TX       (6)    // P0.06

/*
 * SPI Interfaces  (directly using GPIO numbers — 1:1 mapping)
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (43)   // P1.11
#define PIN_SPI_MOSI         (38)   // P1.06
#define PIN_SPI_SCK          (45)   // P1.13

static const uint8_t SS   = (32);   // P1.00
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (17)   // P0.17
#define PIN_WIRE_SCL         (20)   // P0.20

// NFC pins (directly using GPIO numbers)
#define PIN_NFC1             (9)    // P0.09
#define PIN_NFC2             (10)   // P0.10

#ifdef __cplusplus
}
#endif

#endif /* _VARIANT_NICE_NANO_ */
