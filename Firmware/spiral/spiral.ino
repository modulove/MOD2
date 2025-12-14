/*
HAGIWO MOD2 – SPIRAL 4EVER  v1.0
Auditory Illusions & Impossible Sounds

Named after the SPIRAL 4EVER  stairs – the visual equivalent of Shepard tones.

═══════════════════════════════════════════════════════════════════════════════
                              MODES (9 Total)
═══════════════════════════════════════════════════════════════════════════════

  0 – SHEPARD RISING        Infinitely ascending tone
  1 – SHEPARD FALLING       Infinitely descending tone
  2 – BARBER POLE           Enhanced stripe envelope + AM
  3 – RISSET RHYTHM         Infinitely accelerating beats
  4 – TRITONE PARADOX       Ambiguous interval
  5 – TRITONE EXPLORER      Pitch class experiment
  6 – SHEPARD CLUSTER MAJ   Rising/falling major triad
  7 – SHEPARD CLUSTER MIN   Rising/falling minor triad
  8 – EULER SPIRAL          Circular pitch with spectral rotation

═══════════════════════════════════════════════════════════════════════════════
                              CONTROLS
═══════════════════════════════════════════════════════════════════════════════

  POT1 (A0)    Center frequency / CV input (V/Oct)
  POT2 (A1)    Sweep speed
  POT3 (A2)    Envelope width / Pitch class (mode 5) / Spiral spread (mode 8)
  
  BUTTON       SHORT: Cycle modes | LONG (>500ms): Toggle direction
  LED          Solid = direction up

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

constexpr int   NUM_PARTIALS    = 12;
constexpr float SHEPARD_BASE_HZ = 20.0f;

constexpr float MAJOR_THIRD     = 4.0f / 12.0f;
constexpr float MINOR_THIRD     = 3.0f / 12.0f;
constexpr float PERFECT_FIFTH   = 7.0f / 12.0f;

constexpr float CENTER_MIN_HZ   = 32.0f;
constexpr float CENTER_MAX_HZ   = 2000.0f;

constexpr int   RISSET_LAYERS   = 8;

constexpr uint32_t LONG_PRESS_MS = 500;
constexpr uint32_t DEBOUNCE_MS   = 50;

constexpr int   NUM_MODES       = 9;


/* ═══════════════════════════════════════════════════════════════════════════
                              WAVETABLES
   ═══════════════════════════════════════════════════════════════════════════ */

float tableSin[TABLE_SIZE];
float tableSinDeriv[TABLE_SIZE];
float tableTriangle[TABLE_SIZE];


/* ═══════════════════════════════════════════════════════════════════════════
                              GLOBAL STATE
   ═══════════════════════════════════════════════════════════════════════════ */

uint sliceAudio;
uint sliceIRQ;
float sampleRate    = 0.0f;
float sampleRateInv = 0.0f;

volatile uint8_t currentMode = 0;
volatile bool    directionUp = true;
volatile float   speedParam  = 0.5f;
volatile float   centerFreq  = 440.0f;
volatile float   auxParam    = 0.5f;
volatile float   envelopeWidth = 1.8f;

volatile float sweepPosition = 0.0f;

struct PartialState {
  uint32_t phase;
  float    amplitude;
};

volatile PartialState partials[NUM_PARTIALS];
volatile PartialState tritonePartials[NUM_PARTIALS * 2];
volatile PartialState clusterPartials[3 * NUM_PARTIALS];
volatile PartialState eulerPartials[NUM_PARTIALS];

volatile uint32_t rissetPhases[RISSET_LAYERS];
volatile float    rissetAmps[RISSET_LAYERS];

volatile float barberAMPhase = 0.0f;
volatile int   tritoneNoteClass = 0;
volatile float eulerRotation = 0.0f;
volatile float eulerSpread = 1.0f;

volatile float lp1_x1 = 0, lp1_x2 = 0, lp1_y1 = 0, lp1_y2 = 0;
volatile float lp2_x1 = 0, lp2_x2 = 0, lp2_y1 = 0, lp2_y2 = 0;
volatile float dcX1 = 0, dcY1 = 0;


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
    
    float tri = 0.0f;
    for (int h = 0; h < 8; ++h)
    {
      int n = 2 * h + 1;
      float sign = (h & 1) ? -1.0f : 1.0f;
      tri += sign * sinf(angle * n) / static_cast<float>(n * n);
    }
    tableTriangle[i] = tri * (8.0f / (PI * PI));
  }
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

inline float linearInterp(const float* table, uint32_t phase)
{
  int idx = phase >> PHASE_FRAC_BITS;
  float frac = static_cast<float>(phase & PHASE_FRAC_MASK) / PHASE_SCALE;
  int idx1 = (idx + 1) & TABLE_MASK;
  return table[idx] + frac * (table[idx1] - table[idx]);
}

