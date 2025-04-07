const int motor_L1 = 6;
const int motor_L2 = 11;
const int motor_R1 = 10;
const int motor_R2 = 5;

const int numSensors = 8;
const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

const int trigPin = 2;
const int echoPin = 3;

const int speedCommand = 200;
const int obstacleDistance = 29;
const int turnDuration90 = 955;

const int gripperPin = 9;  // Pin for controlling the gripper

bool startMoving = true;

int lastDirection = 0;

void setup() {
  pinMode(motor_L1, OUTPUT);
  pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT);
  pinMode(motor_R2, OUTPUT);
  
  pinMode(gripperPin, OUTPUT);  // Set the gripper pin as an output

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

void openGripper() {
  digitalWrite(gripperPin, LOW);  // Open the gripper (assuming LOW is open)
  Serial.println("Gripper opened.");
}

void closeGripper() {
  digitalWrite(gripperPin, HIGH);  // Close the gripper (assuming HIGH is closed)
  Serial.println("Gripper closed.");
}

void loop() {
  int sensorReadings[numSensors];
  int sum = 0;

  // Lees de sensoren in
  for (int i = 0; i < numSensors; i++) {
    sensorReadings[i] = analogRead(sensorPins[i]);
    sum += sensorReadings[i];
  }

  // Bereken het gemiddelde voor de dodezone
  int avg = sum / numSensors;
  int deadzonelow = avg - 50;
  int deadzonehigh = avg + 50;

  // **Controleer of zowel A0 (rechts) als A7 (links) hoge waarden hebben**
  if (sensorReadings[0] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
    // Als beide sensoren de lijn detecteren, voer dan een lichte correctie uit
    drive(150, 150);  // Verlaag de snelheid iets om de bocht te nemen zonder vast te lopen
    lastDirection = 0;  // Geen specifieke richting (gaat rechtdoor)
  }
  // **Rechtdoor rijden (middelste sensoren volgen de lijn)**
  else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
    drive(180, 200);  // Rij rechtdoor, iets trager
    lastDirection = 0;
  }
  // **Kleine correctie naar rechts (sensoren 5 en 6)**
  else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
    drive(200, 160);  // Kleine correctie naar rechts met vertraging
    lastDirection = -1;
  } 
  // **Scherpe bocht naar rechts (sensoren 2 en 3)**
  else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
    drive(40, 255);   // Scherpe draai naar rechts met versterkt verschil in snelheid
    lastDirection = -1;  // Aanduiding dat we naar rechts draaien
  }  
  // **Kleine correctie naar links (sensoren 1 en 2)**
  else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
    drive(60, 255);  // Mildere correctie naar links om te voorkomen dat hij stopt
    lastDirection = 1;
  }
  // **Scherpe draai naar links (sensoren 0, 1, 3 en 4)**
  else if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) {
    drive(255, 60);  // Scherpe draai naar links
    lastDirection = 1;  // Aanduiding dat we naar links draaien
  }
  // **Gebruik A3 en A4 voor linkse bochten (hier wordt A4 echt belangrijk!)**
  else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
    drive(255, 120);  // Meer controle aan de linkerkant bij een bocht
    lastDirection = 1;  // Aanduiding dat we naar links draaien
  }
  // **Als de lijn niet gedetecteerd wordt, maak een sterke correctie naar rechts of links**
  else {
    if (lastDirection == -1) {
        drive(255, 50);  // Versterkte reactie naar rechts
    } 
    else if (lastDirection == 1) {
        drive(50, 255);  // Versterkte reactie naar links
    } 
    else {
        stop();  // Als er geen lijn is, stop dan
    }
  }

  // Example of activating the gripper
  // Let's say we activate the gripper if an obstacle is detected by the ultrasonic sensor
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Calculate distance

  if (distance < obstacleDistance) {
    closeGripper();  // Close the gripper if an obstacle is detected
  } else {
    openGripper();  // Open the gripper if no obstacle
  }

  delay(50);  // Snelle updates voor stabiliteit
}
