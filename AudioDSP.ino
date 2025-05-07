/*
 * ESP32 Audio DSP System
 * 
 * A complete audio digital signal processing system built on ESP32
 * Features:
 * - 4-band audio processing (Sub, Low, Mid, High)
 * - Crossover filters
 * - Compression and limiting
 * - Time alignment (delay)
 * - LCD UI with rotary encoder
 * - Web interface for remote control
 * - Multiple presets with EEPROM storage
 * - Protection monitoring
 */

#include "src/config.h"
#include "src/audio_dsp.h"
#include "src/preset_manager.h"
#include "src/ui_manager.h"
#include "src/web_server.h"

// System status tracking
SystemStatus sysStatus = {0, false, false, false, false, 0, 0};

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Audio DSP System starting...");
  
  // Initialize subsystems
  initConfig();
  initPresetManager();
  initAudioDSP();
  initUIManager();
  initWebServer();
  
  Serial.println("System initialization complete");
}

void loop() {
  // Handle user interactions
  handleUIEvents();
  
  // Handle web server requests
  handleWebRequests();
  
  // Process audio if enabled
  if (isAudioProcessingEnabled()) {
    processAudioFrames();
  }
  
  // Update system status periodically
  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > STATUS_UPDATE_INTERVAL) {
    updateSystemStatus();
    if (isSystemInfoDisplayed()) {
      refreshDisplay();
    }
    lastStatusUpdate = millis();
  }
  
  // Check WiFi connection if not in AP mode
  if (!isAPMode() && WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > WIFI_RECONNECT_INTERVAL) {
      Serial.println("WiFi connection lost, attempting to reconnect...");
      WiFi.reconnect();
      lastReconnectAttempt = millis();
    }
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}

// Update system status information
void updateSystemStatus() {
  // Read thermistor
  int rawTemp = analogRead(TEMP_SENSOR_PIN);
  sysStatus.temperature = map(rawTemp, 0, 4095, 20, 100); // Map ADC to temperature
  
  // Read amplifier protection status
  sysStatus.subProtection = !digitalRead(AMP_STATUS_SUB);
  sysStatus.lowProtection = !digitalRead(AMP_STATUS_LOW);
  sysStatus.midProtection = !digitalRead(AMP_STATUS_MID);
  sysStatus.highProtection = !digitalRead(AMP_STATUS_HIGH);
  
  // Update uptime
  sysStatus.uptime = millis() / 1000; // Seconds
  
  // Get ESP32 temperature
  sysStatus.cpuTemp = (temprature_sens_read() - 32) / 1.8; // Convert to Celsius
  
  // Update status in UI manager
  updateUIStatus(sysStatus);
}

// Helper function used to read ESP32 internal temperature
extern "C" {
  uint8_t temprature_sens_read();
}
