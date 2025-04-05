// Motor pin definitions
#define MOTOR_L1 11
#define MOTOR_L2 10
#define MOTOR_R1 9
#define MOTOR_R2 6

#define NUMSENSORS 8 
#define ROTATION_1 5

// Speed definitions
#define FULLSPEED 225

#define STEADY_SPEED 184
#define SLOW_SPEED 150
#define SLOWER_SPEED 30
#define SLOWEST_SPEED 0
#define SEARCH_SPEED 20

#define TRIG_PIN 13
#define ECHO_PIN 12

#define DISTANCE_TURN_AROUND 15
// Sensor pins
const int sensorPins[NUMSENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7}; 

int _lastDirection = 0;
bool leftTurnInProgress = false;
bool rightTurnInProgress = false;
int endedTurning = 0;
int startedTurning = 0;
bool started = true;
bool startFinished = true;
int counter = 0;
unsigned long sinceSetup;



void setup() { 
  pinMode(MOTOR_L1, OUTPUT); 
  pinMode(MOTOR_L2, OUTPUT); 
  pinMode(MOTOR_R1, OUTPUT); 
  pinMode(MOTOR_R2, OUTPUT); 
  pinMode(ROTATION_1, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600); 
  sinceSetup = millis();
  stop();   
} 

void drive(int left, int right) { 
  analogWrite(MOTOR_L1, max(0, -left)); 
  analogWrite(MOTOR_L2, max(0, left)); 
  analogWrite(MOTOR_R1, max(0, -right)); 
  analogWrite(MOTOR_R2, max(0, right)); 
} 


void stop() { 
  analogWrite(MOTOR_L1, 0); 
  analogWrite(MOTOR_L2, 0); 
  analogWrite(MOTOR_R1, 0); 
  analogWrite(MOTOR_R2, 0); 
} 

void loop() { 
  // startFunction();
  followLine();
  endedTurning++;
  int sensorReadingsMockup[NUMSENSORS];
  int deadzonehighmockup;


}

void axis(int speed) 
{
  analogWrite(MOTOR_L1, speed); 
  analogWrite(MOTOR_L2, 0); 
  analogWrite(MOTOR_R1, 0); 
  analogWrite(MOTOR_R2, speed); 
}

void axisUntilLineFound(int sensorReadings[NUMSENSORS], int deadzoneHigh) 
{
  
  if (sensorReadings[3] >= deadzoneHigh && sensorReadings[4] >= deadzoneHigh) 
  {
    axis(0);
    return; 
  }

  axis(STEADY_SPEED);
  int sum = 0;
  for (int i = 0; i < NUMSENSORS; i++) { 
      sensorReadings[i] = analogRead(sensorPins[i]);
      sum += sensorReadings[i]; 
  }
  int avg = sum / NUMSENSORS; 
  int _deadzonehigh = avg + 50;  

  axisUntilLineFound(sensorReadings, _deadzonehigh);
}

void startFunction() 
{
    if (started) 
    {
      return;
    }

    if (startFinished) 
    {
      stop();
      return;
    }

    if (millis() - sinceSetup < 1000) 
    {
      drive(STEADY_SPEED,STEADY_SPEED);
      return;
    }

 
    int sensorReadings[NUMSENSORS]; 
    int sum = 0; 
    for (int i = 0; i < NUMSENSORS; i++) { 
        sensorReadings[i] = analogRead(sensorPins[i]); 
        sum += sensorReadings[i]; 
    } 
    int start_avg = sum / NUMSENSORS;
    int high = start_avg + 50;
    bool allBlack = true;
    for (int i = 0; i < NUMSENSORS; i++) { 
      if (!sensorReadings[i] > high) 
      {
        break;
        allBlack = false;
      }
    }

    if (allBlack)  
    {
      counter += 1;
    }
    else {
      counter = 0;
    }

    if (counter  == 5) 
    {
      stop();
      counter = 0;
      startFinished = true;
      // started = true;

    }
    else 
    {
      drive(STEADY_SPEED, STEADY_SPEED);
    }
    Serial.println(counter);

}

float distanceCheck() {  
      float duration, distance;
      digitalWrite(TRIG_PIN, LOW);
      digitalWrite(TRIG_PIN, HIGH);
      digitalWrite(TRIG_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
      return (duration * 0.034) / 2;
    }

void followLine() {
    if (!started) 
    {
      return;
    } 
    int sensorReadings[NUMSENSORS]; 
    int sum = 0; 


    for (int i = 0; i < NUMSENSORS; i++) { 
        sensorReadings[i] = analogRead(sensorPins[i]); 
        sum += sensorReadings[i]; 
    } 

    int avg = sum / NUMSENSORS; 
    int deadzonehigh = avg + 50;  

    if (distanceCheck() < DISTANCE_TURN_AROUND) 
    {
      axis(STEADY_SPEED);
      axisUntilLineFound(sensorReadings, deadzonehigh);
      return;
    }

    if (leftTurnInProgress) {
        if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh && startedTurning >= 25 && endedTurning > 25) {
            leftTurnInProgress = false;
            startedTurning = 0;
            endedTurning = 0;
            Serial.println("Left turn complete, resuming normal follow.");
        } else {
            drive(0, STEADY_SPEED);
            Serial.println("Left turn in progress...");
            startedTurning++;
        }
        return;
    }

    if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) {
        leftTurnInProgress = true;
        endedTurning = 0;
        drive(0, FULLSPEED);
        Serial.println("Initiating left turn.");
        return;
    }

    if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) { 
        drive(STEADY_SPEED, FULLSPEED); 
        _lastDirection = 0; 
        Serial.println("Going straight");
        return;
    }

    if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) { 
        drive(FULLSPEED, SLOW_SPEED); 
        _lastDirection = -1; 
        Serial.println("Slight right turn");
        return;
    }
    if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) { 
        drive(FULLSPEED, SLOWER_SPEED); 
        _lastDirection = -1; 
        Serial.println("Moderate right turn");
        return;
    }
    if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) { 
        drive(SLOW_SPEED, FULLSPEED); 
        _lastDirection = 1; 
        Serial.println("Slight left turn");
        return;
    }
    if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) 
    { 
        drive(SLOWER_SPEED, FULLSPEED); 
        _lastDirection = 1; 
        Serial.println("Moderate left turn");
        return;
    }
    
    if (_lastDirection == 1) 
    { 
        drive(SEARCH_SPEED, FULLSPEED); 
        Serial.println("Searching left...");
        return;
    } 
    else if (_lastDirection == -1) 
    { 
        drive(FULLSPEED, SEARCH_SPEED); 
        Serial.println("Searching right...");
        return;
    } else if(sensorReadings[3] <= deadzonehigh && sensorReadings[4] <= deadzonehigh)
    { 
        axisUntilLineFound(sensorReadings, deadzonehigh);
        Serial.println("Stopping...");
        return;
    }
    else 
    {
      stop();
    }
}


