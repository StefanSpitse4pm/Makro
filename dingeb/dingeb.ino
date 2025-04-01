const int motor_L1 = 10; 

const int motor_L2 = 11; 

const int motor_R1 = 5; 

const int motor_R2 = 6; 

const int buttonOn = 2; 

const int buttonOff = 4; 

int buttonState = 0; 

int buttonState2 = 0; 

const int echo = 9; 

const int trig = 3; 

float duration, distance; 

const int motorsensor1 = 13; 

const int motorsensor2 = 12; 

 

 

void setup() { 

  pinMode(motor_L1, OUTPUT); 

  pinMode(motor_L2, OUTPUT); 

  pinMode(motor_R1, OUTPUT); 

  pinMode(motor_R2, OUTPUT); 

  pinMode(buttonOn, INPUT); 

  pinMode(buttonOff, INPUT); 

  pinMode(trig, OUTPUT); 

  pinMode(echo, INPUT); 

  pinMode(motorsensor1, INPUT); 

  pinMode(motorsensor2, INPUT); 

  

  Serial.begin(9600); 

  stop();  // Ensure motors are stopped at startup 

} 

 

// Function to move forward 

void drive(int speedLeft, int speedRight) { 

  if (speedLeft >= 0) { 

    // Voorwaarts voor de linker motor 

    analogWrite(motor_L1, 0); 

    analogWrite(motor_L2, speedLeft); 

  } else { 

    // Achterwaarts voor de linker motor 

    analogWrite(motor_L1, -speedLeft);  // Negatieve waarde om vooruit om te keren 

    analogWrite(motor_L2, 0); 

  } 

 

  if (speedRight >= 0) { 

    // Voorwaarts voor de rechter motor 

    analogWrite(motor_R1, 0); 

    analogWrite(motor_R2, speedRight); 

  } else { 

    // Achterwaarts voor de rechter motor 

    analogWrite(motor_R1, -speedRight); // Negatieve waarde om vooruit om te keren 

    analogWrite(motor_R2, 0); 

  } 

} 

 

// Function to stop motors 

void stop() { 

    analogWrite(motor_L1, 0); 

    analogWrite(motor_L2, 0); 

    analogWrite(motor_R1, 0); 

    analogWrite(motor_R2, 0); 

} 

 

 

// Function to measure distance 

void measureDistance() { 

  digitalWrite(trig, LOW); 

  delayMicroseconds(2); 

  digitalWrite(trig, HIGH); 

  delayMicroseconds(10); 

  digitalWrite(trig, LOW); 

 

  duration = pulseIn(echo, HIGH); 

  distance = (duration * 0.0343) / 2; 

  Serial.print("Distance: "); 

  Serial.println(distance); 

  delay(100); 

} 

 

// Function to execute motion while measuring distance 

void performActionWithSensor(void (*actionFunc)(int, int), int speedLeft, int speedRight, int durationMs) { 

  unsigned long startTime = millis(); 

  actionFunc(speedLeft, speedRight);  // Start bewegen 

  while (millis() - startTime < durationMs) { 

    measureDistance();  // Blijf afstand meten 

  } 

  stop();  // Stop na de actie 

} 

 

void loop() { 

  // measureDistance();  // Always measure distance 

 

  buttonState = digitalRead(buttonOn); 

  buttonState2 = digitalRead(buttonOff); 

 

  if (buttonState == LOW) { 

    delay(1000); 

    performActionWithSensor(drive, 210, 255, 2000); //vooruit 

    delay(1000); 

    performActionWithSensor(drive, -220, -240, 2000); //achteruit 

    delay(1000); 

    performActionWithSensor(drive, 210, -255, 2080); //rondje as 

    // delay(1000); 

    // performActionWithSensor(drive, 255, 150, 1200); 

    // delay(1000); 

    // performActionWithSensor(drive, 150, 255, 1200); 

 

  } 

 

  if (buttonState2 == LOW) { 

    stop(); 

  } 

} 
