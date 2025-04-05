#define MOTOR_L1 11
#define MOTOR_L2 10
#define MOTOR_R1 9
#define MOTOR_R2 6
#define NUMSENSORS 8 

#define ROTATION_1 5

#define FULLSPEED 255
#define STEADY_SPEED 214
#define SLOW_SPEED 180
#define SLOWER_SPEED 60
#define SLOWEST_SPEED 10
#define SEARCH_SPEED 50

const int sensorPins[NUMSENSORS] = {A0, A1, A2, A3, A4, A5, A6, A7}; 
int _lastDirection = 0; 
int total = 0;
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
  followLine();
  rotation_test();
}

void rotation_test() 
{
  total += digitalRead(ROTATION_1);
  Serial.println(total);
  if (total > 20)
  {
    total = 0;
  }
}

void followLine() 
{
  int sensorReadings[NUMSENSORS]; 
  int sum = 0; 

  for (int i = 0; i < NUMSENSORS; i++) { 
      sensorReadings[i] = analogRead(sensorPins[i]); 
      sum += sensorReadings[i]; 

  } 

  int avg = sum / NUMSENSORS; 
  int deadzonelow = avg - 50; 
  int deadzonehigh = avg + 50; 
  
  if (leftTurnInProgress) {
    if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
      // Line found again, resume normal operation.
      leftTurnInProgress = false;
      Serial.println("Line found again, resuming normal follow.");
    } else {
      // Continue turning left until the line is found.
      drive(SEARCH_SPEED, FULLSPEED);  // Pivot left by stopping the left motor.
      Serial.println("Left turn in progress...");
      return; // Skip further checks while in the left-turn state.
    }
  }

  if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) { 
      drive(STEADY_SPEED, FULLSPEED); 
      _lastDirection = 0; 
  } 

  else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) { 
      drive(FULLSPEED, SLOWEST_SPEED); 
      _lastDirection = -1; 

  }  

  else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) { 
      drive(FULLSPEED, SLOWER_SPEED); 
      _lastDirection = -1; 

  }   

  else if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) { 
      drive(FULLSPEED, SLOW_SPEED); 
      _lastDirection = 1; 

  }   

  else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) { 
      drive(SLOW_SPEED, FULLSPEED); 
      _lastDirection = 1; 

  }   

  else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) { 
      drive(SLOWER_SPEED, FULLSPEED); 
      _lastDirection = 1; 

  }  
    else if (sensorReadings[0] >= deadzonehigh && sensorReadings[1] >= deadzonehigh) { 
      drive(SLOWEST_SPEED, FULLSPEED); 
      _lastDirection = 1;
      leftTurnInProgress = true; 

  }    

  // lastdirection word 1 of -1  

  else { 
      if (_lastDirection == -1) { 
          drive(FULLSPEED, SEARCH_SPEED); 
      }  

      else if (_lastDirection == 1) { 
          drive(SEARCH_SPEED, FULLSPEED); 
      }  

      else { 
          stop(); 
      } 

  } 
}