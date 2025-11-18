/*
  (c) 2025 blueprint@poetaster.de
  GPLv3 the libraries are MIT as the originals for STM from MI were also MIT.
*/

/* toepler +
    // Toepler Plus pins
  #define OUT1 (0u)
  #define OUT2 (1u)
  #define SW1 (8u)
  #define CV1 (26u)
  #define CV2 (27u)
  #define CV3 (28u)

*/
bool debug = true;  // Enable debug output for engine information

#include <Arduino.h>
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "potentiometer.h"

//#include <MIDI.h>
//#include <mozzi_midi.h>

long midiTimer;

float pitch_offset = 36;
float max_voltage_of_adc = 3.3;
float voltage_division_ratio = 0.3333333333333;
float notes_per_octave = 12;
float volts_per_octave = 1;

float mapping_upper_limit = (max_voltage_of_adc / voltage_division_ratio) * notes_per_octave * volts_per_octave;
/*

  struct Serial1MIDISettings : public midi::DefaultSettings
  {
  static const long BaudRate = 31250;
  static const int8_t TxPin  = 12u;
  static const int8_t RxPin  = 13u;
  };

  MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDI, Serial1MIDISettings);
*/

#include <hardware/pwm.h>
#include <PWMAudio.h>

#define SAMPLERATE 48000
//#define PWMOUT A0
#define PWMOUT D7
#define BUTTON_PIN D4 // D8 on the seeed board
#define TRIG_PIN D5 // D7 on mod2?
#define LED 13

#include "utility.h"
#include <STMLIB.h>
#include <BRAIDS.h>
#include "braids.h"


#include <Bounce2.h>
Bounce2::Button button = Bounce2::Button();

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio

// Braids Renaissance engine names (56 total, 0-55)
// Based on Braids Renaissance alternative firmware by Tom Burns
// https://github.com/boourns/eurorack/tree/master/braids
const char* engineNames[56] = {
  // Analog-style oscillators
  "CSAW",      // 0 - Classic sawtooth
  "MORPH",     // 1 - Morphing saw/square
  "SAW_SQ",    // 2 - Saw and square mix
  "SIN_TRI",   // 3 - Sine/triangle (Renaissance: was FOLD)
  "BUZZ",      // 4 - Buzz (Renaissance: was SQ_SUB)

  // Analog with sub-oscillator
  "SQ_SUB",    // 5 - Square with sub
  "SAW_SUB",   // 6 - Sawtooth with sub

  // Sync oscillators
  "SQ_SYNC",   // 7 - Square sync
  "SAW_SYNC",  // 8 - Saw sync

  // Triple oscillators (3x unison)
  "SAW_3",     // 9 - Triple saw
  "SQ_3",      // 10 - Triple square
  "TRI_3",     // 11 - Triple triangle
  "SIN_3",     // 12 - Triple sine
  "RING_3",    // 13 - Triple ring mod

  // Chord modes (NEW - Renaissance feature)
  "CH_SAW",    // 14 - Chord saw
  "CH_SQ",     // 15 - Chord square
  "CH_TRI",    // 16 - Chord triangle
  "CH_SIN",    // 17 - Chord sine
  "CH_WT",     // 18 - Chord wavetable

  // Stack modes (NEW - Renaissance feature, 6-voice)
  "STK_SAW",   // 19 - Stack saw (6x)
  "STK_SQ",    // 20 - Stack square (6x)
  "STK_TRI",   // 21 - Stack triangle (6x)
  "STK_SIN",   // 22 - Stack sine (6x)
  "STK_WT",    // 23 - Stack wavetable (6x)

  // Swarm and comb
  "SWARM",     // 24 - Saw swarm
  "SAW_COMB",  // 25 - Sawtooth comb
  "TOY",       // 26 - Toy synthesizer

  // Filtered oscillators
  "ZLPF",      // 27 - Low-pass filtered saw
  "ZPKF",      // 28 - Peak filtered saw
  "ZBPF",      // 29 - Band-pass filtered saw
  "ZHPF",      // 30 - High-pass filtered saw

  // Formant synthesis
  "VOSIM",     // 31 - VOSIM formant
  "VOWEL",     // 32 - Vowel synthesis
  "VOW_FOF",   // 33 - FOF vowel synthesis

  // Additive/harmonic
  "HARM",      // 34 - Harmonic oscillator

  // FM synthesis
  "FM",        // 35 - 2-operator FM
  "FBFM",      // 36 - Feedback FM
  "CHAOFM",    // 37 - Chaotic feedback FM

  // Physical modeling
  "PLUCK",     // 38 - Karplus-Strong plucked string
  "BOW",       // 39 - Bowed string
  "BLOW",      // 40 - Blown pipe
  "FLUTE",     // 41 - Flute model

  // Struck/percussive models
  "BELL",      // 42 - Struck bell
  "DRUM",      // 43 - Struck drum
  "KICK",      // 44 - Kick drum
  "CYMBAL",    // 45 - Cymbal/hat
  "SNARE",     // 46 - Snare drum

  // Wavetable
  "WTBL",      // 47 - Wavetable
  "WMAP",      // 48 - Wavemap
  "WLIN",      // 49 - Wave terrain/line

  // SAM modes (NEW - Renaissance feature)
  "SAM1",      // 50 - SAM vocoder bank 1
  "SAM2",      // 51 - SAM vocoder bank 2

  // Noise/granular
  "NOISE",     // 52 - Filtered noise
  "TWNQ",      // 53 - Twin peaks noise
  "CLKN",      // 54 - Clocked noise
  "CLOUD",     // 55 - Granular cloud
  "PRTC"       // 56 - Particle noise (Note: array size 56 but max index 55)
};

