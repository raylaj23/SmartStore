#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "HX711.h"
#include <mbedtls/base64.h>
#include <mbedtls/md.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "AzureIotHubClient.h"

#define DT1 23
#define SCK1 22
#define GR1 500
#define NAME1 "Dorna"

#define DT2 21
#define SCK2 19
#define GR2 500
#define NAME2 "Coca Cola"

#define DT3 18
#define SCK3 5
#define GR3 1000
#define NAME3 "Doritos"

#define DT4 17
#define SCK4 16
#define GR4 1500
#define NAME4 "Kinder"

#define SCALE 0.42

HX711 cantar1;
HX711 cantar2;
HX711 cantar3;
HX711 cantar4;

int lastNr1 = -1, lastNr2 = -1, lastNr3 = -1, lastNr4 = -1;
int nr1 = -1, nr2 = -1, nr3 = -1, nr4 = -1;

void inventoryTask(void *pvParameters) {
  while (true) {
    //get the weights
    float w1 = cantar1.get_units();
    float w2 = cantar2.get_units();
    float w3 = cantar3.get_units();
    float w4 = cantar4.get_units();

    //get the count
    nr1 = round(w1 / GR1);
    nr2 = round(w2 / GR2);
    nr3 = round(w3 / GR3);
    nr4 = round(w4 / GR4);

    //changes
    if (nr1 != lastNr1) {
      Serial.print(NAME1); Serial.print(": "); Serial.print(nr1); Serial.println(" in stock");
      //lastNr1 = nr1;
    }

    if (nr2 != lastNr2) {
      Serial.print(NAME2); Serial.print(": "); Serial.print(nr2); Serial.println(" in stock");
      //lastNr2 = nr2;
    }

    if (nr3 != lastNr3) {
      Serial.print(NAME3); Serial.print(": "); Serial.print(nr3); Serial.println(" in stock");
      //lastNr3 = nr3;
    }

    if (nr4 != lastNr4) {
      Serial.print(NAME4); Serial.print(": "); Serial.print(nr4); Serial.println(" in stock");
      //lastNr4 = nr4;
    }

    //delay for stabilization
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void device_to_cloud(void *pvParameters) {
  while(true) {
    if(nr1 != lastNr1 || nr2 != lastNr2 || nr3 != lastNr3 || nr4 != lastNr4){
      lastNr1 = nr1;
      lastNr2 = nr2;
      lastNr3 = nr3;
      lastNr4 = nr4;
      String virtual_basket = String(NAME1) + " " + String(nr1) + "\\n" +
                              String(NAME2) + " " + String(nr2) + "\\n" +
                              String(NAME3) + " " + String(nr3) + "\\n" +
                              String(NAME4) + " " + String(nr4) + "\\n";
      sendTelemetry(virtual_basket);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);

  cantar1.begin(DT1, SCK1);
  cantar2.begin(DT2, SCK2);
  cantar3.begin(DT3, SCK3);
  cantar4.begin(DT4, SCK4);

  //calibration
  cantar1.set_scale(SCALE);
  cantar2.set_scale(SCALE);
  cantar3.set_scale(SCALE);
  cantar4.set_scale(SCALE);

  establishConnection();

  //send virtual basket task
  xTaskCreate(device_to_cloud, "Telemetry Task", 4096, NULL, 2, NULL);

  //get inventory task
  xTaskCreate(inventoryTask, "Inventory Task", 4096, NULL, 1, NULL);
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
