# VOCAL SNIPPET - Vocal/Speech Sample Player

Single long-form audio playback module optimized for creating vocal snippets from podcasts, speeches, and interviews for techno production.

## Overview

This module is designed specifically for sampling vocal content from long-form audio (podcasts, speeches, interviews) and creating dynamic vocal snippets for electronic music production. Instead of managing multiple small samples, you load one continuous audio file and scrub through it to find interesting vocal moments in real-time.

## Controls

### Potentiometers / CV Inputs

| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / A0** | Playback Speed | 0.5x - 1.5x | Sample playback rate (pitch shift) |
| **POT2 / A1** | Direction | Forward/Reverse | < 512 = forward, ≥ 512 = reverse |
| **POT3 / CV3** | Start Position | 0-100% | Scrub through the sample (0% = beginning, 100% = end) |

### Jacks / Pins

| Jack | Function | Type | Description |
|------|----------|------|-------------|
| **OUT (GPIO1)** | Audio Output | PWM Output | 10-bit PWM audio at ~36.6 kHz |
| **IN1 (GPIO7)** | Trigger Input | Digital Input | Rising edge trigger to play from current position |
| **IN2 (GPIO0)** | Loop Mode | Digital Input | LOW = one-shot, HIGH = loop playback |
| **LED (GPIO5)** | Trigger LED | Digital Output | 20ms pulse on trigger |
| **BUTTON (GPIO6)** | Manual Trigger | Button | Manually trigger playback from current position |

### Button Function

- **Press**: Manually trigger playback from the current POT3 position

## Features

- **Long-Form Audio**:
  - Single continuous audio file up to ~30 seconds
  - Perfect for podcast/speech/interview sampling
  - Scrub through content with POT3
  - Real-time position control via CV

- **Variable Speed Playback**:
  - Speed range: 0.5x to 1.5x (half-speed to 1.5x speed)
  - Linear interpolation for smooth pitch-shifting
  - Fixed-point fractional indexing (12-bit fraction)

- **Forward/Reverse Playback**:
  - POT2 controls direction
  - Smooth transition between directions
  - Perfect for creating reverse vocal effects

- **Loop Mode**:
  - Toggle via IN2 jack
  - One-shot mode: plays once then stops
  - Loop mode: continuously loops from start position to end

- **16-Bit PCM Audio**:
  - High-quality audio storage
  - Samples stored in flash memory (sample.h)
  - Anti-aliasing via linear interpolation

## Technical Specifications

- **Platform**: Raspberry Pi RP2040/RP2350 (Pico 2)
- **Sample Rate**: ~36.6 kHz (150 MHz / 4096)
- **Audio Resolution**: 10-bit PWM output, 16-bit sample storage
- **Sample Format**: 16-bit PCM, mono
- **Maximum Storage**: ~1.8 MB available
- **Maximum Duration**: 
  - At 36.6 kHz: ~30 seconds
  - At 32 kHz: ~40 seconds
- **Interpolation**: Linear (fractional indexing)
- **Speed Resolution**: 12-bit fractional indexing

## Workflow

### 1. Preparing Your Audio

The best source material for vocal snippets:
- **Podcasts**: Interview segments, monologues
- **Speeches**: Political speeches, TED talks
- **Audiobooks**: Dramatic readings, storytelling
- **Radio shows**: DJ commentary, call-ins
- **Documentaries**: Narration, interview clips

Tips for preparation:
- Use high-quality recordings for best results
- Consider normalizing audio for consistent levels
- Mono audio works best (stereo will be converted)
- Trim silence from beginning/end to maximize usable content

### 2. Converting to sample.h

Use the included Python script to convert your audio:

```bash
# Basic conversion
python wav_to_sample.py podcast.wav

# Custom sample rate (for longer audio)
python wav_to_sample.py speech.wav --target-rate 32000

# Custom output filename
python wav_to_sample.py vocals.wav --output my_sample.h
```

The script will:
- Convert stereo to mono
- Resample to target rate
- Normalize audio levels
- Generate sample.h file
- Show duration and memory usage

### 3. Loading to Module

1. Copy the generated `sample.h` file to your Arduino project folder
2. Compile and upload `vocal_snippet.ino` to your RP2040
3. The LED will pulse on successful trigger
4. Use POT3 to explore the sample

### 4. Performance Tips

**Finding Snippets:**
- Turn POT3 slowly to scrub through the audio
- Trigger to preview current position
- When you find something interesting, patch a clock/sequencer to IN1

**Creating Rhythm:**
- Use a fast clock (16th notes) with loop mode ON
- Adjust POT1 for pitch variation
- Modulate POT3 with CV for position changes

**Reverse Effects:**
- Toggle POT2 to reverse mode
- Great for "rewind" effects
- Combine with slow speed for ethereal vocals

## Patch Ideas

### Techno Vocal Chops
**Goal**: Rhythmic vocal stutters
- **POT1 (Speed)**: 100% (normal speed)
- **POT2 (Direction)**: Forward
- **POT3 (Position)**: Slowly scan for words
- **IN1**: Trigger from 16th note clock
- **IN2**: Loop mode ON
- **Use**: Tight vocal stabs synced to beat

### Pitched Vocal Sequence
**Goal**: Melodic vocal line
- **POT1 (Speed)**: Vary from 50%-150% for pitch
- **POT3 (Position)**: Fixed on good vowel sound
- **IN1**: Trigger from sequencer with varied timing
- **IN2**: Loop mode OFF for one-shots
- **Use**: Create pitched vocal melody

