#include <Adafruit_NeoPixel.h>

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

const int basisSnelheid = 140;
const int maxSnelheid = 255;
const int zwartDrempel = 500;

#define LED_PIN 4
#define NUMPIXELS 4
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_RGB + NEO_KHZ800);

int sensorWaarden[aantalSensoren];
int sensorMin[aantalSensoren];
int sensorMax[aantalSensoren];
int laatsteRichting = 1;  // 1 = rechts, -1 = links (standaard eerst rechts zoeken)

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(SERVO, OUTPUT);

  strip.begin();
  strip.clear();
  strip.show();

  digitalWrite(SERVO, LOW);
  Serial.begin(9600);
  StopMotors();
  kalibreerSensors();
}

void loop() {
  leesSensoren(sensorWaarden);

  int positie = 0, zwartAantal = 0;
  int witteSensoren = 0;

  // Bereken de gemiddelde positie van de lijn en tel de witte sensoren
  for (int i = 0; i < aantalSensoren; i++) {
    if (sensorWaarden[i] > zwartDrempel) {
      positie += (i - (aantalSensoren / 2)) * 15;  // Bereken de positie van de lijn
      zwartAantal++;
    } else {
      witteSensoren++;  // Tel het aantal witte sensoren
    }
  }

  // Als de robot de lijn niet ziet (alle sensoren zijn wit), blijf niet stil staan, maar zoek actief de lijn
  if (zwartAantal == 0) {
    // Blijf zoeken naar de lijn zonder stil te staan
    zoekLijn();
    return;
  }

  // Als teveel witte sensoren aan de zijkanten, draai dan actief terug naar het midden
  if (witteSensoren >= (aantalSensoren / 2)) { // Als meer dan de helft wit is, is de robot van de lijn af
    terugNaarMidden();
  } else {
    // Bereken de correctie op basis van de positie van de lijn
    int correctie = positie * 0.8;  // Verhoog de correctiefactor om sneller te corrigeren

    int snelheidLinks = basisSnelheid - correctie;
    int snelheidRechts = basisSnelheid + correctie;

    // Beperk de snelheid zodat deze niet te hoog of te laag is
    snelheidLinks = constrain(snelheidLinks, 80, maxSnelheid);
    snelheidRechts = constrain(snelheidRechts, 80, maxSnelheid);

    // Voorkom dat de robot te traag is aan één kant
    if (positie > 0) {
      snelheidLinks = max(snelheidLinks, basisSnelheid / 2);
    } else if (positie < 0) {
      snelheidRechts = max(snelheidRechts, basisSnelheid / 2);
    }

    // Voer de motorbeweging uit met de gecorrigeerde snelheden
    Move(snelheidLinks, snelheidRechts);
    setLightsForward();
    delay(5);
  }

  // Meet obstakels en handel deze af
  long distance = getDistance();
  if (distance <= obstacleDistance) {
    StopMotors();
    setLightsObstacle();
    delay(300);
    handleObstacle();
  }
}

// Functie om terug naar het midden van de lijn te gaan als te veel witte sensoren worden gedetecteerd
void terugNaarMidden() {
  int zoekSnelheid = 100;  // Langzamere snelheid om meer controle te hebben
  int draaiFactor = 1;  // Begin met draaien naar rechts

  setLightsSearching();

  // Blijf draaien totdat de robot de lijn weer vindt
  while (true) {
    Move(zoekSnelheid * draaiFactor, -zoekSnelheid * draaiFactor);  // Draai de robot
    leesSensoren(sensorWaarden);

    int zwartAantal = 0;
    for (int i = 0; i < aantalSensoren; i++) {
      if (sensorWaarden[i] > zwartDrempel) {
        zwartAantal++;
      }
    }

    if (zwartAantal > 0) {  // Als de lijn wordt gevonden
      StopMotors();  // Stop de robot
      return;
    }

    draaiFactor *= -1;  // Draai de richting om als geen lijn gevonden wordt
    delay(200);  // Korte vertraging voor stabiliteit
  }
}

// Functie om actief naar de lijn te zoeken zonder stil te staan
void zoekLijn() {
  int zoekSnelheid = 100;  // Langzamere snelheid om meer controle te hebben
  int draaiFactor = 1;  // Begin met draaien naar rechts

  setLightsSearching();

  // Blijf draaien totdat de robot de lijn weer vindt
  while (true) {
    Move(zoekSnelheid * draaiFactor, -zoekSnelheid * draaiFactor);  // Draai de robot
    leesSensoren(sensorWaarden);

    int zwartAantal = 0;
    for (int i = 0; i < aantalSensoren; i++) {
      if (sensorWaarden[i] > zwartDrempel) {
        zwartAantal++;
      }
    }

    if (zwartAantal > 0) {  // Als de lijn wordt gevonden
      StopMotors();  // Stop de robot
      return;
    }

    draaiFactor *= -1;  // Draai de richting om als geen lijn gevonden wordt
    delay(200);  // Korte vertraging voor stabiliteit
  }
}

