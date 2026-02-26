#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <mbedtls/base64.h>
#include <mbedtls/md.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "AzureIotHubClient.h"

#define PRICE1 5
#define NAME1 "Dorna"

#define PRICE2 7
#define NAME2 "Coca Cola"

#define PRICE3 15
#define NAME3 "Doritos"

#define PRICE4 20
#define NAME4 "Kinder"

#define MOSIPIN 23
#define CSPIN 5
#define DCPIN 16
#define SCKPIN 18
#define MISOPIN 19
#define RSTPIN 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(CSPIN, DCPIN, RSTPIN);

#define buttonPin 17
int buttonState = 0;

extern char incoming_data[INCOMING_DATA_BUFFER_SIZE];

void get_virtual_basket(void *pvParameters) {
  char last_display[INCOMING_DATA_BUFFER_SIZE] = {0};

  while(true) {
    if(strcmp(last_display, incoming_data) != 0 && strlen(incoming_data) > 0) {
    //if(strlen(incoming_data) > 0) {
      strcpy(last_display, incoming_data);

      Serial.println("New basket received:");
      Serial.println(last_display);

      tft.fillScreen(ILI9341_WHITE);
      tft.setCursor(0, 0);
      tft.setTextColor(ILI9341_BLACK);
      tft.setTextSize(2);
      tft.println("Basket:");
      tft.println(last_display);
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void pay_desk(void *pvParameters) {
  while(true) {
    buttonState = digitalRead(buttonPin);

    if(buttonState == HIGH) {
        //payment succesfull -> open the door
        tft.fillScreen(ILI9341_BLACK);
    } else if(buttonState == LOW) {
        //wait to push button
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);

  tft.begin();

  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println("Basket: ");

  establishConnection();

  //get virtual basket
  xTaskCreate(get_virtual_basket, "VirtualBasket Task", 4096, NULL, 2, NULL);

  //pay and send to door
  xTaskCreate(pay_desk, "PayDesk Task", 4096, NULL, 1, NULL);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi();
  }
#ifndef IOT_CONFIG_USE_X509_CERT
  else if (sasToken.IsExpired())
  {
    Logger.Info("SAS token expired; reconnecting with a new one.");
    (void)esp_mqtt_client_destroy(mqtt_client);
    initializeMqttClient();
  }
#endif
  // else if (millis() > next_telemetry_send_time_ms)
  // {
  //   sendTelemetry();
  //   next_telemetry_send_time_ms = millis() + TELEMETRY_FREQUENCY_MILLISECS;
  // }
  delay(500);
}
