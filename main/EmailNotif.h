#ifndef EMAIL_NOTIFICATION_H
#define EMAIL_NOTIFICATION_H

#include <Arduino.h>

void initEmail();
void sendAlertEmail(String eventType, String dateTime, float distanceCm, String lightStatus);

#endif