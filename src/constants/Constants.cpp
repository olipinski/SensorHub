/*
 * Constants.cpp
 * Definitions of constant values for Arduino IoT Sensor Hub
 */

#include "Constants.h"

//=====================================================================
// NETWORK CONFIGURATION
//=====================================================================
// Wi-Fi and MQTT Configuration
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "YOUR_MQTT_BROKER_IP";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ArduinoSensorHub";

// MQTT Topics
const char* MQTT_TOPIC_BASE = "sensors/arduino";
const char* MQTT_CONFIG_TOPIC = "sensors/arduino/config";
const char* MQTT_COMMAND_TOPIC = "sensors/arduino/commands";
const char* MQTT_STATUS_TOPIC = "sensors/arduino/status";
const char* MQTT_ALERTS_TOPIC = "sensors/arduino/alerts";
const char* MQTT_ANOMALIES_TOPIC = "sensors/arduino/anomalies";
const char* MQTT_BUFFERED_DATA_TOPIC = "sensors/arduino/buffered_data";
const char* MQTT_INFLUX_TOPIC = "sensors/influx/environment";

//=====================================================================
// SENSOR CONSTANTS
//=====================================================================
// Gas Sensor Calibration
const float R0 = 1.21;

// Microphone Configuration
const int MIC_CHANNELS = 1;
const int MIC_FREQUENCY = 16000;
const int MIC_BUFFER_SIZE = 512;

// Battery Monitoring Configuration
const float BATTERY_MAX_VOLTAGE = 4.2;
const float BATTERY_MIN_VOLTAGE = 3.3;
const float VOLTAGE_DIVIDER_RATIO = 2.0;
const float EXTERNAL_POWER_THRESHOLD = 4.8;
const float LOW_BATTERY_THRESHOLD = 20.0;
const float BATTERY_RECOVERED_THRESHOLD = 30.0;

//=====================================================================
// BUFFER CONFIGURATION
//=====================================================================
const int BUFFER_BATCH_SIZE = 5;

//=====================================================================
// ANOMALY DETECTION CONFIGURATION
//=====================================================================
const int HISTORY_SIZE = 10;
const float MIN_TEMP_STD_DEV = 0.1;
const float MIN_GAS_STD_DEV = 0.01;

//=====================================================================
// TIMING CONSTANTS
//=====================================================================
const unsigned long NETWORK_CHECK_INTERVAL = 10000; // 10 seconds
const unsigned long BATTERY_CHECK_INTERVAL = 30000; // 30 seconds
const unsigned long CONFIG_SAVE_DELAY = 5000;       // 5 seconds
const unsigned long LED_BLINK_SHORT = 50;           // 50 ms
const unsigned long LED_BLINK_MEDIUM = 100;         // 100 ms
const unsigned long LED_BLINK_LONG = 200;           // 200 ms

//=====================================================================
// EEPROM CONSTANTS
//=====================================================================
const byte CONFIG_SAVED_FLAG = 0xAA;
