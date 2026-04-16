/*
 * variant.cpp — nice!nano v2 (nRF52840)
 *
 * 1:1 GPIO mapping: Arduino pin N == nRF52840 GPIO N
 *   Index i maps to hardware GPIO i.
 */

#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include "nrf.h"

const uint32_t g_ADigitalPinMap[] =
{
   0,  // D0  = P0.00
   1,  // D1  = P0.01
   2,  // D2  = P0.02
   3,  // D3  = P0.03
   4,  // D4  = P0.04
   5,  // D5  = P0.05
   6,  // D6  = P0.06
   7,  // D7  = P0.07
   8,  // D8  = P0.08
   9,  // D9  = P0.09
  10,  // D10 = P0.10
  11,  // D11 = P0.11
  12,  // D12 = P0.12
  13,  // D13 = P0.13  (VCC power control — HIGH = VCC off)
  14,  // D14 = P0.14
  15,  // D15 = P0.15  (blue LED)
  16,  // D16 = P0.16
  17,  // D17 = P0.17
  18,  // D18 = P0.18  (RESET)
  19,  // D19 = P0.19
  20,  // D20 = P0.20
  21,  // D21 = P0.21
  22,  // D22 = P0.22
  23,  // D23 = P0.23
  24,  // D24 = P0.24
  25,  // D25 = P0.25
  26,  // D26 = P0.26
  27,  // D27 = P0.27
  28,  // D28 = P0.28
  29,  // D29 = P0.29
  30,  // D30 = P0.30
  31,  // D31 = P0.31
  32,  // D32 = P1.00
  33,  // D33 = P1.01
  34,  // D34 = P1.02
  35,  // D35 = P1.03
  36,  // D36 = P1.04
  37,  // D37 = P1.05
  38,  // D38 = P1.06
  39,  // D39 = P1.07
  40,  // D40 = P1.08
  41,  // D41 = P1.09
  42,  // D42 = P1.10
  43,  // D43 = P1.11
  44,  // D44 = P1.12
  45,  // D45 = P1.13
  46,  // D46 = P1.14
  47,  // D47 = P1.15
};
