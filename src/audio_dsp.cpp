#include "audio_dsp.h"
#include <driver/i2s.h>
#include <math.h>

// DSP buffer and variables
float dspBufferL[4][256]; // 4 bands, 256 samples per band
float dspBufferR[4][256]; // 4 bands, 256 samples per band
float delayBufferL[4][8820]; // Max 200ms at 44.1kHz
float delayBufferR[4][8820]; // Max 200ms at 44.1kHz
int delayReadIndex[4] = {0, 0, 0, 0};
int delayWriteIndex[4] = {0, 0, 0, 0};

// Audio processing flag
bool audioProcessingEnabled = true;

// Initialize I2S for audio input/output
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WCLK,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_DIN
  };
  
  // Install and configure I2S driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  
  // If using MCLK (Master Clock)
  if (I2S_MCLK > 0) {
    // Set MCLK output. This might need additional setup depending on ESP32 version
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
  }
  
  // Start I2S
  i2s_start(I2S_NUM_0);
}

// Simple IIR filter implementation
float applyLowPassFilter(float input, float cutoff, float sampleRate, float *z) {
  float alpha = 2.0f * M_PI * cutoff / sampleRate;
  float a0 = alpha / (alpha + 1.0f);
  float b1 = 1.0f - a0;
  
  float output = a0 * input + b1 * (*z);
  *z = output;
  
  return output;
}

float applyHighPassFilter(float input, float cutoff, float sampleRate, float *z) {
  float alpha = 2.0f * M_PI * cutoff / sampleRate;
  float a0 = 1.0f / (alpha + 1.0f);
  float a1 = -a0;
  float b1 = (1.0f - alpha) * a0;
  
  float output = a0 * input + a1 * (*z);
  *z = input;
  
  return output;
}

// Band-pass filter (combine low-pass and high-pass)
float applyBandPassFilter(float input, float lowCutoff, float highCutoff, float sampleRate, float *zLow, float *zHigh) {
  float temp = applyHighPassFilter(input, lowCutoff, sampleRate, zHigh);
  return applyLowPassFilter(temp, highCutoff, sampleRate, zLow);
}

// Compressor implementation
float applyCompressor(float input, float threshold, float ratio, float *envState, float attack, float release) {
  // Calculate envelope follower (simplified RMS)
  float inputAbs = fabs(input);
  float attackCoef = exp(-1.0f / (attack * SAMPLE_RATE));
  float releaseCoef = exp(-1.0f / (release * SAMPLE_RATE));
  
  // Envelope follower
  if (inputAbs > *envState) {
    *envState = attackCoef * (*envState) + (1.0f - attackCoef) * inputAbs;
  } else {
    *envState = releaseCoef * (*envState) + (1.0f - releaseCoef) * inputAbs;
  }
  
  // Convert to dB
  float envdB = 20.0f * log10f(max(*envState, 1e-10f));
  
  // Apply compression if above threshold
  float gaindB = 0.0f;
  if (envdB > threshold) {
    gaindB = threshold + (envdB - threshold) / ratio - envdB;
  }
  
  // Convert gain back to linear and apply
  float gain = pow(10.0f, gaindB / 20.0f);
  return input * gain;
}

// Limiter implementation
float applyLimiter(float input, float threshold, float *limiterState) {
  // Calculate envelope follower (peak follower)
  float inputAbs = fabs(input);
  float attackTime = 0.001f; // Fast attack for limiting
  float releaseTime = 0.1f; // Slower release
  float attackCoef = exp(-1.0f / (attackTime * SAMPLE_RATE));
  float releaseCoef = exp(-1.0f / (releaseTime * SAMPLE_RATE));
  
  // Envelope follower
  if (inputAbs > *limiterState) {
    *limiterState = attackCoef * (*limiterState) + (1.0f - attackCoef) * inputAbs;
  } else {
    *limiterState = releaseCoef * (*limiterState) + (1.0f - releaseCoef) * inputAbs;
  }
  
  // Convert to dB
  float envdB = 20.0f * log10f(max(*limiterState, 1e-10f));
  
  // Apply limiting if above threshold
  float gaindB = 0.0f;
  if (envdB > threshold) {
    gaindB = threshold - envdB; // Hard limit at threshold
  }
  
  // Convert gain back to linear and apply
  float gain = pow(10.0f, gaindB / 20.0f);
  return input * gain;
}

