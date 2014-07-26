/*
  Sketch to control a Relay Circuit based on Serial input.
  Includes RF24 snippets  form James Coliz, Jr. <maniacbug@ymail.com> Copyright (C) 2012 

  created 13 Jul. 2014
  modified -
  by Bedlab
*/

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

// Configure pin numbering
RF24 radio(9, 10);
// Network uses that radio
RF24Network network(radio);
// Address of the this node
const uint16_t this_node = 04;
// Address of the base node
const uint16_t other_node = 00;
// How often to send 'hello world to the other unit
const unsigned long interval = 1000; //ms
// When did we last send?
unsigned long last_sent;
// Structure of our sensor readings payload
struct payload_out_t
{
  int nodeType = 0;
  int field1 = 0;//relay0
  int field2 = 0;//relay1
  int field3 = 0;//relay2
  int field4 = 0;//relay3
  boolean field5;//not_used
  boolean field6;//not_used
  float field7;//power
};
// Structure of our light switch payload
struct payload_in_t
{
  int switch0 = 0;
  int switch1 = 0;
  int switch2 = 0;
  int switch3 = 0;
  // TODO send up I/V
};

// Constants won't change
const int relay0 = A0;
const int relay1 = A1;
const int relay2 = A2;
const int relay3 = A3;
//
const int DELAY = 1000;
//
const int switch0 = 5;
const int switch1 = 6;
const int switch2 = 7;
const int switch3 = 8;
//
const int meter0 = A4;
const double Power = 230.0;

// Variables will change
int switches[] = {0, 0, 0, 0};
int prevSwitches[] = {0, 0, 0, 0};
int relays[] = {0, 0, 0, 0};
double Irms = 0.0;

void setup() {
  // Meter Setup
  emon1.voltage(2, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(meter0, 21.3);       // Current: input pin, calibration.  
  //
  // NRF Setup
  Serial.println("RF24Network/examples/helloworld_rx/");
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node);
  //
  // initialize the Relay pin as an output:
  pinMode(relay0, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  // initialize the Switch pins as INPUT_PULLUP
  pinMode(switch0, INPUT_PULLUP);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  // initialize the Serial line
  Serial.begin(9600); 
}

void readSwitch(){
  for (int i; i < 4; i++){
    prevSwitches[i] = switches[i];
  }
  switches[0] = digitalRead(switch0);
  switches[1] = digitalRead(switch1);
  switches[2] = digitalRead(switch2);
  switches[3] = digitalRead(switch3);
}

void readRelay(){
  relays[0] = digitalRead(relay0);
  relays[1] = digitalRead(relay1);
  relays[2] = digitalRead(relay2);
  relays[3] = digitalRead(relay3);
}

void writeSwitch(){
  Serial.print("switch0 = " ); 
  Serial.println(switches[0]);
  Serial.print("switch1 = " ); 
  Serial.println(switches[1]);
  Serial.print("switch2 = " ); 
  Serial.println(switches[2]);
  Serial.print("switch3 = " ); 
  Serial.println(switches[3]);    
}

void writeRelay(){
  Serial.print("relay0 = " ); 
  Serial.println(relays[0]);
  Serial.print("relay1 = " ); 
  Serial.println(relays[1]);
  Serial.print("relay2 = " ); 
  Serial.println(relays[2]);
  Serial.print("relay3 = " ); 
  Serial.println(relays[3]);    
}

void switchRelay(int no){
  switch(no){
    case 0:
      digitalWrite(relay0, !relays[0]);
      break;
    case 1:
      digitalWrite(relay1, !relays[1]);
      break;
    case 2:
      digitalWrite(relay2, !relays[2]);
      break;
    case 3:
      digitalWrite(relay3, !relays[3]);
      break;
  }  
}

void readMeter(){
  Irms = emon1.calcIrms(1480);  // Calculate Irms only
}

void writeMeter(){
  Serial.print("Apparent Power:: ");
  Serial.print(Irms*Power);	       // Apparent power
  Serial.print("\tIrms:: ");
  Serial.println(Irms);		       // Irms
 
}

void loop(){
  // Don't poll constanly, wait a while before polling pins.
  //delay(DELAY); 
  //
  readSwitch();
  //writeSwitch();
  //
  readRelay();
  //writeRelay();
  //
  //switchRelay();
  
  // NFR Pump the network regularly
  network.update();

  sensorMgr();
  
  relayMgr();
}

void relayMgr(){
  for (int i; i < 4; i++){
    if (prevSwitches[i] != switches[i]){
      writeSwitch();
      writeRelay();
      switchRelay(i);
      delay(DELAY);
      readSwitch();
    }    
  }  
}

void readNRF()
{
  if( network.available() )
  {
    RF24NetworkHeader header;
    payload_in_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.println("Print readings");
    Serial.print(payload.switch0);
    Serial.print(payload.switch1);
    Serial.print(payload.switch2);
    Serial.print(payload.switch3);
  }
}

void sensorMgr()
{
  readNRF();
  readMeter();
  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    // Reset timer
    last_sent = now;       
    payload_out_t payload;
    payload.nodeType=this_node;
    payload.field1=digitalRead(relay0);
    payload.field2=digitalRead(relay1);
    payload.field3=digitalRead(relay2);
    payload.field4=digitalRead(relay3);
    payload.field7=Irms*Power;
    Serial.print(payload.field1);
    Serial.print("\r");
    Serial.print(payload.field2);
    Serial.print("\r");
    Serial.print(payload.field3);
    Serial.print("\r");
    Serial.print(payload.field4);
    Serial.print("\r");    
    Serial.print(payload.field7);
    Serial.print("\r");    
    Serial.println("Sending...");
    //writeMeter();
    
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok)
        Serial.println("ok.\n\r");
    else
        Serial.println("failed.\n\r");
  }
}