inline float readSine(uint32_t phase)
{
  return hermiteInterp(tableSin, tableSinDeriv, phase);
}


/* ═══════════════════════════════════════════════════════════════════════════
                         HELPERS
   ═══════════════════════════════════════════════════════════════════════════ */

inline uint32_t freqToPhaseInc(float freq)
{
  return static_cast<uint32_t>(freq * PHASE_SCALE * static_cast<float>(TABLE_SIZE) * sampleRateInv);
}

inline float cosSquaredEnvelope(float distance, float width)
{
  float x = fabsf(distance) / width;
  if (x >= 1.0f) return 0.0f;
  float c = cosf(x * PI * 0.5f);
  return c * c;
}

inline float shepardAmplitude(float freq, float center, float width)
{
  if (freq < 18.0f || freq > 20000.0f) return 0.0f;
  float octaveDistance = log2f(freq / center);
  return cosSquaredEnvelope(octaveDistance, width);
}

constexpr float AMP_SMOOTH = 0.003f;

inline float smoothAmplitude(float current, float target)
{
  return current + (target - current) * AMP_SMOOTH;
}

inline float getPartialFrequency(int idx, float sweep, bool rising)
{
  float octavePos;
  if (rising)
    octavePos = static_cast<float>(idx) + sweep;
  else
    octavePos = static_cast<float>(idx) + (1.0f - sweep);
  
  octavePos = fmodf(octavePos, static_cast<float>(NUM_PARTIALS));
  if (octavePos < 0.0f) octavePos += NUM_PARTIALS;
  
  return SHEPARD_BASE_HZ * powf(2.0f, octavePos);
}


/* ═══════════════════════════════════════════════════════════════════════════
                         SYNTHESIS FUNCTIONS
   ═══════════════════════════════════════════════════════════════════════════ */

float synthesizeShepard(bool rising)
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  for (int i = 0; i < NUM_PARTIALS; ++i)
  {
    float freq = getPartialFrequency(i, sweepPosition, rising);
    float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth);
    
    partials[i].amplitude = smoothAmplitude(partials[i].amplitude, targetAmp);
    float amp = partials[i].amplitude;
    
    if (amp < 0.0005f) continue;
    
    partials[i].phase += freqToPhaseInc(freq);
    mix += readSine(partials[i].phase) * amp;
    totalAmp += amp;
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.92f;
}


float synthesizeBarberPole(bool rising)
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  float amNorm = barberAMPhase / (2.0f * PI);
  
  for (int i = 0; i < NUM_PARTIALS; ++i)
  {
    float freq = getPartialFrequency(i, sweepPosition, rising);
    float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth * 0.65f);
    
    partials[i].amplitude = smoothAmplitude(partials[i].amplitude, targetAmp);
    float amp = partials[i].amplitude;
    
    if (amp < 0.0005f) continue;
    
    partials[i].phase += freqToPhaseInc(freq);
    
    float partialOffset = static_cast<float>(i) / NUM_PARTIALS;
    float am = 0.75f + 0.25f * sinf(2.0f * PI * (amNorm + partialOffset));
    
    mix += readSine(partials[i].phase) * amp * am;
    totalAmp += amp;
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.92f;
}


float synthesizeRisset(bool accelerating)
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  float tempoMult = 0.3f + auxParam * 2.0f;
  float baseBPM = 72.0f * tempoMult;
  
  for (int i = 0; i < RISSET_LAYERS; ++i)
  {
    float layerRatio = powf(2.0f, static_cast<float>(i));
    float sweepMod = accelerating ? powf(2.0f, sweepPosition) : powf(2.0f, 1.0f - sweepPosition);
    
    float pulseFreq = (baseBPM / 60.0f) * layerRatio * sweepMod;
    
    float octaveFromCenter = log2f(layerRatio * sweepMod) - (RISSET_LAYERS / 2.0f);
    float targetAmp = cosSquaredEnvelope(octaveFromCenter, envelopeWidth);
    rissetAmps[i] = smoothAmplitude(rissetAmps[i], targetAmp);
    float amp = rissetAmps[i];
    
    if (amp < 0.001f) continue;
    
    rissetPhases[i] += freqToPhaseInc(pulseFreq);
    
    float phase01 = static_cast<float>(rissetPhases[i] >> PHASE_FRAC_BITS) / static_cast<float>(TABLE_SIZE);
    phase01 = fmodf(phase01, 1.0f);
    
    float pulse;
    if (phase01 < 0.12f)
    {
      float t = phase01 / 0.12f;
      pulse = 0.5f + 0.5f * cosf(PI * t);
    }
    else
    {
      pulse = 0.0f;
    }
    
    mix += pulse * amp;
    totalAmp += amp;
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 2.0f - 1.0f;
}


