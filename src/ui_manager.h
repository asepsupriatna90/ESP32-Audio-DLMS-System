#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include "config.h"

// Initialize UI components
void setupUI();

// Update LCD Display based on current mode
void updateLCD();

// Handle UI inputs
void handleEncoder();
void handleButton();
void handleSaveButton();

// Check system temperature and status
void updateSystemStatus();

// External access to