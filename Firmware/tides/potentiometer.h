// potentiometer.h - Pot handling for MOD2 Tides
// 3 pots: POT1 (A0), POT2 (A1), POT3/CV (A2)

#pragma once

// Pin definitions for Seeeduino XIAO RP2350
#define AIN0  A0  // POT1 - Shape
#define AIN1  A1  // POT2 - Slope
#define AIN2  A2  // POT3 - Frequency / V/Oct CV

#define POT_SAMPLE_TIME 50    // delay time between pot reads (ms) - increased to reduce noise
#define MIN_POT_CHANGE 20     // locked pot reading must change by this to register
#define MIN_COUNTS 12         // unlocked pot must change by this to register
#define POT_AVERAGING 16      // analog sample averaging count - increased for stability
#define POT_MIN 0             // minimum pot value
#define POT_MAX 4096          // maximum pot value (12-bit ADC)

#define NPOTS 3               // number of pots

uint16_t potvalue[NPOTS];     // pot readings
uint16_t lastpotvalue[NPOTS]; // old pot readings
bool potlock[NPOTS];          // when pots are locked they must change by MIN_POT_CHANGE
uint32_t pot_timer;           // reading pots too often causes noise

// Flag all pot values as locked (require larger movement to register)
void lockpots(void) {
  for (int i = 0; i < NPOTS; ++i) potlock[i] = 1;
}

// Sample analog pot input with filtering and locking
uint16_t readpot(uint8_t potnum) {
  int val = 0;
  int input;

  switch (potnum) {
    case 0: input = AIN0; break;
    case 1: input = AIN1; break;
    case 2: input = AIN2; break;
    default: input = AIN0; break;
  }

  // Read pot value with averaging for stability
  for (int j = 0; j < POT_AVERAGING; ++j) {
    val += analogRead(input);
  }
  val = val / POT_AVERAGING;

  // Handle pot locking
  if (potlock[potnum]) {
    int delta = lastpotvalue[potnum] - val;
    if (abs(delta) > MIN_POT_CHANGE) {
      potlock[potnum] = 0;  // unlock pot
      potvalue[potnum] = lastpotvalue[potnum] = val;
    } else {
      val = lastpotvalue[potnum];
    }
  } else {
    if (abs(lastpotvalue[potnum] - val) > MIN_COUNTS) {
      lastpotvalue[potnum] = val;
    } else {
      val = lastpotvalue[potnum];
    }
    potvalue[potnum] = val;
  }

  return val;
}
