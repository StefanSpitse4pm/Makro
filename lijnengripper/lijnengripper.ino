// const int motor_L1 = 11;
// const int motor_L2 = 6;
// const int motor_R1 = 10;
// const int motor_R2 = 5;

// const int numSensors = 8;
// const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

// int lastDirection = 0;

// void setup() {
//   pinMode(motor_L1, OUTPUT);
//   pinMode(motor_L2, OUTPUT);
//   pinMode(motor_R1, OUTPUT);
//   pinMode(motor_R2, OUTPUT);

//   Serial.begin(9600);
//   stop();  
// }

// void drive(int left, int right) {
//   analogWrite(motor_L1, max(0, -left));
//   analogWrite(motor_L2, max(0, left));
//   analogWrite(motor_R1, max(0, -right));
//   analogWrite(motor_R2, max(0, right));
// }

// void stop() {
//   analogWrite(motor_L1, 0);
//   analogWrite(motor_L2, 0);
//   analogWrite(motor_R1, 0);
//   analogWrite(motor_R2, 0);
// }
// void loop() {
//   int sensorReadings[numSensors];
//   int sum = 0;

//   for (int i = 0; i < numSensors; i++) {
//     sensorReadings[i] = analogRead(sensorPins[i]);
//     sum += sensorReadings[i];
//   }

//   int avg = sum / numSensors;
//   int deadzonelow = avg - 50;
//   int deadzonehigh = avg + 50;

//   // Rechtdoor rijden
//   if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
//     drive(214, 255);
//     lastDirection = 0;
//   }
//   // Matige bocht naar rechts
//   else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
//     drive(255, 140);  // Verhoog het verschil
//     lastDirection = -1;
//   }
//   // Scherpe bocht naar rechts
//   else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
//     drive(255, 30);   // Bijna stilstaande rechter motor voor sterke draai
//     lastDirection = -1;
//   }
//   // Matige bocht naar links
//   else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
//     drive(140, 255);
//     lastDirection = 1;
//   }
//   // Scherpe bocht naar links
//   else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
//     drive(30, 255);   // Bijna stilstaande linker motor voor sterke draai
//     lastDirection = 1;
//   }
//   // Zeer scherpe bocht naar links
//   else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
//     drive(0, 255);   // Volledige stop linker motor
//     lastDirection = 1;
//   }
//   // Zeer scherpe bocht naar rechts
//   else if (sensorReadings[6] >= deadzonehigh) {
//     drive(255, 0);   // Volledige stop rechter motor
//     lastDirection = -1;
//   }
//   // Lijn kwijt? Ga verder met laatste richting
//   else {
//     if (lastDirection == -1) {
//       drive(255, 30);  // Zoek naar lijn door sterke draai naar rechts
//     } 
//     else if (lastDirection == 1) {
//       drive(30, 255);  // Zoek naar lijn door sterke draai naar links
//     } 
//     else {
//       stop();
//     }
//   }

//   delay(50);  
// }





// //servo
// #define SERVO 12
// #define GRIPPER_OPEN 1750
// #define GRIPPER_CLOSE 1090

// const int motor_L1 = 6;
// const int motor_L2 = 11;
// const int motor_R1 = 10;
// const int motor_R2 = 5;


// void setup(){
//   pinMode(SERVO, OUTPUT);
//   digitalWrite(SERVO, LOW);
//   Serial.begin(9600);
// }

// void drive(int left, int right) {
//   analogWrite(motor_L1, max(0, -left));
//   analogWrite(motor_L2, max(0, left));
//   analogWrite(motor_R1, max(0, -right));
//   analogWrite(motor_R2, max(0, right));
// }


// void moveForward(int duration) {
//   drive(190, 203);
//   delay(duration);
//   drive(0, 0); // Stop de motoren
// }

// void moveServo(int pulse) {
//   static unsigned long timer;
//   static int lastPulse;
//   if (millis() > timer) {
//     if (pulse > 0) {
//       lastPulse = pulse;
//     } else {
//       pulse = lastPulse;
//     }
//     digitalWrite(SERVO, HIGH);
//     delayMicroseconds(pulse);
//     digitalWrite(SERVO, LOW);
//     timer = millis() + 20; //20ms interval voor servo
//   }
// }

// void loop() {
//   moveServo(GRIPPER_OPEN);
//   delay(1000);
//   moveForward(400); // Klein stukje vooruit
//   delay(500);
//   moveServo(GRIPPER_CLOSE);
//   delay(1000);
//   moveForward(2000); // Twee seconden vooruit
//   delay(2000);
// }




#include <Adafruit_NeoPixel.h>

#define DEBUG

#define NeoLED 13
#define NUM_PIXELS 4
#define DELAY_TIME 500

Adafruit_NeoPixel strip(NUM_PIXELS, NeoLED, NEO_GRB + NEO_KHZ800);

