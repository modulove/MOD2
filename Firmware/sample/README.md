# SAMPLE - Sample Player

Digital sample playback module with 18 sample slots and variable playback speed.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Playback Speed | 0.5x - 1.5x | Sample playback rate (pitch shift) |
| **POT2 / A1** | Sample Group | 3 groups | Selects group: 1-6, 7-12, or 13-18 |
| **POT3 / CV3** | Sample Index | 6 positions | Selects sample within current group (1-6) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger to play selected sample |
| **IN2 (GPIO0)** | Sample Offset | Digital Input | HIGH = add +6 to sample number (access 7-18) |
| **LED (GPIO5)** | Trigger LED | Digital Output | 20ms pulse on trigger |
| **BUTTON (GPIO6)** | Manual Trigger | Button | Manually trigger selected sample |

### Button Function

- **Press**: Manually trigger playback of selected sample

## Features

- **18 Sample Slots**:
  - Two-stage selection system
  - POT2 selects group (1-6, 7-12, 13-18)
  - POT3 selects sample within group (1-6)
  - IN2 jack adds +6 offset for extended range

- **Variable Speed Playback**:
  - Speed range: 0.5x to 1.5x (half-speed to 1.5x speed)
  - Linear interpolation for smooth pitch-shifting
  - Fixed-point fractional indexing (12-bit fraction)

- **16-Bit PCM Samples**:
  - High-quality sample storage
  - Up to 20 seconds total sample storage
  - Samples stored in flash memory (sample.h)

- **Anti-Aliasing**:
  - Linear interpolation prevents aliasing
  - Smooth playback at all speeds

## Sample Selection Matrix

| POT2 (Group) | POT3 (Index) | IN2=LOW | IN2=HIGH |
|--------------|--------------|---------|----------|
| Group 1      | Position 1-6 | Sample 1-6 | Sample 7-12 |
| Group 2      | Position 1-6 | Sample 7-12 | Sample 13-18 |
| Group 3      | Position 1-6 | Sample 13-18 | Sample 19-24* |

*Note: Samples 19-24 require custom sample.h configuration

## Patch Recommendations

### Drum Sampler
- **Samples**: Load kicks (1-3), snares (4-6), hats (7-9)
- **POT1 (Speed)**: 100% (normal speed)
- **POT2/POT3**: Select drum type
- **Use**: Drum machine, live performance

### Melodic Sampler
- **Samples**: Load melodic hits or notes
- **POT1 (Speed)**: Vary for pitch shifting
- **POT3 (Index)**: Sequence different notes
- **Use**: Melodic sequencing, live jamming

### Granular Textures
- **Samples**: Load long ambient recordings
- **POT1 (Speed)**: Very slow (0.5x)
- **Trigger**: Fast clock for granular effect
- **Use**: Ambient textures, soundscapes

### Percussion Kit
- **Samples**: Load 18 different percussion sounds
- **POT2**: Quick group switching
- **POT3/CV3**: CV-controlled sample selection
- **Use**: Full percussion kit, pattern generation

### Sound Effects
- **Samples**: Load various FX samples
- **POT1 (Speed)**: Real-time pitch shifting
- **IN2**: Toggle between two banks
- **Use**: Live FX, performance

## Loading Custom Samples

Samples are stored in the `sample.h` file. 
The one Hagiwo did is located at his patreon page. Go give the maker some love!

To load your own samples:

1. Convert audio to 16-bit PCM, mono
2. Sample rate should match ~36.6 kHz or will be pitch-shifted
3. Format samples as C arrays in sample.h
4. Update sample count and lengths
5. Recompile and upload firmware

## Version History

### v1.0 (2025-01-17)
- Initial release
- 18 sample playback slots
- Variable playback speed (0.5x - 1.5x)
- Two-stage sample selection (group + index)
- 16-bit PCM sample support
- Linear interpolation for pitch-shifting
- Fixed-point fractional indexing (12-bit)
- Sample offset input (+6 via IN2)
- 20ms LED pulse on trigger
- Up to 20 seconds total sample storage
- ~36.6 kHz sample rate

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM output, 16-bit sample storage
- **Sample Format**: 16-bit PCM, mono
- **Maximum Storage**: ~20 seconds total (at 36.6 kHz)
- **Interpolation**: Linear (fractional indexing)
- **Speed Resolution**: 12-bit fractional indexing

## Notes

- Samples are stored in flash memory via sample.h header
- Two-stage selection allows access to 18 samples with 2 pots
- IN2 jack provides +6 offset for extended sample access
- Speed control affects pitch (faster = higher pitch)
- Linear interpolation prevents aliasing during playback
- LED provides visual feedback on trigger events (20ms pulse)
- Custom samples require editing sample.h and recompiling
