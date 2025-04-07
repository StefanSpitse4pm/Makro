#include <Adafruit_NeoPixel.h>
#define DEBUG

#define NeoLED 13
#define NUM_PIXELS 4
#define DELAY_TIME 500

// Servo
#define SERVO 12
#define GRIPPER_OPEN 1550
#define GRIPPER_CLOSE 1000

Adafruit_NeoPixel strip(NUM_PIXELS, NeoLED, NEO_GRB + NEO_KHZ800);

// Motor pins
#define motor_L1 6
#define motor_L2 11
#define motor_R2 5
#define motor_R1 10

// Ultrasonic sensor pins
#define TRIG 8
#define ECHO 9
#define TRIG_LINKS 3
#define ECHO_LINKS 2
#define TRIG_RECHTS 7
#define ECHO_RECHTS 4

int lineFollowerCount = 0;

int deadzonehigh = 0;
int deadzonelow = 0;

// Light sensors
const int numSensors = 8;
const int sensorPins[numSensors] = { A7, A6, A5, A4, A3, A2, A1, A0 };
int lichtsensorWaarden[numSensors];  // Define the array to store light sensor readings
int minValues[numSensors];
int maxValues[numSensors];
int sensorReadings[numSensors];

// Global variables for line-following logic
int lastDirection = 0;
int blackCount = 0;
int whiteCount = 0;
bool blackDetected = false;
bool whiteDetected = false;

void setup() {
  // Motor pins
  pinMode(motor_L1, OUTPUT);
  pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT);
  pinMode(motor_R2, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG_LINKS, OUTPUT);
  pinMode(ECHO_LINKS, INPUT);
  pinMode(TRIG_RECHTS, OUTPUT);
  pinMode(ECHO_RECHTS, INPUT);

  // Light sensor pins
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // Servo
  pinMode(SERVO, OUTPUT);
  digitalWrite(SERVO, LOW);

  Serial.begin(9600);

  // NeoPixel setup
  strip.begin();
  strip.show();

  stop();
}

// Function to move motors
void drive(int speedLeft, int speedRight) {
  if (speedLeft >= 0) {
    analogWrite(motor_L1, 0);
    analogWrite(motor_L2, speedLeft);
  } else {
    analogWrite(motor_L1, -speedLeft);
    analogWrite(motor_L2, 0);
  }

  if (speedRight >= 0) {
    analogWrite(motor_R1, 0);
    analogWrite(motor_R2, speedRight);
  } else {
    analogWrite(motor_R1, -speedRight);
    analogWrite(motor_R2, 0);
  }
}

// Stop motors
void stop() {
  analogWrite(motor_L1, 0);
  analogWrite(motor_L2, 0);
  analogWrite(motor_R1, 0);
  analogWrite(motor_R2, 0);
}

// Measure distance using ultrasonic sensors
float measureDistance(String kant) {
  int trig, echo;

  if (kant == "voor") {
    trig = TRIG;
    echo = ECHO;
  } else if (kant == "links") {
    trig = TRIG_LINKS;
    echo = ECHO_LINKS;
  } else if (kant == "rechts") {
    trig = TRIG_RECHTS;
    echo = ECHO_RECHTS;
  } else {
    return -1;  // Invalid input
  }

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  float duration = pulseIn(echo, HIGH);
  float distance = (duration * 0.0343) / 2;
  if (distance > 50) {
    distance = 50;
  }

  return distance;
}

// Move servo
void moveServo(int pulse) {
  unsigned long startTime = millis();

  // Generate PWM for a short time to ensure the servo moves
  while (millis() - startTime < 500) {  // Run for 500ms to give time to move
    digitalWrite(SERVO, HIGH);
    delayMicroseconds(pulse);  // Pulse width determines position
    digitalWrite(SERVO, LOW);
    delayMicroseconds(20000 - pulse);  // Remaining time in the 20ms period
  }
}

// Read light sensors
void readLightSensors() {
  for (int i = 0; i < numSensors; i++) {
    lichtsensorWaarden[i] = analogRead(sensorPins[i]);
  }
}

// Print light sensor values
void printLightSensorValues() {
  for (int i = 0; i < 8; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(lichtsensorWaarden[i]);
  }
}

