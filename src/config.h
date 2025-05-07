#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// EEPROM configuration
#define EEPROM_SIZE 2048

// Button pins
#define BUTTON_PIN 0
#define SAVE_BUTTON_PIN 2

// Audio parameters
#define MIN_GAIN -60
#define MAX_GAIN 12

// I2S pins configuration
#define I2S_BCLK 26  // Bit clock
#define I2S_WCLK 25  // Word/LR clock
#define I2S_DOUT 22  // Data out (ESP32 -> DAC)
#define I2S_DIN  19  // Data in (ADC -> ESP32)
#define I2S_MCLK 0   // Master clock (optional, set to 0 if not used)

// I2S configuration
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define DMA_BUF_COUNT 8
#define DMA_BUF_LEN 64

// Power amplifier status pins
#define AMP_STATUS_SUB 4
#define AMP_STATUS_LOW 5
#define AMP_STATUS_MID 12
#define AMP_STATUS_HIGH 13

// Thermal protection pins
#define TEMP_SENSOR_PIN 36  // ADC pin for thermistor

// LCD & Encoder pins
#define ENCODER_PIN_A 32 
#define ENCODER_PIN_B 33
#define LCD_I2C_ADDR 0x27

// WiFi configuration
#define WIFI_SSID "ESP32-AUDIO"
#define WIFI_PASSWORD "12345678"
#define WIFI_TIMEOUT 30000  // 30 seconds

// Audio processing parameters
#define AUDIO_BLOCK_SIZE 64

// Preset configuration
#define PRESET_COUNT 5

// Manual UI Modes
enum Mode { HOME, EDIT_GAIN, EDIT_CROSSOVER, EDIT_COMP, EDIT_DELAY, SYSTEM_INFO };

// Gain settings per preset
struct GainPreset { 
  float gainSub, gainLow, gainMid, gainHigh; 
};

// AudioPreset (for DSP/web)
struct AudioPreset {
  // Crossover frequencies
  float fcSub, fcLow, fcMid, fcHigh;
  
  // Compressor settings
  float threshold;
  float ratio;
  float attack;
  float release;
  
  // Limiter settings
  float limiterThresh;
  
  // Delay settings (ms)
  float delayLow, delayLowMid, delayHighMid, delayHigh;
  
  // Individual band settings
  bool subEnabled, lowEnabled, midEnabled, highEnabled;
};

// System status
struct SystemStatus {
  float temperature;
  bool subProtection;
  bool lowProtection;
  bool midProtection;
  bool highProtection;
  unsigned long uptime;
  float cpuTemp;
};

#endif // CONFIG_H