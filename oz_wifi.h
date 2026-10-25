#ifndef _MARU_WIFI_H
#define _MARU_WIFI_H

#include <WiFi.h>

typedef struct {
  const char* ssid;
  const char* pass;
  size_t connection_timeout;
} wifi_credentials;

bool connect_to_wifi(wifi_credentials creds) {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.print("Attempting to connect to WiFi network ");
  Serial.print(creds.ssid);
  Serial.print(" for ");
  Serial.print(creds.connection_timeout);
  Serial.println("ms .... ");
  WiFi.begin(creds.ssid, creds.pass);
  if (WiFi.waitForConnectResult(creds.connection_timeout) == WL_CONNECTED) {
      Serial.print("Connected with IP ");
      Serial.println(WiFi.localIP());
      return true;
  }
  delay(1000);
  return false;
}

void connect_to_wifis(wifi_credentials credentials[], size_t count) {
  if (count == 0) {
    // No network to connect to
    return;
  }

  while (WiFi.status() != WL_CONNECTED) {
    for (size_t i = 0; i < count; i++) {
      if (connect_to_wifi(credentials[i])) {
        // Move the credentials at the top
        if (i > 0 && count > 1) {
          wifi_credentials tmp = credentials[0];
          credentials[0] = credentials[i];
          credentials[i] = tmp;
        }
        return;
      }
    }
  }
}

#endif // _MARU_WIFI_H