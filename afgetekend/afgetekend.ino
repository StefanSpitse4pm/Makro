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

void setup() { 
  pinMode(MOTOR_L1, OUTPUT); 
  pinMode(MOTOR_L2, OUTPUT); 
  pinMode(MOTOR_R1, OUTPUT); 
  pinMode(MOTOR_R2, OUTPUT); 
  pinMode(ROTATION_1, INPUT);

  Serial.begin(9600); 
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
  followLine();
  endedTurning++;

  // rotation_test(); // Your rotation logic (if needed)
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

void followLine() {
    int sensorReadings[NUMSENSORS]; 
    int sum = 0; 

    for (int i = 0; i < NUMSENSORS; i++) { 
        sensorReadings[i] = analogRead(sensorPins[i]); 
        sum += sensorReadings[i]; 
    } 

    int avg = sum / NUMSENSORS; 
    int deadzonehigh = avg + 50;  

    // Handle ongoing left turns first
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

    // Handle ongoing right turns
    if (rightTurnInProgress) {
        if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh && startedTurning >= 25 && endedTurning > 25) {
            rightTurnInProgress = false;
            startedTurning = 0;
            endedTurning = 0;
            Serial.println("Right turn complete, resuming normal follow.");
        } else {
            drive(STEADY_SPEED, 0);
            Serial.println("Right turn in progress...");
            startedTurning++;
        }
        return;
    }

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
    // if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
    //     rightTurnInProgress = true;
    //     endedTurning = 0;
    //     drive(FULLSPEED, 0);
    //     Serial.println("Initiating right turn.");
    //     return;
    // }

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

    // **Lost line: Search left first, then right**
    if (_lastDirection == 1) { 
        drive(SEARCH_SPEED, FULLSPEED); 
        Serial.println("Searching left...");
        return;
    } else if (_lastDirection == -1) { 
        drive(FULLSPEED, SEARCH_SPEED); 
        Serial.println("Searching right...");
        return;
    } else { 
        axisUntilLineFound(sensorReadings, deadzonehigh);
        Serial.println("Stopping...");
        return;
    }
}



