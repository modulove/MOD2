# Braids Renaissance Integration Plan

## Overview
This document outlines the integration of Braids Renaissance alternative firmware features into the MOD2 repository on the `renaissance-modes` branch.

## Renaissance Firmware Features

### New Synthesis Algorithms

#### 1. SAM (Software Automated Mouth) - 2 Banks
- Classic Commodore 64 text-to-speech engine
- Configured as granular sampler
- Timbre parameter scrubs through word selections
- Color control selects different words
- **Files Required**: `vocalist/` directory with SAM implementation
- **Shapes**: `MACRO_OSC_SHAPE_SAM1`, `MACRO_OSC_SHAPE_SAM2` (with NUM_BANKS=2)

#### 2. Chord Algorithms - 5 New Modes
Works with quantizer to keep chords in scale. Color parameter selects chord extensions based on scale/root.
- `MACRO_OSC_SHAPE_CHORD_SAW` - Chord with saw waves
- `MACRO_OSC_SHAPE_CHORD_SQUARE` - Chord with square waves
- `MACRO_OSC_SHAPE_CHORD_TRIANGLE` - Chord with triangle waves
- `MACRO_OSC_SHAPE_CHORD_SINE` - Chord with sine waves
- `MACRO_OSC_SHAPE_CHORD_WAVETABLE` - Chord with wavetable
- **Implementation**: `stack.cc` with chord rendering functions

#### 3. Chord Stack Modes - 5 "x6" Algorithms
Six simultaneous oscillators spaced across quantize scale. Color controls spacing.
- `MACRO_OSC_SHAPE_STACK_SAW` - 6x saw oscillators
- `MACRO_OSC_SHAPE_STACK_SQUARE` - 6x square oscillators
- `MACRO_OSC_SHAPE_STACK_TRIANGLE` - 6x triangle oscillators
- `MACRO_OSC_SHAPE_STACK_SINE` - 6x sine oscillators
- `MACRO_OSC_SHAPE_STACK_WAVETABLE` - 6x wavetable oscillators
- **Implementation**: `stack.cc` with stack rendering functions

#### 4. Additional Shape Changes
- Removed: Text editor, morse code easter egg, QPSK algorithm
- Reorganized: Some shapes reordered to accommodate new modes
- New total: Approximately 54 shapes (vs original 47)

### Hardware Features
- **NVRT Setting**: Reverse encoder rotation for DIY builders

## Current MOD2 Implementation

### Structure
- Arduino-based (RP2040/RP2350) running at 48kHz
- Uses external BRAIDS and STMLIB libraries
- 47 engines currently supported (0-46 in engine array)
- Simple UI: button for engine selection, 3 pots for control

### Key Files
- `braids.ino` - Main firmware with engine names array
- `braids.h` - Audio update logic and voice management
- `utility.h` - Helper functions
- `potentiometer.h` - Input handling

## Integration Strategy

### Phase 1: Add Renaissance Source Files
1. **Copy vocalist directory** from Renaissance to MOD2
   - SAM implementation files
   - Word lists and phoneme tables
   - Render tabs for text-to-speech

2. **Copy stack.cc** - Contains chord and stack rendering functions

3. **Update digital_oscillator.h/.cc** - Add new shape rendering methods

4. **Update macro_oscillator.cc** - Route new shapes to correct render functions

### Phase 2: Update MOD2 Firmware

1. **Update engine names array** in `braids.ino`
   - Add chord mode names (CHORD_SAW, CHORD_SQ, etc.)
   - Add stack mode names (STK_SAW, STK_SQ, etc.)
   - Add SAM banks (SAM1, SAM2)
   - Update total count to 54 (or final count after integration)

2. **Update braids.h**
   - May need to handle new trigger behaviors for chord modes
   - Ensure quantizer integration works properly

3. **Conditional compilation**
   - Set `NUM_BANKS` to control SAM mode availability (2 recommended)
   - Consider memory constraints on RP2040/RP2350

### Phase 3: Library Updates

Since MOD2 uses external BRAIDS/STMLIB libraries, we need to:

1. **Option A: Fork and modify libraries**
   - Create MOD2-specific versions with Renaissance changes
   - More control but requires library maintenance

2. **Option B: Integrate directly into firmware**
   - Copy necessary Renaissance code into Firmware/braids/
   - Less dependency management
   - Easier to customize for MOD2 hardware

**Recommendation**: Option B for easier integration

## Technical Considerations

### Memory Constraints
- RP2040: 264KB RAM, RP2350: 520KB RAM
- SAM vocalist uses significant memory for word lists
- May need to limit NUM_BANKS or optimize word lists

### Sample Rate
- Renaissance designed for 96kHz
- MOD2 runs at 48kHz
- Most algorithms should work fine at lower rate
- May need to adjust some filter coefficients

### Quantizer Integration
- Chord modes rely heavily on quantizer
- Current MOD2 implementation has quantizer initialized
- Need to ensure it's properly configured and accessible

### UI Considerations
- No display in MOD2 (unlike original Braids hardware)
- Engine selection via button only
- May want to group new modes logically in engine array
- Consider serial debug output for development

## Implementation Steps

1. ✅ Create `renaissance-modes` development branch
2. ⬜ Copy Renaissance source files to MOD2
3. ⬜ Update engine enumeration and names
4. ⬜ Implement new render functions
5. ⬜ Test compilation
6. ⬜ Test basic functionality (existing engines still work)
7. ⬜ Test new chord modes
8. ⬜ Test new stack modes
9. ⬜ Test SAM modes
10. ⬜ Document new features in README
11. ⬜ Merge to main when stable

## Testing Plan

### Basic Functionality Tests
- Verify all original 47 engines still work correctly
- Check button navigation through all engines
- Verify pot controls still function

### New Mode Tests
- **Chord modes**: Test with quantizer, verify polyphony
- **Stack modes**: Test 6-voice stacking, color parameter
- **SAM modes**: Test word selection, timbre scrubbing

### Hardware Tests
- Audio quality at 48kHz
- CPU usage acceptable
- No memory overflows
- Trigger response for new modes

## Files Modified/Added

### New Files
- `Firmware/braids/vocalist/` (directory with SAM implementation)
- `Firmware/braids/stack.cc`
- `Firmware/braids/stack.h` (if needed)
- `Firmware/braids/RENAISSANCE_INTEGRATION.md` (this file)

### Modified Files
- `Firmware/braids/braids.ino` - Updated engine names/count
- `Firmware/braids/braids.h` - Updated for new modes
- Potentially: digital_oscillator files if integrated directly

## References

- Renaissance GitHub: https://github.com/boourns/eurorack/tree/master/braids
- Renaissance Info: https://burns.ca/eurorack.html
- Original Braids: https://mutable-instruments.net/modules/braids

## Notes

- Installation resets calibration data (not applicable to MOD2)
- Renaissance removes some original features (text editor, morse code)
- NUM_BANKS set to 2 provides good balance of features vs memory
- Consider making SAM modes optional via compile flag if memory tight
