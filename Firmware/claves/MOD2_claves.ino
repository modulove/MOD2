/*
HAGIWO MOD2 Claves Ver1.1 - With LED envelope display
A simple claves sound source. The waveform mixes sine and triangle waves at an arbitrary rate.
The pitch CV follows the V/oct.
LED brightness follows the envelope shape when triggered.

--Pin assign---
POT1     A0       Decay
POT2     A1       waveform sin<->tri
POT3     A2       Pitch
IN1      GPIO7    Trig in
IN2      GPIO0    N/A
CV       A2       Shared with POT3
OUT      GPIO1    Audio output
BUTTON   GPIO6    Manual Trig
LED      GPIO5    Envelope LED output (PWM)
EEPROM    N/A


CC0 1.0 Universal (CC0 1.0) Public Domain Dedication
You can copy, modify, distribute and perform the work, even for commercial purposes, all without asking permission.

Most of the source code was generated by AI.
Although the code has been tested, it may still exhibit unstable behavior or contain incorrect comments.

*/

#include <Arduino.h>
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include <math.h>

/* ============================================================ */
/* ★ Basic constants                                            */
/* ============================================================ */
const float   SYS_CLOCK   = 150000000.0f;   // 150 MHz system clock (fixed on RP2040)
const uint16_t TABLE_SZ   = 8192;           // Sample table length (generated per trigger)
const uint8_t  LUT_SZ     = 64;             // Size of mini‑waveform LUT
#define        LUT_MASK   (LUT_SZ - 1)      // Bit‑mask for fast modulo operation

const float PWM_FS  = 1023.0f;              // 10‑bit PWM resolution (wrap value)
const float PWM_MID = PWM_FS / 2.0f;        // Mid‑scale for silence (50 % duty)

const float AUDIO_FS = SYS_CLOCK / 4096.0f; // ≈36.6 kHz effective sample rate
const float PH_INC_K = float(LUT_SZ) / AUDIO_FS;  // Phase‑increment scaling factor

/* ---- CV input on A2 ---------------------------------------- */
#define  ADC_RES_BITS 10                         // Use 10‑bit resolution (1023 max)
#define  ADC_MAX_VAL  ((1 << ADC_RES_BITS) - 1)  // 1023
#define  ADC_AVG_CNT  16                         // Simple 16‑sample moving average

const float CV_FULL_V    = 5.0f;            // CV range 0‑5 V (1 V/Oct assumed)
const float PITCH_F0     = 50.0f;           // Base frequency at 5 V CV
const float PITCH_MAX_HZ = 1500.0f;         // Absolute frequency limit

/* ============================================================ */
/* ★ Global variables                                           */
/* ============================================================ */
float sineLUT[LUT_SZ];      // 64‑sample sine table
float triLUT [LUT_SZ];      // 64‑sample triangle table

uint16_t finalTbl[TABLE_SZ];   // 16‑bit PWM samples generated per strike
uint16_t envTbl[TABLE_SZ];     // Envelope values for LED brightness

volatile bool  playing  = false;     // Playback state flag
volatile uint16_t tblIdx = 0;        // Current playback index

volatile float decayRate = 5.0f;     // Envelope decay (CV A0)
volatile float waveMorph = 0.0f;     // 0 = sine, 1 = triangle (CV A1)

uint sliceAudio, sliceIRQ, sliceLED;           // PWM slice numbers

/* ============================================================ */
/* ★ Read ADC with 16‑sample averaging                          */
/* ============================================================ */
inline uint16_t readAnalogAvg(uint8_t pin) {
  uint32_t sum = 0;
  for (int i = 0; i < ADC_AVG_CNT; ++i) sum += analogRead(pin);
  return uint16_t(sum / ADC_AVG_CNT);       // 0‑1023 result
}

