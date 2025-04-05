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
  reads = digitalRead(rotateOne);
  Serial.println(reads);
  delay(2000);
  // if (distance <= 10 ) {
  //   axisLeft(200, balancer);
  //   delay(250);
  //   axisLeft(0, 0);
  // } else {
  analogWrite(linksVoor, 200);
  analogWrite(rechtsVoor, 200 + 30);
  // delay(2000);
  // analogWrite(linksVoor, 0);
  // analogWrite(rechtsVoor, 0);
  
  // }
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