const int motor_L1 = 6;
const int motor_L2 = 11;
const int motor_R1 = 10;
const int motor_R2 = 5;
const int buttonOn = 7;
const int buttonOff = 4;
const int echo = 9;
const int trig = 3;
float duration, distance;
#define MAX_DISTANCE 200  
#define NUM_MEASUREMENTS 5 
const int motorsensor1 = A4;
const int motorsensor2 = A5;
int cws1 = 0, cws2 = 0;  
unsigned long previousMillis = 0;
const long interval = 1000;  
const int servoPin = 12;

const int numSensors = 8;
const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

int minValues[numSensors];
int maxValues[numSensors];

// Global variable for black count
int blackCount = 0;

// Store last known direction: -1 (left), 1 (right), 0 (center)
int lastDirection = 0;

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
  pinMode(servoPin, OUTPUT);

  Serial.begin(9600);

  for (int i = 0; i < numSensors; i++) {
    minValues[i] = 1023;
    maxValues[i] = 0;
  }

  Serial.println("Beweeg de robot over de lijn voor kalibratie...");
  strip.begin();
  strip.show();
  stop();  
}

void drive(int speedLeft, int speedRight) {
  if (speedLeft >= 0) {
    analogWrite(motor_L1, 0);
    analogWrite(motor_L2, speedLeft);
  } else {
    analogWrite(motor_L1, -speedLeft);
    analogWrite(motor_L2, 0);
  }

  if (speedRight >= 0) {
    analogWrite(motor_R1, 0);
    analogWrite(motor_R2, speedRight);
  } else {
    analogWrite(motor_R1, -speedRight);
    analogWrite(motor_R2, 0);
  }
}

// Stop motors
void stop() {
  analogWrite(motor_L1, 0);
  analogWrite(motor_L2, 0);
  analogWrite(motor_R1, 0);
  analogWrite(motor_R2, 0);
}

// Measure distance
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
}

// Read wheel sensor pulses
void readMotorSensors() {
  cws1 = pulseIn(motorsensor1, HIGH);
  cws2 = pulseIn(motorsensor2, HIGH);
}

int getPulseDifference() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    readMotorSensors();

    int pulseDifference = cws1 - cws2;
    pulseDifference /= 1200;

    Serial.print("Pulse Difference: ");
    Serial.println(pulseDifference);
    return pulseDifference;
  }
  return 0;
}

// Function to detect and count black detections
void allBlack(int sensorReadings[], int deadzonelow) {
  bool allDetected = true;

  for (int i = 1; i <= 6; i++) {
    if (sensorReadings[i] > deadzonelow) {
      allDetected = false;
      break;
    }
  }

  if (allDetected) {
    blackCount++;  // Increment black count
    Serial.print("Black Count: ");
    Serial.println(blackCount);
  }
}

void loop() {
  int sensorReadings[numSensors];
  int result = 0;
  
  // Read sensor values and calibrate
  for (int i = 0; i < numSensors; i++) {
    sensorReadings[i] = analogRead(sensorPins[i]);

    minValues[i] = min(sensorReadings[i], minValues[i]);
    maxValues[i] = max(sensorReadings[i], maxValues[i]);
  }

  for (int j = 0; j < numSensors; j++) {
    result += (minValues[j] + maxValues[j]) / 2;
  }
  result /= numSensors;
  
  int deadzonehigh = result + 50;
  int deadzonelow = result - 50;

  // Call function to count black detections
  
  allBlack(sensorReadings, deadzonelow);
  drive(208,255);
  Serial.print(blackCount);
  // If 5 detections reached, turn right and reset count
  if (blackCount >= 5) {
    int countright = 0;
    if (countright == 0)
    {
      drive(255, 10);
      delay(500);  // Allow turn to complete
      countright++;
    }

    for (int i = 0; i < numSensors; i++) {
    sensorReadings[i] = analogRead(sensorPins[i]);

    minValues[i] = min(sensorReadings[i], minValues[i]);
    maxValues[i] = max(sensorReadings[i], maxValues[i]);
    }

    for (int j = 0; j < numSensors; j++) {
      result += (minValues[j] + maxValues[j]) / 2;
    }
    result /= numSensors;
    int deadzonehigh = result + 50;
    int deadzonelow = result - 50;

    if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
      drive(214, 255);  
      lastDirection = 0;
    }
    else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
      drive(255, 170);
      lastDirection = -1;
    } 
    else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
      drive(255, 10);
      lastDirection = -1;
    }  
    else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
      drive(170, 255);
      lastDirection = 1;
    }  
    else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
      drive(60, 255);
      lastDirection = 1;
    }  
    else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
      drive(10, 255);
      lastDirection = 1;
    }  
    else {
      if (lastDirection == -1) {
        drive(255, 0);
      } 
      else if (lastDirection == 1) {
        drive(0, 255);
      } 
      else {
        drive(0, 0);
      }
    }
  }

  // Line Following Logic


  delay(50);
}