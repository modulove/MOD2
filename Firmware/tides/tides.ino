/*
  MOD2 Tides Firmware
  Mutable Instruments Tides port for Seeeduino XIAO RP2350

  (c) 2025 GPLv3
  Based on mi_Ugens by Volker Boehm and MI sources (MIT License)

  Hardware:
    POT1 A0  - Shape (waveform shape 0-1)
    POT2 A1  - Slope (waveform slope, falling to rising ramp)
    POT3 A2  - Frequency (20-2000 Hz)
    IN1  D5  - Trigger/Gate input (active high)
    IN2  D0  - Smoothness modulation (digital gate)
    OUT  D7  - PWM audio output (48kHz, 16-bit)
    LED  13  - Feedback LED
    BTN  D4  - Mode button (short: output mode, long: ramp mode)

  Output modes (short press cycles):
    AMPLITUDES - Main waveform output (default, best for audio)
    PHASES     - Phase-shifted outputs
    FREQUENCIES - Frequency ratio outputs
    GATES      - Gate/trigger outputs

  Ramp modes (long press cycles):
    LOOPING - Free-running LFO/oscillator (default, continuous sound)
    AD      - Attack-Decay envelope (needs trigger)
    AR      - Attack-Release envelope (needs gate)
*/

bool debug = true;

#include <Arduino.h>
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include <hardware/pwm.h>
#include <PWMAudio.h>

// Pin definitions for Seeeduino XIAO RP2350 (matching braids.ino)
#define PWMOUT      D7    // PWM audio output (same as braids)
#define TRIG_PIN    D5    // Trigger input
#define MOD_PIN     D6    // Smoothness modulation
#define BUTTON_PIN  D4    // Mode button
#define LED_PIN     D3    // Feedback LED

#define SAMPLERATE  48000

#include "potentiometer.h"
#include "utility.h"

#include <STMLIB.h>
#include <TIDES.h>
#include "tides.h"

#include <Bounce2.h>
Bounce2::Button button = Bounce2::Button();

PWMAudio DAC(PWMOUT);  // 16-bit PWM audio (same as braids)

// Mode cycling state
int currentOutputMode = 1;  // Start with AMPLITUDES (direct waveform output)
int currentRampMode = 1;    // Start with LOOPING
bool longPressHandled = false;

// LED blink state
unsigned long ledBlinkStart = 0;
int ledBlinkCount = 0;
int ledBlinkPhase = 0;
bool ledBlinkActive = false;

// Timer interrupt for audio (same as braids)
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 4

#include "RPi_Pico_TimerInterrupt.h"

#define TIMER0_INTERVAL_MS 20.833333333333  // 48kHz

volatile int counter = 0;

RPI_PICO_Timer ITimer0(0);

bool TimerHandler0(struct repeating_timer *t) {
  (void) t;
  bool sync = true;
  if (DAC.availableForWrite()) {
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
      DAC.write(voices[0].buffer[i], sync);
    }
    counter = 1;
  }
  return true;
}

// Blink LED n times to indicate mode
void startLedBlink(int count) {
  ledBlinkCount = count;
  ledBlinkPhase = 0;
  ledBlinkStart = millis();
  ledBlinkActive = true;
  digitalWrite(LED_PIN, HIGH);
}

// Update LED blink animation
void updateLedBlink() {
  if (!ledBlinkActive) return;

  unsigned long elapsed = millis() - ledBlinkStart;
  int currentPhase = elapsed / 150;  // 150ms per phase (on/off)

  if (currentPhase != ledBlinkPhase) {
    ledBlinkPhase = currentPhase;

    int blinkNum = ledBlinkPhase / 2;
    bool isOn = (ledBlinkPhase % 2) == 0;

    if (blinkNum >= ledBlinkCount) {
      ledBlinkActive = false;
      digitalWrite(LED_PIN, LOW);
    } else {
      digitalWrite(LED_PIN, isOn ? HIGH : LOW);
    }
  }
}

void setup() {
  if (debug) {
    Serial.begin(57600);
    Serial.println(F("=== MOD2 TIDES FIRMWARE ==="));
    Serial.print(F("Output mode: "));
    Serial.println(outputModeNames[currentOutputMode]);
    Serial.print(F("Ramp mode: "));
    Serial.println(rampModeNames[currentRampMode]);
  }

  // ADC resolution
  analogReadResolution(12);

  // Pin setup
  pinMode(TRIG_PIN, INPUT_PULLDOWN);
  pinMode(MOD_PIN, INPUT);
  pinMode(AIN0, INPUT);
  pinMode(AIN1, INPUT);
  pinMode(AIN2, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Button setup with Bounce2
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.interval(5);
  button.setPressedState(LOW);

  // Timer interrupt for audio (same as braids)
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS, TimerHandler0)) {
    if (debug) {
      Serial.print(F("Timer started, millis() = "));
      Serial.println(millis());
    }
  } else {
    if (debug) Serial.println(F("Timer setup failed!"));
  }

  // PWM audio setup (same as braids)
  DAC.setBuffers(4, 32);
  DAC.setFrequency(SAMPLERATE);
  DAC.begin();

  // Initialize Tides DSP
  initVoices();

  // Set initial modes
  output_mode_in = currentOutputMode;
  ramp_mode_in = currentRampMode;
  range_in = 1;  // AUDIO range for audible output

  // Initial pot reading
  readpot(0);
  readpot(1);
  readpot(2);

  // Map initial pot values to parameters
  shape_in = potvalue[0] / 4095.0f;
  slope_in = potvalue[1] / 4095.0f;
  // Map pot to frequency range 20-2000 Hz
  freq_in = 20.0f + (potvalue[2] / 4095.0f) * 1980.0f;

  if (debug) {
    Serial.println(F("Setup complete"));
  }
}

