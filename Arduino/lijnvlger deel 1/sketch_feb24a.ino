// Pin-definities
const int motor1Pin1 = 10;
const int motor1Pin2 = 11;
const int motor2Pin1 = 5;
const int motor2Pin2 = 6;
const int trigPin = 2;  // Trigger pin van de sonar

int lichtPinnen[] = {A0, A1, A2, A3, A4, A5, A6, A7};  // Array met alle pinnen
int aantalSensoren = 8;  // Aantal sensoren


const int speedCommand = 200;  // Snelheidswaarde (0-255)
const int obstacleDistance = 30; // Drempelwaarde in cm

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  Serial.begin(9600);
}


void MoveForward() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, speedCommand);
  analogWrite(motor2Pin1, speedCommand);
  analogWrite(motor2Pin2, 0);
}

void StopMotors() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, 0);
  analogWrite(motor2Pin2, 0);
}

void Turn() {
  analogWrite(motor1Pin1, speedCommand);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, speedCommand);
  analogWrite(motor2Pin2, 0);
  delay(500); // Korte draai
  StopMotors();
  delay(500);
}

void loop() {
  Serial.print("Afstand: ");
  Serial.println(" cm");
  

  for (int i = 0; i < aantalSensoren; i++) {
        Serial.print(analogRead(lichtPinnen[i]));  // Stuur meetwaarde
        Serial.print("\t");  // Tab voor scheiding
    }
    Serial.println();  // Nieuwe regel
    delay(100);
}












