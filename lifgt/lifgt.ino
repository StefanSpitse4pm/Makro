const int motor_L1 = 6;
const int motor_L2 = 11;
const int motor_R1 = 10;
const int motor_R2 = 5;

const int numSensors = 8;
const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

int lastDirection = 0;

void setup() {
  pinMode(motor_L1, OUTPUT);
  pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT);
  pinMode(motor_R2, OUTPUT);

  Serial.begin(9600);
  stop();  
}

void drive(int left, int right) {
  analogWrite(motor_L1, max(0, -left));
  analogWrite(motor_L2, max(0, left));
  analogWrite(motor_R1, max(0, -right));
  analogWrite(motor_R2, max(0, right));
}

void stop() {
  analogWrite(motor_L1, 0);
  analogWrite(motor_L2, 0);
  analogWrite(motor_R1, 0);
  analogWrite(motor_R2, 0);
}

void loop() {
  int sensorReadings[numSensors];
  int sum = 0;

  for (int i = 0; i < numSensors; i++) {
      sensorReadings[i] = analogRead(sensorPins[i]);
      sum += sensorReadings[i];
  }

  int avg = sum / numSensors;
  int deadzonelow = avg - 50;
  int deadzonehigh = avg + 50;
  //optellen delen door aantal sensores
  // = en - voor deadzones.

  if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
      drive(214, 255);
      lastDirection = 0;
  }
  else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
      drive(255, 180);
      lastDirection = -1;
  } 
  else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
      drive(255, 60);
      lastDirection = -1;
  }  
  else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
      drive(180, 255);
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
  // lastdirection word 1 of -1 en opbasis daarvan links of rechts wa nner lijn weg is. 
  else {
      if (lastDirection == -1) {
          drive(255, 50);
      } 
      else if (lastDirection == 1) {
          drive(50, 255);
      } 
      else {
          stop();
      }
  }

  delay(50);  
}




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
