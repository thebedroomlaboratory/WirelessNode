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

// Configure pin numbering
RF24 radio(9, 10);
// Network uses that radio
RF24Network network(radio);
// Address of the this node
const uint16_t this_node = 03;
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
  int relay0 = 0;
  int relay1 = 0;
  int relay2 = 0;
  int relay3 = 0;
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

// Variables will change
int switches[] = {0, 0, 0, 0};
int prevSwitches[] = {0, 0, 0, 0};
int relays[] = {0, 0, 0, 0};

void setup() {
  // NRF Setup
  Serial.println("RF24Network/examples/helloworld_rx/");
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node);
  
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
  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    // Reset timer
    last_sent = now;       
    payload_out_t payload;
    payload.nodeType=this_node;
    payload.relay0=relay0;
    payload.relay1=relay1;
    payload.relay2=relay2;
    payload.relay3=relay3;
    Serial.println("Sending...\n\r");
    
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &payload, sizeof(payload));
    if (ok)
        Serial.println("ok.\n\r");
    else
        Serial.println("failed.\n\r");
  }
}
