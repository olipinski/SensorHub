/*
 * Network.cpp
 * WiFi and MQTT connection implementation
 */

#include "Network.h"
#include "Config.h"
#include "Communication.h"

//=====================================================================
// GLOBAL VARIABLES
//=====================================================================
bool networkConnected = false;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//=====================================================================
// LED SETUP
//=====================================================================
void setupLEDs() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  
  // All LEDs off
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
}

//=====================================================================
// NETWORK SETUP
//=====================================================================
void setupNetworking() {
  connectWiFi();
  
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  connectMQTT();
}

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for connection with timeout
  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20) {
    delay(500);
    Serial.print(".");
    connectionAttempts++;
    digitalWrite(LEDG, !digitalRead(LEDG));  // Toggle green LED while connecting
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Print signal strength
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI): ");
    Serial.print(rssi);
    Serial.println(" dBm");
  } else {
    Serial.println("\nWiFi connection failed!");
    digitalWrite(LEDR, HIGH);  // Red LED on indicates error
  }
}

void connectMQTT() {
  Serial.print("Connecting to MQTT broker...");
  int attempts = 0;
  
  while (!mqttClient.connected() && attempts < 5) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("\nConnected to MQTT broker");
      
      // Publish a connection message
      StaticJsonDocument<256> jsonDoc;
      jsonDoc["device_id"] = MQTT_CLIENT_ID;
      jsonDoc["status"] = "online";
      
      // Get access to battery percentage from Sensors.cpp
      extern float batteryPercentage;
      jsonDoc["battery"] = batteryPercentage;
      
      char jsonBuffer[256];
      serializeJson(jsonDoc, jsonBuffer);
      mqttClient.publish(MQTT_STATUS_TOPIC, jsonBuffer, true);  // retained message
      
      // Subscribe to command and config topics
      mqttClient.subscribe(MQTT_CONFIG_TOPIC);
      mqttClient.subscribe(MQTT_COMMAND_TOPIC);
      
      Serial.println("Subscribed to configuration and command topics");
    } else {
      Serial.print(".");
      delay(2000);
      attempts++;
    }
  }
  
  if (!mqttClient.connected()) {
    Serial.println("\nMQTT connection failed!");
    digitalWrite(LEDR, HIGH);  // Red LED on indicates error
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  if (strcmp(topic, MQTT_CONFIG_TOPIC) == 0) {
    processConfigMessage(message, length);
  } else if (strcmp(topic, MQTT_COMMAND_TOPIC) == 0) {
    processCommandMessage(message, length);
  }
}

void checkNetworkStatus() {
  bool previousStatus = networkConnected;
  
  networkConnected = (WiFi.status() == WL_CONNECTED) && mqttClient.connected();
  
  if (networkConnected && !previousStatus) {
    // Just came back online
    Serial.println("Network connection restored");
    digitalWrite(LEDG, HIGH);
    delay(LED_BLINK_LONG);
    digitalWrite(LEDG, LOW);
  } else if (!networkConnected && previousStatus) {
    // Just went offline
    Serial.println("Network connection lost");
    digitalWrite(LEDR, HIGH);
    delay(LED_BLINK_LONG);
    digitalWrite(LEDR, LOW);
  }
}