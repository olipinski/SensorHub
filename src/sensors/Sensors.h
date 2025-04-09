/*
 * Sensors.cpp
 * Sensor reading implementation
 */

#include "Config.h"
#include "DataProcessing.h"
#include "Sensors.h"

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
// Sensor instance
DHT dht(DHTPIN, DHTTYPE);

// Sensor Data Variables
float temperature = 0.0;
float humidity = 0.0;
float heatIndex = 0.0;
float Ax = 0.0, Ay = 0.0, Az = 0.0;
float Gx = 0.0, Gy = 0.0, Gz = 0.0;
float gasRatio = 0.0;
float soundLevel = 0.0;
float batteryVoltage = 0.0;
float batteryPercentage = 0.0;

// Microphone buffer
short sampleBuffer[MIC_BUFFER_SIZE];
volatile int samplesRead = 0;

// State Tracking
bool vibrationSpikeDetected = false;
bool soundSpikeDetected = false;
bool isOnBattery = false;
bool lowBatteryWarning = false;

//=====================================================================
// SENSOR FUNCTIONS
//=====================================================================
void setupSensors() {
    pinMode(BATTERY_PIN, INPUT);

    // Initialize DHT sensor
    dht.begin();

    // Initialize IMU
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        digitalWrite(LEDR, HIGH);
        delay(1000);
    }

    // Initialize microphone
    PDM.onReceive(onPDMdata);
    if (!PDM.begin(MIC_CHANNELS, MIC_FREQUENCY)) {
        Serial.println("Failed to start PDM microphone!");
        digitalWrite(LEDR, HIGH);
        delay(1000);
    }
}

void readSensors() {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (!isnan(humidity) && !isnan(temperature)) {
        heatIndex = dht.computeHeatIndex(temperature, humidity, false);
        updateSensorHistory(temperature, tempHistory);
        updateSensorHistory(humidity, humidityHistory);
    }

    if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(Ax, Ay, Az);
        float accelMag = sqrt(Ax * Ax + Ay * Ay + Az * Az);
        updateSensorHistory(accelMag, accelHistory);
    }

    if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(Gx, Gy, Gz);
    }

    int sensorValue = analogRead(GAS_SENSOR_PIN);
    float sensorVolt = (float)sensorValue / 1024 * 5.0;
    float RS_gas = (5.0 - sensorVolt) / sensorVolt;
    gasRatio = RS_gas / R0;
    updateSensorHistory(getCO_ppm(gasRatio), gasHistory);

    if (samplesRead > 0) {
        int16_t maxSample = 0;
        for (int i = 0; i < samplesRead; i++) {
            if (abs(sampleBuffer[i]) > abs(maxSample)) {
                maxSample = sampleBuffer[i];
            }
        }

        soundLevel = abs(maxSample);
        samplesRead = 0;
        updateSensorHistory(soundLevel, soundHistory);
    }
}

void readBatteryStatus() {
    int rawValue = analogRead(BATTERY_PIN);
    batteryVoltage = (rawValue / 1023.0) * 5.0 * VOLTAGE_DIVIDER_RATIO;

    batteryPercentage = 100.0 * (batteryVoltage - BATTERY_MIN_VOLTAGE) /
                        (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);
    batteryPercentage = constrain(batteryPercentage, 0.0, 100.0);

    isOnBattery = (batteryVoltage < EXTERNAL_POWER_THRESHOLD);
}

void onPDMdata() {
    int bytesAvailable = PDM.available();
    PDM.read(sampleBuffer, bytesAvailable);
    samplesRead = bytesAvailable / 2;
}

bool checkForVibrationSpike() {
    float accelMagnitude = sqrt(Ax * Ax + Ay * Ay + Az * Az);
    float adjustedMagnitude = accelMagnitude;

    // Adjust for gravity (roughly 1g when stationary)
    if (accelMagnitude > 0.8 && accelMagnitude < 1.2) {
        adjustedMagnitude -= 1.0;
    }

    return (adjustedMagnitude > config.accelSpikeThreshold);
}

bool checkForSoundSpike() {
    return (soundLevel >= config.soundSpikeThreshold);
}

//=====================================================================
// GAS SENSOR CALCULATIONS
//=====================================================================
float getCO_ppm(float ratio) {
    return pow(4.07 / ratio, 4.0);
}

float getCH4_ppm(float ratio) {
    return pow(7.08 / ratio, 4.17);
}

float getLPG_ppm(float ratio) {
    return pow(10.96 / ratio, 2.78);
}
