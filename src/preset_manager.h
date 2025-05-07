#ifndef PRESET_MANAGER_H
#define PRESET_MANAGER_H

#include <Arduino.h>
#include "config.h"

// External access to preset data
extern String presetNames[PRESET_COUNT];
extern int currentPresetIndex;
extern GainPreset gainPresets[PRESET_COUNT];
extern AudioPreset presets[PRESET_COUNT];

// Initialize and load presets from EEPROM
void initPresets();

// Save/load GainPreset
void saveGainPreset(int idx);
void loadGainPresets();

// Save/load AudioPreset
void savePreset(int idx);
void loadPresets();

#endif // PRESET_MANAGER_H