void loop() {
  // Core 0: Audio processing (same pattern as braids)
  if (counter > 0) {
    updateTidesAudio();
    counter = 0;
  }

  // Debug: print audio buffer stats periodically
  static unsigned long lastDebugPrint = 0;
  if (debug && (millis() - lastDebugPrint > 2000)) {
    int16_t minVal = 32767, maxVal = -32768;
    for (int i = 0; i < BLOCK_SIZE; i++) {
      if (voices[0].buffer[i] < minVal) minVal = voices[0].buffer[i];
      if (voices[0].buffer[i] > maxVal) maxVal = voices[0].buffer[i];
    }
    Serial.print(F("Audio buffer min/max: "));
    Serial.print(minVal);
    Serial.print(F(" / "));
    Serial.print(maxVal);
    Serial.print(F("  freq_in: "));
    Serial.print(freq_in);
    Serial.print(F("  shape: "));
    Serial.print(shape_in);
    Serial.print(F("  slope: "));
    Serial.println(slope_in);
    lastDebugPrint = millis();
  }
}

// Core 1 setup
void setup1() {
  delay(200);  // Wait for core 0 to initialize
  if (debug) Serial.println(F("Core 1 started"));
}

// Core 1: Control rate updates (pots, buttons, LED)
void loop1() {
  uint32_t now = millis();
  static uint32_t lastTrigState = 0;

  // Update button state
  button.update();

  // Handle button (same pattern as braids):
  // - Long press (>500ms): cycle ramp mode (backwards)
  // - Short press (release): cycle output mode (forwards)

  if (button.isPressed() && button.currentDuration() > 500) {
    if (!longPressHandled) {
      // Long press: cycle ramp mode backwards
      currentRampMode--;
      if (currentRampMode < 0) currentRampMode = 2;
      ramp_mode_in = currentRampMode;
      longPressHandled = true;

      // Blink LED to indicate ramp mode
      startLedBlink(currentRampMode + 1);

      if (debug) {
        Serial.print(F("Ramp mode: "));
        Serial.println(rampModeNames[currentRampMode]);
      }
    }
  } else if (button.released()) {
    // Short press: cycle output mode forwards
    if (!longPressHandled) {
      currentOutputMode = (currentOutputMode + 1) % 4;
      output_mode_in = currentOutputMode;

      // Blink LED to indicate output mode
      startLedBlink(currentOutputMode + 1);

      if (debug) {
        Serial.print(F("Output mode: "));
        Serial.println(outputModeNames[currentOutputMode]);
      }
    }
    longPressHandled = false;
  }

  // Read trigger input (IN1)
  bool trigState = digitalRead(TRIG_PIN);
  if (trigState && !lastTrigState) {
    // Rising edge trigger
    trigger_in = 1.0f;
    if (!ledBlinkActive) digitalWrite(LED_PIN, HIGH);
  } else if (!trigState && lastTrigState) {
    // Falling edge
    trigger_in = 0.0f;
    if (!ledBlinkActive) digitalWrite(LED_PIN, LOW);
  }
  lastTrigState = trigState;

  // Clear manual trigger after a short time if no external trigger
  static unsigned long manualTrigTime = 0;
  if (trigger_in > 0.5f && !trigState) {
    if (manualTrigTime == 0) {
      manualTrigTime = now;
    } else if (now - manualTrigTime > 50) {
      trigger_in = 0.0f;
      if (!ledBlinkActive) digitalWrite(LED_PIN, LOW);
      manualTrigTime = 0;
    }
  } else {
    manualTrigTime = 0;
  }

  // Read pots at controlled rate
  if ((now - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    readpot(2);

    // Map pot values to Tides parameters
    // POT1 (A0): Shape
    shape_in = potvalue[0] / 4095.0f;

    // POT2 (A1): Slope
    slope_in = potvalue[1] / 4095.0f;

    // POT3 (A2): Frequency (20-2000 Hz range)
    freq_in = 20.0f + (potvalue[2] / 4095.0f) * 1980.0f;

    // IN2 (D0) for smoothness modulation
    // Note: D0 is digital-only (not ADC capable), so use as gate
    // HIGH = smooth (1.0), LOW = sharp (0.0)
    smooth_in = digitalRead(MOD_PIN) ? 0.8f : 0.2f;

    pot_timer = now;
  }

  // Update LED blink animation
  updateLedBlink();

  // Debug: verify core 1 is running
  static unsigned long lastCore1Debug = 0;
  if (debug && (now - lastCore1Debug > 3000)) {
    Serial.print(F("Core1: btn_bounce="));
    Serial.print(button.isPressed() ? "PRESSED" : "released");
    Serial.print(F(" btn_raw="));
    Serial.print(digitalRead(BUTTON_PIN));  // Raw pin state (1=HIGH, 0=LOW)
    Serial.print(F(" pot0="));
    Serial.print(potvalue[0]);
    Serial.print(F(" pot1="));
    Serial.print(potvalue[1]);
    Serial.print(F(" pot2="));
    Serial.println(potvalue[2]);
    lastCore1Debug = now;
  }
}