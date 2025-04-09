/*
 * DataProcessing.h
 * Statistical analysis and anomaly detection
 */

#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <Arduino.h>
#include "Constants.h"

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
// Anomaly Detection Variables
extern float tempHistory[HISTORY_SIZE];
extern float humidityHistory[HISTORY_SIZE];
extern float accelHistory[HISTORY_SIZE];
extern float soundHistory[HISTORY_SIZE];
extern float gasHistory[HISTORY_SIZE];
extern int historyIndex;
extern bool historyFilled;

//=====================================================================
// FUNCTION PROTOTYPES
//=====================================================================
// Update sensor history arrays
void updateSensorHistory(float value, float* history);

// Calculate mean of history array
float calculateMean(float* history);

// Calculate standard deviation of history array
float calculateStdDev(float* history);

// Check for anomalies in sensor data
void checkForAnomalies();

#endif // DATA_PROCESSING_H