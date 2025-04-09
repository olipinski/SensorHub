/*
 * Arduino IoT Sensor Hub
 * Advanced implementation with offline buffering and remote configuration
 */

// Include all necessary libraries
#include <ArduinoJson.h>
#include <Arduino_LSM6DSOX.h>
#include <DHT.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WiFiNINA.h>
#include <math.h>

// Include all project headers
#include "Buffer.h"
#include "Communication.h"
#include "Config.h"
#include "Constants.h"
#include "DataProcessing.h"
#include "Network.h"
#include "Sensors.h"

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================

// Timing Control
unsigned long lastSensorReadTime = 0;
unsigned long lastMqttPublishTime = 0;
unsigned long lastBatteryCheckTime = 0;
unsigned long lastConfigSaveTime = 0;
unsigned long lastNetworkCheckTime = 0;

//=====================================================================
// SETUP AND MAIN LOOP
//=====================================================================

void setup() {
    Serial.begin(9600);
    while (!Serial && millis() < 5000)
        ;

    setupLEDs();
    EEPROM.begin();
    loadConfigFromEEPROM();

    initializeOfflineBuffer();
    setupSensors();
    readBatteryStatus();
    setupNetworking();

    // Blink to indicate ready
    for (int i = 0; i < 3; i++) {
        digitalWrite(LEDG, HIGH);
        delay(LED_BLINK_MEDIUM);
        digitalWrite(LEDG, LOW);
        delay(LED_BLINK_MEDIUM);
    }

    // Send initial configuration status
    publishDeviceStatus();
}

void loop() {
    unsigned long currentMillis = millis();

    // Check network status regularly
    if (currentMillis - lastNetworkCheckTime >= NETWORK_CHECK_INTERVAL) {
        lastNetworkCheckTime = currentMillis;
        checkNetworkStatus();
    }

    // Maintain MQTT connection if network is up
    if (networkConnected && !mqttClient.connected()) {
        connectMQTT();
    }

    if (networkConnected) {
        mqttClient.loop();
    }

    // Check battery status regularly
    if (currentMillis - lastBatteryCheckTime >= BATTERY_CHECK_INTERVAL) {
        lastBatteryCheckTime = currentMillis;
        readBatteryStatus();

        if (batteryPercentage < LOW_BATTERY_THRESHOLD && !lowBatteryWarning) {
            lowBatteryWarning = true;

            // Flash red LED to indicate low battery
            for (int i = 0; i < 5; i++) {
                digitalWrite(LEDR, HIGH);
                delay(LED_BLINK_MEDIUM);
                digitalWrite(LEDR, LOW);
                delay(LED_BLINK_MEDIUM);
            }

            // Send alert if connected
            if (networkConnected) {
                publishLowBatteryAlert();
            }
        } else if (batteryPercentage >= BATTERY_RECOVERED_THRESHOLD) {
            lowBatteryWarning = false;
        }
    }

    // Read sensors at regular intervals
    if (currentMillis - lastSensorReadTime >= config.sensorReadInterval) {
        lastSensorReadTime = currentMillis;
        readSensors();

        // Detect spikes and anomalies
        vibrationSpikeDetected = checkForVibrationSpike();
        soundSpikeDetected = checkForSoundSpike();

        if (config.anomalyDetectionEnabled && historyFilled) {
            checkForAnomalies();
        }

        // If network is down, store readings in buffer
        if (!networkConnected) {
            storeReadingInBuffer();
            networkWasDown = true;

            // Blue LED blink pattern indicates offline storage
            digitalWrite(LEDB, HIGH);
            delay(LED_BLINK_SHORT);
            digitalWrite(LEDB, LOW);
        }

        // Visual indicator for spikes
        if (vibrationSpikeDetected || soundSpikeDetected) {
            digitalWrite(LEDB, HIGH);

            // Send immediate alert if connected
            if (networkConnected) {
                publishSpikeAlert();
            }

            delay(LED_BLINK_LONG);
            digitalWrite(LEDB, LOW);
        }
    }

    // Publish data at regular intervals when connected
    if (networkConnected && (currentMillis - lastMqttPublishTime >= config.mqttPublishInterval)) {
        lastMqttPublishTime = currentMillis;

        // If there was a network outage, send buffered data first
        if (networkWasDown) {
            sendBufferedData();
            networkWasDown = false;
        }

        publishToMQTT();
    }

    // Save configuration if it was changed
    if (lastConfigSaveTime > 0 && (currentMillis - lastConfigSaveTime) >= CONFIG_SAVE_DELAY) {
        saveConfigToEEPROM();
        lastConfigSaveTime = 0;
    }
}
