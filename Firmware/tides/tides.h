// tides.h - Tides DSP engine for MOD2
// Based on Mutable Instruments Tides2 PolySlopeGenerator

#pragma once

#include <STMLIB.h>
#include <TIDES.h>

// Block size for audio processing
#define BLOCK_SIZE 16

// Mode names for debug output
const char* outputModeNames[] = {"GATES", "AMPLITUDES", "PHASES", "FREQUENCIES"};
const char* rampModeNames[] = {"AD", "LOOPING", "AR"};

// DSP parameters (directly set from controls)
float freq_in = 440.0f;      // Frequency in Hz (default A4)
float shape_in = 0.5f;       // Shape parameter 0-1
float slope_in = 0.5f;       // Slope parameter 0-1
float smooth_in = 0.5f;      // Smoothness parameter 0-1
float trigger_in = 0.0f;     // Trigger input 0-1 (gate for envelope modes)
int output_mode_in = 1;      // Output mode: 0=GATES, 1=AMPLITUDES, 2=PHASES, 3=FREQUENCIES
int ramp_mode_in = 1;        // Ramp mode: 0=AD, 1=LOOPING, 2=AR
int range_in = 1;            // Range: 0=CONTROL (slow LFO), 1=AUDIO (audible)

// Trigger edge detection (like braids)
bool last_trigger = false;

// Smoothed parameters for zipper-free control
float freq_smooth = 440.0f;
float shape_smooth = 0.5f;
float slope_smooth = 0.5f;
float smooth_smooth = 0.5f;

// Smoothing coefficient (lower = smoother, more latency)
const float SMOOTH_COEFF = 0.02f;

// Gate state tracking
stmlib::GateFlags previous_gate_flags = stmlib::GATE_FLAG_LOW;

// Voice structure
struct Voice {
  tides::PolySlopeGenerator poly_slope_generator;
  int16_t buffer[BLOCK_SIZE];  // 16-bit signed for PWMAudio
};

Voice voices[1];

// Initialize voices
void initVoices() {
  voices[0].poly_slope_generator.Init();

  // Clear buffer
  for (int i = 0; i < BLOCK_SIZE; i++) {
    voices[0].buffer[i] = 0;
  }
}

// Update Tides audio - called from main loop when buffer needs refill
void updateTidesAudio() {
  // Smooth parameters to avoid zipper noise
  freq_smooth += SMOOTH_COEFF * (freq_in - freq_smooth);
  shape_smooth += SMOOTH_COEFF * (shape_in - shape_smooth);
  slope_smooth += SMOOTH_COEFF * (slope_in - slope_smooth);
  smooth_smooth += SMOOTH_COEFF * (smooth_in - smooth_smooth);

  // Output buffer
  tides::PolySlopeGenerator::OutputSample out[BLOCK_SIZE];

  // Convert frequency to normalized form (cycles per sample)
  // Tides expects frequency in range 0.0 to 0.25 max
  float normalized_freq = freq_smooth / (float)SAMPLERATE;

  // Clamp to valid range
  if (normalized_freq < 0.0001f) normalized_freq = 0.0001f;
  if (normalized_freq > 0.25f) normalized_freq = 0.25f;

  // Build gate flags array
  stmlib::GateFlags gate_flags[BLOCK_SIZE];
  bool gate_high = (trigger_in > 0.5f);

  for (int i = 0; i < BLOCK_SIZE; i++) {
    gate_flags[i] = stmlib::ExtractGateFlags(previous_gate_flags, gate_high);
    previous_gate_flags = gate_flags[i];
  }

  // Render the poly slope generator
  // Note: shift=0.6 gives maximum output on channel 0 (OUT1)
  // The shift parameter distributes output across 4 channels:
  //   shift ~0.4 or ~0.6 -> channel 0 (OUT1) active
  //   shift ~0.5 -> channels crossfaded, near zero on channel 0!
  voices[0].poly_slope_generator.Render(
    static_cast<tides::RampMode>(ramp_mode_in),
    static_cast<tides::OutputMode>(output_mode_in),
    static_cast<tides::Range>(range_in),
    normalized_freq,
    slope_smooth,        // pw / slope parameter
    shape_smooth,        // shape parameter
    smooth_smooth,       // smoothness parameter
    0.6f,                // shift - set for channel 0 output
    gate_flags,
    nullptr,             // no external ramp input
    out,
    BLOCK_SIZE
  );

  // Convert output to 16-bit signed for PWMAudio
  // Tides LOOPING mode output is approximately ±5 range
  // Use higher gain for louder output
  const float output_scale = 32767.0f / 5.0f;

  for (int i = 0; i < BLOCK_SIZE; i++) {
    // Use channel 0 (main output)
    float sample = out[i].channel[0] * output_scale;

    // Clamp to int16 range
    if (sample > 32767.0f) sample = 32767.0f;
    if (sample < -32768.0f) sample = -32768.0f;

    voices[0].buffer[i] = static_cast<int16_t>(sample);
  }
}
