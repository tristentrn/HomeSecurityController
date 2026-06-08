#include "LcdDisplay.h"
#include <Arduino.h>
#include <DIYables_LCD_I2C.h>

DIYables_LCD_I2C lcd_i2c(0x27, 16, 2);

void initDisplay() {
  lcd_i2c.init();
  lcd_i2c.backlight();

  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Smart Home");
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Starting...");
  delay(1500);
}

void showNormalMode(String dateTime) {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Mode: Home");

  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print(dateTime);
}

void showSecurityMode() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Mode: Security");
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Monitoring...");
}

void showAlertMode() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Mode: Alert");
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Warning!");
}

void showLightAlert() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Alert!");
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Light on");
}

void showMotionAlert() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Alert!");
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Motion detected");
}
void showSecurityMode(String dateTime) {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Mode: Security");

  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print(dateTime);
}

void showLightAlert(String dateTime) {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Alert! Light on");

  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print(dateTime);
}

void showMotionAlert(String dateTime) {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Alert! Motion");

  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print(dateTime);
}

void showTemperature(float temperatureF, float humidity) {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Temp: ");
  lcd_i2c.print(temperatureF, 1);
  lcd_i2c.print(" F");

  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("Humidity: ");
  lcd_i2c.print(humidity, 0);
  lcd_i2c.print("%");
}