// Apply delay to a sample
float applyDelay(float input, float delayMs, int band, float* delayBufferL, float* delayBufferR, bool isLeftChannel) {
  float* delayBuffer = isLeftChannel ? delayBufferL : delayBufferR;
  
  // Calculate delay in samples
  int delaySamples = (int)(delayMs * SAMPLE_RATE / 1000.0f);
  if (delaySamples <= 0) return input; // No delay
  
  // Make sure we don't exceed buffer size
  delaySamples = min(delaySamples, 8820);
  
  // Write current sample to buffer
  delayBuffer[band * 8820 + delayWriteIndex[band]] = input;
  
  // Calculate read index
  int readIndex = (delayWriteIndex[band] - delaySamples + 8820) % 8820;
  
  // Read delayed sample
  float delayedSample = delayBuffer[band * 8820 + readIndex];
  
  // Update write index
  delayWriteIndex[band] = (delayWriteIndex[band] + 1) % 8820;
  
  return delayedSample;
}

// Process a single audio frame with DSP
void processAudioFrame(int16_t* input, int16_t* output, int frames, AudioPreset &preset, GainPreset &gain) {
  static float lpfStateSub[2] = {0.0f, 0.0f}; // [L,R]
  static float lpfStateLow[2] = {0.0f, 0.0f};
  static float hpfStateLow[2] = {0.0f, 0.0f};
  static float lpfStateMid[2] = {0.0f, 0.0f};
  static float hpfStateMid[2] = {0.0f, 0.0f};
  static float lpfStateHigh[2] = {0.0f, 0.0f};
  static float hpfStateHigh[2] = {0.0f, 0.0f};

  static float compStateSub[2] = {0.0f, 0.0f};
  static float compStateLow[2] = {0.0f, 0.0f};
  static float compStateMid[2] = {0.0f, 0.0f};
  static float compStateHigh[2] = {0.0f, 0.0f};
  
  static float limiterState[2] = {0.0f, 0.0f};
  
  for (int i = 0; i < frames * 2; i += 2) {
    // Convert int16 to float (-1.0 to 1.0)
    float sampleL = (float)input[i] / 32768.0f;
    float sampleR = (float)input[i+1] / 32768.0f;
    
    // Crossover filters
    float subL = applyLowPassFilter(sampleL, preset.fcSub, SAMPLE_RATE, &lpfStateSub[0]);
    float subR = applyLowPassFilter(sampleR, preset.fcSub, SAMPLE_RATE, &lpfStateSub[1]);
    
    float lowL = applyBandPassFilter(sampleL, preset.fcSub, preset.fcLow, SAMPLE_RATE, &lpfStateLow[0], &hpfStateLow[0]);
    float lowR = applyBandPassFilter(sampleR, preset.fcSub, preset.fcLow, SAMPLE_RATE, &lpfStateLow[1], &hpfStateLow[1]);
    
    float midL = applyBandPassFilter(sampleL, preset.fcLow, preset.fcMid, SAMPLE_RATE, &lpfStateMid[0], &hpfStateMid[0]);
    float midR = applyBandPassFilter(sampleR, preset.fcLow, preset.fcMid, SAMPLE_RATE, &lpfStateMid[1], &hpfStateMid[1]);
    
    float highL = applyHighPassFilter(sampleL, preset.fcMid, SAMPLE_RATE, &hpfStateHigh[0]);
    float highR = applyHighPassFilter(sampleR, preset.fcMid, SAMPLE_RATE, &hpfStateHigh[1]);
    
    // Apply band enable/disable
    subL *= preset.subEnabled ? 1.0f : 0.0f;
    subR *= preset.subEnabled ? 1.0f : 0.0f;
    lowL *= preset.lowEnabled ? 1.0f : 0.0f;
    lowR *= preset.lowEnabled ? 1.0f : 0.0f;
    midL *= preset.midEnabled ? 1.0f : 0.0f;
    midR *= preset.midEnabled ? 1.0f : 0.0f;
    highL *= preset.highEnabled ? 1.0f : 0.0f;
    highR *= preset.highEnabled ? 1.0f : 0.0f;
    
    // Apply gain per band (convert dB to linear)
    subL *= pow(10.0f, gain.gainSub / 20.0f);
    subR *= pow(10.0f, gain.gainSub / 20.0f);
    lowL *= pow(10.0f, gain.gainLow / 20.0f);
    lowR *= pow(10.0f, gain.gainLow / 20.0f);
    midL *= pow(10.0f, gain.gainMid / 20.0f);
    midR *= pow(10.0f, gain.gainMid / 20.0f);
    highL *= pow(10.0f, gain.gainHigh / 20.0f);
    highR *= pow(10.0f, gain.gainHigh / 20.0f);
    
    // Apply compression per band
    subL = applyCompressor(subL, preset.threshold, preset.ratio, &compStateSub[0], preset.attack, preset.release);
    subR = applyCompressor(subR, preset.threshold, preset.ratio, &compStateSub[1], preset.attack, preset.release);
    lowL = applyCompressor(lowL, preset.threshold, preset.ratio, &compStateLow[0], preset.attack, preset.release);
    lowR = applyCompressor(lowR, preset.threshold, preset.ratio, &compStateLow[1], preset.attack, preset.release);
    midL = applyCompressor(midL, preset.threshold, preset.ratio, &compStateMid[0], preset.attack, preset.release);
    midR = applyCompressor(midR, preset.threshold, preset.ratio, &compStateMid[1], preset.attack, preset.release);
    highL = applyCompressor(highL, preset.threshold, preset.ratio, &compStateHigh[0], preset.attack, preset.release);
    highR = applyCompressor(highR, preset.threshold, preset.ratio, &compStateHigh[1], preset.attack, preset.release);
    
    // Apply delay per band
    lowL = applyDelay(lowL, preset.delayLow, 0, delayBufferL[0], delayBufferR[0], true);
    lowR = applyDelay(lowR, preset.delayLow, 0, delayBufferL[0], delayBufferR[0], false);
    midL = applyDelay(midL, preset.delayLowMid, 1, delayBufferL[1], delayBufferR[1], true);
    midR = applyDelay(midR, preset.delayLowMid, 1, delayBufferL[1], delayBufferR[1], false);
    highL = applyDelay(highL, preset.delayHighMid, 2, delayBufferL[2], delayBufferR[2], true);
    highR = applyDelay(highR, preset.delayHighMid, 2, delayBufferL[2], delayBufferR[2], false);
    
    // Sum all bands
    float mixL = subL + lowL + midL + highL;
    float mixR = subR + lowR + midR + highR;
    
    // Apply final limiter
    mixL = applyLimiter(mixL, preset.limiterThresh, &limiterState[0]);
    mixR = applyLimiter(mixR, preset.limiterThresh, &limiterState[1]);
    
    // Convert back to int16 (with clipping)
    output[i] = (int16_t)constrain(mixL * 32767.0f, -32768.0f, 32767.0f);
    output[i+1] = (int16_t)constrain(mixR * 32767.0f, -32768.0f, 32767.0f);
  }
}

