# BRAIDS Library Changes for Renaissance Integration

## ✅ Now Using Forked Library

**Update**: These changes have been committed to your arduinoMI fork on the `renaissance` branch:
- Repository: https://github.com/modulove/arduinoMI
- Branch: `renaissance`
- Commits:
  - ca2f333 - Add Renaissance firmware support to BRAIDS library
  - 3e06a53 - Fix BRAIDS.h includes and wire Renaissance render functions

## Installation Instructions

To use the Renaissance-enabled BRAIDS library with Arduino IDE:

1. Navigate to your Arduino libraries folder:
   ```
   C:\Users\linus\Documents\Arduino\libraries\
   ```

2. Backup your current BRAIDS library (if desired)

3. Replace with the Renaissance version:
   - Option A: Clone from your fork:
     ```bash
     cd "C:\Users\linus\Documents\Arduino\libraries\"
     rm -rf BRAIDS
     git clone -b renaissance https://github.com/modulove/arduinoMI.git temp
     mv temp/BRAIDS ./BRAIDS
     rm -rf temp
     ```

   - Option B: Copy from your local clone:
     ```bash
     cp -r "M:/Meine Ablage/11_Code/Github_Clones_vscode/arduinoMI/BRAIDS" "C:/Users/linus/Documents/Arduino/libraries/"
     ```

4. Restart Arduino IDE

## Original Documentation

This document tracks all changes made to the external BRAIDS library.
These changes are required for the Renaissance firmware integration to compile and function correctly.

## Files Modified

### 1. `src/braids/digital_oscillator.h`

#### Added Renaissance oscillator shape enums (after line 56):
```cpp
enum DigitalOscillatorShape {
  OSC_SHAPE_TRIPLE_RING_MOD,

  // Renaissance: Chord modes
  OSC_SHAPE_CHORD_SAW,
  OSC_SHAPE_CHORD_SQUARE,
  OSC_SHAPE_CHORD_TRIANGLE,
  OSC_SHAPE_CHORD_SINE,
  OSC_SHAPE_CHORD_WAVETABLE,

  // Renaissance: Stack modes (6-voice)
  OSC_SHAPE_STACK_SAW,
  OSC_SHAPE_STACK_SQUARE,
  OSC_SHAPE_STACK_TRIANGLE,
  OSC_SHAPE_STACK_SINE,
  OSC_SHAPE_STACK_WAVETABLE,

  OSC_SHAPE_SAW_SWARM,
  // ... rest of enums
```

#### Added StackState structure (after HatState, around line 214):
```cpp
// Renaissance: Stack state for chord and stack modes
struct StackState {
  uint32_t phase[12];
  int16_t previous_sample;
};
```

#### Added stack state to union (around line 242):
```cpp
union DigitalOscillatorState {
  // ... existing states
  HarmonicsState hrm;
  StackState stack;  // Renaissance: for chord and stack modes
  uint32_t modulator_phase;
};
```

#### Added Renaissance method declarations (after line 331):
```cpp
// Renaissance additions: Chord and Stack modes
void RenderDiatonicChord(const uint8_t*, int16_t*, size_t);
void RenderStack(const uint8_t*, int16_t*, size_t);
void renderChord(const uint8_t*, int16_t*, size_t, uint8_t*, uint8_t);
void renderChordSine(const uint8_t*, int16_t*, size_t, uint32_t*, uint8_t);
void renderChordSaw(const uint8_t*, int16_t*, size_t, uint32_t*, uint8_t);
void renderChordTriangle(const uint8_t*, int16_t*, size_t, uint32_t*, uint8_t);
void renderChordSquare(const uint8_t*, int16_t*, size_t, uint32_t*, uint8_t);
void renderChordWavetable(const uint8_t*, int16_t*, size_t, uint32_t*, uint8_t);
```

### 2. `src/braids/quantizer.h`

#### Made private members public and added index field (around line 59):
```cpp
void Configure(const Scale& scale) {
  Configure(scale.notes, scale.span, scale.num_notes);
}

// Renaissance: Made public for chord/stack mode access
bool enabled_;
int16_t codebook_[128];
int32_t codeword_;
int32_t previous_boundary_;
int32_t next_boundary_;
int16_t index;  // Renaissance: track current quantized note index

private:
void Configure(const int16_t* notes, int16_t span, size_t num_notes);
```

**Rationale**: Renaissance chord/stack modes need direct access to quantizer internals to build scale-aware chords. The `index` field tracks which scale degree is currently active.

### 3. `src/braids/quantizer.cc`

#### Updated Process() method to use index member (around line 88):
```cpp
int16_t best_distance = 16384;
index = -1;  // Renaissance: use index member instead of local q
for (int16_t i = lower_bound_index; i <= upper_bound_index; ++i) {
  int16_t distance = abs(pitch - codebook_[i]);
  if (distance < best_distance) {
    best_distance = distance;
    index = i;  // Renaissance: store in index member
  }
}
codeword_ = codebook_[index];
// Enlarge the current voronoi cell a bit for hysteresis.
previous_boundary_ = (9 * codebook_[index - 1] + 7 * codeword_) >> 4;
next_boundary_ = (9 * codebook_[index + 1] + 7 * codeword_) >> 4;
```

**Changed**: Replaced local variable `q` with member variable `index` so chord/stack modes can access it.

## Summary of Changes

| File | Lines Changed | Type |
|------|---------------|------|
| digital_oscillator.h | ~30 lines | Added enums, struct, methods |
| quantizer.h | ~10 lines | Moved members to public, added index |
| quantizer.cc | ~5 lines | Use index member instead of local var |

## Compatibility Notes

These changes are **backwards compatible** with existing MOD2 braids firmware. They only add new functionality without breaking existing code.

- New oscillator shapes are additive
- StackState is just another union member
- Quantizer changes expose existing functionality publicly
- The index field provides information that was computed but not accessible

## Testing Required

After making these changes:

1. ✅ Compilation should succeed without errors
2. ⬜ Test original 47 engines still work correctly
3. ⬜ Test new Renaissance modes (chord, stack, SAM)
4. ⬜ Verify quantizer still functions for all modes
5. ⬜ Check memory usage is acceptable

## Reverting Changes

If you need to revert these changes, you can:

1. Back up your current BRAIDS library
2. Re-download the original from the repository
3. Reinstall via Arduino Library Manager

Or manually remove:
- The Renaissance enum additions
- The StackState structure
- The stack union member
- The Renaissance method declarations
- Revert quantizer.h visibility changes
- Revert quantizer.cc to use local variable `q`

## Future Considerations

These library modifications could be:
1. Contributed back to the BRAIDS library as optional Renaissance support
2. Maintained as a fork (BRAIDS-Renaissance)
3. Kept as local modifications (current approach)

The current approach (local modifications) is simplest for now but requires manual tracking of changes.
