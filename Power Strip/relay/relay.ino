/*
 Sketch to control a Relay Circuit based on Serial input.
 
 created 13 Jul. 2014
 modified -
 by Bedlab
*/

// Constants won't change
const int relayPin = 7;
const int DELAY = 5000;

// Variables will change
int sensorValue = 0;
int sensorCurValue = 0;
int sensorPrevValue = 0;
int debug = 0;


void setup() {
  // initialize the Relay pin as an output:
  pinMode(relayPin, OUTPUT);      
  // initialize the Serial line
  Serial.begin(9600); 
}

void loop(){
  // Don't poll constanly, wait a while before polling pins.
  delay(DELAY); 
  
  // Print out debugging information
  if (debug != 0 ){
    Serial.print("sensor = " ); 
    Serial.println(sensorValue);
    Serial.print("sensorPrev = " ); 
    Serial.println(sensorPrevValue);
    Serial.print("sensorCur = " ); 
    Serial.println(sensorCurValue);
  }

  // Read in the serial input
  sensorPrevValue = sensorCurValue;
  sensorValue = Serial.read();

  // Only work if Serial input isn't garbage
  if ( sensorValue != -1 ) {
    sensorCurValue = sensorValue;
    
    // Turn on and off debug mode based on serial input
    // 50 = 2 in decimal
    if (sensorValue == 50) {
       if (debug == 0){
         debug = 1;
       }else {
         debug = 0;
       }
    }
    
    // If the Serial input changes to High or Low, switch the Relay pin accordingly
    // 48 = 0 in decimal
    // 49 = 1 in decimal    
    if (sensorPrevValue != sensorCurValue){ 
      if (sensorCurValue == 49) {    
        if (debug != 0){
          Serial.println("Switch High"); 
        }
        digitalWrite(relayPin, HIGH);  
          
      }else if (sensorCurValue == 48) {
        if (debug != 0){
          Serial.println("Switch Low"); 
        }
        digitalWrite(relayPin, LOW); 
      }
    }
  }
  
}
