// Motor and button pins
const int motor_L1 = 10, motor_L2 = 11, motor_R1 = 5, motor_R2 = 6;
const int buttonOn = 2, buttonOff = 4, trig = 3, echo = 9;

// Maze settings (7x3 grid)
const int mazeWidth = 7, mazeHeight = 3;
int maze[mazeWidth][mazeHeight] = {0}; // 0 = unexplored, 1 = path, 2 = obstacle

// Robot position and direction
int posX = 0, posY = 0;
int direction = 0; // 0 = North, 1 = East, 2 = South, 3 = West

// Movement history
char path[50]; // Stores the correct route
int pathIndex = 0;
bool mappingComplete = false;

void setup() {
  pinMode(motor_L1, OUTPUT); pinMode(motor_L2, OUTPUT);
  pinMode(motor_R1, OUTPUT); pinMode(motor_R2, OUTPUT);
  pinMode(buttonOn, INPUT_PULLUP);
  pinMode(buttonOff, INPUT_PULLUP);
  pinMode(trig, OUTPUT); pinMode(echo, INPUT);

  Serial.begin(9600);
}

// Move functions
void drive(int left, int right) {
  analogWrite(motor_L1, left > 0 ? 0 : -left);
  analogWrite(motor_L2, left > 0 ? left : 0);
  analogWrite(motor_R1, right > 0 ? 0 : -right);
  analogWrite(motor_R2, right > 0 ? right : 0);
}

void stop() {
  drive(0, 0);
}

// Ultrasonic distance measurement
unsigned long duration;
int distance;

bool isObstacleDetected() {
  measureDistance();
  return distance < 20; // Adjust the threshold as needed
}

void measureDistance() {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration * 0.0343) / 2;
}

// Turns the robot 90 degrees to the right
void turnRight() {
  drive(-203, 190);
  delay(800);
  stop();
  direction = (direction + 1) % 4;
}

// Turns the robot 180 degrees (backtracking)
void turnAround() {
  drive(-203, -190);
  delay(1600);
  stop();
  direction = (direction + 2) % 4;
}

// Updates position based on direction
void updatePosition() {
  switch (direction) {
    case 0: posY--; break; // North
    case 1: posX++; break; // East
    case 2: posY++; break; // South
    case 3: posX--; break; // West
  }
}

// Prints the maze in Serial Monitor
void printMaze() {
  Serial.println("\nMaze Map:");
  for (int y = 0; y < mazeHeight; y++) {
    for (int x = 0; x < mazeWidth; x++) {
      if (x == posX && y == posY) Serial.print('P'); // Mark current position
      else if (maze[x][y] == 1) Serial.print('*');
      else if (maze[x][y] == 2) Serial.print('#');
      else Serial.print('.');
      Serial.print(" ");
    }
    Serial.println();
  }
}

void moveForward() {
  drive(190, 203);
  delay(800); // Move one full step
  stop();
}

// Follows the recorded path
void followPath() {
  Serial.println("Following stored path...");
  for (int i = 0; i < pathIndex; i++) {
    if (path[i] == 'F') {
      moveForward();
    } else if (path[i] == 'R') {
      turnRight();
    } else if (path[i] == 'B') {
      turnAround();
    }
  }
  stop();
  Serial.println("Path completed!");
}

void driveForward() { // Start moving
  moveForward(); 
}

void loop() {
  if (digitalRead(buttonOn) == LOW) {
    delay(200);

    if (mappingComplete) {
      followPath(); // If maze is mapped, follow the stored route
    } else {
      Serial.println("Starting Maze Mapping...");
      driveForward(); // Start moving

      while (true) {
        if (isObstacleDetected()) {
          stop();
          delay(200);

          // Mark obstacle
          maze[posX][posY] = 2;

          // Try turning right
          turnRight();
          path[pathIndex++] = 'R';

          updatePosition();

          // If out of bounds, turn around
          if (posX < 0 || posX >= mazeWidth || posY < 0 || posY >= mazeHeight) {
            turnAround();
            path[pathIndex++] = 'B';
            updatePosition();
          }
        } else {
          moveForward();
        }

        printMaze();

        // If robot reaches the end, return to start
        if (posX == mazeWidth - 1 && posY == mazeHeight - 1) {
          Serial.println("Mapping complete! Returning to start...");

          // Reverse path
          for (int i = pathIndex - 1; i >= 0; i--) {
            if (path[i] == 'F') {
              driveForward();
            } else if (path[i] == 'R') {
              turnAround();
            }
          }
          stop();
          Serial.println("Back at start. Press button to follow path.");

          mappingComplete = true;
          break;
        }

        if (digitalRead(buttonOff) == LOW) {
          stop();
          break;
        }
      }
    }
  }
}