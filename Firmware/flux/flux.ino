/*
HAGIWO MOD2 – FLUX v1.0
Physical Modeling, Resonance & Noise

From ordered resonance to pure chaos.

═══════════════════════════════════════════════════════════════════════════════
                              MODES (7 Total)
═══════════════════════════════════════════════════════════════════════════════

  RESONANCE:
   0 – MODAL            Tuned resonant filter bank (string/piano/bell)
   1 – KARPLUS          Plucked string physical model
  
  NOISE:
   2 – WHITE            Full spectrum noise
   3 – PINK             1/f natural spectrum noise
   4 – S&H              Sample-and-hold stepped noise
   5 – QUANTUM          Lorenz attractor chaotic noise
  
  TEXTURE:
   6 – DRONE            Evolving harmonic texture

═══════════════════════════════════════════════════════════════════════════════
                              CONTROLS
═══════════════════════════════════════════════════════════════════════════════

  POT1 (A0)    Frequency / Pitch / CV input (V/Oct)
  POT2 (A1)    Rate / Trigger speed / Chaos rate
  POT3 (A2)    Character / Brightness / Slew
               - Modal: Harmonic→Inharmonic (string→bell)
               - Karplus: Brightness/Decay
               - S&H: Slew amount
               - Quantum: Chaos intensity
               - Drone: Evolution rate
  
  BUTTON       SHORT: Cycle modes | LONG (>500ms): Manual trigger/pluck
  LED          Blinks on trigger

═══════════════════════════════════════════════════════════════════════════════

  License: CC0 1.0 Universal – Public Domain
  Hardware: HAGIWO MOD2 (RP2350)

*/

#include <Arduino.h>
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include <math.h>


/* ═══════════════════════════════════════════════════════════════════════════
                              CONSTANTS
   ═══════════════════════════════════════════════════════════════════════════ */

constexpr int   TABLE_BITS      = 13;
constexpr int   TABLE_SIZE      = 1 << TABLE_BITS;
constexpr int   TABLE_MASK      = TABLE_SIZE - 1;

constexpr int   PHASE_FRAC_BITS = 32 - TABLE_BITS;
constexpr uint32_t PHASE_FRAC_MASK = (1UL << PHASE_FRAC_BITS) - 1;
constexpr float PHASE_SCALE     = static_cast<float>(1UL << PHASE_FRAC_BITS);

constexpr float FULL_SCALE      = 1023.0f;
constexpr float MID_LEVEL       = FULL_SCALE / 2.0f;

constexpr float SYS_CLK         = 150000000.0f;
constexpr int   PWM_WRAP_IRQ    = 4095;

constexpr float CENTER_MIN_HZ   = 32.0f;
constexpr float CENTER_MAX_HZ   = 2000.0f;

// Modal resonator
constexpr int   NUM_MODES_RES   = 12;

// Karplus-Strong
constexpr int   KS_BUFFER_SIZE  = 4096;
constexpr int   KS_BUFFER_MASK  = KS_BUFFER_SIZE - 1;

// Pink noise
constexpr int   PINK_STAGES     = 6;

// Drone
constexpr int   DRONE_HARMONICS = 16;

// Button
constexpr uint32_t LONG_PRESS_MS = 500;
constexpr uint32_t DEBOUNCE_MS   = 50;

constexpr int   NUM_MODES       = 7;


/* ═══════════════════════════════════════════════════════════════════════════
                              WAVETABLES
   ═══════════════════════════════════════════════════════════════════════════ */

float tableSin[TABLE_SIZE];
float tableSinDeriv[TABLE_SIZE];


/* ═══════════════════════════════════════════════════════════════════════════
                              GLOBAL STATE
   ═══════════════════════════════════════════════════════════════════════════ */

uint sliceAudio;
uint sliceIRQ;
float sampleRate    = 0.0f;
float sampleRateInv = 0.0f;

volatile uint8_t currentMode = 0;
volatile float   speedParam  = 0.5f;
volatile float   centerFreq  = 440.0f;
volatile float   auxParam    = 0.5f;

// Noise state
volatile uint32_t noiseState = 0x12345678;

// Pink noise (Voss-McCartney)
volatile float pinkRows[PINK_STAGES];
volatile int   pinkIndex = 0;
volatile float pinkRunningSum = 0.0f;

