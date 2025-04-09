/*
 * Config.h
 * Configuration structures and EEPROM handling
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "Constants.h"
#include <Arduino.h>

//=====================================================================
// CONFIGURATION STRUCTURE
//=====================================================================
struct Config {
    float accelSpikeThreshold;
    int soundSpikeThreshold;
    unsigned long sensorReadInterval;
    unsigned long mqttPublishInterval;
    bool anomalyDetectionEnabled;
    float anomalyThresholdMultiplier;
    int offlineBufferSize;
    byte configSaved;
};

// Global configuration variable
extern Config config;

// Default configuration
extern const Config DEFAULT_CONFIG;

//=====================================================================
// FUNCTION PROTOTYPES
//=====================================================================
// Load configuration from EEPROM
void loadConfigFromEEPROM();

// Save configuration to EEPROM
void saveConfigToEEPROM();

// Process configuration message from MQTT
void processConfigMessage(char* payload, unsigned int length);

// Process command message from MQTT
void processCommandMessage(char* payload, unsigned int length);

#endif // CONFIG_H
