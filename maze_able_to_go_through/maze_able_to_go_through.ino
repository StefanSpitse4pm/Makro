// Motor pin definitions
#define MOTOR_L1 11
#define MOTOR_L2 10
#define MOTOR_R1 9
#define MOTOR_R2 6

#define NUMSENSORS 8 
#define ROTATION_1 5

// Speed definitions
#define FULLSPEED 255
#define STEADY_SPEED 214
#define SLOW_SPEED 180
#define SLOWER_SPEED 60
#define SLOWEST_SPEED 10
#define SEARCH_SPEED 50

// Sensor pins
const int sensorPins[NUMSENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7}; 

// Last direction for searching when line is lost (-1: right, 1: left)
int _lastDirection = 0;

// Global variable for left-turn state
bool leftTurnInProgress = false;

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

  // If we are already in a left-turn state,
  // check if the center sensors detect the line again.
  int whiteSensors = 0;
  for (int i = 0; i < NUMSENSORS; i++) 
  {
    if (sensorReadings[i] >= deadzonehigh) 
    {
      break;
    }
    whiteSensors++;

    if (whiteSensors == 8) 
    {
      axisUntilLineFound(sensorReadings, deadzonehigh);
    }

  }

  if (leftTurnInProgress) {
    if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
      // Line has been found again, resume normal operation.
      leftTurnInProgress = false;
      Serial.println("Line found again, resuming normal follow.");
    } 
    else {
      // Continue turning left until the line is found.
      drive(0, FULLSPEED);  // For a pivot turn, stop the left motor
      Serial.println("Left turn in progress...");
      return; // Skip the rest of the logic while in left-turn state
    }
  }
  
  // Normal left-turn detection: if left sensors are strong,
  // initiate left-turn state.
  if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) {
    leftTurnInProgress = true;
    drive(0, FULLSPEED); // Begin turning left immediately
    Serial.println("Initiating left turn.");
    return;
  }

  // --- Normal line-following logic ---
  if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) { 
      drive(STEADY_SPEED, FULLSPEED); 
      _lastDirection = 0; 
      Serial.println("Going straight");
  }
  else if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) { 
      drive(FULLSPEED, SLOW_SPEED); 
      _lastDirection = -1; 
      Serial.println("Slight right turn");
  }   
  else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) { 
      drive(FULLSPEED, SLOWER_SPEED); 
      _lastDirection = -1; 
      Serial.println("Moderate right turn");
  }  
  else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) { 
      drive(FULLSPEED, SLOWEST_SPEED); 
      _lastDirection = -1; 
      Serial.println("Sharp right turn");
  }  
  else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) { 
      drive(SLOW_SPEED, FULLSPEED); 
      _lastDirection = 1; 
      Serial.println("Slight left turn");
  }  
  else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) { 
      drive(SLOWER_SPEED, FULLSPEED); 
      _lastDirection = 1; 
      Serial.println("Moderate left turn");
  }  
  else if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) { 
      drive(SLOWEST_SPEED, FULLSPEED); 
      _lastDirection = 1; 
      Serial.println("Sharp left turn");
  }  
  else { 
      // If no clear line is detected, use the last direction to search.
      if (_lastDirection == -1) { 
          drive(FULLSPEED, SEARCH_SPEED); 
          Serial.println("Searching right...");
      }  
      else if (_lastDirection == 1) { 
          drive(SEARCH_SPEED, FULLSPEED); 
          Serial.println("Searching left...");
      }  
      else { 
          stop(); 
          Serial.println("Stopping...");
      } 
  } 
}