// S&H noise
volatile float shValue = 0.0f;
volatile float shTarget = 0.0f;
volatile uint32_t shPhase = 0;
volatile uint32_t shLastPhaseHigh = 0;

// Quantum noise (Lorenz)
volatile float qx = 0.1f, qy = 0.0f, qz = 0.0f;
volatile float quantumOut = 0.0f;

// Modal resonator
struct ResonatorMode {
  float phase;
  float velocity;
  float amp;
};
volatile ResonatorMode resModes[NUM_MODES_RES];
volatile float resExcitation = 0.0f;

// Karplus-Strong
float ksBuffer[KS_BUFFER_SIZE];
volatile int   ksWriteIdx = 0;
volatile float ksLastOut = 0.0f;
volatile float ksLastOut2 = 0.0f;
volatile bool  ksNeedsPluck = true;

// Drone
volatile uint32_t dronePhases[DRONE_HARMONICS];
volatile float    droneAmps[DRONE_HARMONICS];
volatile float    droneTargetAmps[DRONE_HARMONICS];

// Output processing
volatile float lp1_x1 = 0, lp1_x2 = 0, lp1_y1 = 0, lp1_y2 = 0;
volatile float lp2_x1 = 0, lp2_x2 = 0, lp2_y1 = 0, lp2_y2 = 0;
volatile float dcX1 = 0, dcY1 = 0;

// Manual trigger flag
volatile bool manualTrigger = false;


/* ═══════════════════════════════════════════════════════════════════════════
                         INITIALIZATION
   ═══════════════════════════════════════════════════════════════════════════ */

