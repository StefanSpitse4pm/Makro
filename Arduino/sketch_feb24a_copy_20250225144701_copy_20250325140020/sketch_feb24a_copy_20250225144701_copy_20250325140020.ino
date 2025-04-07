#define SERVO 9
#define GRIPPER_OPEN 1750
#define GRIPPER_CLOSE 1090

const int motor1Pin1 = 10;
const int motor1Pin2 = 11;
const int motor2Pin1 = 5;
const int motor2Pin2 = 6;

const int aantalSensoren = 8;
int lichtPinnen[aantalSensoren] = {A0, A1, A2, A3, A4, A5, A6, A7};

const int trigPin = 2;
const int echoPin = 3;
const int obstacleDistance = 29;

const int basisSnelheid = 180;  // Verhoogd voor snellere vooruitbeweging
const int maxSnelheid = 255;
const int draaiSnelheid = 255;  // Verhoogd voor scherpere bochten

// Versterkte correctie voor scherpere sturen
const float proportionalFactor = 2.0;  // Vergroot de correctie voor scherpere bochten (verhoogd van 1.6 naar 2.0)

const int zwartDrempel = 500;

// Variabelen
bool startMoving = true;
int sensorMin[aantalSensoren]; 
int sensorMax[aantalSensoren];

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(SERVO, OUTPUT);

  digitalWrite(SERVO, LOW);
  Serial.begin(9600);
  
  StopMotors();
  kalibreerSensors();
}

void loop() {
  int sensorWaarden[aantalSensoren];
  leesSensoren(sensorWaarden);
  int positie = 0, zwartAantal = 0;  // Declareer positie hier

  // Lijnpositie bepalen
  for (int i = 0; i < aantalSensoren; i++) {
    if (sensorWaarden[i] > zwartDrempel) {
      positie += (i - (aantalSensoren / 2)) * 10;
      zwartAantal++;
    }
  }

  // Als geen lijn gevonden, zoek opnieuw
  if (zwartAantal == 0) {
    ZoekLijn();
    return;
  }

  // Kruisingdetectie
  if (isKruising(sensorWaarden)) {
    Move(basisSnelheid, basisSnelheid);
    delay(200);
    return;
  }

  // Correctie berekenen met de juiste weging voor beide zijden (links en rechts)
  int correctie = positie * proportionalFactor;

  // Als de lijn naar links afwijkt, moeten we de snelheid van rechts vergroten
  // Als de lijn naar rechts afwijkt, moeten we de snelheid van links vergroten
  int snelheidLinks = basisSnelheid - correctie;  // Versterkt de snelheid aan de linkerkant als de lijn naar rechts afwijkt
  int snelheidRechts = basisSnelheid + correctie; // Versterkt de snelheid aan de rechterkant als de lijn naar links afwijkt

  // Verminder snelheid voor grotere correcties (scherpere bochten)
  if (abs(correctie) > 40) {  // Als de correctie groot is, gaat de bocht waarschijnlijk scherp zijn
    snelheidLinks = max(snelheidLinks - 50, 50);  // Verlaag snelheid van de linker motor
    snelheidRechts = max(snelheidRechts - 50, 50);  // Verlaag snelheid van de rechter motor
  }

  snelheidLinks = constrain(snelheidLinks, 50, maxSnelheid);
  snelheidRechts = constrain(snelheidRechts, 50, maxSnelheid);

  // Bewegingen naar links en rechts aanpassen op basis van de correctie
  Move(snelheidLinks, snelheidRechts);
  delay(20);  // Versnelde sensorupdate

  // Obstacle avoidance
  long distance = getDistance();
  if (distance <= obstacleDistance) {
    StopMotors();
    delay(500);

    // Check links en rechts
    TurnLeft();
    delay(500);
    long leftCheck = getDistance();

    if (leftCheck > obstacleDistance) {
      MoveForward(800);
      TurnRight();
      MoveForward(1200);
      TurnRight();
      MoveForward(800);
      TurnLeft();
      return;
    }

    TurnRight90();
    MoveForward(800);
    TurnLeft();
    MoveForward(1200);
    TurnLeft();
    MoveForward(800);
    TurnRight();
  }
}

