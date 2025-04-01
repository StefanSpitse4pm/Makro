const int motor_L1 = 6;
const int motor_L2 = 11;
const int motor_R1 = 10;
const int motor_R2 = 5;
const int echo = 9;
const int trig = 3;

float duration, distance;

void setup() {
  pinMode(motor_L1, OUTPUT);
  pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT);
  pinMode(motor_R2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
  stop();
}

void drive(int speedLeft, int speedRight) {
  analogWrite(motor_L1, speedLeft >= 0 ? 0 : -speedLeft);
  analogWrite(motor_L2, speedLeft >= 0 ? speedLeft : 0);
  analogWrite(motor_R1, speedRight >= 0 ? 0 : -speedRight);
  analogWrite(motor_R2, speedRight >= 0 ? speedRight : 0);
}

void stop() {
  analogWrite(motor_L1, 0);
  analogWrite(motor_L2, 0);
  analogWrite(motor_R1, 0);
  analogWrite(motor_R2, 0);
}

void measureDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = (duration * 0.0343) / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(150);
}

void avoidObstacle() {
  Serial.println("Avoiding obstacle...");
  stop();
  delay(100);

  // Draai naar links
  drive(100, 220);
  delay(600);

  // Rijd vooruit
  drive(210, 210);
  delay(500);

  // Draai naar rechts
  drive(220, 100);
  delay(600);

  // Rijd vooruit
  drive(210, 210);
  delay(500);

  // Draai naar rechts
  drive(220, 100);
  delay(600);

  // Rijd vooruit
  drive(210, 210);
  delay(500);

  // Draai naar links
  drive(100, 220);
  delay(600);

  stop();
  delay(200);
}

void loop() {
  measureDistance();

  if (distance > 0 && distance <= 25) {
    avoidObstacle();
  } else {
    drive(-251, 255);
    Serial.println("No obstacle, moving forward...");
  }
  
  delay(50);
}