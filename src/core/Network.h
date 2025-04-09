/*
 * Network.h
 * WiFi and MQTT connection management
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "Constants.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiNINA.h>

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
extern bool networkConnected;
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;

//=====================================================================
// FUNCTION PROTOTYPES
//=====================================================================
// Setup LEDs and networking
void setupLEDs();
void setupNetworking();

// WiFi connection
void connectWiFi();

// MQTT connection and callback
void connectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);

// Network status check
void checkNetworkStatus();

#endif // NETWORK_H
