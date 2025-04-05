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