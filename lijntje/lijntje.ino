// Motor and sensor pins
const int motor_L1 = 10, motor_L2 = 11, motor_R1 = 5, motor_R2 = 6;
const int buttonOn = 2, buttonOff = 4;
const int leftSideSensor = A0, rightSideSensor = A3;
const int centerLeftSensor = A1, centerRightSensor = A2;

// Sensor thresholds (adjust these values based on your light sensor calibration)
const int sensorThreshold = 400;  // Lower threshold to detect black line on light surface

void setup() {
  pinMode(motor_L1, OUTPUT); pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT); pinMode(motor_R2, OUTPUT);

  pinMode(buttonOn, INPUT_PULLUP);
  pinMode(buttonOff, INPUT_PULLUP);

  pinMode(leftSideSensor, INPUT); 
  pinMode(rightSideSensor, INPUT);
  pinMode(centerLeftSensor, INPUT);
  pinMode(centerRightSensor, INPUT);

  Serial.begin(9600);
  stop(); // Ensure motors are off at start
}

// Controls motor direction (forward or backward)
void drive(int left, int right) {
  analogWrite(motor_L1, left > 0 ? 0 : -left);
  analogWrite(motor_L2, left > 0 ? left : 0);
  analogWrite(motor_R1, right > 0 ? 0 : -right);
  analogWrite(motor_R2, right > 0 ? right : 0);
}

// Stops all motors
void stop() {
  analogWrite(motor_L1, 0); analogWrite(motor_L2, 0);
  analogWrite(motor_R1, 0); analogWrite(motor_R2, 0);
}

// Read the light sensors
int readLeftSideSensor() {
  return analogRead(leftSideSensor);
}

int readRightSideSensor() {
  return analogRead(rightSideSensor);
}

int readCenterLeftSensor() {
  return analogRead(centerLeftSensor);
}

int readCenterRightSensor() {
  return analogRead(centerRightSensor);
}

void loop() {
  if (digitalRead(buttonOn) == LOW) {
    delay(200); // Prevent multiple triggers

    while (true) {
      int leftSide = readLeftSideSensor();
      int rightSide = readRightSideSensor();
      int centerLeft = readCenterLeftSensor();
      int centerRight = readCenterRightSensor();

      // Debugging: print sensor values to serial monitor
      Serial.print("Left Side: "); Serial.print(leftSide);
      Serial.print(" Right Side: "); Serial.print(rightSide);
      Serial.print(" Center Left: "); Serial.print(centerLeft);
      Serial.print(" Center Right: "); Serial.println(centerRight);

      // Follow the line
      if (centerLeft < sensorThreshold && centerRight < sensorThreshold) {
        // Both center sensors on the black line, go forward
        drive(190, 203); // Continue moving forward
      }
      else if (leftSide < sensorThreshold) {
        // Left side sensor detects the black line, steer right
        drive(190, 210);  // Turn right to get back to the line
      }
      else if (rightSide < sensorThreshold) {
        // Right side sensor detects the black line, steer left
        drive(210, 190);  // Turn left to get back to the line
      }
      else if (centerLeft > sensorThreshold && centerRight < sensorThreshold) {
        // Left sensor off the line, steer right to correct
        drive(190, 210); // Turn right to bring it back to the line
      }
      else if (centerRight > sensorThreshold && centerLeft < sensorThreshold) {
        // Right sensor off the line, steer left to correct
        drive(210, 190); // Turn left to bring it back to the line
      }
      else {
        // If no sensors detect the line (both side sensors off), stop
        stop();
      }

      delay(100);  // Adjust to prevent too fast of a loop
    }
  }

  // Stop if stop button is pressed
  if (digitalRead(buttonOff) == LOW) stop();
}
