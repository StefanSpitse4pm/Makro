

#include <Adafruit_NeoPixel.h>
#define DEBUG

#define NeoLED 13
#define NUM_PIXELS 4
#define DELAY_TIME 500

// Servo
#define SERVO 12
#define GRIPPER_OPEN 1550
#define GRIPPER_CLOSE 1000

Adafruit_NeoPixel strip(NUM_PIXELS, NeoLED, NEO_GRB + NEO_KHZ800);

// Motor pins
#define motor_L1 11
#define motor_L2 6
#define motor_R1 10
#define motor_R2 5

// Buttons
const int buttonOn = 3;   // Changed pin to avoid conflict
const int buttonOff = 2;  // Changed pin to avoid conflict
int buttonState = HIGH;
int buttonState2 = HIGH;


// Motor sensor
int cws1 = 0, cws2 = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
const int fullspeedLinksVooruit = 214;
const int fullspeedLinksAchteruit = -241.5;

// Light sensors
int lichtsensorWaarden[8];
const int lichtsensorPins[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };

void setup() {
  // Motor pins
  pinMode(motor_L1, OUTPUT);
  pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT);
  pinMode(motor_R2, OUTPUT);

  // Button pins
  pinMode(buttonOn, INPUT_PULLUP);
  pinMode(buttonOff, INPUT_PULLUP);


  // Ultrasonic sensor pins
#define ECHO 9
#define TRIG 3
#define TRIG_LINKS 2
#define ECHO_LINKS 8
#define TRIG_RECHTS 7
#define ECHO_RECHTS 4
  // Ultrasonic sensor pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG_LINKS, OUTPUT);
  pinMode(ECHO_LINKS, INPUT);
  pinMode(TRIG_RECHTS, OUTPUT);
  pinMode(ECHO_RECHTS, INPUT);

  // Light sensor pins
  for (int i = 0; i < 8; i++) {
    pinMode(lichtsensorPins[i], INPUT);
  }

  // Servo
  pinMode(SERVO, OUTPUT);
  digitalWrite(SERVO, LOW);

  Serial.begin(9600);

  // NeoPixel setup
  strip.begin();
  strip.show();

  stop();
}

// Function to move motors
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

// Measure distance using ultrasonic sensors
float measureDistance(String kant) {
  int trig, echo;

  if (kant == "voor") {
    trig = TRIG;
    echo = ECHO;
  } else if (kant == "links") {
    trig = TRIG_LINKS;
    echo = ECHO_LINKS;
  } else if (kant == "rechts") {
    trig = TRIG_RECHTS;
    echo = ECHO_RECHTS;
  } else {
    return -1;  // Invalid input
  }

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  float duration = pulseIn(echo, HIGH);
  float distance = (duration * 0.0343) / 2;
  if (distance > 50) {
    distance = 50;
  }

  Serial.print("Distance (");
  Serial.print(kant);
  Serial.print("): ");
  Serial.println(distance);

  return distance;
}