void handleObstacle() {
  long leftCheck = getDistance();
  TurnLeft();
  delay(300);
  if (leftCheck > obstacleDistance) {
    MoveForward(700);
    TurnRight();
    MoveForward(1100);
    TurnRight();
    MoveForward(700);
    TurnLeft();
  } else {
    TurnRight90();
    MoveForward(700);
    TurnLeft();
    MoveForward(1100);
    TurnLeft();
    MoveForward(700);
    TurnRight();
  }
  ZoekLijnNaObstacle(laatsteRichting);
}

void ZoekLijnNaObstacle(int laatsteRichting) {
  int zoekSnelheid = 100;  // Langzamere snelheid om meer controle te hebben
  int draaiFactor = laatsteRichting;
  int maxPogingen = 10;  // Limiteer het aantal zoekpogingen om vastlopen te voorkomen
  int pogingen = 0;  // Telt het aantal pogingen

  setLightsSearching();

  // Blijf draaien totdat de robot de lijn weer vindt of het maximaal aantal pogingen is bereikt
  while (pogingen < maxPogingen) {
    Move(zoekSnelheid * draaiFactor, -zoekSnelheid * draaiFactor);
    leesSensoren(sensorWaarden);

    for (int i = 0; i < aantalSensoren; i++) {
      if (sensorWaarden[i] > zwartDrempel) {
        StopMotors();
        return;
      }
    }

    draaiFactor *= -1;  // Draai de richting om als geen lijn gevonden wordt
    pogingen++;  // Verhoog het aantal pogingen
    delay(200);  // Korte vertraging voor stabiliteit
  }

  // Als de lijn niet gevonden is, stop dan en voer alternatieve actie uit
  StopMotors();
  // Je kunt hier extra logica toevoegen voor alternatieve actie (bijv. terugkeren naar startpositie)
  Serial.println("Lijn niet gevonden na max pogingen.");
}

void Move(int snelheidLinks, int snelheidRechts) {
  snelheidLinks = constrain(snelheidLinks, 80, maxSnelheid);
  snelheidRechts = constrain(snelheidRechts, 80, maxSnelheid);

  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, snelheidLinks);
  analogWrite(motor2Pin1, snelheidRechts);
  analogWrite(motor2Pin2, 0);

  // Update laatste richting
  if (snelheidLinks > snelheidRechts) {
    laatsteRichting = -1;  // Links gaat harder → beweging naar links
  } else if (snelheidRechts > snelheidLinks) {
    laatsteRichting = 1;   // Rechts gaat harder → beweging naar rechts
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

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void StopMotors() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, 0);
  analogWrite(motor2Pin2, 0);
  strip.clear();
  strip.show();
}

void MoveForward(int duration) {
  Move(basisSnelheid, basisSnelheid);
  setLightsForward();
  delay(duration);
  StopMotors();
}

void TurnLeft() {
  Move(-maxSnelheid, maxSnelheid);
  setLightsLeft();
  delay(350);
  StopMotors();
}

void TurnRight() {
  Move(maxSnelheid, -maxSnelheid);
  setLightsRight();
  delay(350);
  StopMotors();
}

void TurnRight90() {
  Move(maxSnelheid, -maxSnelheid);
  setLightsRight();
  delay(800);
  StopMotors();
}

void setLightsForward() {
  strip.setPixelColor(0, strip.Color(0, 0, 255)); // Blauw voor vooruit
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.show();
}

void setLightsObstacle() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));  // Rood voor obstakel
  }
  strip.show();
}

void setLightsSearching() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));  // Groen voor zoeken
  }
  strip.show();
}

void setLightsLeft() {
  strip.setPixelColor(0, strip.Color(255, 165, 0));  // Oranje voor links
  strip.setPixelColor(1, strip.Color(255, 165, 0));
  strip.show();
}

void setLightsRight() {
  strip.setPixelColor(2, strip.Color(255, 165, 0));  // Oranje voor rechts
  strip.setPixelColor(3, strip.Color(255, 165, 0));
  strip.show();
}
