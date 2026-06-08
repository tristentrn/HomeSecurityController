#include "Modes.h"
#include "LcdDisplay.h"
#include "DhtSensor.h"
#include "LightSensor.h"
#include "UltrasonicSensor.h"
#include "EmailNotif.h"
#include <Arduino.h>

// Internal function prototypes
void setNormalMode();
void setSecurityMode();
void checkSecuritySensors();
void triggerAlert(bool lightChanged, bool objectDetected, float distance);
void ledAlert();
void setColor(int red, int green, int blue);
void updateNormalLcdLoop();
void updateSecurityLcdLoop();

extern String getDateTimeString();

extern void updateFirebaseStatus(String mode, float temperatureF, float humidity,
                                 String lightStatus, bool objectDetected,
                                 float distanceCm, String dateTime);

extern void logFirebaseEvent(String mode, String eventType, float temperatureF,
                             float humidity, String lightStatus,
                             bool objectDetected, float distanceCm,
                             String dateTime);

// RGB LED pins
const int PIN_RED   = 11;
const int PIN_GREEN = 10;
const int PIN_BLUE  = 9;

// Button pin
const int BUTTON_PIN = 2;

// false = Normal/Home Mode, true = Security Mode
bool securityMode = false;
bool alertActive = false;

// Save light state when security mode starts
bool startingLightState = false;

// Alert threshold
const float MOTION_DISTANCE_CM = 20.0;

// Button debounce
int lastButtonReading = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Sensor timing
unsigned long lastSensorCheck = 0;
const unsigned long sensorCheckDelay = 500;

// LCD timing
unsigned long lastScreenSwitch = 0;
const unsigned long screenSwitchDelay = 3000;
bool showModeScreen = true;

void initModes() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  setNormalMode();

  Serial.println("Modes initialized.");
}

void checkModeButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        Serial.println("Button press confirmed.");

        securityMode = !securityMode;
        alertActive = false;

        showModeScreen = true;
        lastScreenSwitch = millis();
        lastSensorCheck = millis();

        if (securityMode) {
          setSecurityMode();
        } else {
          setNormalMode();
        }
      }
    }
  }

  lastButtonReading = reading;
}

void runCurrentMode() {
  if (securityMode) {
    checkSecuritySensors();

    if (!alertActive) {
      updateSecurityLcdLoop();
    }
  } else {
    updateNormalLcdLoop();
  }
}

void updateNormalLcdLoop() {
  if (millis() - lastScreenSwitch >= screenSwitchDelay) {
    lastScreenSwitch = millis();
    showModeScreen = !showModeScreen;

    if (showModeScreen) {
      showNormalMode(getDateTimeString());
      Serial.println("LCD: Normal/Home mode screen");
    } else {
      float temperatureF = getTemperatureF();
      float humidity = getHumidity();

      if (isnan(temperatureF) || isnan(humidity)) {
        Serial.println("ERROR: DHT22 read failed.");
        return;
      }

      showTemperature(temperatureF, humidity);

      String lightStatus = isLightDetected() ? "LIGHT ON" : "LIGHT OFF";

      updateFirebaseStatus(
        "Home",
        temperatureF,
        humidity,
        lightStatus,
        false,
        -1,
        getDateTimeString()
      );

      Serial.println("LCD: Temperature/Humidity screen");
    }
  }
}

void updateSecurityLcdLoop() {
  if (millis() - lastScreenSwitch >= screenSwitchDelay) {
    lastScreenSwitch = millis();
    showModeScreen = !showModeScreen;

    if (showModeScreen) {
      showSecurityMode(getDateTimeString());
      Serial.println("LCD: Security mode screen");
    } else {
      float temperatureF = getTemperatureF();
      float humidity = getHumidity();

      if (isnan(temperatureF) || isnan(humidity)) {
        Serial.println("ERROR: DHT22 read failed.");
        return;
      }

      showTemperature(temperatureF, humidity);
      Serial.println("LCD: Temperature/Humidity screen");
    }
  }
}

void checkSecuritySensors() {
  if (millis() - lastSensorCheck >= sensorCheckDelay) {
    lastSensorCheck = millis();

    bool currentLightState = isLightDetected();
    bool lightChanged = (currentLightState != startingLightState);

    float distance = getDistanceCm();
    bool objectDetected = (distance >= 2 && distance < MOTION_DISTANCE_CM);

    String lightStatus = currentLightState ? "LIGHT ON" : "LIGHT OFF";

    updateFirebaseStatus(
      "Away",
      getTemperatureF(),
      getHumidity(),
      lightStatus,
      objectDetected,
      distance,
      getDateTimeString()
    );

    Serial.print(" | Start Light: ");
    Serial.print(startingLightState ? "LIGHT" : "DARK");

    Serial.print(" | Current Light: ");
    Serial.print(currentLightState ? "LIGHT" : "DARK");

    Serial.print(" | Light Changed: ");
    Serial.print(lightChanged ? "YES" : "NO");

    Serial.print(" | Object: ");
    Serial.print(objectDetected ? "DETECTED" : "NOT DETECTED");

    Serial.print(" | Distance: ");
    if (distance == -1) {
      Serial.println("INVALID");
    } else {
      Serial.print(distance);
      Serial.println(" cm");
    }

    if (lightChanged || objectDetected) {
      triggerAlert(lightChanged, objectDetected, distance);
    } else {
      //alertActive = false;
      setColor(255, 180, 0);
    }
  }
}

void triggerAlert(bool lightChanged, bool objectDetected, float distance) {
  if (!alertActive) {
    Serial.println("ALERT TRIGGERED!");

    String eventType = "";

    if (lightChanged && objectDetected) {
      eventType = "Light changed and motion detected";
    } else if (lightChanged) {
      eventType = "Light changed";
    } else if (objectDetected) {
      eventType = "Motion detected";
    }

    ledAlert();

    if (lightChanged) {
      showLightAlert(getDateTimeString());
      Serial.println("Reason: Light state changed.");
    } 
    else if (objectDetected) {
      showMotionAlert(getDateTimeString());
      Serial.println("Reason: Motion detected.");
    }

    alertActive = true;

    String lightStatus = isLightDetected() ? "LIGHT ON" : "LIGHT OFF";

    logFirebaseEvent(
      "Alert",
      eventType,
      getTemperatureF(),
      getHumidity(),
      lightStatus,
      objectDetected,
      distance,
      getDateTimeString()
    );

    if (objectDetected) {
      sendAlertEmail(eventType, getDateTimeString(), distance, lightStatus);
    } else {
      sendAlertEmail(eventType, getDateTimeString(), -1, lightStatus);
    }
  }
}

void setNormalMode() {
  setColor(0, 255, 0);
  showNormalMode(getDateTimeString());
  Serial.println("Mode: Home");
}

void setSecurityMode() {
  alertActive = false;

  setColor(255, 180, 0);
  showSecurityMode(getDateTimeString());

  startingLightState = isLightDetected();

  Serial.println("Mode: Security");
  Serial.print("Starting light state saved as: ");
  Serial.println(startingLightState ? "LIGHT" : "DARK");
}

void ledAlert() {
  setColor(255, 0, 0);
  Serial.println("LED: Alert Red");
}

void setColor(int red, int green, int blue) {
  analogWrite(PIN_RED, red);
  analogWrite(PIN_GREEN, green);
  analogWrite(PIN_BLUE, blue);
}