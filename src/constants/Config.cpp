/*
 * Config.cpp
 * Configuration implementation
 */

#include "Config.h"
#include "Buffer.h"
#include "Communication.h"
#include <ArduinoJson.h>
#include <EEPROM.h>

// Global configuration variable
Config config;

// Default configuration
const Config DEFAULT_CONFIG = {
    1.2,              // accelSpikeThreshold
    10000,            // soundSpikeThreshold
    1000,             // sensorReadInterval (1 sec)
    5000,             // mqttPublishInterval (5 sec)
    true,             // anomalyDetectionEnabled
    3.0,              // anomalyThresholdMultiplier (3 sigma)
    20,               // offlineBufferSize (entries)
    CONFIG_SAVED_FLAG // configSaved flag
};

// Timestamp of last config change
extern unsigned long lastConfigSaveTime;

void loadConfigFromEEPROM() {
    EEPROM.get(0, config);

    if (config.configSaved != CONFIG_SAVED_FLAG) {
        // Invalid config or first run, load defaults
        config = DEFAULT_CONFIG;
        saveConfigToEEPROM();
    }

    // Ensure buffer size is within bounds
    if (config.offlineBufferSize <= 0 || config.offlineBufferSize > MAX_BUFFER_SIZE) {
        config.offlineBufferSize = DEFAULT_CONFIG.offlineBufferSize;
    }
}

void saveConfigToEEPROM() {
    EEPROM.put(0, config);
    EEPROM.commit();
}

void processConfigMessage(char* payload, unsigned int length) {
    StaticJsonDocument<512> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, payload, length);

    if (error)
        return;

    bool configChanged = false;

    if (jsonDoc.containsKey("accel_threshold")) {
        config.accelSpikeThreshold = jsonDoc["accel_threshold"].as<float>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("sound_threshold")) {
        config.soundSpikeThreshold = jsonDoc["sound_threshold"].as<int>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("sensor_interval")) {
        config.sensorReadInterval = jsonDoc["sensor_interval"].as<unsigned long>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("publish_interval")) {
        config.mqttPublishInterval = jsonDoc["publish_interval"].as<unsigned long>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("anomaly_detection")) {
        config.anomalyDetectionEnabled = jsonDoc["anomaly_detection"].as<bool>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("anomaly_threshold")) {
        config.anomalyThresholdMultiplier = jsonDoc["anomaly_threshold"].as<float>();
        configChanged = true;
    }

    if (jsonDoc.containsKey("buffer_size")) {
        int newSize = jsonDoc["buffer_size"].as<int>();
        if (newSize > 0 && newSize <= MAX_BUFFER_SIZE) {
            config.offlineBufferSize = newSize;
            configChanged = true;
        }
    }

    if (configChanged) {
        lastConfigSaveTime = millis();
        publishDeviceStatus();
    }
}

void processCommandMessage(char* payload, unsigned int length) {
    StaticJsonDocument<256> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, payload, length);

    if (error)
        return;

    const char* command = jsonDoc["command"];
    if (command == NULL)
        return;

    // Get access to MQTT client from Network.cpp
    extern PubSubClient mqttClient;

    if (strcmp(command, "reboot") == 0) {
        mqttClient.publish(MQTT_STATUS_TOPIC, "{\"status\":\"rebooting\"}");
        delay(1000);
        // Reboot implementation depends on board - use appropriate method
    } else if (strcmp(command, "led") == 0) {
        const char* ledColor = jsonDoc["color"];
        bool ledState = jsonDoc["state"];

        if (ledColor != NULL) {
            if (strcmp(ledColor, "red") == 0) {
                digitalWrite(LEDR, ledState ? HIGH : LOW);
            } else if (strcmp(ledColor, "green") == 0) {
                digitalWrite(LEDG, ledState ? HIGH : LOW);
            } else if (strcmp(ledColor, "blue") == 0) {
                digitalWrite(LEDB, ledState ? HIGH : LOW);
            }
        }
    } else if (strcmp(command, "reset_config") == 0) {
        config = DEFAULT_CONFIG;
        saveConfigToEEPROM();
        mqttClient.publish(MQTT_STATUS_TOPIC, "{\"status\":\"config_reset\"}");
    } else if (strcmp(command, "request_data") == 0) {
        publishToMQTT();
    } else if (strcmp(command, "clear_buffer") == 0) {
        initializeOfflineBuffer();
        mqttClient.publish(MQTT_STATUS_TOPIC, "{\"status\":\"buffer_cleared\"}");
    } else if (strcmp(command, "power_save") == 0) {
        bool enable = jsonDoc["enable"];
        // Implementation depends on specific power saving capabilities of your hardware
        mqttClient.publish(MQTT_STATUS_TOPIC, enable ? "{\"status\":\"power_save_enabled\"}"
                                                     : "{\"status\":\"power_save_disabled\"}");
    }
}