/* ============================================================ */
/* ★ PWM‑wrap ISR  (≈36.6 kHz sample rate)                      */
/*    Called every time sliceIRQ reaches its wrap value.        */
/* ============================================================ */
void on_pwm_wrap() {
  pwm_clear_irq(sliceIRQ);                   // Clear IRQ for next cycle

  if (!playing) {                           // Output mid‑scale when idle
    pwm_set_chan_level(sliceAudio, PWM_CHAN_B, uint16_t(PWM_MID));
    pwm_set_chan_level(sliceLED, PWM_CHAN_A, 0);  // LED off
    return;
  }

  // Set audio output
  pwm_set_chan_level(sliceAudio, PWM_CHAN_B, finalTbl[tblIdx]);
  
  // Set LED brightness based on envelope
  pwm_set_chan_level(sliceLED, PWM_CHAN_A, envTbl[tblIdx]);

  if (++tblIdx >= TABLE_SZ) {               // Stop after final sample
    playing = false;
    tblIdx  = 0;
    pwm_set_chan_level(sliceLED, PWM_CHAN_A, 0);  // Ensure LED is off
  }
}

/* ============================================================ */
/* ★ Trigger ISR (GPIO7 rising edge or GPIO6 falling edge)      */
/*    Generates a new sample table and starts playback.         */
/* ============================================================ */
void onTrigger() {
  /* ----- Stop audio IRQ & enter edit state ----------------- */
  playing = false;
  irq_set_enabled(PWM_IRQ_WRAP, false);

  /* ----- Allow CV to settle, then read knobs/CV ------------- */
  delay(5);   // Small wait to avoid ADC glitch (≈5 ms)

  decayRate = 1.0f + 9.0f * (analogRead(A0) / 1023.0f);   // 1‑10 range
  waveMorph =               analogRead(A1) / 1023.0f;     // 0‑1

  uint16_t adc   = ADC_MAX_VAL - readAnalogAvg(A2);       // Invert: 0 V → high pitch
  float cvV      = (adc / float(ADC_MAX_VAL)) * CV_FULL_V;
  float baseFreq = PITCH_F0 * powf(2.0f, cvV);            // 1 V/Oct mapping
  if (baseFreq > PITCH_MAX_HZ) baseFreq = PITCH_MAX_HZ;   // Clamp

  /* ----- Generate sample table ----------------------------- */
  const float invN   = 1.0f / (TABLE_SZ - 1);
  const float expK   = expf(-decayRate * invN);           // Per‑sample decay
  float env          = 1.0f;                              // Envelope start

  const float phInc  = baseFreq * PH_INC_K;               // Phase increment
  float ph           = 0.0f;

  const uint16_t fadeStart = uint16_t(TABLE_SZ * 0.95f);  // Last 5 % tail fade
  const uint16_t fadeLen   = TABLE_SZ - 1 - fadeStart;
  const float    invFadeLen = 1.0f / fadeLen;

  for (uint16_t i = 0; i < TABLE_SZ; ++i) {
    /* -------------- Linear interpolation in 64‑point LUT -------------- */
    uint8_t idx  = uint8_t(ph) & LUT_MASK;
    uint8_t idx2 = (idx + 1) & LUT_MASK;
    float   frac = ph - uint8_t(ph);

    float s = sineLUT[idx] * (1 - frac) + sineLUT[idx2] * frac;
    float t =  triLUT[idx] * (1 - frac) +  triLUT[idx2] * frac;
    float w = s * (1 - waveMorph) + t * waveMorph;        // Morph mix

    /* -------------- Apply exponential decay envelope ------------------ */
    float y = w * env;
    
    /* -------------- Store current envelope value for LED -------------- */
    float envValue = env;
    
    env *= expK;

    /* -------------- Additional cosine fade‑out at very end ------------ */
    if (i >= fadeStart) {
      float mu  = (i - fadeStart) * invFadeLen;
      float mu2 = 0.5f * (1 - cosf(mu * PI));  // Smooth 0→1 curve
      y *= (1 - mu2);
      envValue *= (1 - mu2);  // Apply fade to LED envelope too
    }

    /* -------------- Convert −1..+1 to 0..1023 for PWM ----------------- */
    finalTbl[i] = uint16_t((y + 1.0f) * (PWM_FS / 2.0f));
    
    /* -------------- Convert envelope 0..1 to 0..1023 for LED PWM ------ */
    envTbl[i] = uint16_t(envValue * PWM_FS);

    /* -------------- Advance phase and wrap ---------------------------- */
    ph += phInc;
    if (ph >= LUT_SZ) ph -= LUT_SZ;
  }

  /* ----- Restart playback --------------------------------- */
  tblIdx  = 0;
  playing = true;
  irq_set_enabled(PWM_IRQ_WRAP, true);
}

