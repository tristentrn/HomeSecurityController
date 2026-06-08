#include "DhtSensor.h"
#include <Arduino.h>
#include <DHT.h>

#define DHT22_PIN D3
#define DHTTYPE DHT22

DHT dht22(DHT22_PIN, DHTTYPE);

void initDhtSensor() {
  dht22.begin();
  Serial.println("DHT22 sensor initialized.");
}

float getTemperatureF() {
  return dht22.readTemperature(true);  // true = Fahrenheit
}

float getHumidity() {
  return dht22.readHumidity();
}