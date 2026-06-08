#include "LightSensor.h"
#include <Arduino.h>

#define LIGHT_SENSOR_PIN D6

void initLightSensor() {
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  Serial.println("Light sensor initialized.");
}

bool isLightDetected() {
  return !digitalRead(LIGHT_SENSOR_PIN);
}
void printLightDebug() {
  bool lightDetected  = isLightDetected();

  Serial.print("Light sensor State: ");

  if (lightDetected) {
    Serial.println("LIGHT DETECTED");
  } else {
    Serial.println("DARK");
  }

  Serial.println("----------------------");
}