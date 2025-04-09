/*
 * Buffer.cpp
 * Offline data buffer implementation
 */

#include "Buffer.h"
#include "Config.h"
#include "Sensors.h"
#include <ArduinoJson.h>

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
SensorReading offlineBuffer[MAX_BUFFER_SIZE];
int bufferWriteIndex = 0;
int bufferReadIndex = 0;
int bufferCount = 0;
bool networkWasDown = false;

// External MQTT client
extern PubSubClient mqttClient;

//=====================================================================
// BUFFER FUNCTIONS
//=====================================================================
void initializeOfflineBuffer() {
    for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
        offlineBuffer[i].valid = false;
    }
    bufferWriteIndex = 0;
    bufferReadIndex = 0;
    bufferCount = 0;
}

void storeReadingInBuffer() {
    if (bufferCount >= config.offlineBufferSize) {
        // Buffer is full, oldest data will be overwritten
        bufferReadIndex = (bufferReadIndex + 1) % config.offlineBufferSize;
    } else {
        bufferCount++;
    }

    // Store current sensor data
    offlineBuffer[bufferWriteIndex].timestamp = millis();
    offlineBuffer[bufferWriteIndex].temperature = temperature;
    offlineBuffer[bufferWriteIndex].humidity = humidity;
    offlineBuffer[bufferWriteIndex].accelMagnitude = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    offlineBuffer[bufferWriteIndex].gasRatio = gasRatio;
    offlineBuffer[bufferWriteIndex].soundLevel = soundLevel;
    offlineBuffer[bufferWriteIndex].batteryPercentage = batteryPercentage;
    offlineBuffer[bufferWriteIndex].valid = true;

    // Move write pointer
    bufferWriteIndex = (bufferWriteIndex + 1) % config.offlineBufferSize;

    Serial.print("Reading stored in buffer. Count: ");
    Serial.println(bufferCount);
}

void sendBufferedData() {
    Serial.print("Sending buffered data (");
    Serial.print(bufferCount);
    Serial.println(" readings)");

    // Send each buffered reading in batches
    int sentCount = 0;

    while (bufferCount > 0) {
        StaticJsonDocument<1024> jsonDoc;
        JsonArray dataArray = jsonDoc.createNestedArray("buffered_data");

        // Create a batch of readings
        for (int i = 0; i < BUFFER_BATCH_SIZE && bufferCount > 0; i++) {
            if (offlineBuffer[bufferReadIndex].valid) {
                JsonObject reading = dataArray.createNestedObject();
                reading["timestamp"] = offlineBuffer[bufferReadIndex].timestamp;
                reading["temperature"] = offlineBuffer[bufferReadIndex].temperature;
                reading["humidity"] = offlineBuffer[bufferReadIndex].humidity;
                reading["accel_magnitude"] = offlineBuffer[bufferReadIndex].accelMagnitude;
                reading["gas_ratio"] = offlineBuffer[bufferReadIndex].gasRatio;
                reading["sound_level"] = offlineBuffer[bufferReadIndex].soundLevel;
                reading["battery"] = offlineBuffer[bufferReadIndex].batteryPercentage;

                sentCount++;
            }

            // Mark as invalid and move read pointer
            offlineBuffer[bufferReadIndex].valid = false;
            bufferReadIndex = (bufferReadIndex + 1) % config.offlineBufferSize;
            bufferCount--;
        }

        // Send the batch
        char jsonBuffer[1024];
        serializeJson(jsonDoc, jsonBuffer);
        mqttClient.publish(MQTT_BUFFERED_DATA_TOPIC, jsonBuffer);

        // Brief delay to prevent overwhelming the broker
        delay(100);
    }

    Serial.print("Sent ");
    Serial.print(sentCount);
    Serial.println(" buffered readings");
}
