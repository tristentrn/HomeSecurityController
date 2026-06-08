#include "LcdDisplay.h"
#include "DhtSensor.h"
#include "LightSensor.h"
#include "UltrasonicSensor.h"
#include "Modes.h"
#include <WiFi.h>
#include "time.h"
#include "EmailNotif.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
//for hotspot setup
const char* ssid = "Tristen";
const char* password = "4082256624";

//firebase startup
//#define API_KEY "AIzaSyDfWpfhd8saLRFPt45TunqR1wL41otV7cY"
#define FIREBASE_HOST "eecs113-final-default-rtdb.firebaseio.com/"

void initClockTime() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");

  configTime(-8 * 3600, 3600, "pool.ntp.org", "time.nist.gov");

  Serial.println("Clock time initialized.");
}

bool updateFirebaseStatus(String mode, float temperatureF, float humidity,
                          String lightStatus, bool objectDetected,
                          float distanceCm, String dateTime) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Firebase update skipped: WiFi not connected.");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = String("https://") + FIREBASE_HOST + "/smartHome/currentStatus.json";

  if (!https.begin(client, url)) {
    Serial.println("Firebase currentStatus begin() failed.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> doc;
  doc["mode"] = mode;
  doc["temperatureF"] = temperatureF;
  doc["humidity"] = humidity;
  doc["lightStatus"] = lightStatus;
  doc["objectDetected"] = objectDetected;
  doc["distanceCm"] = distanceCm;
  doc["dateTime"] = dateTime;

  String body;
  serializeJson(doc, body);

  int code = https.PUT(body);
  String response = https.getString();
  https.end();

  Serial.print("Firebase currentStatus PUT HTTP ");
  Serial.println(code);

  if (code < 200 || code >= 300) {
    Serial.println(response);
    return false;
  }

  return true;
}


bool logFirebaseEvent(String mode, String eventType, float temperatureF,
                      float humidity, String lightStatus,
                      bool objectDetected, float distanceCm,
                      String dateTime) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Firebase log skipped: WiFi not connected.");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = String("https://") + FIREBASE_HOST + "/smartHome/eventLogs.json";

  if (!https.begin(client, url)) {
    Serial.println("Firebase eventLogs begin() failed.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> doc;
  doc["mode"] = mode;
  doc["eventType"] = eventType;
  doc["temperatureF"] = temperatureF;
  doc["humidity"] = humidity;
  doc["lightStatus"] = lightStatus;
  doc["objectDetected"] = objectDetected;
  doc["distanceCm"] = distanceCm;
  doc["dateTime"] = dateTime;

  String body;
  serializeJson(doc, body);

  int code = https.POST(body);
  String response = https.getString();
  https.end();

  Serial.print("Firebase eventLogs POST HTTP ");
  Serial.println(code);

  if (code < 200 || code >= 300) {
    Serial.println(response);
    return false;
  }

  return true;
}

String getDateTimeString() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "Time unavailable";
  }

  char buffer[17];
  strftime(buffer, sizeof(buffer), "%m/%d %I:%M%p", &timeinfo);

  return String(buffer);
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("================================");
  Serial.println("Smart Home System Starting...");
  Serial.println("================================");

  initDisplay();
  initDhtSensor();
  initLightSensor();
  initUltrasonicSensor();
  initClockTime();
  initModes();
  initEmail();

  Serial.println("Setup complete.");
  Serial.println("================================");
}

void loop() {
  checkModeButton();
  runCurrentMode();
}