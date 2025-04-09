/*
 * Communication.h
 * MQTT publishing functions
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include "Constants.h"

//=====================================================================
// FUNCTION PROTOTYPES
//=====================================================================
// Publish device status
void publishDeviceStatus();

// Publish low battery alert
void publishLowBatteryAlert();

// Publish sensor spike alerts
void publishSpikeAlert();

// Publish all sensor data to MQTT
void publishToMQTT();

#endif // COMMUNICATION_H