void checkAndReverse(float &prevDistanceFront, float &prevDistanceLeft, float &prevDistanceRight) {
    float distanceFront = measureDistance("voor");
    float distanceLeft = measureDistance("links");
    float distanceRight = measureDistance("rechts");
    
    static int countFront = 0;
    static int countLeft = 0;
    static int countRight = 0;

    // Controleer of de afstand nauwelijks verandert (met marge van ±2) én onder 50 is
    if (abs(distanceFront - prevDistanceFront) <= 0.5 && distanceFront < 20) {
        countFront++;
    } else {
        countFront = 0;  // Reset als verandering groot genoeg is
    }

    if (abs(distanceLeft - prevDistanceLeft) <= 0.5 && distanceLeft < 20) {
        countLeft++;
    } else {
        countLeft = 0;
    }

    if (abs(distanceRight - prevDistanceRight) <= 0.5 && distanceRight < 20) {
        countRight++;
    } else {
        countRight = 0;
    }

    // Als een van de tellers 10 of meer bereikt, achteruit rijden
    if (countFront >= 15 || countLeft >= 15 || countRight >= 15) {
        Serial.println("Obstacle detected or stuck: moving back slightly...");
        drive(-255, -255);  // Rijd achteruit
        delay(500);
        drive(0, 0);
        drive(208,255);
        delay(200);

        // Reset tellers pas NA het achteruit rijden
        countFront = 0;
        countLeft = 0;
        countRight = 0;
    }

    // Update vorige metingen
    prevDistanceFront = distanceFront;
    prevDistanceLeft = distanceLeft;
    prevDistanceRight = distanceRight;
}

void turn180(float distanceFront, float distanceLeft, float distanceRight)
{
  unsigned long currentMillis = millis();
  float initialLeft =  distanceLeft;
  float initialRight = distanceRight;


  while (currentMillis < 800)
  {
    if (currentMillis < 500)
    {
      stop();
    }
    if (currentMillis < 800)
    {
      drive(-255,-255);
    }
  }

  while (distanceFront < 30 || abs(distanceRight - distanceLeft) < 5)
  {
    distanceFront = measureDistance("voor");
    distanceLeft = measureDistance("links");
    distanceRight = measureDistance("rechts");
    if (initialRight > initialLeft)
    {
      drive(150,-220);
    }
    else 
    {
      drive(-220,150);
    }
  }
  delay(75);
  return;
}

// **Base Speed Settings**
int baseSpeed = 200;
int maxSpeed = 255;
int minSpeed = 100;

// **Safe Distance Thresholds**
const float minRightDistance = 8.0;   // If closer than this, must turn left
const float minLeftDistance = 3.0;    // If closer than this, must turn right
const float minFrontDistance = 23.0;  // If closer than this, stop or reverse
const float deadEndThreshold = 5.0;   // Threshold to detect if it's completely surrounded

float prevDistanceFront = 0;
float prevDistanceLeft = 0;
float prevDistanceRight = 0;
  
int count1 = 0;

void mazeLogic() {
  // **Parallel Distance Measurement (Faster)**
  float distanceFront = measureDistance("voor");
  float distanceLeft = measureDistance("links");
  float distanceRight = measureDistance("rechts");

  checkAndReverse(prevDistanceFront, prevDistanceLeft, prevDistanceRight);

  // **Initialize Base Speed**
  int speedLeft = baseSpeed;
  int speedRight = baseSpeed;

  // **Handle Dead End (Immediate Reverse)**
  if (distanceFront <= 15 && distanceLeft <= 15 && distanceRight <= 15) {
    turn180(distanceFront, distanceLeft, distanceRight);
    return;  // Exit to avoid further calculations
  }

  // **Steering & Obstacle Avoidance**
  if (distanceFront >= minFrontDistance) {
    if (distanceRight > 20) {   // Strong Right Turn
      speedLeft = maxSpeed;
      speedRight = 100;
    } 
    else if (distanceRight < 20 && distanceFront < 15) {  // Adjust Right
      speedLeft = 100;
      speedRight = maxSpeed;
    }
    else if (distanceRight < 3) {  // 
      speedLeft = 100;
      speedRight = maxSpeed;
    } 
       else if (distanceLeft < 3) {  // Minor Left Turn
      speedLeft = maxSpeed;
      speedRight = 100;
    }
    else if (distanceRight > 9) {  // Minor Right Turn
      speedLeft = maxSpeed;
      speedRight = 190;
    } 
    else if (distanceRight < 9) {  // Minor Left Turn
      speedLeft = 190;
      speedRight = maxSpeed;
    } 
    else {  // Default Slight Right Bias
      speedLeft = 253;
      speedRight = 255;
    }
  } 

  else if (distanceRight - distanceLeft > 3 && distanceRight > 20) {  
    speedLeft = 240;
    speedRight = 25;  // Strong Right Turn
  } 
  else  if (distanceLeft  -  distanceRight > 3 && distanceLeft > 15){  
    speedLeft = 10;
    speedRight = 255;  // Strong Left Turn
  }

  // **Non-blocking Speed Update**
  drive(speedLeft, speedRight);
}

