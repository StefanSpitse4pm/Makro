// Motor pin definitions
#define MOTOR_L1 11
#define MOTOR_L2 10
#define MOTOR_R1 9
#define MOTOR_R2 6

#define NUMSENSORS 8 
#define ROTATION_1 5

// Speed definitions
#define FULLSPEED 255

#define STEADY_SPEED 154
#define SLOW_SPEED 150
#define SLOWER_SPEED 30
#define SLOWEST_SPEED 0
#define SEARCH_SPEED 50

#define TRIG_PIN 13
#define ECHO_PIN 12

#define DISTANCE_TURN_AROUND 15
// Sensor pins
const int sensorPins[NUMSENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7}; 

// Last direction for searching when line is lost (-1: right, 1: left)
int _lastDirection = 0;

// Global variable for left-turn state
// Global variable for turn states
bool leftTurnInProgress = false;
bool rightTurnInProgress = false;
int endedTurning = 0;
int startedTurning = 0;
bool started = false;
bool startFinished = false;
int counter = 0;
unsigned long sinceSetup = millis();
int startAmount = 0;



void setup() {
  pinMode(SERVO_PIN, OUTPUT);
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

// Drive function: positive value drives one direction, negative reverses.
void drive(int left, int right) { 
  analogWrite(MOTOR_L1, max(0, -left)); 
  analogWrite(MOTOR_L2, max(0, left)); 
  analogWrite(MOTOR_R1, max(0, -right)); 
  analogWrite(MOTOR_R2, max(0, right)); 
} 

// Stops the motors.
void stop() { 
  analogWrite(MOTOR_L1, 0); 
  analogWrite(MOTOR_L2, 0); 
  analogWrite(MOTOR_R1, 0); 
  analogWrite(MOTOR_R2, 0); 
} 

void loop() { 
  startFunction();
  followLine();
  endedTurning++;

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
      
      if (distanceCheck() < DISTANCE_TURN_AROUND) 
      {
        startAmount++;
      }
      else {
        startAmount = 0;
      }
      Serial.println(startAmount);

      if (startAmount == 10) 
      {
        started = true;
        return;
      }

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

    }
    else 
    {
      drive(STEADY_SPEED, STEADY_SPEED);
    }
    Serial.println(counter);

}

float distanceCheck() {
      // calculate the distance, turn it on for 2 microseconds, then turn it off for 10 microseconds
      
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

    if (distanceCheck() < DISTANCE_TURN_AROUND && millis() - sinceSetup > 200) 
    {
      axis(STEADY_SPEED);
      axisUntilLineFound(sensorReadings, deadzonehigh);
      return;
    }


    // Handle ongoing left turns first
    if (leftTurnInProgress) {
        if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh && startedTurning >= 25 && endedTurning > 25) {
            leftTurnInProgress = false;
            startedTurning = 0;
            endedTurning = 0;
            Serial.println("Left turn complete, resuming normal follow.");
        } 
        else {
            drive(0, STEADY_SPEED);
            Serial.println("Left turn in progress...");
            startedTurning++;
        }
        return;
    }

    // Handle ongoing right turns
    // if (rightTurnInProgress) {
    //     if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh && startedTurning >= 25 && endedTurning > 25) {
    //         rightTurnInProgress = false;
    //         startedTurning = 0;
    //         endedTurning = 0;
    //         Serial.println("Right turn complete, resuming normal follow.");
    //     } else {
    //         drive(STEADY_SPEED, 0);
    //         Serial.println("Right turn in progress...");
    //         startedTurning++;
    //     }
    //     return;
    // }

    // **Priority 1: Detect left turn and initiate it**
    if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) {
        leftTurnInProgress = true;
        endedTurning = 0;
        drive(0, FULLSPEED);
        Serial.println("Initiating left turn.");
        return;
    }

    // **Priority 2: Move forward if possible**
    if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) { 
        drive(STEADY_SPEED, FULLSPEED); 
        _lastDirection = 0; 
        Serial.println("Going straight");
        return;
    }

    // **Priority 3: Detect right turn (only if left and forward are not possible)**
    if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
        rightTurnInProgress = true;
        endedTurning = 0;
        drive(FULLSPEED, 0);
        Serial.println("Initiating right turn.");
        return;
    }

    // **Minor Adjustments (Turning Corrections)**
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
    if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) { 
        drive(SLOWER_SPEED, FULLSPEED); 
        _lastDirection = 1; 
        Serial.println("Moderate left turn");
        return;
    }


    if (_lastDirection == -1  && sensorReadings[0] <= deadzonehigh) { 
          drive(FULLSPEED, SEARCH_SPEED);
           
          Serial.println("Searching right...");
          return;
      }  

    // **Lost line: Search left first, then right**
    if (sensorReadings[3] <= deadzonehigh && sensorReadings[4] <= deadzonehigh &&_lastDirection == 0 ){ 
        axisUntilLineFound(sensorReadings, deadzonehigh);
        Serial.println("Stopping...");
        return; 
    }


}


