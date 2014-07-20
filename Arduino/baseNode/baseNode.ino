/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 00;

uint16_t other_node = 01;

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

boolean zone1Light = false;
boolean zone2Light = false;
boolean zone3Light = false;
//boolean socket1 = false;
//boolean socket1 = false;
//boolean socket1 = false;
//boolean socket1 = false;

void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node);
  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // Is there anything ready for us?
  if ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    SensorPayload_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.print("From: 0");
    Serial.print(header.from_node, OCT);
    Serial.print(", Node Type: ");
    Serial.print(payload.nodeType);
    Serial.print(", Light level: ");
    Serial.print(payload.lightReading);
    Serial.print(", Temperature: ");
    Serial.print(payload.tempReading);
    Serial.print(", Humidity: ");
    Serial.print(payload.humReading);
    Serial.print(", Light is ");
    Serial.println(payload.lightStat ? "on" : "off");
  }
  
  // Check for switching orders from Web
  checkSwitches();
}

void checkSwitches(){
  if (digitalRead(2)!=zone1Light){
    zone1Light=digitalRead(2);
    LSwitchPayload_t payload = { zone1Light };
    Serial.println("Sending...");
    
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok)
        Serial.println("ok.");
    else
        Serial.println("failed.");
  }
}
