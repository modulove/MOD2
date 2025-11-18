# BREAK BEATS - Amen Break Sample Player

Classic break beat sample player featuring the legendary "Amen Break" and "Think Break" samples.

##   Sample Data Required

This firmware requires pre-compiled sample data that is **not included** in this repository.

**Download the complete firmware with samples from Hagiwo's Patreon:**

**[Download Break Beats Firmware from Hagiwo's Patreon](https://www.patreon.com/posts/code-for-mod2-133952127)**

Please support the original creator!

---

## About

This firmware is based on Hagiwo's sample player code and features:
- **Amen Break** - The most sampled drum break in history (The Winstons - "Amen, Brother", 1969)
- **Think Break** - Classic funk break (Lyn Collins - "Think (About It)", 1972)

These iconic breaks have been used in countless hip-hop, jungle, drum & bass, and electronic music tracks.

## Controls

Based on the sample player architecture, this firmware likely features:

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Playback Speed | 0.5x - 1.5x | Sample playback rate (pitch shift) |
| **POT2 / A1** | Sample Select | 2+ samples | Selects between different break variations |
| **POT3 / CV3** | Start Position | 0-100% | Sets playback start point within the sample |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger to play sample |
| **BUTTON (GPIO6)** | Manual Trigger | Button | Manually trigger sample playback |
| **LED (GPIO5)** | Trigger LED | Digital Output | Visual feedback on trigger |

### Button Function

- **Press**: Manually trigger break playback

## Features

- **Classic Break Samples**:
  - Amen Break (The Winstons)
  - Think Break (Lyn Collins)
  - Possibly additional break variations or slices

- **Variable Playback Speed**:
  - Speed range: 0.5x to 1.5x (half-speed to 1.5x speed)
  - Real-time pitch shifting via speed control
  - Linear interpolation for smooth playback

- **Sample Slicing**:
  - Start position control for slice playback
  - Perfect for creating rhythmic variations
  - Triggered playback for sequencing

- **16-Bit Audio Quality**:
  - High-quality PCM samples
  - ~36.6 kHz sample rate
  - 10-bit PWM output

## Patch Recommendations

### Classic Jungle
- **POT1 (Speed)**: 130-140% (sped up)
- **POT2 (Sample)**: Amen Break
- **POT3 (Start)**: Vary for different slices
- **Trigger**: Fast clock (160-180 BPM)
- **Use**: Jungle, drum & bass patterns

### Hip-Hop Break
- **POT1 (Speed)**: 90-100% (original or slightly slower)
- **POT2 (Sample)**: Think Break
- **POT3 (Start)**: Various slice points
- **Trigger**: 80-100 BPM clock
- **Use**: Hip-hop, boom-bap beats

### Slow Motion Break
- **POT1 (Speed)**: 50-70% (half-speed)
- **POT2 (Sample)**: Either break
- **POT3 (Start)**: 0% (full loop)
- **Use**: Ambient, experimental, slow beats

### Chopped Breaks
- **POT1 (Speed)**: 100% (normal)
- **POT2 (Sample)**: Amen Break
- **POT3 (Start)**: CV modulated for random slices
- **Trigger**: Irregular clock or manual
- **Use**: Glitch, IDM, broken beat

### Speed Jungle
- **POT1 (Speed)**: 150% (maximum speed)
- **POT2 (Sample)**: Amen Break
- **POT3 (Start)**: Different hit points
- **Trigger**: Very fast clock (180+ BPM)
- **Use**: Hardcore, speedcore, breakcore

## Historical Context

### The Amen Break
The "Amen Break" is a 6-second drum solo from "Amen, Brother" by The Winstons (1969). It became the most sampled drum break in music history, forming the foundation of:
- Jungle / Drum & Bass
- Hip-Hop
- Breakbeat
- Hardcore
- Countless other genres

### The Think Break
The "Think Break" from Lyn Collins' "Think (About It)" (1972), produced by James Brown, is another hugely influential break, used extensively in:
- Hip-Hop (especially 80s and 90s)
- Breakbeat
- House music
- Pop music

## Installation

1. **Download from Patreon**: Get the complete firmware with samples from [Hagiwo's Patreon](https://www.patreon.com/posts/code-for-mod2-133952127)
2. **Flash to Hardware**:
   - Hold BOOTSEL button on Raspberry Pi Pico/Pico 2
   - Connect USB cable
   - Release BOOTSEL
   - Drag and drop the .uf2 file
   - Hardware will reboot with break beats firmware

## Building from Source

If you want to compile from source with your own samples:

### Prerequisites
- Original sample.h file from Hagiwo's Patreon (required)
- Arduino IDE or arduino-cli
- RP2040/RP2350 board support

### Custom Samples
To use your own break samples:
1. Convert audio to 16-bit PCM, mono
2. Sample rate should match ~36.6 kHz
3. Format samples as C arrays in sample.h
4. Update sample count and lengths
5. Recompile and upload firmware

**Note**: Using copyrighted samples requires proper licensing/clearance.

## Version History

### v1.0 (Original Release by Hagiwo)
- Amen Break sample implementation
- Think Break sample implementation
- Variable playback speed (0.5x - 1.5x)
- Start position control for slicing
- Trigger input for sequencing
- ~36.6 kHz sample rate
- 16-bit PCM audio quality

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM output, 16-bit sample storage
- **Sample Format**: 16-bit PCM, mono
- **Interpolation**: Linear (fractional indexing)
- **Speed Range**: 0.5x to 1.5x

## Credits

- **Original Firmware**: [Hagiwo](https://note.com/solder_state)
- **Amen Break**: The Winstons - "Amen, Brother" (1969)
- **Think Break**: Lyn Collins - "Think (About It)" (1972)
- **Hardware Design**: MOD2 by Hagiwo

## Legal Note

The Amen Break and Think Break samples are widely used in music production. When using these samples in commercial releases, proper clearance and licensing may be required. This firmware is for educational and personal use.

## Support the Creator

Please support Hagiwo's work by subscribing to his Patreon:
- [Hagiwo's Patreon](https://www.patreon.com/user?u=30636742)
- [Hagiwo's Blog](https://note.com/solder_state)

## License

Check Hagiwo's Patreon post for specific license information regarding this firmware and the included samples.
