# Braids Renaissance for MOD2

This is the **renaissance-modes** development branch featuring integration of the Braids Renaissance alternative firmware into MOD2.

## What is Braids Renaissance?

Braids Renaissance is an alternative firmware for the Mutable Instruments Braids module created by Tom Burns. It adds new synthesis algorithms while maintaining the core Braids architecture.

**Original Renaissance firmware**: https://github.com/boourns/eurorack/tree/master/braids
**More info**: https://burns.ca/eurorack.html

## New Features in This Branch

### 🎵 Chord Modes (5 new engines)

Advanced chord synthesis that works with the quantizer to keep chords in scale. The Color parameter (POT2/CV2) intelligently selects chord extensions based on the configured scale and root note.

- **CH_SAW** (14) - 4-5 voice chord using sawtooth waves
- **CH_SQ** (15) - 4-5 voice chord using square waves
- **CH_TRI** (16) - 4-5 voice chord using triangle waves
- **CH_SIN** (17) - 4-5 voice chord using sine waves
- **CH_WT** (18) - 4-5 voice chord using wavetables

**Usage Tips:**
- Configure the quantizer scale for best results
- Timbre controls harmonic content
- Color/Morph selects chord voicings (major, minor, 7th, etc.)
- Works great with slow pitch sequences

### 🎹 Stack Modes (5 new engines)

Six simultaneous oscillators spaced across the quantizer scale. Creates lush, dense textures.

- **STK_SAW** (19) - 6 sawtooth oscillators
- **STK_SQ** (20) - 6 square oscillators
- **STK_TRI** (21) - 6 triangle oscillators
- **STK_SIN** (22) - 6 sine oscillators
- **STK_WT** (23) - 6 wavetable oscillators

**Usage Tips:**
- Color controls spacing between notes
- Creates instant pad sounds
- Great for ambient drones
- Can create cluster chords or arpeggiated effects

### 🗣️ SAM Vocoder (2 new engines)

Classic Commodore 64 Software Automated Mouth (SAM) text-to-speech engine configured as a granular sampler.

- **SAM1** (50) - SAM vocoder word bank 1
- **SAM2** (51) - SAM vocoder word bank 2

**Usage Tips:**
- Timbre scrubs through different word selections
- Color chooses which words to speak
- Pitch controls the voice pitch
- Trigger input restarts the phrase
- Creates everything from robotic speech to abstract vocal textures

### 🔄 Reorganized Engines

Some original engines have been reordered or renamed to accommodate the new modes:

- **SIN_TRI** (3) - Sine/Triangle morph (was FOLD)
- **BUZZ** (4) - Buzz oscillator (was in different position)
- **SAW_SYNC** (8) - Saw sync (previously SQ_3)
- **TRI_3** (11) - Triple triangle (new position)
- **SIN_3** (12) - Triple sine (new position)
- **RING_3** (13) - Triple ring mod (new position)
- **CHAOFM** (37) - Chaotic feedback FM (expanded from original)

## Complete Engine List

Total: **56 engines** (indices 0-55)

### Analog-Style (0-4)
0. CSAW - Classic sawtooth
1. MORPH - Morphing saw/square
2. SAW_SQ - Saw and square mix
3. SIN_TRI - Sine/triangle morph
4. BUZZ - Buzz oscillator

### Analog Sub-Oscillator (5-6)
5. SQ_SUB - Square with sub
6. SAW_SUB - Sawtooth with sub

### Sync Oscillators (7-8)
7. SQ_SYNC - Square sync
8. SAW_SYNC - Saw sync

### Triple Oscillators (9-13)
9. SAW_3 - Triple saw
10. SQ_3 - Triple square
11. TRI_3 - Triple triangle
12. SIN_3 - Triple sine
13. RING_3 - Triple ring mod

### ✨ Chord Modes - NEW (14-18)
14. CH_SAW - Chord saw
15. CH_SQ - Chord square
16. CH_TRI - Chord triangle
17. CH_SIN - Chord sine
18. CH_WT - Chord wavetable

### ✨ Stack Modes - NEW (19-23)
19. STK_SAW - Stack saw (6x)
20. STK_SQ - Stack square (6x)
21. STK_TRI - Stack triangle (6x)
22. STK_SIN - Stack sine (6x)
23. STK_WT - Stack wavetable (6x)

### Swarm & Comb (24-26)
24. SWARM - Saw swarm
25. SAW_COMB - Sawtooth comb
26. TOY - Toy synthesizer

### Filtered Oscillators (27-30)
27. ZLPF - Low-pass filtered saw
28. ZPKF - Peak filtered saw
29. ZBPF - Band-pass filtered saw
30. ZHPF - High-pass filtered saw

### Formant Synthesis (31-33)
31. VOSIM - VOSIM formant
32. VOWEL - Vowel synthesis
33. VOW_FOF - FOF vowel synthesis

### Additive (34)
34. HARM - Harmonic oscillator

### FM Synthesis (35-37)
35. FM - 2-operator FM
36. FBFM - Feedback FM
37. CHAOFM - Chaotic feedback FM