// Process audio continuously
void processAudio() {
  if (!audioProcessingEnabled) return;
  
  size_t bytes_read = 0;
  size_t bytes_written = 0;
  int16_t i2s_input_buffer[AUDIO_BLOCK_SIZE*2];  // stereo
  int16_t i2s_output_buffer[AUDIO_BLOCK_SIZE*2]; // stereo
  
  // Read audio from I2S
  esp_err_t result = i2s_read(I2S_NUM_0, i2s_input_buffer, AUDIO_BLOCK_SIZE*4, &bytes_read, 0);
  
  if (result == ESP_OK && bytes_read > 0) {
    // Get current preset and gain from external reference (will be set by preset_manager)
    extern int currentPresetIndex;
    extern AudioPreset presets[];
    extern GainPreset gainPresets[];
    
    // Process audio with crossover, compression, etc.
    processAudioFrame(i2s_input_buffer, i2s_output_buffer, AUDIO_BLOCK_SIZE, 
                      presets[currentPresetIndex], gainPresets[currentPresetIndex]);
    
    // Write processed audio to I2S
    i2s_write(I2S_NUM_0, i2s_output_buffer, bytes_read, &bytes_written, 0);
  }
}

// Apply DSP settings
void applyDSPSettings(AudioPreset &preset) {
  // No need to send commands to external DSP chip as we're doing DSP in software
  // Just inform via serial for debugging
  Serial.println("Applying DSP settings:");
  Serial.println("Crossover: " + String(preset.fcSub) + ", " + 
                 String(preset.fcLow) + ", " + 
                 String(preset.fcMid) + ", " + 
                 String(preset.fcHigh) + " Hz");
  Serial.println("Compressor: T=" + String(preset.threshold) + 
                 "dB, R=" + String(preset.ratio) + 
                 ":1, A=" + String(preset.attack) + 
                 "s, R=" + String(preset.release) + "s");
  Serial.println("Limiter: " + String(preset.limiterThresh) + "dB");
  
  // Reset any DSP state variables if needed
}