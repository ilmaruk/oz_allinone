#ifndef _OTA_H
#define _OTA_H

#include <ArduinoOTA.h>

#include "config.h"
#include "secrets.h"

void ota_init() {
  ArduinoOTA.setHostname(DEVICE_ID);

  // Optional: require a password for OTA uploads
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
    .onStart([]() {
      Serial.println("OTA update started");
    })
    .onEnd([]() {
      Serial.println("\nOTA update finished");
    })
    .onProgress([](unsigned int progress,
                    unsigned int total) {
      Serial.printf("Progress: %u%%\r",
                    (progress * 100) / total);
    })
    .onError([](ota_error_t error) {
      Serial.printf("OTA error: %u\n", error);
    });

  ArduinoOTA.begin();

  Serial.println("OTA ready");
}

void ota_handle() {
  ArduinoOTA.handle();
}

#endif // _OTA_H