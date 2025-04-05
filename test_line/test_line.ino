const int sensorPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7}; 
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600); 
}

int sensorReadings[8]; 

void loop() {
  int sum = 0;
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 8; i++) { 
      sensorReadings[i] = analogRead(sensorPins[i]); 
      sum += sensorReadings[i]; 

  }
  Serial.println(sum / 8);
}
