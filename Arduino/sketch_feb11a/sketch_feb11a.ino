
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
const int obstacleDistance = 16;

const int basisSnelheid = 165;  // Verhoogd voor snellere vooruitbeweging
const int maxSnelheid = 235;
const int draaiSnelheid = 290;  // Verhoogd voor scherpere bochten

// Versterkte correctie voor scherpere sturen
const float proportionalFactor = 1.0;  // Vergroot de correctie voor scherpere bochten (verhoogd van 1.6 naar 2.0)

const int zwartDrempel = 400;

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
  
  // Afdrukken van sensorwaarden voor debugging
  for (int i = 0; i < aantalSensoren; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorWaarden[i]);
    Serial.print("\t");
  }
  Serial.println();

  // Kijken of de lijn afwijkt naar links of rechts
  bool lijnNaarLinks = sensorWaarden[0] > zwartDrempel && sensorWaarden[1] < zwartDrempel;
  bool lijnNaarRechts = sensorWaarden[aantalSensoren - 1] > zwartDrempel && sensorWaarden[aantalSensoren - 2] < zwartDrempel;

  // Als de lijn naar links afwijkt (linker sensor ziet de lijn)
  if (lijnNaarLinks) {
    Move(basisSnelheid, basisSnelheid - 50);  // Draai naar links (versterk links)
  }
  // Als de lijn naar rechts afwijkt (rechter sensor ziet de lijn)
  else if (lijnNaarRechts) {
    Move(basisSnelheid - 50, basisSnelheid);  // Draai naar rechts (versterk rechts)
  }
  // Als de lijn recht voor de robot is (de robot volgt de lijn zonder correctie)
  else {
    Move(basisSnelheid, basisSnelheid);  // Rechtdoor bewegen
  }

  delay(12);  // Versnelde sensorupdate
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
  Serial.println("Sensoren gekalibreerd!");
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
  int maxDraaiTijd = 330;  // Verkorte maximale draai tijd voor scherpere bochten
  
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

void LijnKwijt(int laatstePositie) {
    int zoekRichting = (laatstePositie > 0) ? 1 : -1; // Was de lijn rechts? Draai zachtjes terug naar links.

    while (true) {
        int sensorWaarden[aantalSensoren];
        leesSensoren(sensorWaarden);
        
        // Controleer of de lijn weer gevonden is
        for (int i = 0; i < aantalSensoren; i++) {
            if (sensorWaarden[i] > zwartDrempel) {
                return;  // Stop met zoeken, want lijn is terug
            }
        }

        // Maak een zachte bocht terug naar de lijn
        if (zoekRichting == 1) {
            Move(100, 60);  // Langzaam terug naar links
        } else {
            Move(60, 100);  // Langzaam terug naar rechts
        }

        delay(20);  // Wacht kort voor stabiliteit
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