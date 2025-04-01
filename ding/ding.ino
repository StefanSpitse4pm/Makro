// Motor and button pins
const int motor_L1 = 10, motor_L2 = 11, motor_R1 = 5, motor_R2 = 6;
const int buttonOn = 2, buttonOff = 4, trig = 3, echo = 9;

float duration, distance;

void setup() {
  // Set motor and sensor pins
  pinMode(motor_L1, OUTPUT); pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT); pinMode(motor_R2, OUTPUT);

  // Use internal pull-ups to avoid floating pin issues
  pinMode(buttonOn, INPUT_PULLUP);
  pinMode(buttonOff, INPUT_PULLUP);

  pinMode(trig, OUTPUT); pinMode(echo, INPUT);

  Serial.begin(9600);
  stop(); // Ensure motors are off at start
}

// Controls motor speed and direction
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

// Measures distance using an ultrasonic sensor
void measureDistance() {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration * 0.0343) / 2; // Convert time to distance (cm)
  Serial.print("Distance: "); Serial.println(distance);
}

// Performs an action for a set duration while measuring distance
void performAction(void (*action)(int, int), int left, int right, int durationMs) {
  unsigned long startTime = millis();
  action(left, right);
  while (millis() - startTime < durationMs) measureDistance(); // Keep measuring distance
  stop();
  delay(200); // Prevent accidental re-triggering
}

void loop() {
  if (digitalRead(buttonOn) == LOW) {
    delay(200); //prevent multiple triggers

    performAction(drive, 190, 203, 2000); // Forward
    performAction(drive, -190, -203, 2000); // Backward
    performAction(drive, 190, -203, 550); // Turn right
    performAction(drive, -190, 203, 550); // Return to center
    performAction(drive, -190, 203, 550); // Turn left
    performAction(drive, 190, -203, 550); // Return to center

    stop();// full stop
  }

  // Stop if stop button is pressed
  if (digitalRead(buttonOff) == LOW) stop();
}
