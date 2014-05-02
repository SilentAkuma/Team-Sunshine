#include "Timer.h"
#include "Event.h"

//gyro
//motor drivers
//encoder
//receivers
//algorithm

// 24 pulses per rotation encoder digital input
// 1.3 rotations per block => 64 pulses per block
// receivers analog input

Timer t;

int previousTime = millis();
int currentTime = millis();

// MOTOR DEFINES
const int motor1Pin1 = 3; //1A
const int motor1Pin2 = 4; //2A
const int motor2Pin1 = 6; //4A
const int motor2Pin2 = 5; //3A
const int enablePin2 = 12; //3_4EN
const int enablePin1 = 11; //1_2EN

// RECEIVER DEFINES
/*
const int receiverPinL;
const int receiverPinML;
const int receiverPinMR;
const int receiverPinR;
*/

// ENCODER DEFINES
const int encLeftA = 14;
//const int encLeftB = 13;
//const int encRightA = 16;
//const int encRightB = 15;

void left_reverse() {
  // set enablePin high so that motor can turn on
  // set motor pins high so it just runs
  digitalWrite(enablePin1, HIGH);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
}

void left_forward() {
  // set enablePin high so that motor can turn on
  // set motor pins high so it just runs
  digitalWrite(enablePin1, HIGH);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
}

void right_reverse() {
  // set enablePin high so that motor can turn on
  // set motor pins high so it just runs
  digitalWrite(enablePin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void right_forward() {
  // set enablePin high so that motor can turn on
  // set motor pins high so it just runs
  digitalWrite(enablePin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void stop_wheels() {
  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, LOW);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void turn_left() {
  previousTime = millis();
  currentTime = millis();
  while((currentTime - previousTime) < 500) {
    right_forward();
    left_reverse();
  }
  stop_wheels();  
}

void wait(int milliseconds) {
  previousTime = millis();
  currentTime = millis();
  while((currentTime - previousTime) < milliseconds);
}

void setup() {

  // input pins
  
  // set all the other pins you're using as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  
}

void loop() {
  turn_left();
  wait(2000);
}