### Physical Modeling (38-41)
38. PLUCK - Karplus-Strong plucked
39. BOW - Bowed string
40. BLOW - Blown pipe
41. FLUTE - Flute model

### Percussive (42-46)
42. BELL - Struck bell
43. DRUM - Struck drum
44. KICK - Kick drum
45. CYMBAL - Cymbal/hat
46. SNARE - Snare drum

### Wavetable (47-49)
47. WTBL - Wavetable
48. WMAP - Wavemap
49. WLIN - Wave terrain/line

### ✨ SAM Vocoder - NEW (50-51)
50. SAM1 - SAM vocoder bank 1
51. SAM2 - SAM vocoder bank 2

### Noise & Granular (52-55)
52. NOISE - Filtered noise
53. TWNQ - Twin peaks noise
54. CLKN - Clocked noise
55. CLOUD - Granular cloud
56. PRTC - Particle noise

## Controls

### Hardware Controls
| Control | Function | Range | Description |
|---------|----------|-------|-------------|
| **POT1 / CV1** | Timbre | 0-32767 | Timbral characteristics (varies by engine) |
| **POT2 / CV2** | Morph/Color | 0-32767 | Sound morphing, chord voicings, word selection |
| **POT3 / CV3** | Pitch | 3072-8192 | 1V/Oct pitch control |
| **TRIG (D5)** | Trigger | Digital | Trigger/gate input |
| **BUTTON (D4)** | Engine Select | Button | Short press: next engine, Long press: previous engine |

### Navigation
- **Short press** (<500ms): Advance to next engine (wraps at 55→0)
- **Long press** (>500ms): Go to previous engine (wraps at 0→55)

## Patch Ideas

### Renaissance Chord Pad
- **Engine**: CH_SIN (17) or CH_WT (18)
- **Timbre**: 50% for balanced harmonics
- **Color**: Sweep for different chord voicings
- **Pitch**: Slow LFO or sequencer
- **Use**: Instant lush pad sounds

### Dense Cluster Stack
- **Engine**: STK_SAW (19) or STK_SIN (22)
- **Timbre**: Adjust for brightness
- **Color**: Controls note spacing (tight clusters to wide intervals)
- **Pitch**: Static or slow drift
- **Use**: Ambient textures, sound design

### Robotic Voice
- **Engine**: SAM1 (50) or SAM2 (51)
- **Timbre**: Scrub through word bank
- **Color**: Select different words
- **Trigger**: Gate to restart phrases
- **Use**: Glitchy vocals, sci-fi sounds

### Evolving FM Texture
- **Engine**: CHAOFM (37)
- **Timbre**: Modulation amount (patch with CV)
- **Color**: Chaos amount
- **Pitch**: Melodic sequence
- **Use**: Evolving, unpredictable tones

## Technical Notes

### Memory Considerations
- RP2040: 264KB RAM
- RP2350: 520KB RAM
- SAM modes use significant memory for word banks (NUM_BANKS=2)
- Stack/Chord modes use 4-6 oscillators each

### Sample Rate
- MOD2 runs at 48kHz (vs Renaissance original 96kHz)
- All algorithms adapted for 48kHz operation
- Filter coefficients may differ slightly from original

### Quantizer
- Chord and Stack modes work best with quantizer enabled
- Original Braids scales are available
- Quantizer configured in firmware initialization

### Files Added
- `vocalist/` - SAM speech synthesis implementation
- `stack.cc` - Chord and stack mode rendering
- `RENAISSANCE_INTEGRATION.md` - Integration documentation
- `RENAISSANCE_README.md` - This file

## Development Status

This is the **renaissance-modes** development branch.

### ✅ Completed
- ✅ Created development branch
- ✅ Copied Renaissance source files (vocalist/, stack.cc)
- ✅ Updated engine names array (56 engines)
- ✅ Updated engine count limits (0-55)
- ✅ Documentation created

### ⬜ TODO
- ⬜ Integrate stack.cc rendering functions into build
- ⬜ Integrate vocalist/SAM implementation into build
- ⬜ Update BRAIDS/STMLIB libraries with Renaissance changes
- ⬜ Test compilation
- ⬜ Test all original engines still work
- ⬜ Test new chord modes
- ⬜ Test new stack modes
- ⬜ Test SAM vocoder modes
- ⬜ Optimize memory usage if needed
- ⬜ Update main README
- ⬜ Merge to main when stable

## Building

*Build instructions will be added once integration is complete.*

## Credits

- **Original Braids**: Émilie Gillet / Mutable Instruments
- **Braids Renaissance**: Tom Burns (https://burns.ca)
- **MOD2 Port**: MOD2 project team
- **Integration**: This branch

## License

- Renaissance additions: MIT License (as per original)
- Original Braids: MIT License
- MOD2 firmware: GPLv3

## References

- Original Braids: https://mutable-instruments.net/modules/braids
- Braids Renaissance: https://github.com/boourns/eurorack/tree/master/braids
- Tom Burns Eurorack: https://burns.ca/eurorack.html
- MOD2 Project: This repository

---

**Note**: This is a work-in-progress development branch. Some features may not be fully implemented yet. Check the Development Status section above for current progress.
