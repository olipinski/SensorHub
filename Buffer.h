/*
 * Buffer.h
 * Offline data buffer management
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <Arduino.h>
#include "Constants.h"

//=====================================================================
// DATA STRUCTURES
//=====================================================================
// Network buffer for offline storage
struct SensorReading {
  unsigned long timestamp;
  float temperature;
  float humidity;
  float accelMagnitude;
  float gasRatio;
  float soundLevel;
  float batteryPercentage;
  bool valid;
};

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
extern SensorReading offlineBuffer[MAX_BUFFER_SIZE];
extern int bufferWriteIndex;
extern int bufferReadIndex;
extern int bufferCount;
extern bool networkWasDown;

//=====================================================================
// FUNCTION PROTOTYPES
//=====================================================================
// Initialize the offline buffer
void initializeOfflineBuffer();

// Store current sensor readings in buffer
void storeReadingInBuffer();

// Send all buffered data to MQTT broker
void sendBufferedData();

#endif // BUFFER_H