int engineCount = 0;
int engineInc = 0;
bool longPressHandled = false;

// clock timer  stuff

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     4

// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "RPi_Pico_TimerInterrupt.h"

//unsigned int SWPin = CLOCKIN;

#define TIMER0_INTERVAL_MS 20.833333333333

//24.390243902439025 // 44.1
// \20.833333333333 running at 48Khz
// 10.416666666667  96kHz

#define DEBOUNCING_INTERVAL_MS   2// 80
#define LOCAL_DEBUG              0

volatile int counter = 0;

// Init RPI_PICO_Timer, can use any from 0-15 pseudo-hardware timers
RPI_PICO_Timer ITimer0(0);

bool TimerHandler0(struct repeating_timer *t) {
  (void) t;
  bool sync = true;
  if ( DAC.availableForWrite()) {
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
      DAC.write( voices[0].pd.buffer[i], sync);
    }
    counter =  1;
  }

  return true;
}

void cb() {
  bool sync = true;
  if (DAC.availableForWrite() >= BLOCK_SIZE) {
    for (int i = 0; i <  BLOCK_SIZE; i++) {
      // out = ;   // left channel called .aux
      DAC.write( voices[0].pd.buffer[i]);
    }
  }
}

void HandleNoteOn(byte channel, byte note, byte velocity) {
  pitch_in = note << 7;
  trigger_in = velocity / 127.0;

  //aSin.setFreq(mtof(float(note)));
  //envelope.noteOn();
  //digitalWrite(LED, HIGH);
}
void HandleNoteOff(byte channel, byte note, byte velocity) {

  trigger_in = 0.0f;

  //aSin.setFreq(mtof(float(note)));
  //envelope.noteOn();
  //digitalWrite(LED, LOW);
}

