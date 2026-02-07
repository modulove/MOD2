# Quick Start Guide - Vocal Snippet Sampler

## Installation Steps

### 1. Install Python Dependencies

```bash
pip install numpy scipy
```

### 2. Prepare Your Audio

Find a podcast, speech, or interview recording you want to sample. Best formats:
- WAV file (any sample rate)
- Mono or stereo (will be converted to mono)
- Up to ~2-3 minutes duration recommended

### 3. Convert Audio to sample.h

```bash
python wav_to_sample.py your_podcast.wav
```

This will create `sample.h` in the same directory.

**For longer audio (lower quality):**
```bash
python wav_to_sample.py your_podcast.wav --target-rate 32000
```

**Output:**
```
Converting your_podcast.wav to sample.h...
Target sample rate: 36600 Hz

Input WAV info:
  Channels: 2
  Sample width: 2 bytes
  Frame rate: 44100 Hz
  Duration: 120.50 seconds

  Converted stereo to mono
  Normalized audio to 95% peak
  Resampling from 44100 Hz to 36600 Hz...

Output info:
  Sample length: 4409700 samples
  Byte length: 8819400 bytes (8612.7 KB)
  Duration: 120.50 seconds
  Memory usage: 8.41 MB

⚠️  WARNING: Sample too large!
   Maximum size: 1.72 MB
   Your sample: 8.41 MB
   Please use a shorter audio file or lower sample rate
```

### 4. Upload to Module

1. Open Arduino IDE
2. Open `vocal_snippet.ino`
3. Make sure `sample.h` is in the same folder
4. Select board: "Raspberry Pi Pico" or "Raspberry Pi Pico 2"
5. Select correct COM port
6. Click "Upload"

### 5. Test the Module

1. **Power on** - LED should be off
2. **Press BUTTON** - LED pulses, audio plays from beginning
3. **Turn POT3** - Scrubs to different start positions
4. **Press BUTTON again** - Plays from new position
5. **Turn POT2 past halfway** - Reverses playback direction
6. **Turn POT1** - Changes pitch/speed

## Maximum Audio Length

Your maximum audio length depends on sample rate:

| Sample Rate | Quality | Max Duration | Use Case |
|-------------|---------|--------------|----------|
| 36600 Hz | Best | ~130 sec | High-quality vocals |
| 32000 Hz | Good | ~140 sec | Good balance |
| 22050 Hz | Medium | ~200 sec | Longer content |
| 16000 Hz | Low | ~280 sec | Speech only |

**Example for 2-minute podcast at good quality:**
```bash
python wav_to_sample.py podcast_2min.wav --target-rate 36600
```

**Example for 3-minute speech at medium quality:**
```bash
python wav_to_sample.py speech_3min.wav --target-rate 22050
```

## Troubleshooting

### "Sample too large" Error

Your audio is too long. Solutions:
1. **Reduce sample rate**: `--target-rate 32000` or `--target-rate 22050`
2. **Trim audio file**: Use Audacity to cut it shorter
3. **Use better compression**: Focus on the most interesting parts

### No Audio Output

1. Check `sample.h` is in the Arduino project folder
2. Verify the .ino file compiled without errors
3. Try pressing the BUTTON to manually trigger
4. Check power supply to module

### Distorted Audio

1. Re-convert with normalization (default)
2. Check your source audio isn't already distorted
3. Try a different audio file

### Position Control Not Working

1. Test POT3 by turning fully left and right
2. Check wiring to A2 pin
3. Try different start positions

## Example Workflow

### Quick Test with Short Audio

1. Find a 30-second audio clip
2. Convert: `python wav_to_sample.py test.wav`
3. Upload to module
4. Test all three pots
5. Confirm everything works

### Real Project: 2-Minute Podcast Sample

1. **Find content**: Download interesting podcast segment
2. **Edit in Audacity**: 
   - Trim to best 2 minutes
   - Remove long silences
   - Export as WAV (44.1kHz, 16-bit, mono)
3. **Convert**: `python wav_to_sample.py podcast_2min.wav`
4. **Upload**: Flash to module
5. **Explore**: 
   - Scrub POT3 to find good words/phrases
   - Set POT1 to taste (try 75% for lower pitch)
   - Patch a clock to IN1 for rhythmic triggers

## File Structure

Your Arduino project folder should contain:
```
vocal_snippet_project/
├── vocal_snippet.ino       (main firmware)
├── sample.h                (generated audio data)
└── wav_to_sample.py        (converter script)
```

## Tips for Best Results

### Audio Selection
- Choose clear, well-recorded source material
- Avoid heavily compressed MP3s (use WAV/FLAC if possible)
- Interviews work better than music with vocals
- Speech with good enunciation is ideal

### Conversion
- Always normalize (it's default) for consistent levels
- Start with 36600 Hz, drop to 32000 Hz if too large
- Mono conversion is automatic and lossless

### Performance
- Use CV modulation on POT3 for automatic scanning
- Loop mode (IN2 HIGH) great for sustained textures
- Forward/reverse switching creates variety
- Speed variation adds musical interest

## Next Steps

Once installed and tested:
1. Read the full README.md for patch ideas
2. Experiment with different source material
3. Try CV modulation on POT3 position
4. Combine with other modules for complex patches
5. Create your own techno vocal chops!

## Support

For issues or questions:
- Check the full README.md for detailed info
- Review troubleshooting section
- Test with shorter audio files first
- Verify Python dependencies are installed

Happy sampling!
