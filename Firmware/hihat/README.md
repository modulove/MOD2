# HIHAT - Blue/White Noise Hi-Hat

Noise-based hi-hat synthesizer with switchable blue/white noise and band-pass filtering.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Decay Base | 0.1-9.1 | Exponential decay base (inverted pot) |
| **POT2 / A1** | Decay Curve | 0.2-5.2 | Exponential decay curvature (inverted pot) |
| **POT3 / CV3** | Filter Frequency | 100 Hz - 16 kHz | Band-pass center frequency (inverted pot) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger for hi-hat |
| **IN2 (GPIO0)** | Accent Input | Digital Input | HIGH = half volume (-6dB attenuation) |
| **LED (GPIO5)** | Envelope LED | PWM Output | LED brightness follows amplitude envelope |
| **BUTTON (GPIO6)** | Trigger / Noise Toggle | Button | Short=trigger, Long=noise type toggle |

### Button Function

- **Short Press (<500ms)**: Manually trigger hi-hat sound
- **Long Press (>500ms)**: Toggle between blue noise (0) and white noise (1)
- 20ms debounce protection

## Features

- **Dual Noise Types**:
  - Blue noise: First-order difference (brighter, rolling sound)
  - White noise: Flat spectrum (classic hi-hat)
  - Switchable via long button press

- **Band-Pass Filtering**:
  - 2-pole filter (cookbook formula)
  - Fixed Q = 0.8
  - Frequency range: 100 Hz to 16 kHz
  - Real-time control via POT3/CV3

- **Flexible Envelope**:
  - Exponential decay with adjustable base and curve
  - Decay base: 0.1-9.1 (inverted control)
  - Decay curve: 0.2-5.2 (inverted control)
  - 2ms fade-in, 1ms fade-out

- **Audio Quality**:
  - ~36.6 kHz sample rate
  - 30,000 sample buffer
  - Visual envelope via LED follower

## Patch Recommendations

### Classic Closed Hi-Hat
- **POT1 (Decay Base)**: 80% (short)
- **POT2 (Decay Curve)**: 70% (sharp decay)
- **POT3 (Frequency)**: 30% (~10 kHz, bright)
- **Noise Type**: White
- **Use**: Standard closed hi-hat

### Open Hi-Hat
- **POT1 (Decay Base)**: 30% (long decay)
- **POT2 (Decay Curve)**: 40% (gradual decay)
- **POT3 (Frequency)**: 35% (~8 kHz)
- **Noise Type**: Blue
- **Use**: Open hi-hat sound

### Metallic Sizzle
- **POT1 (Decay Base)**: 50%
- **POT2 (Decay Curve)**: 60%
- **POT3 (Frequency)**: 20% (~14 kHz, very bright)
- **Noise Type**: Blue
- **Use**: Sizzling cymbals, ride

### Dark Ride
- **POT1 (Decay Base)**: 20% (long)
- **POT2 (Decay Curve)**: 30% (slow decay)
- **POT3 (Frequency)**: 60% (~3 kHz, darker)
- **Noise Type**: White
- **Use**: Ride cymbal, darker tones

### Tight Electronic Hat
- **POT1 (Decay Base)**: 90% (very short)
- **POT2 (Decay Curve)**: 85% (instant decay)
- **POT3 (Frequency)**: 25% (~12 kHz)
- **Noise Type**: White
- **Use**: Electronic drums, techno

### Splash Cymbal
- **POT1 (Decay Base)**: 60%
- **POT2 (Decay Curve)**: 50%
- **POT3 (Frequency)**: 15% (~15 kHz, brilliant)
- **Noise Type**: Blue
- **Use**: Accent hits, cymbal splashes

## Version History

### v1.0 (2025-01-17)
- Initial release
- Switchable blue/white noise generation
- Blue noise via first-order difference
- 2-pole band-pass filter (Q=0.8)
- Frequency range: 100 Hz - 16 kHz
- Dual exponential decay parameters (base + curve)
- Dual-function button (trigger / noise toggle)
- 500ms press duration detection for noise switching
- 20ms debounce protection
- 2ms fade-in, 1ms fade-out
- LED envelope follower
- ~36.6 kHz sample rate
- 30,000 sample buffer
- Accent input for velocity sensitivity

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Buffer Size**: 30,000 samples
- **Filter Type**: 2-pole band-pass (Q=0.8)
- **Noise Types**: Blue (1st-order difference), White (flat spectrum)

## Notes

- All three pots are inverted (clockwise = lower value)
- Blue noise provides brighter, more rolling character
- White noise gives classic, flat spectrum hi-hat
- Long button press (>500ms) toggles noise type
- Accent input provides -6dB attenuation for velocity sensitivity
- LED output can drive external LED for visual envelope indication
- Decay curve and base work together for complex envelope shapes