void setup() {

  if (debug) {
    Serial.begin(57600);
    Serial.println(F("=== MOD2 BRAIDS FIRMWARE ==="));
  }

  // Set default engine
  engineCount = 22;  // PLUCK (default)
  engine_in = engineCount;
  if (debug) {
    Serial.print(F("Default engine: "));
    Serial.print(engineCount);
    Serial.print(F(" - "));
    Serial.println(engineNames[engineCount]);
  }

  analogReadResolution(12);
  // thi is to switch to PWM for power to avoid ripple noise
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
  
  pinMode(TRIG_PIN, INPUT_PULLDOWN);
  pinMode(AIN0, INPUT);
  pinMode(AIN1, INPUT);
  pinMode(AIN2, INPUT);
  pinMode(SCL, INPUT_PULLDOWN);

  pinMode(LED, OUTPUT);
  //MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  //MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  // Initiate MIDI communications, listen to all channels (not needed with Teensy usbMIDI)
  //MIDI.begin(MIDI_CHANNEL_OMNI);

  button.attach( BUTTON_PIN , INPUT_PULLUP);
  button.interval(5);
  button.setPressedState(LOW);

  // pwm timing setup, we're using a pseudo interrupt

  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS, TimerHandler0)) // that's 48kHz
  {
    if (debug) Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }  else {
    if (debug) Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
  }


  // set up Pico PWM audio output
  DAC.setBuffers(4, 32); // plaits::kBlockSize); // DMA buffers
  //DAC.onTransmit(cb);
  DAC.setFrequency(SAMPLERATE);
  DAC.begin();


  // init the braids voices
  initVoices();

  // Force oscillator to use our default engine (initVoices sets VOWEL_FOF)
  voices[0].pd.osc->set_shape(static_cast<braids::MacroOscillatorShape>(engine_in));

  if (debug) {
    Serial.print(F("Oscillator shape set to engine: "));
    Serial.println(engine_in);
  }

  // initial reading of the pots with debounce
  readpot(0);
  readpot(1);
  readpot(2);


  int16_t timbre = (map(potvalue[0], POT_MIN, POT_MAX, 0, 32767));
  timbre_in = timbre;

  int16_t morph = (map(potvalue[1], POT_MIN, POT_MAX, 0, 32767));
  morph_in = morph;

  // fm / pitch updates
  // int16_t  pitch = map(potvalue[2], POT_MIN, POT_MAX, 16383, 0); // convert pitch CV data value to valid range
  // pitch_in = pitch - 1638;
  // used to switch between FM and note on cv3

  midiTimer = millis();

}



void loop() {

  if ( counter > 0 ) {
    updateBraidsAudio();
    counter = 0; // increments on each pass of the timer after the timer writes samples
  }

}

// second core dedicated to display foo

void setup1() {
  delay (200); // wait for main core to start up perhipherals
}




// second core deals with ui / control rate updates
void loop1() {

  //MIDI.read();
  uint32_t now = millis();
  // pot updates
  // reading A/D seems to cause noise in the audio so don't do it too often

  int16_t timbre = (map(potvalue[0], POT_MIN, POT_MAX, 0, 32767));
  timbre_in = timbre;
  int16_t morph = (map(potvalue[1], POT_MIN, POT_MAX, 0, 32767));
  morph_in = morph;

  // fm / pitch updates
  // int16_t  pitch = map(potvalue[2], POT_MIN, POT_MAX, 170, 0); // convert pitch CV data value to valid range
  // pitch_fm = pitch;

  int16_t pitch = map(potvalue[2], POT_MIN, POT_MAX, 3072, 8192); // convert pitch CV data value to valid range
  int16_t pitch_delta = abs(previous_pitch - pitch);
  if (pitch_delta > 10) {
    pitch_in = pitch;
    previous_pitch = pitch;
    //trigger_in = 1.0f; //retain for cv only input?
  }

  button.update();

  // Check for long press first (>500ms): decrement engine
  if (button.isPressed() && button.currentDuration() > 500) {
    if (!longPressHandled) {
      engineCount--;
      if (engineCount < 0) {
        engineCount = 55;  // Renaissance: 56 engines (0-55)
      }
      engine_in = engineCount;
      longPressHandled = true;

      if (debug) {
        Serial.print(F("Engine (long): "));
        Serial.print(engineCount);
        Serial.print(F(" - "));
        Serial.println(engineNames[engineCount]);
      }
    }
  } else if (button.released()) {
    // Short press: increment engine (only trigger on release if it wasn't a long press)
    if (!longPressHandled) {
      engineCount++;
      if (engineCount > 55) {  // Renaissance: 56 engines (0-55)
        engineCount = 0;
      }
      engine_in = engineCount;

      if (debug) {
        Serial.print(F("Engine: "));
        Serial.print(engineCount);
        Serial.print(F(" - "));
        Serial.println(engineNames[engineCount]);
      }
    }
    longPressHandled = false;  // Reset flag when button is released
  }

  // reading A/D seems to cause noise in the audio so don't do it too often


  if ((now - pot_timer) > POT_SAMPLE_TIME) {
    readpot(0);
    readpot(1);
    readpot(2);
    if (digitalRead(TRIG_PIN) ) {
      trigger_in = 1.0f;
      digitalWrite(LED, HIGH);  // Turn LED on when trigger received
    } else {
      trigger_in = 0.0f;
      digitalWrite(LED, LOW);   // Turn LED off when no trigger
    }
    pot_timer = now;
  }






}