void initTables()
{
  const float twoPiOverSize = 2.0f * PI / static_cast<float>(TABLE_SIZE);
  
  for (int i = 0; i < TABLE_SIZE; ++i)
  {
    float angle = twoPiOverSize * static_cast<float>(i);
    tableSin[i] = sinf(angle);
    tableSinDeriv[i] = cosf(angle) * twoPiOverSize;
  }
  
  for (int i = 0; i < KS_BUFFER_SIZE; ++i)
    ksBuffer[i] = 0.0f;
  
  for (int i = 0; i < PINK_STAGES; ++i)
    pinkRows[i] = 0.0f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         INTERPOLATION
   ═══════════════════════════════════════════════════════════════════════════ */

inline float hermiteInterp(const float* table, const float* deriv, uint32_t phase)
{
  int idx = phase >> PHASE_FRAC_BITS;
  float frac = static_cast<float>(phase & PHASE_FRAC_MASK) / PHASE_SCALE;
  
  int idx1 = (idx + 1) & TABLE_MASK;
  
  float y0 = table[idx];
  float y1 = table[idx1];
  float d0 = deriv[idx];
  float d1 = deriv[idx1];
  
  float t = frac;
  float t2 = t * t;
  float t3 = t2 * t;
  
  return (2.0f * t3 - 3.0f * t2 + 1.0f) * y0 +
         (t3 - 2.0f * t2 + t) * d0 +
         (-2.0f * t3 + 3.0f * t2) * y1 +
         (t3 - t2) * d1;
}

inline float readSine(uint32_t phase)
{
  return hermiteInterp(tableSin, tableSinDeriv, phase);
}

inline uint32_t freqToPhaseInc(float freq)
{
  return static_cast<uint32_t>(freq * PHASE_SCALE * static_cast<float>(TABLE_SIZE) * sampleRateInv);
}


/* ═══════════════════════════════════════════════════════════════════════════
                         NOISE GENERATORS
   ═══════════════════════════════════════════════════════════════════════════ */

inline uint32_t xorshift32()
{
  noiseState ^= noiseState << 13;
  noiseState ^= noiseState >> 17;
  noiseState ^= noiseState << 5;
  return noiseState;
}

inline float whiteNoise()
{
  return static_cast<float>(static_cast<int32_t>(xorshift32())) / 2147483648.0f;
}

inline float pinkNoise()
{
  int numZeros = __builtin_ctz(pinkIndex + 1);
  if (numZeros >= PINK_STAGES) numZeros = PINK_STAGES - 1;
  
  pinkRunningSum -= pinkRows[numZeros];
  pinkRows[numZeros] = whiteNoise() * 0.5f;
  pinkRunningSum += pinkRows[numZeros];
  
  pinkIndex = (pinkIndex + 1) & 63;
  
  return (pinkRunningSum + whiteNoise() * 0.5f) * 0.22f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: MODAL RESONATOR
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeModal()
{
  float mix = 0.0f;
  
  // Excitation decay
  float excite = 0.0f;
  if (resExcitation > 0.001f)
  {
    excite = whiteNoise() * resExcitation;
    resExcitation *= 0.994f;
  }
  
  // Resonance from speed param
  float baseDecay = 0.9985f + speedParam * 0.0014f;
  
  for (int i = 0; i < NUM_MODES_RES; ++i)
  {
    // Mode ratios based on aux param
    float modeRatio;
    
    if (auxParam < 0.25f)
    {
      // Harmonic (string-like)
      modeRatio = static_cast<float>(i + 1);
    }
    else if (auxParam < 0.5f)
    {
      // Slightly inharmonic (piano-like)
      float n = static_cast<float>(i + 1);
      float inharmonicity = (auxParam - 0.25f) * 4.0f * 0.001f;
      modeRatio = n * (1.0f + inharmonicity * n * n);
    }
    else if (auxParam < 0.75f)
    {
      // More inharmonic (marimba-like)
      float ratios[12] = {1.0f, 2.76f, 5.4f, 8.93f, 13.34f, 18.64f, 
                          24.82f, 31.87f, 39.81f, 48.62f, 58.31f, 68.88f};
      float blend = (auxParam - 0.5f) * 4.0f;
      float harm = static_cast<float>(i + 1);
      modeRatio = harm * (1.0f - blend) + ratios[i] * blend;
    }
    else
    {
      // Bell-like (very inharmonic)
      float ratios[12] = {1.0f, 1.88f, 2.83f, 3.76f, 4.67f, 5.52f,
                          6.35f, 7.15f, 7.93f, 8.69f, 9.43f, 10.15f};
      modeRatio = ratios[i];
    }
    
    float modeFreq = centerFreq * modeRatio;
    if (modeFreq > sampleRate * 0.45f) continue;
    
    // Higher modes decay faster
    float modeDecay = baseDecay - static_cast<float>(i) * 0.0003f;
    if (modeDecay < 0.99f) modeDecay = 0.99f;
    
    float phaseInc = modeFreq * 2.0f * PI * sampleRateInv;
    
    // Add excitation
    float exciteGain = 1.0f / (1.0f + static_cast<float>(i) * 0.4f);
    resModes[i].velocity += excite * exciteGain;
    
    // Update oscillator
    resModes[i].phase += phaseInc;
    if (resModes[i].phase > 2.0f * PI) resModes[i].phase -= 2.0f * PI;
    
    // Damping
    resModes[i].velocity *= modeDecay;
    
    // Amplitude with rolloff
    float modeGain = 1.0f / (1.0f + static_cast<float>(i) * 0.25f);
    mix += resModes[i].velocity * sinf(resModes[i].phase) * modeGain;
  }
  
  return mix * 0.35f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: KARPLUS-STRONG
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeKarplus()
{
  if (ksNeedsPluck)
  {
    int period = static_cast<int>(sampleRate / centerFreq);
    if (period > KS_BUFFER_SIZE - 1) period = KS_BUFFER_SIZE - 1;
    if (period < 2) period = 2;
    
    // Shaped noise burst
    for (int i = 0; i < period; ++i)
    {
      float env = 0.5f - 0.5f * cosf(2.0f * PI * static_cast<float>(i) / static_cast<float>(period));
      // Mix of noise and impulse for different timbres
      float impulse = (i < period / 8) ? (1.0f - static_cast<float>(i) / (period / 8.0f)) : 0.0f;
      float noise = whiteNoise();
      ksBuffer[i] = (noise * 0.7f + impulse * 0.3f) * env;
    }
    
    // Clear rest of buffer
    for (int i = period; i < KS_BUFFER_SIZE; ++i)
      ksBuffer[i] = 0.0f;
    
    ksWriteIdx = period;
    ksNeedsPluck = false;
    ksLastOut = 0.0f;
    ksLastOut2 = 0.0f;
  }
  
  float delayLength = sampleRate / centerFreq;
  if (delayLength > KS_BUFFER_SIZE - 2) delayLength = KS_BUFFER_SIZE - 2;
  if (delayLength < 2) delayLength = 2;
  
  // Read with cubic interpolation
  float readPos = static_cast<float>(ksWriteIdx) - delayLength;
  while (readPos < 0) readPos += KS_BUFFER_SIZE;
  
  int idx0 = static_cast<int>(readPos) & KS_BUFFER_MASK;
  int idx1 = (idx0 + 1) & KS_BUFFER_MASK;
  int idx_1 = (idx0 - 1 + KS_BUFFER_SIZE) & KS_BUFFER_MASK;
  int idx2 = (idx0 + 2) & KS_BUFFER_MASK;
  
  float frac = readPos - floorf(readPos);
  
  // Catmull-Rom interpolation
  float y_1 = ksBuffer[idx_1];
  float y0 = ksBuffer[idx0];
  float y1 = ksBuffer[idx1];
  float y2 = ksBuffer[idx2];
  
  float c0 = y0;
  float c1 = 0.5f * (y1 - y_1);
  float c2 = y_1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
  float c3 = 0.5f * (y2 - y_1) + 1.5f * (y0 - y1);
  
  float sample = ((c3 * frac + c2) * frac + c1) * frac + c0;
  
  // Two-point averaging filter with controllable damping
  float filterCoef = 0.2f + auxParam * 0.5f;
  float filtered = ksLastOut * filterCoef + sample * (1.0f - filterCoef);
  
  // Additional one-pole for brightness control
  float brightness = 0.3f + auxParam * 0.6f;
  filtered = ksLastOut2 + (filtered - ksLastOut2) * brightness;
  ksLastOut2 = filtered;
  
  ksLastOut = sample;
  
  // Feedback with loss
  float feedback = 0.998f - (1.0f - auxParam) * 0.015f;
  ksBuffer[ksWriteIdx] = filtered * feedback;
  ksWriteIdx = (ksWriteIdx + 1) & KS_BUFFER_MASK;
  
  return filtered * 0.95f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: WHITE NOISE
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeWhite()
{
  float level = 0.4f + auxParam * 0.55f;
  return whiteNoise() * level;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: PINK NOISE
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizePink()
{
  float level = 0.5f + auxParam * 0.5f;
  return pinkNoise() * level;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: S&H NOISE
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeSH()
{
  // Sample rate from center frequency
  float sampleFreq = centerFreq * 0.05f;
  if (sampleFreq < 0.5f) sampleFreq = 0.5f;
  if (sampleFreq > 500.0f) sampleFreq = 500.0f;
  
  shPhase += freqToPhaseInc(sampleFreq);
  
  // Check for phase wrap = new sample
  uint32_t phaseHigh = shPhase >> 24;
  if (phaseHigh < shLastPhaseHigh)
  {
    shTarget = whiteNoise();
  }
  shLastPhaseHigh = phaseHigh;
  
  // Slew rate from aux (0 = instant, 1 = very smooth)
  float slewRate = 0.01f + (1.0f - auxParam) * 0.99f;
  shValue += (shTarget - shValue) * slewRate;
  
  return shValue * 0.95f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: QUANTUM NOISE
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeQuantum()
{
  // Lorenz attractor parameters
  const float sigma = 10.0f;
  const float rho = 28.0f;
  const float beta = 8.0f / 3.0f;
  
  // Time step from speed param
  float dt = 0.00005f + speedParam * 0.0006f;
  
  // Multiple integration steps for stability
  for (int step = 0; step < 2; ++step)
  {
    float dx = sigma * (qy - qx);
    float dy = qx * (rho - qz) - qy;
    float dz = qx * qy - beta * qz;
    
    qx += dx * dt;
    qy += dy * dt;
    qz += dz * dt;
  }
  
  // Soft bounds
  if (qx > 50.0f) qx = 50.0f;
  if (qx < -50.0f) qx = -50.0f;
  if (qy > 50.0f) qy = 50.0f;
  if (qy < -50.0f) qy = -50.0f;
  if (qz > 80.0f) qz = 80.0f;
  if (qz < 0.0f) qz = 0.1f;
  
  // Mix x and y for richer output
  float chaosOut = (qx * 0.7f + qy * 0.3f) / 25.0f;
  
  // Add quantum "uncertainty" based on aux param
  float uncertainty = whiteNoise() * 0.2f * auxParam;
  
  // Smooth output
  quantumOut += (chaosOut + uncertainty - quantumOut) * 0.08f;
  
  return quantumOut;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS: HARMONIC DRONE
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeDrone()
{
  float mix = 0.0f;
  
  for (int i = 0; i < DRONE_HARMONICS; ++i)
  {
    int harmonic = i + 1;
    float freq = centerFreq * harmonic;
    
    if (freq > sampleRate * 0.45f) continue;
    
    // Slow amplitude interpolation
    droneAmps[i] += (droneTargetAmps[i] - droneAmps[i]) * 0.00002f;
    
    dronePhases[i] += freqToPhaseInc(freq);
    
    float baseAmp = 1.0f / (1.0f + 0.3f * static_cast<float>(harmonic));
    mix += readSine(dronePhases[i]) * baseAmp * droneAmps[i];
  }
  
  return mix * 0.16f;
}


/* ═══════════════════════════════════════════════════════════════════════════
                         OUTPUT PROCESSING
   ═══════════════════════════════════════════════════════════════════════════ */

constexpr float LP_B0 = 0.1518f;
constexpr float LP_B1 = 0.3036f;
constexpr float LP_B2 = 0.1518f;
constexpr float LP_A1 = -0.5765f;
constexpr float LP_A2 = 0.1838f;

inline float biquadLP(float in, volatile float& x1, volatile float& x2,
                      volatile float& y1, volatile float& y2)
{
  float out = LP_B0 * in + LP_B1 * x1 + LP_B2 * x2 - LP_A1 * y1 - LP_A2 * y2;
  x2 = x1; x1 = in;
  y2 = y1; y1 = out;
  return out;
}

constexpr float DC_ALPHA = 0.998f;

inline float dcBlock(float in)
{
  float out = in - dcX1 + DC_ALPHA * dcY1;
  dcX1 = in;
  dcY1 = out;
  return out;
}

inline float softSat(float x)
{
  if (x > 3.0f) return 1.0f;
  if (x < -3.0f) return -1.0f;
  float x2 = x * x;
  return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}


/* ═══════════════════════════════════════════════════════════════════════════
                              PWM ISR
   ═══════════════════════════════════════════════════════════════════════════ */

void __isr onPwmWrap()
{
  float sample = 0.0f;
  
  switch (currentMode)
  {
    case 0:  sample = synthesizeModal();   break;
    case 1:  sample = synthesizeKarplus(); break;
    case 2:  sample = synthesizeWhite();   break;
    case 3:  sample = synthesizePink();    break;
    case 4:  sample = synthesizeSH();      break;
    case 5:  sample = synthesizeQuantum(); break;
    case 6:
    default: sample = synthesizeDrone();   break;
  }
  
  sample = dcBlock(sample);
  
  // Less filtering for noise modes
  if (currentMode >= 2 && currentMode <= 5)
  {
    sample = biquadLP(sample, lp1_x1, lp1_x2, lp1_y1, lp1_y2);
  }
  else
  {
    sample = biquadLP(sample, lp1_x1, lp1_x2, lp1_y1, lp1_y2);
    sample = biquadLP(sample, lp2_x1, lp2_x2, lp2_y1, lp2_y2);
  }
  
  sample = softSat(sample);
  
  float output = MID_LEVEL + MID_LEVEL * sample;
  
  if (output < 0.0f) output = 0.0f;
  if (output > FULL_SCALE) output = FULL_SCALE;
  
  pwm_set_chan_level(sliceAudio, PWM_CHAN_B, static_cast<uint16_t>(output + 0.5f));
  pwm_clear_irq(sliceIRQ);
}


/* ═══════════════════════════════════════════════════════════════════════════
                              SETUP
   ═══════════════════════════════════════════════════════════════════════════ */

void setup()
{
  for (int i = 0; i < NUM_MODES_RES; ++i)
  {
    resModes[i].phase = 0.0f;
    resModes[i].velocity = 0.0f;
    resModes[i].amp = 0.0f;
  }
  
  for (int i = 0; i < DRONE_HARMONICS; ++i)
  {
    dronePhases[i] = 0;
    droneAmps[i] = 0.3f + 0.7f * (random(1000) / 1000.0f);
    droneTargetAmps[i] = droneAmps[i];
  }
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(6,  INPUT_PULLUP);
  pinMode(5,  OUTPUT);
  
  pinMode(1, OUTPUT);
  gpio_set_function(1, GPIO_FUNC_PWM);
  sliceAudio = pwm_gpio_to_slice_num(1);
  pwm_set_clkdiv(sliceAudio, 1);
  pwm_set_wrap(sliceAudio, 1023);
  pwm_set_enabled(sliceAudio, true);
  
  gpio_set_function(2, GPIO_FUNC_PWM);
  sliceIRQ = pwm_gpio_to_slice_num(2);
  pwm_clear_irq(sliceIRQ);
  pwm_set_irq_enabled(sliceIRQ, true);
  irq_set_exclusive_handler(PWM_IRQ_WRAP, onPwmWrap);
  irq_set_enabled(PWM_IRQ_WRAP, true);
  pwm_set_clkdiv(sliceIRQ, 1);
  pwm_set_wrap(sliceIRQ, PWM_WRAP_IRQ);
  pwm_set_enabled(sliceIRQ, true);
  
  sampleRate = SYS_CLK / (PWM_WRAP_IRQ + 1);
  sampleRateInv = 1.0f / sampleRate;
  
  initTables();
  
  noiseState = micros() ^ 0xDEADBEEF;
  
  // Initial trigger
  resExcitation = 1.0f;
  
  digitalWrite(5, HIGH);
}


/* ═══════════════════════════════════════════════════════════════════════════
                              MAIN LOOP
   ═══════════════════════════════════════════════════════════════════════════ */

void loop()
{
  static int      lastBtn      = HIGH;
  static uint32_t btnDownTime  = 0;
  static bool     btnHandled   = false;
  
  int btn = digitalRead(6);
  uint32_t now = millis();
  
  if (lastBtn == HIGH && btn == LOW)
  {
    btnDownTime = now;
    btnHandled = false;
  }
  
  // Long press: manual trigger
  if (btn == LOW && !btnHandled && (now - btnDownTime >= LONG_PRESS_MS))
  {
    // Trigger based on mode
    if (currentMode == 0)
    {
      resExcitation = 1.2f;
    }
    else if (currentMode == 1)
    {
      ksNeedsPluck = true;
    }
    btnHandled = true;
    
    digitalWrite(5, HIGH); delay(40);
    digitalWrite(5, LOW);  delay(40);
    digitalWrite(5, HIGH); delay(40);
    digitalWrite(5, LOW);
  }
  
  // Short press: mode change
  if (lastBtn == LOW && btn == HIGH && !btnHandled && (now - btnDownTime >= DEBOUNCE_MS))
  {
    currentMode = (currentMode + 1) % NUM_MODES;
    
    // Reset state for new mode
    if (currentMode == 0) resExcitation = 1.0f;
    if (currentMode == 1) ksNeedsPluck = true;
    
    digitalWrite(5, HIGH); delay(60);
    digitalWrite(5, LOW);
  }
  
  lastBtn = btn;
  
  // Read controls
  float cv = analogRead(A0) / 1023.0f;
  float oct = cv * 5.0f;
  centerFreq = CENTER_MIN_HZ * powf(2.0f, oct);
  centerFreq = fminf(fmaxf(centerFreq, CENTER_MIN_HZ), CENTER_MAX_HZ);
  
  speedParam = analogRead(A1) / 1023.0f;
  auxParam = analogRead(A2) / 1023.0f;
  
  // Auto-trigger for modal and karplus based on speed
  if (currentMode == 0)
  {
    static uint32_t lastExcite = 0;
    uint32_t interval = 3000 - static_cast<uint32_t>(speedParam * 2700);
    if (now - lastExcite > interval)
    {
      resExcitation = 0.7f + auxParam * 0.5f;
      lastExcite = now;
      digitalWrite(5, HIGH);
    }
    else if (now - lastExcite > 50)
    {
      digitalWrite(5, LOW);
    }
  }
  
  if (currentMode == 1)
  {
    static uint32_t lastPluck = 0;
    uint32_t interval = 3500 - static_cast<uint32_t>(speedParam * 3200);
    if (now - lastPluck > interval)
    {
      ksNeedsPluck = true;
      lastPluck = now;
      digitalWrite(5, HIGH);
    }
    else if (now - lastPluck > 50)
    {
      digitalWrite(5, LOW);
    }
  }
  
  // Drone evolution
  if (currentMode == 6)
  {
    static uint32_t lastDrone = 0;
    uint32_t interval = 4000 - static_cast<uint32_t>(speedParam * 3500);
    if (now - lastDrone > interval)
    {
      lastDrone = now;
      for (int i = 0; i < DRONE_HARMONICS; ++i)
        droneTargetAmps[i] = 0.1f + 0.9f * (random(1000) / 1000.0f);
    }
  }
  
  delay(1);
}
