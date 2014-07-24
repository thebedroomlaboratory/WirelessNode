/*
    Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.
    */

/**
    * Simplest possible example of using RF24Network
    *
    * TRANSMITTER NODE
    * Every 1 seconds, send a payload to the receiver node.
    */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "dht11.h"
#include "nodeconfig.h"
#include "printf.h"

// Configure pin numbering
int lightSensorPin = 0; 
#define DHT11PIN 2
int ledPin = 3;
int reedPin = 4;
RF24 radio(9, 10);

// Initialize our temperature sensor
dht11 DHT11;

// Network uses that radio
RF24Network network(radio);

// Our node configuration 
eeprom_info_t this_node;
uint8_t has_recent_system_config = false;

// Address of the base node
const uint16_t other_node = 00;

// How often to send 'hello world to the other unit
const unsigned long interval = 1000; //ms

// When did we last send?
unsigned long last_sent;

// Structure of our sensor readings payload
struct SensorPayload_t
{
  int nodeType;
  int lightReading;
  int tempReading;
  int humReading;
  boolean lightStat;
  boolean reedReading;
};

// Structure of our light switch payload
struct LSwitchPayload_t
{
  boolean newLightStat;
};

boolean lightStat = false;

void setup(void)
{
    Serial.begin(57600);
    printf_begin();
    printf_P(PSTR("\n\rRF24Network/examples/helloworld_tx/\n\r"));
    // Which node are we?
    this_node = nodeconfig_read();
    SPI.begin();
    radio.begin();
    network.begin(/*channel*/ 100, /*node address*/ this_node.address);
    pinMode(ledPin, OUTPUT);
}

void loop(void)
{
  // Pump the network regularly
  network.update();
  switch (this_node.profile)
  {
//  case 0:
//    // Gateway profile
//    // Check to see if a packet is waiting for us
//    if( network.available() )
//    {
//      // If so, grab it and print it out
//      RF24NetworkHeader header;
//      SensorPayload_t payload;
//      network.read(header,&payload,sizeof(payload));
//      printf_P(PSTR("From: 0%o, Node type: %d, Temperature: %d, On: %s\n\r"),header.from_node,payload.nodeType,payload.tempReading,payload.lightStat ? "true" : "false");
//    }
//    break;
  case 1:
    // Basic Sensor Profile
    sensorReading(1);
    break;
  case 2:
    // Door Sensor Profile
    sensorReading(2);
    break;
//  case 3:
//    // Smart Power Profile
//    //powerReading();
//    break;
  default: 
    // if nothing else matches, do the default
    // default is optional
    break;
  }
  // Listen for a new node address
  nodeconfig_listen();
}

void switchLight()
{
  if( network.available() )
  {
    RF24NetworkHeader header;
    LSwitchPayload_t payload;
    network.read(header,&payload,sizeof(payload));
    printf_P(PSTR("From: 0%o, Turn light: %s\n\r"),header.from_node,payload.newLightStat ? "on" : "off");
    if (payload.newLightStat != lightStat)
      lightStat = payload.newLightStat;
      digitalWrite(ledPin,lightStat);
  }
}

void sensorReading(int nodeType)
{
  switchLight();
  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    // Reset timer
    last_sent = now;       
    // getting the voltage reading from the LDR
    int lightReading = analogRead(lightSensorPin);
    // getting the temperature and humidity levels from the DHT11
    int chk = DHT11.read(DHT11PIN);
//    Serial.print("Read sensor: ");
//    switch (chk)
//    {
//      case DHTLIB_OK: 
//  		Serial.println("OK"); 
//  		break;
//      case DHTLIB_ERROR_CHECKSUM: 
//  		Serial.println("Checksum error"); 
//  		break;
//      case DHTLIB_ERROR_TIMEOUT: 
//  		Serial.println("Time out error"); 
//  		break;
//      default: 
//  		Serial.println("Unknown error"); 
//  		break;
//    }
    int tempReading = DHT11.temperature;
    int humReading = DHT11.humidity;
    boolean reedReading = false;
    if (nodeType == 1){
      reedReading = digitalRead(reedPin);
    }
    SensorPayload_t payload = { nodeType, lightReading, tempReading, humReading, lightStat, reedReading };
    printf_P(PSTR("Sending...%d %d\n\r"),tempReading, humReading);
    
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok)
        printf_P(PSTR("ok.\n\r"));
    else
        printf_P(PSTR("failed.\n\r"));
  }
}

