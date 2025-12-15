// utility.h - Helper functions for MOD2 Tides

#pragma once

// Random number in range
double randomDouble(double minf, double maxf) {
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);
}

// MIDI note to frequency conversion
static const float a4_frequency = 440.0f;
static const uint32_t a4_midi_note = 69;
static const float semitones_per_octave = 12.0f;

float midi_frequency(uint32_t midi_note) {
  float semitones_away_from_a4 = (float)(midi_note) - (float)(a4_midi_note);
  return powf(2.0f, semitones_away_from_a4 / semitones_per_octave) * a4_frequency;
}

// V/Oct conversion from ADC value to frequency
// Assuming 0-3.3V range with 12-bit ADC (0-4095)
// 1V/octave standard, with 0V = some base note
float voct_to_frequency(uint16_t adc_value, float base_freq = 130.81f) {
  // Convert ADC to voltage (assuming 3.3V reference)
  float voltage = (adc_value / 4095.0f) * 3.3f;
  // 1V per octave
  float octaves = voltage;
  return base_freq * powf(2.0f, octaves);
}
