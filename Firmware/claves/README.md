# CLAVES - Simple Percussion Voice

Tunable claves/woodblock percussion synthesizer with morphable waveform and 1V/Oct pitch tracking.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Decay Time | 1-10 | Envelope decay rate (exponential) |
| **POT2 / A1** | Waveform | 0.0-1.0 | Morph between sine (0) and triangle (1) |
| **POT3 / CV3** | Pitch CV | 0-5V | 1V/Oct pitch control (inverted, 16-sample averaging) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger for claves hit |
| **LED (GPIO5)** | Envelope LED | PWM Output | LED brightness follows amplitude envelope |
| **BUTTON (GPIO6)** | Manual Trigger | Button | Triggers claves sound manually |
| **IN2 (GPIO0)** | Not Used | - | Reserved for future use |

### Button Function

- **Press**: Manually trigger claves sound

## Features

- **Morphable Waveform**:
  - Continuously variable between sine and triangle waves
  - 64-point lookup tables with linear interpolation
  - Smooth timbral transitions

- **1V/Oct Pitch Tracking**:
  - Frequency range: ~50 Hz to 1500 Hz
  - 16-sample ADC averaging for stable tuning
  - Inverted CV input (higher voltage = lower pitch)

- **Exponential Envelope**:
  - Adjustable decay (1-10 range)
  - 95% cosine fade-out at tail prevents clicks
  - Visual envelope following via LED

- **Audio Quality**:
  - ~36.6 kHz sample rate
  - Linear interpolation between waveform samples
  - 8,192 sample buffer

## Patch Recommendations

### Classic Claves
- **POT1 (Decay)**: 30% (short, punchy)
- **POT2 (Waveform)**: 80% (mostly triangle, bright)
- **POT3 (Pitch)**: Tune to ~800 Hz
- **Use**: Latin percussion, rhythmic patterns

### Woodblock
- **POT1 (Decay)**: 20% (very short)
- **POT2 (Waveform)**: 90% (triangle, hard attack)
- **POT3 (Pitch)**: Tune to ~1200 Hz (higher pitch)
- **Use**: Electronic percussion, sequenced patterns

### Melodic Percussion
- **POT1 (Decay)**: 50% (medium decay)
- **POT2 (Waveform)**: 40% (more sine, rounded)
- **POT3 (Pitch)**: Use 1V/Oct CV from sequencer
- **Use**: Melodic lines, tuned percussion

### Deep Tom
- **POT1 (Decay)**: 70% (longer decay)
- **POT2 (Waveform)**: 20% (mostly sine, soft)
- **POT3 (Pitch)**: Tune to ~100-200 Hz (low)
- **Use**: Low percussion, tom-like sounds

### Rim Shot
- **POT1 (Decay)**: 15% (ultra-short)
- **POT2 (Waveform)**: 100% (full triangle, sharp)
- **POT3 (Pitch)**: Tune to ~1500 Hz (very high)
- **Use**: Rim shots, metallic hits

## Version History

### v1.0 (2025-01-17)
- Initial release
- Morphable sine/triangle waveform oscillator
- 1V/Oct pitch tracking with CV input
- Exponential decay envelope (1-10 range)
- 16-sample ADC averaging for stable pitch
- 64-point waveform LUTs with linear interpolation
- LED envelope follower
- 95% cosine fade-out for click-free tail
- ~36.6 kHz sample rate
- 8,192 sample buffer

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Buffer Size**: 8,192 samples
- **Waveform Resolution**: 64-point LUTs
- **Pitch Range**: ~50 Hz to 1500 Hz
- **CV Input**: 1V/Oct (inverted)

## Notes

- CV input is inverted: higher voltage = lower pitch
- 16-sample averaging on pitch CV provides stable tuning
- Waveform morph is continuous and real-time
- Cosine fade-out at 95% of envelope prevents clicks
- LED output can drive external LED for visual envelope indication
- Linear interpolation ensures smooth waveform playback