/* ============================================================ */
/* ★ SETUP                                                     */
/*    Hardware initialisation runs once at boot.               */
/* ============================================================ */
void setup() {
  analogReadResolution(ADC_RES_BITS);        // 10‑bit ADC resolution

  /* --- Build 64‑point sine & triangle LUTs --------------------------- */
  for (uint8_t i = 0; i < LUT_SZ; ++i) {
    float th      = 2.0f * PI * i / LUT_SZ;
    sineLUT[i]    = sinf(th);
    triLUT[i]     = (2.0f / PI) * asinf(sineLUT[i]);  // Triangle via inverse‑sine
  }

  /* --- PWM output pin (GPIO1) --------------------------------------- */
  pinMode(1, OUTPUT);
  gpio_set_function(1, GPIO_FUNC_PWM);
  sliceAudio = pwm_gpio_to_slice_num(1);     // Note: GPIO0/1 share slice 0

  /* --- LED PWM output pin (GPIO8) ----------------------------------- */
  pinMode(5, OUTPUT);
  gpio_set_function(5, GPIO_FUNC_PWM);
  sliceLED = pwm_gpio_to_slice_num(5);       // GPIO8/9 use slice 4

  /* --- Second PWM slice as 36.6 kHz timer (GPIO2) ------------------- */
  pinMode(2, OUTPUT);
  gpio_set_function(2, GPIO_FUNC_PWM);
  sliceIRQ = pwm_gpio_to_slice_num(2);       // GPIO2/3 use slice 1

  /* --- Audio PWM channel (channel B) -------------------------------- */
  pwm_set_clkdiv(sliceAudio, 1);             // Full speed (150 MHz / 1)
  pwm_set_wrap  (sliceAudio, 1023);          // 10‑bit resolution
  pwm_set_enabled(sliceAudio, true);

  /* --- LED PWM channel (channel A) ---------------------------------- */
  pwm_set_clkdiv(sliceLED, 1);               // Full speed (150 MHz / 1)
  pwm_set_wrap  (sliceLED, 1023);            // 10‑bit resolution
  pwm_set_chan_level(sliceLED, PWM_CHAN_A, 0); // Start with LED off
  pwm_set_enabled(sliceLED, true);

  /* --- Timer PWM channel -------------------------------------------- */
  pwm_set_clkdiv(sliceIRQ, 1);               // Same clock divisor
  pwm_set_wrap  (sliceIRQ, 4095);            // 150 MHz / 4096 ≈ 36.6 kHz
  pwm_set_enabled(sliceIRQ, true);
  pwm_clear_irq     (sliceIRQ);
  pwm_set_irq_enabled(sliceIRQ, true);
  irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
  irq_set_enabled(PWM_IRQ_WRAP, true);

  /* --- Trigger input (jack) on GPIO7, rising edge ------------------- */
  pinMode(7, INPUT);
  attachInterrupt(digitalPinToInterrupt(7), onTrigger, RISING);

  /* --- Push‑button input on GPIO6, falling edge (active‑low) --------- */
  pinMode(6, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(6), onTrigger, FALLING);
}

/* ============================================================ */
/* ★ Main loop (empty)                                          */
/*    All real‑time work is done in ISRs.                       */
/* ============================================================ */
void loop() {
  /* Nothing here – trigger and playback handled by interrupts. */
}