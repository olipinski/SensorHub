/*
 * Constants.h
 * Constants and pin definitions for Arduino IoT Sensor Hub
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

//=====================================================================
// PIN DEFINITIONS
//=====================================================================
#define DHTPIN 13
#define DHTTYPE DHT11
#define GAS_SENSOR_PIN A0
#define BATTERY_PIN A1
#define LEDR 22
#define LEDG 23
#define LEDB 24

//=====================================================================
// NETWORK CONFIGURATION
//=====================================================================
// Wi-Fi and MQTT Configuration
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_CLIENT_ID;

// MQTT Topics
extern const char* MQTT_TOPIC_BASE;
extern const char* MQTT_CONFIG_TOPIC;
extern const char* MQTT_COMMAND_TOPIC;
extern const char* MQTT_STATUS_TOPIC;
extern const char* MQTT_ALERTS_TOPIC;
extern const char* MQTT_ANOMALIES_TOPIC;
extern const char* MQTT_BUFFERED_DATA_TOPIC;
extern const char* MQTT_INFLUX_TOPIC;

//=====================================================================
// SENSOR CONSTANTS
//=====================================================================
// Gas Sensor Calibration
extern const float R0;

// Microphone Configuration
extern const int MIC_CHANNELS;
extern const int MIC_FREQUENCY;
extern const int MIC_BUFFER_SIZE;

// Battery Monitoring Configuration
extern const float BATTERY_MAX_VOLTAGE;
extern const float BATTERY_MIN_VOLTAGE;
extern const float VOLTAGE_DIVIDER_RATIO;
extern const float EXTERNAL_POWER_THRESHOLD;
extern const float LOW_BATTERY_THRESHOLD;
extern const float BATTERY_RECOVERED_THRESHOLD;

//=====================================================================
// BUFFER CONFIGURATION
//=====================================================================
#define MAX_BUFFER_SIZE 50
extern const int BUFFER_BATCH_SIZE;

//=====================================================================
// ANOMALY DETECTION CONFIGURATION
//=====================================================================
extern const int HISTORY_SIZE;
extern const float MIN_TEMP_STD_DEV;
extern const float MIN_GAS_STD_DEV;

//=====================================================================
// TIMING CONSTANTS
//=====================================================================
extern const unsigned long NETWORK_CHECK_INTERVAL;
extern const unsigned long BATTERY_CHECK_INTERVAL;
extern const unsigned long CONFIG_SAVE_DELAY;
extern const unsigned long LED_BLINK_SHORT;
extern const unsigned long LED_BLINK_MEDIUM;
extern const unsigned long LED_BLINK_LONG;

//=====================================================================
// EEPROM CONSTANTS
//=====================================================================
extern const byte CONFIG_SAVED_FLAG;

#endif // CONSTANTS_H