### Reverse Sweep
**Goal**: Reverse vocal effects
- **POT1 (Speed)**: 75% (slower for drama)
- **POT2 (Direction)**: Reverse
- **POT3 (Position)**: Start near end of sample
- **IN1**: Trigger on downbeats
- **IN2**: Loop mode OFF
- **Use**: Reverse vocal risers

### Granular Textures
**Goal**: Ambient vocal atmosphere
- **POT1 (Speed)**: Very slow (50-70%)
- **POT2 (Direction)**: Alternate per trigger
- **POT3/CV3**: Slow LFO or random CV
- **IN1**: Fast random triggers
- **IN2**: Loop mode ON
- **Use**: Dense vocal clouds

### Glitch Stutter
**Goal**: Broken transmission effect
- **POT1 (Speed)**: High (120-150%)
- **POT3/CV3**: Fast random CV or stepped LFO
- **IN1**: Very fast clock (32nd notes)
- **IN2**: Loop mode ON
- **Use**: Glitchy, digital vocal artifacts

### Dialog Sampler
**Goal**: Full phrase playback
- **POT1 (Speed)**: 100% (natural speech)
- **POT2 (Direction)**: Forward
- **POT3 (Position)**: Select different phrases
- **IN1**: Manual triggers for each phrase
- **IN2**: Loop mode OFF
- **Use**: Trigger complete sentences/phrases

### Pitch Dive Bomb
**Goal**: Dramatic pitch drops
- **POT1 (Speed)**: Start at 150%, decrease to 50%
- **POT2 (Direction)**: Forward
- **POT3 (Position)**: Fixed position
- **IN1**: Single trigger
- **IN2**: Loop mode OFF
- **Use**: Dramatic pitch drops on transitions

## Loading Custom Audio

### Using the Python Script

**Requirements:**
```bash
pip install numpy scipy
```

**Basic Usage:**
```bash
python wav_to_sample.py your_audio.wav
```

**Options:**
- `--target-rate RATE`: Set sample rate (default: 36600 Hz)
- `--output FILE`: Set output filename (default: sample.h)
- `--no-normalize`: Skip audio normalization

The script automatically:
- Converts stereo to mono
- Resamples to target rate
- Normalizes audio to 95% peak
- Generates properly formatted C header
- Validates flash memory limits

### Manual Method (Advanced)

If you prefer manual conversion:
1. Convert audio to 16-bit PCM, mono WAV
2. Resample to ~36.6 kHz (or lower for longer duration)
3. Use a hex editor or custom tool to generate byte array
4. Format as C header with proper structure (see sample.h format)

*Based on 1.8 MB available flash memory for samples*

## Troubleshooting

**No audio output:**
- Check that sample.h is in the same folder as the .ino file
- Verify module is receiving power
- Test with manual trigger (button)

**Distorted audio:**
- Audio may be clipping - try re-converting with normalization
- Check that sample rate matches conversion rate

**Sample too large:**
- Use lower sample rate (--target-rate 32000)
- Trim your audio file to shorter duration
- Split into multiple recordings

**Pops/clicks:**
- Normal at very high or very low speeds
- Try intermediate speed settings
- Use loop mode for smoother transitions

**Position not working:**
- Check POT3 connection
- Verify CV input if using external CV
- Try different start positions

## Version History

### v1.0 (2025-01-17)
- Initial release
- Single long-form audio file support
- Variable playback speed (0.5x - 1.5x)
- Start position control (0-100%)
- Forward/reverse playback direction
- Loop mode toggle
- 16-bit PCM sample support
- Linear interpolation for pitch-shifting
- Up to ~30 seconds at 36.6 kHz
- Python conversion script included

## Creative Applications

**Techno Production:**
- Rhythmic vocal chops
- Build-up effects with reverse
- Pitch-shifted vocal hooks
- Glitch/stutter effects
- Ambient vocal pads

**Live Performance:**
- Real-time vocal sampling
- Dynamic phrase triggering
- Speed manipulation for energy changes
- Position CV for automated scanning
- Loop mode for sustained textures

**Sound Design:**
- Create unique vocal textures
- Extreme pitch shifting for alien voices
- Granular-style vocal clouds
- Reverse vocal atmospheres
- Dialog-based rhythmic patterns

## Example Workflow: Creating a Techno Track

1. **Find source material**: 
   - Load a podcast interview (2 minutes)
   - Convert: `python wav_to_sample.py interview.wav`

2. **Explore content**:
   - Turn POT3 to scrub through
   - Find interesting words/phrases
   - Note good start positions

3. **Create vocal chop**:
   - Set POT3 to chosen word
   - IN2 → HIGH (loop mode)
   - IN1 → 16th note clock
   - Adjust POT1 for slight pitch shift

4. **Add variation**:
   - Modulate POT3 with slow LFO
   - Occasionally toggle POT2 for reverse
   - Use different speeds for choruses

5. **Layer with rhythm**:
   - Vocal chop provides rhythmic element
   - Works alongside percussion
   - Creates human element in electronic track

## Notes

- Optimized for RP2040/RP2350 flash memory
- Sample stored in flash, not RAM
- Position control is immediate (no delay)
- Direction switching is instantaneous
- Loop mode provides continuous playback
- Best results with clear, well-recorded source material
- Mono conversion preserves all important information
- Linear interpolation prevents aliasing at all speeds

## Credits

Based on HAGIWO MOD2 Sample Player
Modified for vocal/speech sampling applications
Firmware: CC0 1.0 Universal Public Domain

For more HAGIWO projects: https://www.patreon.com/HAGIWO
