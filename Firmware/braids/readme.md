# BRAIDS - Macro Oscillator

Port of Mutable Instruments Braids macro oscillator for MOD2 hardware, featuring 47 different synthesis engines.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / CV1** | Timbre | 0-32767 | Controls timbral characteristics (varies by engine) |
| **POT2 / CV2** | Morph | 0-32767 | Controls sound morphing parameter (varies by engine) |
| **POT3 / CV3** | Pitch | 3072-8192 | 1V/Oct pitch control with delta detection |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (D7)** | Audio Output | PWM Output | 16-bit PWM audio at 48 kHz |
| **TRIG (D5)** | Trigger Input | Digital Input | Rising edge trigger for percussive engines |
| **BUTTON (D4)** | Engine Select | Button | Cycles through 47 synthesis engines |

### Button Function

- **Short Press**: Advance to next synthesis engine (0-46, wraps around to 0)
- Current engine number cycles through all available models

## Features

- **47 Synthesis Engines** including:
  - Oscillator models: CSAW, Morph, Vowel, FM, Square Sub
  - Granular synthesis: Grain Cloud, Grain Formant
  - Additive synthesis: Additive, Harmonic, Inharmonic
  - Physical modeling: String, Plucked, Bowed, Blown, Fluted, Struck Bell/Drum/Wood
  - Modal synthesis: Modal resonator
  - Percussion: Kick, Snare, Cymbal, multiple Drum models
  - Noise generators and more

- **Dual-Core Implementation**:
  - Core 0: Real-time audio rendering at 48 kHz
  - Core 1: UI and control rate parameter updates

- **Smart Triggering**:
  - Percussive models auto-trigger on gate input
  - Continuous oscillators use simple AR envelope
  - Pitch delta detection prevents spurious triggers

- **Audio Quality**:
  - 48 kHz sample rate
  - 16-bit PWM output
  - Block-based processing for efficiency

## Patch Recommendations

### Lead Voice
- **Engine**: CSAW or Morph
- **POT1 (Timbre)**: 50% for rich harmonics
- **POT2 (Morph)**: Sweep for timbral movement
- **POT3 (Pitch)**: Use 1V/Oct CV from keyboard

### Percussion
- **Engine**: Kick, Snare, or Cymbal
- **TRIG**: Clock or trigger input
- **POT1/POT2**: Adjust tone and decay
- **POT3**: Pitch tune for desired tone

### Texture/Drone
- **Engine**: Grain Cloud or Modal
- **POT1 (Timbre)**: Grain density or resonance
- **POT2 (Morph)**: Texture variation
- **POT3 (Pitch)**: Base frequency

### FM Tones
- **Engine**: FM or Feedback FM
- **POT1 (Timbre)**: Modulation index
- **POT2 (Morph)**: Carrier/modulator ratio
- **POT3 (Pitch)**: Melodic control

## Version History

### v1.0 (2025-11-17)
- Initial release

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: 48 kHz
- **Audio Resolution**: 16-bit PWM
- **Processing**: Dual-core (audio + control)
- **Libraries**: STMLIB, BRAIDS, Bounce2, RPI_PICO_TimerInterrupt, PWMAudio
- **Buffer Size**: Block-based processing (BLOCK_SIZE defined in library)

## Notes

- Potentiometer readings are debounced to minimize noise in audio output
- Some engines respond better to trigger input than continuous gates
- Pitch tracking uses delta detection to avoid spurious note changes
- All parameters are updated in real-time from the second core
