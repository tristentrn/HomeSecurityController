#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>

void initDisplay();

void showNormalMode(String dateTime);
void showSecurityMode(String dateTime);

void showTemperature(float temperatureF, float humidity);

void showLightAlert(String dateTime);
void showMotionAlert(String dateTime);

#endif