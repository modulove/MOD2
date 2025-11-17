# FM_DRUM - Two-Operator FM Percussion

Two-operator FM percussion synthesizer with dual mode operation and EEPROM parameter storage.

## Controls

The FM_DRUM has two modes, each controlling 3 parameters. Toggle between modes with the button.

### Mode 0 - Pitch/Ratio/Index (LED OFF)

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Pitch | 30-1200 Hz | Base frequency of carrier oscillator |
| **POT2 / A1** | Operator Ratio | 0.5-8.0 | Modulator frequency ratio (carrier × ratio) |
| **POT3 / CV3** | Modulation Index | 1-10 | FM depth/brightness |

### Mode 1 - Envelope (LED ON)

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Decay Time | 0.5-10 | Exponential decay rate |
| **POT2 / A1** | Ratio Envelope | 0-1 | Modulator ratio envelope depth |
| **POT3 / CV3** | Modulation Index | 1-10 | FM depth/brightness (same as Mode 0) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger for drum hit |
| **IN2 (GPIO0)** | Accent Input | Digital Input | HIGH = half volume (-6dB attenuation) |
| **LED (GPIO5)** | Mode Indicator | Digital Output | OFF=Mode 0, ON=Mode 1 |
| **BUTTON (GPIO6)** | Mode Toggle | Button | Switch between modes, saves parameters |

### Button Function

- **Press**: Toggle between Mode 0 and Mode 1
- **Auto-Save**: All 6 parameters saved to EEPROM on mode change

## Features

- **Two-Operator FM Synthesis**:
  - Carrier + modulator architecture
  - Variable operator ratio (0.5x to 8x)
  - Adjustable modulation index (1-10)

- **Dual Mode Operation**:
  - Mode 0: Sound design (pitch, ratio, index)
  - Mode 1: Envelope shaping (decay, ratio envelope, index)
  - LED indicates current mode

- **Parameter Pickup**:
  - Prevents value jumping when switching modes
  - 5% threshold for smooth takeover
  - 5-sample pot smoothing/averaging

- **EEPROM Storage**:
  - Saves all 6 parameters on mode change
  - Settings persist across power cycles
  - Automatic parameter recall on startup

- **Audio Processing**:
  - 2-pole band-pass filter (cookbook formula)
  - Soft-clipping with tanh() waveshaping
  - 2% cosine fade-in, 10% fade-out
  - Fixed 0.3 second duration
  - 4,096 sample wavetable

## Patch Recommendations

### Classic 808 Tom
- **Mode 0**: Pitch=150Hz, Ratio=1.5, Index=5
- **Mode 1**: Decay=3, Ratio Env=0.6, Index=5
- **Use**: Classic tom sound, tunable

### Metallic Snare
- **Mode 0**: Pitch=200Hz, Ratio=6.2, Index=8
- **Mode 1**: Decay=2, Ratio Env=0.3, Index=8
- **Use**: FM snare with metallic attack

### Punchy Kick
- **Mode 0**: Pitch=60Hz, Ratio=2.0, Index=7
- **Mode 1**: Decay=4, Ratio Env=0.8, Index=7
- **Use**: Electronic kick drum

### Bell Tone
- **Mode 0**: Pitch=400Hz, Ratio=3.5, Index=4
- **Mode 1**: Decay=8, Ratio Env=0.2, Index=4
- **Use**: Tuned percussion, melodic hits

### Industrial Hit
- **Mode 0**: Pitch=800Hz, Ratio=7.8, Index=10
- **Mode 1**: Decay=1.5, Ratio Env=0.5, Index=10
- **Use**: Harsh metallic percussion

### Deep Bass Drum
- **Mode 0**: Pitch=40Hz, Ratio=1.2, Index=6
- **Mode 1**: Decay=5, Ratio Env=0.7, Index=6
- **Use**: Sub bass kick

## Version History

### v1.0 (2025-01-17)
- Initial release
- Two-operator FM synthesis
- Dual mode operation (6 total parameters)
- EEPROM parameter storage and recall
- Pickup feature prevents parameter jumping
- 5-sample pot smoothing
- 2-pole band-pass filter
- Soft-clipping with tanh() waveshaping
- Cosine fade-in/fade-out (2% / 10%)
- Exponential decay envelope
- ~36.6 kHz sample rate
- Accent input for velocity sensitivity

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Wavetable Size**: 4,096 samples
- **Duration**: Fixed 0.3 seconds
- **Filter Type**: 2-pole band-pass
- **Storage**: EEPROM for parameter persistence

## Notes

- Mode 0 controls sound character (pitch, ratio, FM index)
- Mode 1 controls envelope behavior (decay, ratio modulation depth)
- Pickup feature prevents parameter jumps when switching modes
- Parameters are saved to EEPROM automatically on mode change
- Accent input provides -6dB attenuation for velocity sensitivity
- Ratio envelope creates pitch sweep effect on modulator
