#define NUM_SENSORS 8 // Adjust this based on your sensor array
#define WHITE_COLOR 600
#define LINKS_VOOR 10
#define RECHTS_VOOR 6
#define RECHTS_ACHTER 9
#define LINKS_ACHTER 11
#define FORWARD_SPEED 20
#define TURNING_SPEED 150



int _speed = FORWARD_SPEED;
int _turning_speed = TURNING_SPEED;

int _sensors[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
int _sensorValues[8];


void setup() {
  Serial.begin(9600);
} 
void loop() {
  for (int i = 0; i < NUM_SENSORS; i++) 
  {
    _sensorValues[i] = analogRead(_sensors[i]);
  }
}

void turningRight() 
{
  analogWrite(LINKS_ACHTER, _speed);
  analogWrite(RECHTS_VOOR, _turning_speed);
}

void turningLeft() 
{
  analogWrite(LINKS_ACHTER, _turning_speed);
  analogWrite(RECHTS_VOOR, _speed);
}

void forward() 
{
  analogWrite(LINKS_VOOR, _speed);
  analogWrite(LINKS_ACHTER, 0);
  analogWrite(RECHTS_VOOR, _speed);
  analogWrite(RECHTS_ACHTER, 0);
}
