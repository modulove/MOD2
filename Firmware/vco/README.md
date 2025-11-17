# VCO - Six-Waveform Voltage Controlled Oscillator

Six-waveform VCO with 1V/Oct tracking, PolyBLEP anti-aliasing, and octave shifting.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Waveform Select | 6 waveforms | 0=Sine, 1=Triangle, 2=Square, 3=Saw, 4=FM-4x, 5=FM-2x |
| **POT2 / A1** | Coarse Tune | 320-410 Hz | Base frequency adjustment |
| **POT3 / CV3** | 1V/Oct CV | -8.3 octaves | Pitch CV input (inverted, negative slope) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **BUTTON (GPIO6)** | Octave Shift | Button | Cycles through 0, +1, +2, +3 octaves |
| **IN1 (GPIO7)** | Not Used | - | Reserved for future use |
| **IN2 (GPIO0)** | Not Used | - | Reserved for future use |
| **LED (GPIO5)** | Not Used | - | Reserved for future use |

### Button Function

- **Press**: Cycle through octave offsets: 0, +1, +2, +3 (modulo-4)
- Allows quick pitch range changes

## Features

- **Six Waveforms**:
  - **Sine**: Pure sine wave (smooth, fundamental only)
  - **Triangle**: Triangle wave (odd harmonics)
  - **Square**: PolyBLEP anti-aliased square wave (50% duty cycle)
  - **Saw**: PolyBLEP anti-aliased sawtooth wave
  - **FM-4x**: FM synthesis with 4x modulator ratio
  - **FM-2x**: FM synthesis with 2x modulator ratio

- **PolyBLEP Anti-Aliasing**:
  - Reduces aliasing artifacts on square and saw waves
  - Polynomial bandwidth-limited step function
  - Cleaner sound at high frequencies

- **1V/Oct Tracking**:
  - Standard 1V/Oct CV input
  - Inverted (negative slope): higher voltage = lower pitch
  - -8.3 octave range from CV
  - TUNE_CAL constant (0.992) for calibration

- **Octave Shifting**:
  - Button cycles through 0, +1, +2, +3 octaves
  - Quick range changes without adjusting CV

- **Audio Filtering**:
  - 1-pole RC low-pass filter (alpha=0.18)
  - Softens PWM and aliasing noise
  - Cleaner output signal

## Waveform Details

| Waveform | Harmonic Content | Character | Use Case |
|----------|------------------|-----------|----------|
| **Sine** | Fundamental only | Pure, soft | Sub bass, FM modulator |
| **Triangle** | Odd harmonics (falling) | Mellow, hollow | Leads, pads |
| **Square** | Odd harmonics (strong) | Hollow, clarinet-like | Bass, leads |
| **Saw** | All harmonics | Bright, rich | Classic analog leads |
| **FM-4x** | Inharmonic sidebands | Metallic, bell-like | FM tones, bells |
| **FM-2x** | Inharmonic sidebands | Warm FM | FM bass, keys |

## Patch Recommendations

### Sub Bass
- **Waveform**: Sine (0)
- **Coarse Tune**: Low (~320 Hz)
- **Octave**: 0
- **Use**: Deep sub bass, minimal techno

### Classic Lead
- **Waveform**: Saw (3)
- **Coarse Tune**: Mid (~365 Hz)
- **Octave**: +1 or +2
- **CV**: Melodic sequence
- **Use**: Classic analog lead sound

### Warm Pad
- **Waveform**: Triangle (1)
- **Coarse Tune**: Mid (~350 Hz)
- **Octave**: +1
- **Use**: Soft pads, ambient

### Bass Synth
- **Waveform**: Square (2)
- **Coarse Tune**: Low (~320 Hz)
- **Octave**: 0
- **Use**: Bass lines, techno

### FM Bell
- **Waveform**: FM-4x (4)
- **Coarse Tune**: High (~400 Hz)
- **Octave**: +2
- **Use**: Bell tones, metallic hits

### FM Bass
- **Waveform**: FM-2x (5)
- **Coarse Tune**: Low (~330 Hz)
- **Octave**: 0
- **Use**: FM bass, electric piano

## Frequency Formula

```
f_out = baseFreq × 2^(octShift + cvOct) × TUNE_CAL
```

Where:
- `baseFreq` = POT2 value (320-410 Hz)
- `octShift` = button selection (0, 1, 2, or 3)
- `cvOct` = CV input in octaves (-8.3 to 0, inverted)
- `TUNE_CAL` = 0.992 (calibration constant)

## Version History

### v1.0 (2025-01-17)
- Initial release
- Six discrete waveforms (sine, triangle, square, saw, FM-4x, FM-2x)
- PolyBLEP anti-aliasing on square and saw waves
- 1V/Oct tracking with CV input
- Octave shift button (0, +1, +2, +3)
- 1-pole RC low-pass filter (alpha=0.18)
- 1,024 sample wavetable resolution
- Linear interpolation on all waveforms
- FM depth adjustable (2.0 radians default)
- Inverted CV input (negative slope)
- Calibration constant (TUNE_CAL = 0.992)
- ~36.6 kHz ISR-driven audio

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM (0-1023)
- **Wavetable Size**: 1,024 samples
- **Pitch Range**: 320-410 Hz base × 2^(0 to 3) octaves
- **CV Range**: -8.3 octaves (inverted)
- **Anti-Aliasing**: PolyBLEP (square, saw)
- **Filter**: 1-pole RC LP (alpha=0.18)

## Notes

- CV input is inverted (higher voltage = lower pitch)
- PolyBLEP reduces aliasing on square and saw waveforms
- 1-pole RC filter softens PWM and high-frequency noise
- Octave button provides quick range changes (0, +1, +2, +3)
- TUNE_CAL constant can be adjusted for calibration (default 0.992)
- FM amount adjustable via fmAmount constant (default 2.0 radians)
- Linear interpolation ensures smooth waveform playback
- All waveforms use 1,024 sample resolution
