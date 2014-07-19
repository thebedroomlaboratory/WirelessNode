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
    * Every 2 seconds, send a payload to the receiver node.
    */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "nodeconfig.h"
#include "printf.h"

// nRF24L01(+) radio attached using Getting Started board
RF24 radio(9, 10);

// Network uses that radio
RF24Network network(radio);

// Our node configuration 
eeprom_info_t this_node;
uint8_t has_recent_system_config = false;

// Address of the other node
const uint16_t other_node = 00;

// How often to send 'hello world to the other unit
const unsigned long interval = 2000; //ms

// When did we last send?
unsigned long last_sent;

// How many have we sent already
unsigned long packets_sent;

// Structure of our payload
struct payload_t
{
    unsigned long ms;
    unsigned long counter;
};

//TMP36 Pin Variables
int sensorPin = 1; //the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
int sensorPin1 = 0;
int temperatureCOUT = 0;

void setup(void)
{
    Serial.begin(57600);
    printf_begin();
    printf_P(PSTR("\n\rRF24Network/examples/helloworld_tx/\n\r"));
    // Which node are we?
    this_node = nodeconfig_read();
    SPI.begin();
    radio.begin();
    network.begin(/*channel*/ 90, /*node address*/ this_node.address);
}

void loop(void)
{
    // Pump the network regularly
    network.update();
    switch (this_node.profile)
    {
    case 0:
      if( network.available() )
      {
        // If so, grab it and print it out
        RF24NetworkHeader header;
        payload_t payload;
        network.read(header,&payload,sizeof(payload));
        printf_P(PSTR("From 0%o Received packet #%d at %d"),header.from_node,payload.counter,payload.ms);
      }
      break;
    case 1:
      sendReading();
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
      break;
    }    
    nodeconfig_listen();
}

void sendReading()
{
    // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
      last_sent = now;       
      //getting the voltage reading from the temperature sensor
      int reading = analogRead(sensorPin);
      // converting that reading to voltage, for 3.3v arduino use 3.3
      float voltage = reading * 3.3;
      voltage /= 1024.0;

      int reading2 = analogRead(sensorPin1);  
      // now print out the temperature
      float temperatureC = ((voltage - 0.5) * 100) + 24 ; //converting from 10 mv per degree wit 500 mV offset
      //to degrees ((volatge - 500mV) times 100)
      temperatureCOUT = (temperatureC * 100);
      // now convert to Fahrenheight
      float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

      printf_P(PSTR("Sending...\n\r"));
      payload_t payload = { reading2, temperatureCOUT };
      RF24NetworkHeader header(/*to node*/ other_node);
      bool ok = network.write(header, &payload, sizeof(payload));
      if (ok)
          printf_P(PSTR("ok.\n\r"));
      else
          printf_P(PSTR("failed.\n\r"));
  }
}