float synthesizeTritone()
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  for (int set = 0; set < 2; ++set)
  {
    float tritoneOffset = (set == 0) ? 0.0f : 0.5f;
    
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
      float sweep = fmodf(sweepPosition + tritoneOffset, 1.0f);
      float freq = getPartialFrequency(i, sweep, true);
      
      int idx = set * NUM_PARTIALS + i;
      float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth);
      tritonePartials[idx].amplitude = smoothAmplitude(tritonePartials[idx].amplitude, targetAmp);
      float amp = tritonePartials[idx].amplitude;
      
      if (amp < 0.0005f) continue;
      
      tritonePartials[idx].phase += freqToPhaseInc(freq);
      mix += readSine(tritonePartials[idx].phase) * amp;
      totalAmp += amp;
    }
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.88f;
}


float synthesizeTritoneExplorer()
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  float pitchOffset = static_cast<float>(tritoneNoteClass) / 12.0f;
  
  for (int set = 0; set < 2; ++set)
  {
    float offset = (set == 0) ? pitchOffset : fmodf(pitchOffset + 0.5f, 1.0f);
    
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
      float octavePos = fmodf(static_cast<float>(i) + offset, static_cast<float>(NUM_PARTIALS));
      float freq = SHEPARD_BASE_HZ * powf(2.0f, octavePos);
      
      int idx = set * NUM_PARTIALS + i;
      float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth);
      tritonePartials[idx].amplitude = smoothAmplitude(tritonePartials[idx].amplitude, targetAmp);
      float amp = tritonePartials[idx].amplitude;
      
      if (amp < 0.0005f) continue;
      
      tritonePartials[idx].phase += freqToPhaseInc(freq);
      mix += readSine(tritonePartials[idx].phase) * amp;
      totalAmp += amp;
    }
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.88f;
}


float synthesizeCluster(bool rising, bool minor)
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  float intervals[3] = {0.0f, minor ? MINOR_THIRD : MAJOR_THIRD, PERFECT_FIFTH};
  float voiceGains[3] = {1.0f, 0.72f, 0.6f};
  
  for (int voice = 0; voice < 3; ++voice)
  {
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
      float octavePos;
      if (rising)
        octavePos = static_cast<float>(i) + sweepPosition + intervals[voice];
      else
        octavePos = static_cast<float>(i) + (1.0f - sweepPosition) + intervals[voice];
      
      octavePos = fmodf(octavePos, static_cast<float>(NUM_PARTIALS));
      if (octavePos < 0.0f) octavePos += NUM_PARTIALS;
      
      float freq = SHEPARD_BASE_HZ * powf(2.0f, octavePos);
      
      int idx = voice * NUM_PARTIALS + i;
      float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth * 0.8f) * voiceGains[voice];
      clusterPartials[idx].amplitude = smoothAmplitude(clusterPartials[idx].amplitude, targetAmp);
      float amp = clusterPartials[idx].amplitude;
      
      if (amp < 0.0005f) continue;
      
      clusterPartials[idx].phase += freqToPhaseInc(freq);
      mix += readSine(clusterPartials[idx].phase) * amp;
      totalAmp += amp;
    }
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.88f;
}