void kalibreerSensors() {
  for (int i = 0; i < aantalSensoren; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }
  for (int k = 0; k < 100; k++) {
    for (int i = 0; i < aantalSensoren; i++) {
      int waarde = analogRead(lichtPinnen[i]);
      if (waarde < sensorMin[i]) sensorMin[i] = waarde;
      if (waarde > sensorMax[i]) sensorMax[i] = waarde;
    }
    delay(10);
  }
}

void leesSensoren(int *sensorWaarden) {
  for (int i = 0; i < aantalSensoren; i++) {
    sensorWaarden[i] = analogRead(lichtPinnen[i]);
  }
}

bool isKruising(int *sensorWaarden) {
  int zwartGeteld = 0;
  for (int i = 0; i < aantalSensoren; i++) {
    if (sensorWaarden[i] > zwartDrempel) zwartGeteld++;
  }
  return zwartGeteld > 5;
}

void ZoekLijn() {
  int sensorWaarden[aantalSensoren];
  int zoekRichting = random(0, 2);  // Willekeurige richting kiezen (0 voor rechts, 1 voor links)
  int draaiTijd = 0;  // Houdt de tijd bij die de robot in een bepaalde richting draait
  int maxDraaiTijd = 300;  // Verkorte maximale draai tijd voor scherpere bochten
  
  while (true) {
    leesSensoren(sensorWaarden);
    
    // Controleer of er weer een lijn is
    for (int i = 0; i < aantalSensoren; i++) {
      if (sensorWaarden[i] > zwartDrempel) {
        return;  // Lijn gevonden, stop met zoeken
      }
    }
    
    // Als we te lang in één richting draaien, verander de richting
    if (draaiTijd > maxDraaiTijd) {
      zoekRichting = (zoekRichting == 0) ? 1 : 0;  // Omkeren van richting
      draaiTijd = 0;  // Reset de draai tijd
    }
    
    // Afhankelijk van de gekozen richting draai naar links of rechts
    if (zoekRichting == 0) {
      Move(draaiSnelheid, -draaiSnelheid);  // Draai naar rechts
    } else {
      Move(-draaiSnelheid, draaiSnelheid);  // Draai naar links
    }
    
    draaiTijd += 20;  // Voeg draai-tijd toe
    delay(20);  // Kleine vertraging voor stabiliteit
  }
}

long getDistance() {
  long sum = 0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    sum += duration * 0.034 / 2;
  }
  return sum / 3;
}

void Move(int snelheidLinks, int snelheidRechts) {
  snelheidLinks = constrain(snelheidLinks, 50, maxSnelheid);
  snelheidRechts = constrain(snelheidRechts, 50, maxSnelheid);

  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, snelheidLinks);
  analogWrite(motor2Pin1, snelheidRechts);
  analogWrite(motor2Pin2, 0);
}

void StopMotors() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, 0);
  analogWrite(motor2Pin2, 0);
}

void MoveForward(int duration) {
  Move(basisSnelheid, basisSnelheid);
  delay(duration);
  StopMotors();
}

void TurnLeft() {
  Move(-draaiSnelheid, draaiSnelheid);
  delay(500);
  StopMotors();
}

void TurnRight() {
  Move(draaiSnelheid, -draaiSnelheid);
  delay(500);
  StopMotors();
}

void TurnRight90() {
  Move(draaiSnelheid, -draaiSnelheid);
  delay(955);
  StopMotors();
}

void moveServo(int pulse, int holdTime = 500) {
  unsigned long startTime = millis();
  while (millis() - startTime < holdTime) {
    digitalWrite(SERVO, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(SERVO, LOW);
    delay(20);
  }
}

void gebruikGripper() {
  moveServo(GRIPPER_OPEN, 3500);
  delay(1000);
  MoveForward(400);
  delay(500);
  moveServo(GRIPPER_CLOSE, 3500);
  delay(1000);
  MoveForward(2000);
  delay(2000);
}
