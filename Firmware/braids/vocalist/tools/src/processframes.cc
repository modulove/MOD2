#include <stdlib.h>
#include <stdio.h>

#include "sam.h"
#include "RenderTabs.h"

void SAM::CombineGlottalAndFormants(unsigned char phase1, unsigned char phase2, unsigned char phase3, unsigned char Y, int *bufferpos, char *buffer)
{
  unsigned int tmp;

  tmp   = multtable[sinus[phase1]     | amplitude1[Y]];
  tmp  += multtable[sinus[phase2]     | amplitude2[Y]];
  tmp  += tmp > 255 ? 1 : 0; // if addition above overflows, we for some reason add one;
  tmp  += multtable[rectangle[phase3] | amplitude3[Y]];
  tmp  += 136;
  tmp >>= 4; // Scale down to 0..15 range of C64 audio.

  Output(0, tmp & 0xf, bufferpos, buffer);
}

// PROCESS THE FRAMES
//
// In traditional vocal synthesis, the glottal pulse drives filters, which
// are attenuated to the frequencies of the formants.
//
// SAM generates these formants directly with sin and rectangular waves.
// To simulate them being driven by the glottal pulse, the waveforms are
// reset at the beginning of each glottal pulse.
//

#define MAX_TINY_BUFFER 5000
char tinyBuffer[MAX_TINY_BUFFER];
int tinyBufferPos;

void SAM::InitFrameProcessor() {
  frameProcessorPosition = 0;
  glottal_pulse = pitches[0];
  mem38 = glottal_pulse - (glottal_pulse >> 2); // mem44 * 0.75
  tinyBufferPos = 0;
}

int SAM::Drain(int threshold, int count, char *buffer)
{
  int available = (tinyBufferPos / 50) - threshold;
  if (available <= 0) {
    return 0;
  }

  // copy out either total available, or remaining count, whichever is lower
  available = (available > count) ? count : available;

  // consume N sound bytes
  for (int k = 0; k < available; k++) {
    buffer[k] = tinyBuffer[k];
  }

  // move buffer to the left. This could be removed if we implemented a ring buffer with mod in Output
  int max = (tinyBufferPos/50) + 5;
  if (max >= 5000) {
    max = 5000;
  }
  for (int k = available; k < max; k++) {
    tinyBuffer[k-available] = tinyBuffer[k];
  }
  tinyBufferPos -= (available * 50);

  return available;
}

int SAM::FillBufferFromFrame(int count, char *buffer)
{
  int written = 0;
  while(framesRemaining && written < count) {
    unsigned char absorbed = ProcessFrame(frameProcessorPosition, framesRemaining, &tinyBufferPos, &tinyBuffer[0]);
    written += Drain(20, count - written, &buffer[written]);

    frameProcessorPosition += absorbed;
    framesRemaining -= absorbed;
  }
  if (written < count) {
    written += Drain(0, count - written, &buffer[written]);
  }
  return written;
}

unsigned char SAM::ProcessFrame(unsigned char Y, unsigned char mem48, int *bufferpos, char *buffer)
{
    unsigned char flags = sampledConsonantFlag[Y];
    unsigned char absorbed = 0;

    // unvoiced sampled phoneme?
    if(flags & 248) {
      RenderSample(&mem66, flags, Y, bufferpos, buffer);
      // skip ahead two in the phoneme buffer
      speedcounter = speed;
      absorbed = 2;
    } else {
      CombineGlottalAndFormants(phase1, phase2, phase3, Y, bufferpos, buffer);

      speedcounter--;
      if (speedcounter == 0) {
        absorbed = 1;

        if(mem48 == 1) {
          return absorbed;
        }
        speedcounter = speed;
      }

      --glottal_pulse;

      if(glottal_pulse != 0) {
        // not finished with a glottal pulse

        --mem38;
        // within the first 75% of the glottal pulse?
        // is the count non-zero and the sampled flag is zero?
        if((mem38 != 0) || (flags == 0)) {
          // reset the phase of the formants to match the pulse
          phase1 += frequency1[Y + absorbed];
          phase2 += frequency2[Y + absorbed];
          phase3 += frequency3[Y + absorbed];
          return absorbed;
        }

        // voiced sampled phonemes interleave the sample with the
        // glottal pulse. The sample flag is non-zero, so render
        // the sample for the phoneme.
        RenderSample(&mem66, flags, Y + absorbed, bufferpos, buffer);
      }
    }

    glottal_pulse = pitches[Y + absorbed];
    mem38 = glottal_pulse - (glottal_pulse>>2); // mem44 * 0.75

    // reset the formant wave generators to keep them in
    // sync with the glottal pulse
    phase1 = 0;
    phase2 = 0;
    phase3 = 0;

    return absorbed;
}
