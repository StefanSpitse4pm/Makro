#include <Arduino.h>

const int motor_L1 = 11;
const int motor_L2 = 6;
const int motor_R1 = 10;
const int motor_R2 = 5;

const int numSensors = 8;
const int sensorPins[numSensors] = {A7, A6, A5, A4, A3, A2, A1, A0};

#define SERVO 12
#define GRIPPER_OPEN 1750
#define GRIPPER_CLOSE 1090

int lastDirection = 0;

void setup() {
    pinMode(motor_L1, OUTPUT);
    pinMode(motor_L2, OUTPUT);
    pinMode(motor_R1, OUTPUT);
    pinMode(motor_R2, OUTPUT);
    pinMode(SERVO, OUTPUT);
    digitalWrite(SERVO, LOW);
    Serial.begin(9600);
    stop();
    calibrate();
}

void drive(int left, int right) {
    analogWrite(motor_L1, max(0, -left));
    analogWrite(motor_L2, max(0, left));
    analogWrite(motor_R1, max(0, -right));
    analogWrite(motor_R2, max(0, right));
}

void stop() {
    drive(0, 0);
}

void calibrate() {
    delay(1000);
    drive(100, 100);
    delay(500);
    stop();
    delay(500);
}

void moveServo(int pulse) {
    static unsigned long timer;
    static int lastPulse;
    if (millis() > timer) {
        if (pulse > 0) {
            lastPulse = pulse;
        } else {
            pulse = lastPulse;
        }
        digitalWrite(SERVO, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(SERVO, LOW);
        timer = millis() + 20;
    }
}

void pickUpPion() {
    moveServo(GRIPPER_OPEN);
    delay(1000);
    drive(100, 100);
    delay(400);
    stop();
    delay(500);
    moveServo(GRIPPER_CLOSE);
    delay(1000);
    drive(-100, -100);
    delay(400);
    stop();
    delay(500);
}

void turnLeft() {
    drive(-100, 100);
    delay(800);
    stop();
}

void followLine() {
    while (true) {
        int sensorReadings[numSensors];
        int sum = 0;
        for (int i = 0; i < numSensors; i++) {
            sensorReadings[i] = analogRead(sensorPins[i]);
            sum += sensorReadings[i];
        }
        int avg = sum / numSensors;
        int deadzonelow = avg - 50;
        int deadzonehigh = avg + 50;

        if (sensorReadings[4] >= deadzonehigh && sensorReadings[5] >= deadzonehigh) {
            drive(214, 255);
            lastDirection = 0;
        } else if (sensorReadings[5] >= deadzonehigh && sensorReadings[6] >= deadzonehigh) {
            drive(255, 140);
            lastDirection = -1;
        } else if (sensorReadings[6] >= deadzonehigh && sensorReadings[7] >= deadzonehigh) {
            drive(255, 30);
            lastDirection = -1;
        } else if (sensorReadings[3] >= deadzonehigh && sensorReadings[4] >= deadzonehigh) {
            drive(140, 255);
            lastDirection = 1;
        } else if (sensorReadings[2] >= deadzonehigh && sensorReadings[3] >= deadzonehigh) {
            drive(30, 255);
            lastDirection = 1;
        } else if (sensorReadings[1] >= deadzonehigh && sensorReadings[2] >= deadzonehigh) {
            drive(0, 255);
            lastDirection = 1;
        } else if (sensorReadings[6] >= deadzonehigh) {
            drive(255, 0);
            lastDirection = -1;
        } else {
            if (lastDirection == -1) {
                drive(255, 30);
            } else if (lastDirection == 1) {
                drive(30, 255);
            } else {
                stop();
            }
        }
        delay(50);
    }
}

void loop() {
    pickUpPion();
    turnLeft();
    followLine();
}
