/*
 * DataProcessing.cpp
 * Data analysis and anomaly detection implementation
 */

#include "DataProcessing.h"
#include "Sensors.h"
#include "Config.h"
#include <ArduinoJson.h>
#include <math.h>

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
// Anomaly Detection Variables
float tempHistory[HISTORY_SIZE] = {0};
float humidityHistory[HISTORY_SIZE] = {0};
float accelHistory[HISTORY_SIZE] = {0};
float soundHistory[HISTORY_SIZE] = {0};
float gasHistory[HISTORY_SIZE] = {0};
int historyIndex = 0;
bool historyFilled = false;

// External MQTT client
extern PubSubClient mqttClient;

//=====================================================================
// DATA ANALYSIS FUNCTIONS
//=====================================================================
void updateSensorHistory(float value, float* history) {
  history[historyIndex] = value;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  if (historyIndex == 0) {
    historyFilled = true;
  }
}

float calculateMean(float* history) {
  float sum = 0.0;
  int count = historyFilled ? HISTORY_SIZE : historyIndex;
  
  if (count == 0) return 0.0;
  
  for (int i = 0; i < count; i++) {
    sum += history[i];
  }
  
  return sum / count;
}

float calculateStdDev(float* history) {
  float mean = calculateMean(history);
  float sumSquaredDiff = 0.0;
  int count = historyFilled ? HISTORY_SIZE : historyIndex;
  
  if (count == 0) return 0.0;
  
  for (int i = 0; i < count; i++) {
    float diff = history[i] - mean;
    sumSquaredDiff += diff * diff;
  }
  
  return sqrt(sumSquaredDiff / count);
}

void checkForAnomalies() {
  // Get current values
  float tempMean = calculateMean(tempHistory);
  float tempStdDev = calculateStdDev(tempHistory);
  float humidityMean = calculateMean(humidityHistory);
  float humidityStdDev = calculateStdDev(humidityHistory);
  float accelMean = calculateMean(accelHistory);
  float accelStdDev = calculateStdDev(accelHistory);
  float gasMean = calculateMean(gasHistory);
  float gasStdDev = calculateStdDev(gasHistory);
  
  // Temperature anomaly check
  if (abs(temperature - tempMean) > config.anomalyThresholdMultiplier * tempStdDev && 
      tempStdDev > MIN_TEMP_STD_DEV) {
    
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["alert"] = "anomaly_detected";
    jsonDoc["sensor"] = "temperature";
    jsonDoc["value"] = temperature;
    jsonDoc["mean"] = tempMean;
    jsonDoc["std_dev"] = tempStdDev;
    jsonDoc["z_score"] = (temperature - tempMean) / tempStdDev;
    
    char jsonBuffer[256];
    serializeJson(jsonDoc, jsonBuffer);
    mqttClient.publish(MQTT_ANOMALIES_TOPIC, jsonBuffer);
    
    Serial.println("Temperature anomaly detected!");
  }
  
  // Gas anomaly check - particularly important for safety
  if (abs(gasRatio - gasMean) > config.anomalyThresholdMultiplier * gasStdDev && 
      gasStdDev > MIN_GAS_STD_DEV) {
    
    float co_ppm = getCO_ppm(gasRatio);
    
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["alert"] = "anomaly_detected";
    jsonDoc["sensor"] = "gas";
    jsonDoc["value"] = co_ppm;
    jsonDoc["mean"] = getCO_ppm(gasMean);
    jsonDoc["std_dev"] = gasStdDev;
    jsonDoc["z_score"] = (gasRatio - gasMean) / gasStdDev;
    
    char jsonBuffer[256];
    serializeJson(jsonDoc, jsonBuffer);
    mqttClient.publish(MQTT_ANOMALIES_TOPIC, jsonBuffer);
    
    // Flash warning LED for gas anomalies
    for (int i = 0; i < 3; i++) {
      digitalWrite(LEDR, HIGH);
      delay(LED_BLINK_MEDIUM);
      digitalWrite(LEDR, LOW);
      delay(LED_BLINK_MEDIUM);
    }
    
    Serial.println("Gas level anomaly detected!");
  }
}