# KICK - Six-Parameter Kick Drum

Sine wave based kick drum synthesizer with 6 parameters, dual mode operation, and EEPROM storage.

## Controls

The KICK has two modes, each controlling 3 parameters. Toggle between modes with the button.

### Mode 0 - Character (LED OFF)

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Pitch Multiplier | 0.5-2.0 | Multiplies base pitch envelope |
| **POT2 / A1** | Soft Clip Rate | 0.5-10.0 | Harmonic distortion amount |
| **POT3 / CV3** | Decay Rate | 1-10 | Amplitude envelope decay speed |

### Mode 1 - Pitch Envelope (LED ON)

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Start Frequency | 3-1026 Hz | Initial pitch of kick |
| **POT2 / A1** | End Frequency | 2-513 Hz | Final pitch of kick |
| **POT3 / CV3** | Pitch Curve | 0-31 | Selects from 32 pitch envelope curves |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger for kick |
| **IN2 (GPIO0)** | Accent Input | Digital Input | HIGH = half volume (-6dB attenuation) |
| **LED (GPIO5)** | Mode Indicator | Digital Output | LOW=Mode 0, HIGH=Mode 1 |
| **BUTTON (GPIO6)** | Mode Toggle | Button | Switch between modes, saves parameters |

### Button Function

- **Press**: Toggle between Mode 0 and Mode 1
- **Auto-Save**: All 6 parameters saved to EEPROM on mode change

## Features

- **Sine Wave Synthesis**:
  - Pure sine wave oscillator
  - 2,048 sample wavetable
  - Pitch envelope with 32 curve options

- **Dual Mode Operation**:
  - Mode 0: Sound character (pitch mult, distortion, decay)
  - Mode 1: Pitch envelope (start freq, end freq, curve)
  - LED indicates current mode

- **32 Pitch Envelope Curves**:
  - Curves range from 0.1 to 2.0 curvature
  - Piecewise-linear interpolation (8 segments)
  - Covers linear to highly exponential shapes

- **Soft-Clipping**:
  - Tanh waveshaping with normalization
  - Adjustable clip rate (0.5-10.0)
  - Adds harmonic richness and punch

- **Parameter Pickup**:
  - Prevents value jumping when switching modes
  - 2% threshold for smooth takeover
  - 4-sample pot smoothing

- **EEPROM Storage**:
  - Saves all 6 parameters on mode change
  - Settings persist across power cycles
  - Automatic parameter recall on startup

## Patch Recommendations

### Classic 808 Kick
- **Mode 0**: Pitch Mult=1.0, Clip=2.0, Decay=6
- **Mode 1**: Start=150Hz, End=50Hz, Curve=10
- **Use**: Classic analog kick drum

### Sub Bass Kick
- **Mode 0**: Pitch Mult=0.6, Clip=1.0, Decay=8
- **Mode 1**: Start=80Hz, End=35Hz, Curve=5
- **Use**: Deep bass, club music

### Punchy Techno Kick
- **Mode 0**: Pitch Mult=1.2, Clip=5.0, Decay=4
- **Mode 1**: Start=200Hz, End=60Hz, Curve=15
- **Use**: Hard techno, EDM

### Tight Electronic Kick
- **Mode 0**: Pitch Mult=1.5, Clip=3.0, Decay=3
- **Mode 1**: Start=250Hz, End=80Hz, Curve=20
- **Use**: Minimal, fast sequences

### Boomy Kick
- **Mode 0**: Pitch Mult=0.8, Clip=1.5, Decay=9
- **Mode 1**: Start=100Hz, End=40Hz, Curve=3
- **Use**: Hip-hop, boom-bap

### Industrial Kick
- **Mode 0**: Pitch Mult=1.8, Clip=10.0, Decay=2
- **Mode 1**: Start=400Hz, End=120Hz, Curve=28
- **Use**: Industrial, harsh electronic

## Version History

### v1.0 (2025-01-17)
- Initial release
- Sine wave based kick synthesis
- Dual mode operation (6 total parameters)
- 32 pitch envelope curve LUTs (0.1 to 2.0 curvature)
- Piecewise-linear interpolation (8 segments)
- Tanh soft-clipping with normalization
- EEPROM parameter storage and recall
- Pickup feature prevents parameter jumping (2% threshold)
- 4-sample pot smoothing
- 95% cosine fade-out
- Fixed 0.3 second duration
- ~36.6 kHz sample rate
- Accent input for velocity sensitivity

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Wavetable Size**: 2,048 samples
- **Duration**: Fixed 0.3 seconds
- **Pitch Curves**: 32 exponential curves
- **Interpolation**: 8-segment piecewise-linear

## Notes

- Mode 0 controls sound character (timbre, distortion, decay)
- Mode 1 controls pitch behavior (start/end frequency, envelope shape)
- Pickup feature prevents parameter jumps when switching modes
- Parameters are saved to EEPROM automatically on mode change
- Accent input provides -6dB attenuation for velocity sensitivity
- Pitch envelope sweeps from start frequency to end frequency
- Soft-clipping adds harmonics and punch to the sound
- 32 curves provide wide range from linear to highly exponential pitch sweeps
