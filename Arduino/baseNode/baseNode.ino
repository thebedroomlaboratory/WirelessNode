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

// How often to send 'hello world to the other unit
const unsigned long interval = 1000; //ms

// When did we last send?
unsigned long last_sent;

uint16_t other_node;
int nodeType;
int field1; // lightReading;relay0
int field2; // tempReading;relay1
int field3; // humReading;relay2
int field4; // Nothing;relay4
boolean field5; // lightStat;Nothing
boolean field6; // reedReading;Nothing
float field7; // Nothing;Power

// Structure of our sensor readings payload
struct SensorPayload_t
{
  int nodeType;
  int field1; // lightReading;relay0
  int field2; // tempReading;relay1
  int field3; // humReading;relay2
  int field4; // Nothing;relay3
  boolean field5; // lightStat;Nothing
  boolean field6; // reedReading;Nothing
  float field7; // Nothing;Power
};

// Structure of our light switch payload
struct LSwitchPayload_t
{
  boolean newLightStat;
};

// Structure of our relay switch payload
struct RSwitchPayload_t
{
  int switch0;
  int switch1;
  int switch2;
  int switch3;
};

boolean zone1Light = false;
boolean zone2Light = false;
boolean zone3Light = false;
boolean relaySwitch = false;
int switches[5] = {0,0,0,0};
RF24NetworkHeader header;
SensorPayload_t currentReadings[4];

void setup(void)
{
  Serial.begin(57600);
//  Serial.println("RF24Network/BaseNode");
 
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
    SensorPayload_t payload;
    network.read(header,&payload,sizeof(payload));
//    Serial.print("From: 0");
//    Serial.print(header.from_node, OCT);
    other_node = header.from_node;
    currentReadings[header.from_node].nodeType = payload.nodeType;
    currentReadings[header.from_node].field1 = payload.field1;
    currentReadings[header.from_node].field2 = payload.field2;
    currentReadings[header.from_node].field3 = payload.field3;
    currentReadings[header.from_node].field4 = payload.field4;
    currentReadings[header.from_node].field5 = payload.field5;
    currentReadings[header.from_node].field6 = payload.field6;
    currentReadings[header.from_node].field7 = payload.field7;
    switch(other_node)
    {
      case 1:
      {
//        Serial.print(", Node Type: ");
//        Serial.print(payload.nodeType);
//        Serial.print(", Light level: ");
//        Serial.print(payload.field1);
//        Serial.print(", Temperature: ");
//        Serial.print(payload.field2);
//        Serial.print(", Humidity: ");
//        Serial.print(payload.field3);
//        Serial.print(", Light is ");
//        Serial.print(payload.field5 ? "on" : "off");
//        Serial.print(", Door Open: ");
//        Serial.println(payload.field6);
        break;
      }
      case 4:
      {
        processPowerStrip();
        break;
      }
    }

    
    // Check for switching orders from Web
    checkSwitches();
  }
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    // Reset timer
    last_sent = now;
    Serial.print("230 ");
    Serial.print(currentReadings[4].field7);
    Serial.print(" ");
    Serial.print(currentReadings[1].field1);
    Serial.print(" ");
    Serial.print(currentReadings[1].field2);
    Serial.print(" ");
    Serial.print(currentReadings[1].field3);
    Serial.print(" ");
    Serial.print(currentReadings[1].field6);
    Serial.print(" ");
    Serial.print(currentReadings[2].field1);
    Serial.print(" ");
    Serial.print(currentReadings[2].field2);
    Serial.print(" ");
    Serial.print(currentReadings[2].field3);
    Serial.print(" ");
    Serial.print(currentReadings[3].field1);
    Serial.print(" ");
    Serial.print(currentReadings[3].field2);
    Serial.print(" ");
    Serial.println(currentReadings[3].field3);
  }   
}

void processPowerStrip(){
//  Serial.print(", Node Type: ");
//  Serial.print(currentReadings[header.from_node].nodeType);
//  Serial.print(", Relay0: ");
//  Serial.print(currentReadings[header.from_node].field1);
//  Serial.print(", Relay1: ");
//  Serial.print(currentReadings[header.from_node].field2);
//  Serial.print(", Relay2: ");
//  Serial.print(currentReadings[header.from_node].field3);
//  Serial.print(", Relay3: ");
//  Serial.print(currentReadings[header.from_node].field4);
//  Serial.print(", Power: ");
//  Serial.print(currentReadings[header.from_node].field7);
//  Serial.println("W");
  switches[0]=currentReadings[header.from_node].field1;
  switches[1]=currentReadings[header.from_node].field2;
  switches[2]=currentReadings[header.from_node].field3;
  switches[3]=currentReadings[header.from_node].field4;
}

boolean switchLight(boolean value){
  LSwitchPayload_t payload = { value };
//  Serial.println("Sending...");
  
  RF24NetworkHeader header(/*to node*/ other_node);
  bool ok = network.write(header, &payload, sizeof(payload));
  if (ok){
//      Serial.println("ok.");
      return value;
  }
  else{
//      Serial.println("failed.");
      return !value;
  }
}

void switchRelay(int sw0, int sw1, int sw2, int sw3){
  if ( switches[0]!=sw0 || switches[1]!=sw1 || switches[2]!=sw2 || switches[3]!=sw3 ){
    RSwitchPayload_t payload = { sw0,sw1,sw2,sw3 };
//    Serial.println("Sending...");
    
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok){
//        Serial.println("ok.");
        switches[0]=sw0;
        switches[1]=sw1;
        switches[2]=sw2;
        switches[3]=sw3;
    }
    else{
//        Serial.println("failed.");
    }
  }
}

void checkSwitches(){
  if (other_node == 1) {
    if (digitalRead(2)!=zone1Light){
      zone1Light=switchLight(digitalRead(2));
    }
  }
  else if (other_node == 2) {
    if (digitalRead(3)!=zone2Light){
      zone2Light=switchLight(digitalRead(3));
    }
  }
  else if (other_node == 4) {
    if (digitalRead(4)){
      switchRelay(1,0,0,0);
    }
  }
}
