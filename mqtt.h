#ifndef _MQTT_H
#define _MQTT_H

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <timer.h>
#include <config.h>
#include <secrets.h>
#include <display.h>

void reconnect_to_mqtt(PubSubClient* client) {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client->connect(DEVICE_ID)) {
      Serial.println("connected");
      // Subscribe
      client->subscribe(MQTT_TOPIC, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
}

bool info_has_changed = false;

void handle_mqtt_message(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  JsonDocument doc;
  deserializeJson(doc, message);

  if (doc.containsKey("command")) {
    const char* cmd = doc["command"];
    Serial.println(cmd);
    Serial.print("Received command: ");
    Serial.println(cmd);

    if (strcmp(cmd, "SET") == 0) {
      size_t duration = doc["duration"];
      timer_set(duration * 1000);
    } else if (strcmp(cmd, "START") == 0) {
      timer_start();
    } else if (strcmp(cmd, "STOP") == 0) {
      timer_stop();
    } else if (strcmp(cmd, "RESUME") == 0) {
      timer_start();
    } else if (strcmp(cmd, "INTENSITY") == 0) {
      size_t intensity = doc["intensity"];
      display_set_intensity(intensity);
    } else {
      Serial.println("Command unknown");
    }
  } else {
    const char* home_id = doc["home_id"];
    const char* away_id = doc["away_id"];
    size_t home_score = doc["home_score"];
    size_t away_score = doc["away_score"];
    
    strncpy(szMesgScore[0], home_id, 2);
    snprintf(szMesgScore[1], sizeof(szMesgScore[1]), "%d-%d", home_score, away_score);
    strncpy(szMesgScore[2], away_id, 2);

    info_has_changed = true;
  }
}

#endif // _MQTT_H