void calibrateLineSensor() {
  static bool firstRun = true;  // Track first run to initialize min/max properly

  for (int i = 0; i < numSensors; i++) {
    sensorReadings[i] = analogRead(sensorPins[i]);

    // Initialize min/max values on first run
    if (firstRun) {
      minValues[i] = sensorReadings[i];
      maxValues[i] = sensorReadings[i];
    }

    // Update min/max adaptively
    minValues[i] = min(sensorReadings[i], minValues[i]);
    maxValues[i] = max(sensorReadings[i], maxValues[i]);
  }

  firstRun = false;  // No longer the first run

  // Calculate adaptive thresholds
  int sum = 0;
  for (int j = 0; j < numSensors; j++) {
    sum += (minValues[j] + maxValues[j]) / 2;
  }
  int avgThreshold = sum / numSensors;

  // Adaptive deadzones
  deadzonehigh = avgThreshold + 30;  // Tweak for sensitivity
  deadzonelow = avgThreshold - 30;
}

void lineFollower() {

  calibrateLineSensor();

  // Line Following Logic
  if (lineFollowerCount == 1) {
    mazeLogic();
  }
  else if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
    drive(214, 255);    // Move forward
    lastDirection = 0;  // Reset direction when moving straight
    lineFollowerCount = 1;
  } else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
    drive(255, 170);     // Slight left
    lastDirection = -1;  // Remember last seen black was on the left
    lineFollowerCount = 1;
  } else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
    drive(255, 10);  // More left
    lastDirection = -1;
  } else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
    drive(170, 255);    // Slight right
    lineFollowerCount = 1;
    lastDirection = 1;  // Remember last seen black was on the right
  } else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
    drive(60, 255);  // More right
    lineFollowerCount = 1;
    lastDirection = 1;
  } else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
    drive(10, 255);  // Sharp right
    lineFollowerCount = 1;
    lastDirection = 1;
  } else {
    if (lineFollowerCount == 1) {
      return;
    } else if (lastDirection == -1) {
      drive(255, 0);  // Turn left to search
    } else if (lastDirection == 1) {
      drive(0, 255);  // Turn right to search
    } else {
      drive(214, 255);
    }
  }
  delay(50);
}

const long intervalieks = 100;          // Interval in milliseconds (1000ms / 10 = 100ms)
bool start = true;
bool turning = true;
float initialDistanceLeft = 0;
bool maze = false;

void loop() {
  calibrateLineSensor();
  float distanceLeft = measureDistance("links");

  if (initialDistanceLeft == 0)
  {
    initialDistanceLeft = measureDistance("links");
    delay(100);
    drive(252,255);
    moveServo(GRIPPER_OPEN);
    start = true;
  }

  if (distanceLeft > initialDistanceLeft && start == true)
  {
    drive(240,255);
  }
  if (distanceLeft < initialDistanceLeft && start == true)
  {
    drive(240,255);
  }
  if (distanceLeft > initialDistanceLeft + 10 && start == true)
  {
    start = false;
  }

  // Example line following logic
  if (!start) {
    if (!maze) {
      moveServo(GRIPPER_CLOSE);
      
      if (turning) {
        // Execute turn sequence
        drive(-255, -255);
        drive(-255, 255);
        delay(500);
        drive(253, 255);
        delay(100);
        
        // Update state flags
        maze = true;
        turning = false;
      }
    }
    
    lastDirection = -1;
    lineFollower();  // Follow the line
  }
}