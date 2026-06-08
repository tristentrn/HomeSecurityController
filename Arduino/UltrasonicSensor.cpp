#include "UltrasonicSensor.h"
#include <Arduino.h>

#define TRIG_PIN D4
#define ECHO_PIN D5

void initUltrasonicSensor() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Ultrasonic sensor initialized.");
  Serial.println("TRIG = D4");
  Serial.println("ECHO = D5");
}

float getDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration_us = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration_us == 0) {
    Serial.println("Ultrasonic: No echo received");
    return -1;
  }

  float distance_cm = duration_us * 0.0343 / 2.0;

  if (distance_cm < 2 || distance_cm > 400) {
    Serial.println("Ultrasonic: Invalid reading");
    return -1;
  }

  return distance_cm;
}

void printUltrasonicDebug() {
  float distance_cm = getDistanceCm();

  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  Serial.println("----------------------");
}