// Move servo
void moveServo(int pulse) {
  static unsigned long timer;
  static int lastPulse;

  if (millis() > timer) {
    if (pulse > 0) {
      lastPulse = pulse;
    } else {
      pulse = lastPulse;
    }

    digitalWrite(SERVO, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(SERVO, LOW);

    timer = millis() + 20;  // 20ms interval for servo
  }
}

// Read light sensors
void readLightSensors() {
  for (int i = 0; i < 8; i++) {
    lichtsensorWaarden[i] = analogRead(lichtsensorPins[i]);
  }
}

// Print light sensor values
void printLightSensorValues() {
  for (int i = 0; i < 8; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(lichtsensorWaarden[i]);
  }
}

void loop() {
  float distanceFront = measureDistance("voor");
  float distanceLeft = measureDistance("links");
  float distanceRight = measureDistance("rechts");

  // If the front distance is greater than 23 and the difference between left and right is less than 15 (both ways)
  if (distanceFront > 22) {
    if (abs(distanceLeft - distanceRight) > 3){
      if(distanceRight > 9){
        drive(25, 255);
      } else if (distanceRight < 9){
        drive(180, 255);
      }
    } else {
      drive(251, 255);  // Move forward with a slight bias to the right
    }
  } 
  // If the right side has more space, turn right
  else if (distanceRight - distanceLeft > 7) {
    drive(255, 80);  // Turn right slightly
  } 
  // If the left side has more space, turn left
  else if (distanceLeft - distanceRight > 7 && distanceRight < 40) {
    drive(80, 255);  // Turn left slightly
  } 
  // If all directions are blocked, reverse and make turns to reorient
  else if (distanceFront <= 15 && distanceLeft <= 15 && distanceRight <= 15) {
    if (distanceLeft > distanceRight) {
      drive(-50, -255);  // Reverse and turn left
    } else {
      drive(-255, -60); // Reverse and turn right
    }
    delay(200);
    
    // Reverse further and turn more aggressively
    drive(-255, 255);  
    delay(500);
    
    // Reverse even more and reorient the robot
    drive(-255, -255);
    delay(100);
  }
}


// #include <Arduino.h>

// const int motor_L1 = 6;
// const int motor_L2 = 11;
// const int motor_R1 = 10;
// const int motor_R2 = 5;

// const int numSensors = 8;
// const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

// #define SERVO 12
// #define GRIPPER_OPEN 1750
// #define GRIPPER_CLOSE 1090

// int lastDirection = 0;

// void setup() {
//     pinMode(motor_L1, OUTPUT);
//     pinMode(motor_L2, OUTPUT);
//     pinMode(motor_R1, OUTPUT);
//     pinMode(motor_R2, OUTPUT);
//     pinMode(SERVO, OUTPUT);
//     digitalWrite(SERVO, LOW);
//     Serial.begin(9600);
//     stop();
//     calibrate();
// }

// void drive(int left, int right) {
//     analogWrite(motor_L1, max(0, -left));
//     analogWrite(motor_L2, max(0, left));
//     analogWrite(motor_R1, max(0, -right));
//     analogWrite(motor_R2, max(0, right));
// }

// void stop() {
//     drive(0, 0);
// }

// void calibrate() {
//     delay(1000);
//     drive(100, 100);
//     delay(500);
//     stop();
//     delay(500);
// }

// void moveServo(int pulse) {
//     static unsigned long timer;
//     static int lastPulse;
//     if (millis() > timer) {
//         if (pulse > 0) {
//             lastPulse = pulse;
//         } else {
//             pulse = lastPulse;
//         }
//         digitalWrite(SERVO, HIGH);
//         delayMicroseconds(pulse);
//         digitalWrite(SERVO, LOW);
//         timer = millis() + 20;
//     }
// }

// void pickUpPion() {
//     moveServo(GRIPPER_OPEN);
//     delay(1000);
//     drive(100, 100);
//     delay(400);
//     stop();
//     delay(500);
//     moveServo(GRIPPER_CLOSE);
//     delay(1000);
//     drive(-100, -100);
//     delay(400);
//     stop();
//     delay(500);
// }

// void turnLeft() {
//     drive(-100, 100);
//     delay(800);
//     stop();
// }

// void followLine() {
//     while (true) {
//         int sensorReadings[numSensors];
//         int sum = 0;
//         for (int i = 0; i < numSensors; i++) {
//             sensorReadings[i] = analogRead(sensorPins[i]);
//             sum += sensorReadings[i];
//         }
//         int avg = sum / numSensors;
//         int deadzonelow = avg - 50;
//         int deadzonehigh = avg + 50;

//         if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
//             drive(214, 255);
//             lastDirection = 0;
//         } else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
//             drive(255, 140);
//             lastDirection = -1;
//         } else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
//             drive(255, 30);
//             lastDirection = -1;
//         } else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
//             drive(140, 255);
//             lastDirection = 1;
//         } else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
//             drive(30, 255);
//             lastDirection = 1;
//         } else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
//             drive(0, 255);
//             lastDirection = 1;
//         } else if (sensorReadings[6] >= deadzonehigh) {
//             drive(255, 0);
//             lastDirection = -1;
//         } else {
//             if (lastDirection == -1) {
//                 drive(255, 30);
//             } else if (lastDirection == 1) {
//                 drive(30, 255);
//             } else {
//                 stop();
//             }
//         }
//         delay(50);
//     }
// }

// void loop() {
//     pickUpPion();
//     turnLeft();
//     followLine();
// }
