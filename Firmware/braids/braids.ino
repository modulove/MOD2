/*
  XIAO RA4M1 Braids Port - Step 1: Basic DAC Test
  Tests:
  - DAC output functionality
  - Timer interrupt for audio rate
  - Basic sine wave generation
*/

#include <Arduino.h>
//#include <FspTimer.h>

// ===== CONFIGURATION =====
#define DAC_PIN A0          // RA4M1 has hardware DAC on A0
#define SAMPLERATE 48000    // Audio sample rate
#define BLOCK_SIZE 32       // Samples per block
#define LED LED_BUILTIN

// ===== AUDIO VARIABLES =====
volatile bool buffer_ready = false;
int16_t audio_buffer[BLOCK_SIZE];
uint32_t phase_accumulator = 0;
uint32_t phase_increment = 0;

// Timer for audio interrupt
FspTimer audio_timer;

// ===== TIMER CALLBACK =====
void audioTimerCallback(timer_callback_args_t __attribute((unused)) *p_args) {
    static uint8_t sample_index = 0;
    
    // Output one sample to DAC
    if (sample_index < BLOCK_SIZE) {
        // Convert signed 16-bit to unsigned 12-bit for DAC (0-4095)
        int32_t sample = audio_buffer[sample_index];
        uint16_t dac_value = (sample + 32768) >> 4;  // Scale to 12-bit
        
        // Write to DAC
        analogWrite(DAC_PIN, dac_value);
        
        sample_index++;
    } else {
        sample_index = 0;
        buffer_ready = true;  // Signal main loop to prepare next buffer
    }
}

// ===== GENERATE TEST SINE WAVE =====
void generateSineWave(int16_t* buffer, uint32_t frequency) {
    // Simple sine wave using lookup table
    static const int16_t sine_table[32] = {
        0, 6393, 12540, 18205, 23170, 27246, 30274, 32138,
        32767, 32138, 30274, 27246, 23170, 18205, 12540, 6393,
        0, -6393, -12540, -18205, -23170, -27246, -30274, -32138,
        -32767, -32138, -30274, -27246, -23170, -18205, -12540, -6393
    };
    
    phase_increment = (frequency * 4294967296ULL) / SAMPLERATE;
    
    for (int i = 0; i < BLOCK_SIZE; i++) {
        uint8_t index = (phase_accumulator >> 27) & 0x1F;  // Get top 5 bits for table index
        buffer[i] = sine_table[index] >> 2;  // Reduce amplitude to 1/4
        phase_accumulator += phase_increment;
    }
}

// ===== SETUP =====
void setup() {
    Serial.begin(115200);
    delay(1000);  // Wait for serial
    
    Serial.println("XIAO RA4M1 Braids Port - Step 1: DAC Test");
    Serial.println("Testing 440Hz sine wave output on pin A0");
    
    // Configure LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    
    // Configure DAC pin
    pinMode(DAC_PIN, OUTPUT);
    analogWriteResolution(12);  // Set DAC to 12-bit resolution
    
    // Prepare initial audio buffer with 440Hz sine
    generateSineWave(audio_buffer, 440);
    
    // Setup timer for audio rate
    uint8_t timer_type = GPT_TIMER;
    int8_t tindex = FspTimer::get_available_timer(timer_type);
    
    if (tindex < 0) {
        Serial.println("ERROR: No timer available!");
        while(1) { digitalWrite(LED, !digitalRead(LED)); delay(100); }
    }
    
    // Calculate timer frequency: SAMPLERATE Hz
    float timer_freq = SAMPLERATE;
    
    Serial.print("Timer index: ");
    Serial.println(tindex);
    Serial.print("Timer frequency: ");
    Serial.print(timer_freq);
    Serial.println(" Hz");
    
    // Initialize timer
    audio_timer.begin(TIMER_MODE_PERIODIC, timer_type, tindex, 
                      timer_freq, 0.0f, audioTimerCallback);
    audio_timer.setup_overflow_irq();
    audio_timer.open();
    audio_timer.start();
    
    Serial.println("Audio timer started!");
    Serial.println("You should hear a 440Hz tone on DAC pin A0");
}

// ===== MAIN LOOP =====
void loop() {
    static unsigned long last_print = 0;
    static unsigned long buffer_count = 0;
    static bool led_state = false;
    
    // Check if we need to prepare next audio buffer
    if (buffer_ready) {
        buffer_ready = false;
        
        // Generate next buffer of samples
        generateSineWave(audio_buffer, 440);  // Keep generating 440Hz
        
        buffer_count++;
        
        // Toggle LED every 1000 buffers (visual feedback)
        if (buffer_count % 1000 == 0) {
            led_state = !led_state;
            digitalWrite(LED, led_state);
        }
    }
    
    // Print status every second
    unsigned long now = millis();
    if (now - last_print >= 1000) {
        last_print = now;
        Serial.print("Buffers processed: ");
        Serial.println(buffer_count);
        Serial.print("Effective sample rate: ");
        Serial.print((buffer_count * BLOCK_SIZE) / (now / 1000.0));
        Serial.println(" Hz");
    }
}