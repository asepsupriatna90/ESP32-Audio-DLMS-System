#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#include <Arduino.h>
#include "config.h"

// Initialize I2S for audio input/output
void setupI2S();

// Process audio continuously
void processAudio();

// Process a single audio frame with DSP
void processAudioFrame(int16_t* input, int16_t* output, int frames, AudioPreset &preset, GainPreset &gain);

// Apply DSP settings
void applyDSPSettings(AudioPreset &preset);

// Simple IIR filter implementation
float applyLowPassFilter(float input, float cutoff, float sampleRate, float *z);
float applyHighPassFilter(float input, float cutoff, float sampleRate, float *z);
float applyBandPassFilter(float input, float lowCutoff, float highCutoff, float sampleRate, float *zLow, float *zHigh);

// Compressor implementation
float applyCompressor(float input, float threshold, float ratio, float *envState, float attack, float release);

// Limiter implementation
float applyLimiter(float input, float threshold, float *limiterState);

// Apply delay to a sample
float applyDelay(float input, float delayMs, int band, float* delayBufferL, float* delayBufferR, bool isLeftChannel);

// External access to audio processing flag
extern bool audioProcessingEnabled;

// DSP buffer and variables
extern float dspBufferL[4][256]; // 4 bands, 256 samples per band
extern float dspBufferR[4][256]; // 4 bands, 256 samples per band
extern float delayBufferL[4][8820]; // Max 200ms at 44.1kHz
extern float delayBufferR[4][8820]; // Max 200ms at 44.1kHz
extern int delayReadIndex[4];
extern int delayWriteIndex[4];

#endif // AUDIO_DSP_H