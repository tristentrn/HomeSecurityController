#include "EmailNotif.h"
#include <Arduino.h>
#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "tristentran@gmail.com"
#define AUTHOR_PASSWORD "dimv xtnc bdjy qkzn"

#define RECIPIENT_EMAIL "tristentran@gmail.com"

SMTPSession smtp;

void initEmail() {
  Serial.println("Email system ready.");
}

void sendAlertEmail(String eventType, String dateTime, float distanceCm, String lightStatus) {
  Session_Config config;

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "Smart Home Controller";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Security Alert";
  message.addRecipient("User", RECIPIENT_EMAIL);

  String body = "Security Alert!\n\n";
  body += "Mode: Security Mode\n\n";
  body += "Event: " + eventType + "\n\n";

  if (distanceCm >= 2) {
    body += "Ultrasonic distance: ";
    body += String(distanceCm, 1);
    body += " cm\n\n";
  }

  body += "Light sensor status: " + lightStatus + "\n\n";
  body += "Time: " + dateTime + "\n";
  message.text.content = body.c_str();

  if (!smtp.connect(&config)) {
    Serial.println("Email connection failed.");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.print("Email failed: ");
    Serial.println(smtp.errorReason());
  } else {
    Serial.println("Email sent successfully.");
  }

  smtp.closeSession();
}