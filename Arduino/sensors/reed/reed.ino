int led = 5 ; // define LED Interface
int reed = 7; // define the Reed sensor interfaces
int val; // define numeric variables val
int prevVal;

void setup(){
  pinMode(led, OUTPUT);
  pinMode(reed, INPUT);
  Serial.begin(9600);
}


void loop(){
  delay(1000);
  prevVal=val;
  val = digitalRead(reed);
  if (prevVal != val){
    if(val == LOW){
      digitalWrite(led, HIGH);
      writeData();
    }
    else{
      digitalWrite(led, LOW);
      writeData();
    }
  }
}

void writeData(){
  Serial.print("Reed + LED = ");
  Serial.println(val);
}
