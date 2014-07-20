int sensorPin = 1; 
float temp = 0.0;  

void setup()
{
  Serial.begin(9600); 
}
 
void loop()
{
  temp = getTemp(analogRead(sensorPin));

  Serial.print(temp); Serial.println(" C");
  delay(1000);
}

double getTemp(int voltage) {
  double Temp;
  Temp = log(((10240000/voltage) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;// Convert Kelvin to Celcius
  return Temp;
}


