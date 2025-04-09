/*
 * Communication.cpp
 * MQTT publishing implementation
 */

#include "Communication.h"
#include "Config.h"
#include "Sensors.h"
#include <ArduinoJson.h>

//=====================================================================
// EXTERNAL VARIABLES
//=====================================================================
// From Network.cpp
extern PubSubClient mqttClient;
extern bool networkConnected;

//=====================================================================
// MQTT PUBLISHING FUNCTIONS
//=====================================================================
void publishDeviceStatus() {
    if (!networkConnected)
        return;

    StaticJsonDocument<512> jsonDoc;
    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["status"] = "online";
    jsonDoc["firmware_version"] = "1.1.0";
    jsonDoc["battery"] = batteryPercentage;

    JsonObject configObj = jsonDoc.createNestedObject("config");
    configObj["accel_threshold"] = config.accelSpikeThreshold;
    configObj["sound_threshold"] = config.soundSpikeThreshold;
    configObj["sensor_interval"] = config.sensorReadInterval;
    configObj["publish_interval"] = config.mqttPublishInterval;
    configObj["anomaly_detection"] = config.anomalyDetectionEnabled;
    configObj["buffer_size"] = config.offlineBufferSize;

    char jsonBuffer[512];
    serializeJson(jsonDoc, jsonBuffer);
    mqttClient.publish(MQTT_STATUS_TOPIC, jsonBuffer, true);
}

void publishLowBatteryAlert() {
    if (!networkConnected)
        return;

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["alert"] = "low_battery";
    jsonDoc["battery_percentage"] = batteryPercentage;
    jsonDoc["battery_voltage"] = batteryVoltage;

    char jsonBuffer[256];
    serializeJson(jsonDoc, jsonBuffer);
    mqttClient.publish(MQTT_ALERTS_TOPIC, jsonBuffer);

    Serial.println("Low battery alert published");
}

void publishSpikeAlert() {
    if (!networkConnected)
        return;

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["timestamp"] = millis();

    if (vibrationSpikeDetected) {
        jsonDoc["alert"] = "vibration_spike";
        jsonDoc["acceleration_magnitude"] = sqrt(Ax * Ax + Ay * Ay + Az * Az);

        char jsonBuffer[256];
        serializeJson(jsonDoc, jsonBuffer);
        mqttClient.publish(MQTT_ALERTS_TOPIC, jsonBuffer);

        Serial.println("Vibration spike alert published");
    }

    if (soundSpikeDetected) {
        jsonDoc["alert"] = "sound_spike";
        jsonDoc["sound_level"] = soundLevel;

        char jsonBuffer[256];
        serializeJson(jsonDoc, jsonBuffer);
        mqttClient.publish(MQTT_ALERTS_TOPIC, jsonBuffer);

        Serial.println("Sound spike alert published");
    }
}

void publishToMQTT() {
    if (!networkConnected || !mqttClient.connected())
        return;

    StaticJsonDocument<768> jsonDoc;

    jsonDoc["device_id"] = MQTT_CLIENT_ID;
    jsonDoc["timestamp"] = millis();
    jsonDoc["battery"] = batteryPercentage;
    jsonDoc["on_battery"] = isOnBattery;

    JsonObject envData = jsonDoc.createNestedObject("environment");
    envData["temperature"] = temperature;
    envData["humidity"] = humidity;
    envData["heat_index"] = heatIndex;

    JsonObject imuData = jsonDoc.createNestedObject("imu");
    imuData["accel_x"] = Ax;
    imuData["accel_y"] = Ay;
    imuData["accel_z"] = Az;
    imuData["accel_magnitude"] = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    imuData["gyro_x"] = Gx;
    imuData["gyro_y"] = Gy;
    imuData["gyro_z"] = Gz;

    JsonObject gasData = jsonDoc.createNestedObject("gas");
    gasData["rs_ratio"] = gasRatio;
    gasData["co_ppm"] = getCO_ppm(gasRatio);
    gasData["ch4_ppm"] = getCH4_ppm(gasRatio);
    gasData["lpg_ppm"] = getLPG_ppm(gasRatio);

    JsonObject soundData = jsonDoc.createNestedObject("sound");
    soundData["level"] = soundLevel;

    char jsonBuffer[768];
    serializeJson(jsonDoc, jsonBuffer);

    bool published = mqttClient.publish(MQTT_TOPIC_BASE, jsonBuffer);

    if (published) {
        digitalWrite(LEDG, HIGH);
        delay(LED_BLINK_SHORT);
        digitalWrite(LEDG, LOW);
    }

    // Publish in InfluxDB line protocol format
    unsigned long timestamp = millis();
    char buffer[256];

    // Environment data in InfluxDB format
    snprintf(
        buffer, sizeof(buffer),
        "environment,device=%s,on_battery=%s temperature=%.2f,humidity=%.2f,heat_index=%.2f %lu",
        MQTT_CLIENT_ID, isOnBattery ? "true" : "false", temperature, humidity, heatIndex,
        timestamp);
    mqttClient.publish(MQTT_INFLUX_TOPIC, buffer);
}
