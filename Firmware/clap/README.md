# CLAP - TR-808 Style Hand Clap

TR-808 inspired digital hand clap synthesizer with band-pass filtered noise and adjustable decay.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Filter Q | 0.5-4.0 | Band-pass filter resonance (left=wide, right=narrow) |
| **POT2 / A1** | Decay Time | 20-200 ms | Envelope decay duration |
| **POT3 / CV3** | Filter Frequency | 50 Hz - 8 kHz | Band-pass filter center frequency |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger for clap sound |
| **IN2 (GPIO0)** | Accent Input | Digital Input | HIGH = half volume (-6dB attenuation) |
| **LED (GPIO5)** | Envelope LED | PWM Output | LED brightness follows amplitude envelope |
| **BUTTON (GPIO6)** | Manual Trigger | Button | Triggers clap sound manually |

### Button Function

- **Press**: Manually trigger clap sound

## Features

- **Three-Burst Structure**:
  - Two 4ms noise bursts at start (15ms spacing)
  - Exponential decay tail for realistic clap sound
  - Total ~60ms-215ms duration depending on decay setting

- **Filtered Noise Synthesis**:
  - White noise generation
  - 2-pole band-pass filter (cookbook formula)
  - Adjustable center frequency (50 Hz - 8 kHz)
  - Variable Q factor (0.5-4.0)

- **Real-Time Control**:
  - All parameters adjustable during playback
  - Smooth parameter interpolation
  - 22,000 sample buffer (~0.60 seconds maximum)

- **Audio Quality**:
  - ~36.6 kHz sample rate
  - 2ms fade-in, 1ms fade-out for click-free operation
  - Exponential decay envelope with curvature control

- **Visual Feedback**:
  - LED envelope follower (PWM brightness)
  - Shows amplitude envelope in real-time

## Patch Recommendations

### Classic 808 Clap
- **POT1 (Filter Q)**: 60% (moderate resonance)
- **POT2 (Decay)**: 40% (~90ms decay)
- **POT3 (Frequency)**: 50% (~2 kHz center)
- **Use**: Drum patterns, on backbeat

### Tight Clap
- **POT1 (Filter Q)**: 70% (higher resonance)
- **POT2 (Decay)**: 20% (short, ~50ms)
- **POT3 (Frequency)**: 60% (~3 kHz, bright)
- **Use**: Fast patterns, rhythmic fills

### Reverb Clap
- **POT1 (Filter Q)**: 40% (wide filter)
- **POT2 (Decay)**: 80% (long, ~180ms)
- **POT3 (Frequency)**: 45% (~1.5 kHz)
- **Use**: Ambient, large spaces, send to reverb

### Snappy Clap
- **POT1 (Filter Q)**: 80% (narrow, ringing)
- **POT2 (Decay)**: 15% (very short)
- **POT3 (Frequency)**: 75% (~5 kHz, very bright)
- **Use**: Electronic percussion, hi-energy tracks

### Deep Clap
- **POT1 (Filter Q)**: 30% (wide)
- **POT2 (Decay)**: 60% (~140ms)
- **POT3 (Frequency)**: 20% (~500 Hz, dark)
- **Use**: Techno, industrial sounds

## Version History

### v1.0 (2025-01-17)
- Initial release
- TR-808 inspired clap synthesis
- Three-burst noise structure with exponential tail
- 2-pole band-pass filter with adjustable Q and frequency
- Real-time parameter control
- Accent input for velocity sensitivity
- LED envelope follower
- Click-free fade-in/fade-out
- ~36.6 kHz sample rate

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Buffer Size**: 22,000 samples (~0.60 seconds)
- **Filter Type**: 2-pole band-pass (Cookbook formula)
- **Envelope**: Exponential decay

## Notes

- Accent input provides -6dB attenuation when HIGH (useful for velocity sensitivity)
- Three-burst structure is fixed timing (15ms spacing between bursts)
- Parameters can be adjusted in real-time while sound is rendering
- LED output can drive external LED for visual envelope indication
- Fade-in/fade-out prevents clicking at start/end of sound
