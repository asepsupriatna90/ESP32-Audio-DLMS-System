#include "preset_manager.h"
#include <EEPROM.h>

// Preset data
String presetNames[PRESET_COUNT] = {"Default","Rock","Jazz","Dangdut","Pop"};
int currentPresetIndex = 0;
GainPreset gainPresets[PRESET_COUNT];
AudioPreset presets[PRESET_COUNT];

// Initialize and load presets from EEPROM
void initPresets() {
  EEPROM.begin(EEPROM_SIZE);
  loadGainPresets();
  loadPresets();
}

// EEPROM helpers
template<typename T> void eepromWrite(int addr, const T &val) { EEPROM.put(addr, val); }
template<typename T> void eepromRead(int addr, T &val)  { EEPROM.get(addr, val); }

// Save/load GainPreset
void saveGainPreset(int idx) {
  eepromWrite(idx * sizeof(GainPreset), gainPresets[idx]);
  EEPROM.commit();
}

void loadGainPresets() {
  for (int i = 0; i < PRESET_COUNT; i++) {
    int addr = i * sizeof(GainPreset);
    eepromRead(addr, gainPresets[i]);
    if (isnan(gainPresets[i].gainSub)) {
      // Initialize with default values if not set
      gainPresets[i] = {0,0,0,0}; 
      saveGainPreset(i);
    }
  }
}

// Save/load AudioPreset
void savePreset(int idx) {
  int addr = idx * sizeof(AudioPreset) + 512;
  eepromWrite(addr, presets[idx]);
  EEPROM.commit();
}

void loadPresets() {
  for (int i = 0; i < PRESET_COUNT; i++) {
    int addr = i * sizeof(AudioPreset) + 512;
    eepromRead(addr, presets[i]);
    if (isnan(presets[i].fcSub)) {
      // Default preset values
      presets[i] = {
        30, 100, 1000, 4000,  // Crossover frequencies
        -18, 4, 0.01, 0.05,   // Compressor settings
        -1,                   // Limiter threshold
        0, 0, 0, 0,           // Delay settings
        true, true, true, true // All bands enabled
      }; 
      savePreset(i);
    }
  }
}