const int linksVoor = 10;
const int linksAchter = 11;
const int rechtsVoor = 6;
const int rechtsAchter = 9;

const int trigpin = 13;
const int echopin = 12;
const int balancer = 35;
const int rotateOne = 3;
const int rotateTwo = 5;
int distance;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(rotateOne, OUTPUT);
  Serial.begin(9600);

  }

void loop() {
  // put your main code here, to run repeatedly:
  distance = distanceCheck();

  if (distance <= 15) {
    analogWrite(linksVoor, 0);
    analogWrite(rechtsVoor, 0); 
    delay(100);
    pathfinder();
  } else {
    forward(200);
  }
}

void pathfinder() {
  Serial.println("In pathfinder");
  axisLeft(200, balancer);
  delay(500);
  axisLeft(0, 0);
  delay(200);
  forward(200);
  delay(1000);
  forward(0);
  axisRight(200, balancer);
  delay(500);
  axisRight(0, 0);
  forward(200);
  delay(1000);
  forward(0)
  axisRight(200, balancer);
  delay(500);
  axisRight(0,0);
  Serial.println("out pathfinder");
}

void forward(int speed) {
  analogWrite(linksVoor, speed);
  analogWrite(rechtsVoor, speed + balancer);
}

float distanceCheck() {
  float duration, distance;
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  return distance = (duration * 0.034) / 2;
}

void axisLeft(int speed, int balancer) {
  analogWrite(linksVoor, 0);
  analogWrite(linksAchter, speed);
  analogWrite(rechtsAchter, 0);
  analogWrite(rechtsVoor,speed + balancer);
}

void axisRight(int speed, int balancer) {
  analogWrite(rechtsVoor, 0);
  analogWrite(rechtsAchter, speed);
  analogWrite(linksAchter, 0);
  analogWrite(linksVoor,speed + balancer);
}