float synthesizeEulerSpiral()
{
  float mix = 0.0f;
  float totalAmp = 0.0f;
  
  for (int i = 0; i < NUM_PARTIALS; ++i)
  {
    float angle = eulerRotation + static_cast<float>(i) * PI * 0.25f;
    float radius = 0.5f + static_cast<float>(i) * 0.15f * eulerSpread;
    
    float octaveOffset = radius * cosf(angle);
    float harmonicMix = 0.5f + 0.5f * sinf(angle);
    
    float baseOctave = static_cast<float>(i) + sweepPosition;
    baseOctave = fmodf(baseOctave + octaveOffset * 0.5f, static_cast<float>(NUM_PARTIALS));
    if (baseOctave < 0) baseOctave += NUM_PARTIALS;
    
    float freq = SHEPARD_BASE_HZ * powf(2.0f, baseOctave);
    
    float targetAmp = shepardAmplitude(freq, centerFreq, envelopeWidth);
    eulerPartials[i].amplitude = smoothAmplitude(eulerPartials[i].amplitude, targetAmp);
    float amp = eulerPartials[i].amplitude;
    
    if (amp < 0.0005f) continue;
    
    eulerPartials[i].phase += freqToPhaseInc(freq);
    
    float sine = readSine(eulerPartials[i].phase);
    float tri = linearInterp(tableTriangle, eulerPartials[i].phase);
    float wave = sine * (1.0f - harmonicMix * 0.4f) + tri * harmonicMix * 0.4f;
    
    mix += wave * amp;
    totalAmp += amp;
  }
  
  if (totalAmp > 0.01f) mix /= totalAmp;
  return mix * 0.9f;
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
    case 0:  sample = synthesizeShepard(true);              break;
    case 1:  sample = synthesizeShepard(false);             break;
    case 2:  sample = synthesizeBarberPole(directionUp);    break;
    case 3:  sample = synthesizeRisset(directionUp);        break;
    case 4:  sample = synthesizeTritone();                  break;
    case 5:  sample = synthesizeTritoneExplorer();          break;
    case 6:  sample = synthesizeCluster(directionUp, false); break;
    case 7:  sample = synthesizeCluster(directionUp, true);  break;
    case 8:
    default: sample = synthesizeEulerSpiral();              break;
  }
  
  sample = dcBlock(sample);
  sample = biquadLP(sample, lp1_x1, lp1_x2, lp1_y1, lp1_y2);
  sample = biquadLP(sample, lp2_x1, lp2_x2, lp2_y1, lp2_y2);
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
  for (int i = 0; i < NUM_PARTIALS; ++i)
  {
    partials[i].phase = 0;
    partials[i].amplitude = 0.0f;
    eulerPartials[i].phase = 0;
    eulerPartials[i].amplitude = 0.0f;
  }
  
  for (int i = 0; i < NUM_PARTIALS * 2; ++i)
  {
    tritonePartials[i].phase = 0;
    tritonePartials[i].amplitude = 0.0f;
  }
  
  for (int i = 0; i < 3 * NUM_PARTIALS; ++i)
  {
    clusterPartials[i].phase = 0;
    clusterPartials[i].amplitude = 0.0f;
  }
  
  for (int i = 0; i < RISSET_LAYERS; ++i)
  {
    rissetPhases[i] = 0;
    rissetAmps[i] = 0.0f;
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
  
  if (btn == LOW && !btnHandled && (now - btnDownTime >= LONG_PRESS_MS))
  {
    directionUp = !directionUp;
    btnHandled = true;
    
    digitalWrite(5, HIGH); delay(40);
    digitalWrite(5, LOW);  delay(40);
    digitalWrite(5, HIGH); delay(40);
    digitalWrite(5, LOW);
  }
  
  if (lastBtn == LOW && btn == HIGH && !btnHandled && (now - btnDownTime >= DEBOUNCE_MS))
  {
    currentMode = (currentMode + 1) % NUM_MODES;
    
    digitalWrite(5, HIGH); delay(60);
    digitalWrite(5, LOW);
  }
  
  lastBtn = btn;
  
  static uint32_t lastLed = 0;
  if (now - lastLed > 150)
  {
    digitalWrite(5, directionUp ? HIGH : LOW);
    lastLed = now;
  }
  
  // Read controls
  float cv = analogRead(A0) / 1023.0f;
  float oct = cv * 5.0f;
  centerFreq = CENTER_MIN_HZ * powf(2.0f, oct);
  centerFreq = fminf(fmaxf(centerFreq, CENTER_MIN_HZ), CENTER_MAX_HZ);
  
  speedParam = analogRead(A1) / 1023.0f;
  auxParam = analogRead(A2) / 1023.0f;
  
  switch (currentMode)
  {
    case 0: case 1: case 2: case 4: case 6: case 7:
      envelopeWidth = 1.0f + auxParam * 2.0f;
      break;
    case 5:
      tritoneNoteClass = static_cast<int>(auxParam * 11.99f);
      envelopeWidth = 1.8f;
      break;
    case 8:
      eulerSpread = 0.5f + auxParam * 1.5f;
      envelopeWidth = 1.5f + auxParam * 0.8f;
      break;
    default:
      envelopeWidth = 1.8f;
      break;
  }
  
  // Update sweeps
  float cyclesPerSec = 0.008f * powf(120.0f, speedParam);
  float increment = cyclesPerSec * 0.001f;
  
  sweepPosition += increment;
  if (sweepPosition >= 1.0f) sweepPosition -= 1.0f;
  
  if (currentMode == 2)
  {
    barberAMPhase += increment * 2.0f * PI * 0.35f;
    if (barberAMPhase >= 2.0f * PI) barberAMPhase -= 2.0f * PI;
  }
  
  if (currentMode == 8)
  {
    eulerRotation += increment * 2.0f * PI * 0.2f;
    if (eulerRotation >= 2.0f * PI) eulerRotation -= 2.0f * PI;
  }
  
  delay(1);
}
