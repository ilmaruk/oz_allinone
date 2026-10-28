#include "config.h"
#include "secrets.h"
#include "timer.h"
#include "ota.h"

#include <MD_Parola.h>

// #include <Arduino.h>
#include <PubSubClient.h>

#define ZONE_CLOCK   0
#define ZONE_HOME_ID 1
#define ZONE_SCORE   2
#define ZONE_AWAY_ID 3
#define ZONE_DATE    4

char szMesgClock[10] = "BOOT...";
char szMesgDate[11] = "BOOT...";

bool info_has_changed = false;
char szMesgScore[3][8] = {"-", "0-0", "-"};

#include "oz_wifi.h"
#include "mqtt.h"
#include "clock.h"
#include "clock_font.h"

WiFiClient espClient;
PubSubClient client(espClient);

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void setup()
{
  // Serial setup
  Serial.begin(115200);
  Serial.println("Welcome in oz-allinone!");

  sntp_set_time_sync_notification_cb(time_available);
  
  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  // sntp_servermode_dhcp(1);    // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTzTime(time_zone, ntpServer1);

  // WiFi setup
  WiFi.mode(WIFI_STA);
  connect_to_wifis(wifi_networks, ARRAY_SIZE(wifi_networks));

  // MQTT setup
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(handle_mqtt_message);

  // Display setop
  display.begin(5);

  display.setZone(ZONE_CLOCK, 0, 3);
  display.setZone(ZONE_HOME_ID, 7, 7);
  display.setZone(ZONE_SCORE, 5, 6);
  display.setZone(ZONE_AWAY_ID, 4, 4);
  display.setZone(ZONE_DATE, 4, 7);

  display.setFont(ZONE_CLOCK, oz_score);
  display.setFont(ZONE_HOME_ID, font_w3);
  display.setFont(ZONE_SCORE, oz_score);
  display.setFont(ZONE_AWAY_ID, font_w3);
  display.setFont(ZONE_DATE, font_w3);

  display.setIntensity(ZONE_CLOCK, 0);
  display.setIntensity(ZONE_HOME_ID, 0);
  display.setIntensity(ZONE_SCORE, 0);
  display.setIntensity(ZONE_AWAY_ID, 0);
  display.setIntensity(ZONE_DATE, 0);

  display.displayZoneText(ZONE_CLOCK,  szMesgClock, PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);
  display.displayZoneText(ZONE_DATE,  szMesgDate, PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);

  ota_init();
}

void loop()
{
  connect_to_wifis(wifi_networks, ARRAY_SIZE(wifi_networks));

  ota_handle();

  if (!client.connected()) {
    reconnect_to_mqtt(&client);
  }
  client.loop();

  if (display.displayAnimate()) {
    display.setFont(ZONE_CLOCK, oz_score);
    display.setInvert(ZONE_CLOCK, 0);

    if (timer_is_set()) {
      if (timer_left() <= 0) {
        display.setFont(ZONE_CLOCK, oz_score_h6);
        display.setInvert(ZONE_CLOCK, 1);
      }
      timer_format(szMesgClock);
    } else {
      clock_format(szMesgClock, szMesgDate);
      // The date zone is only displayed when the times is not set
      display.displayZoneText(ZONE_DATE, szMesgDate, PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);
    }

    // The clock zone is always displayed, with the relevant informtion
    display.displayZoneText(ZONE_CLOCK, szMesgClock, PA_CENTER, 0, 0, PA_NO_EFFECT, PA_NO_EFFECT);

    if (timer_is_set() || info_has_changed) {
      display.displayZoneText(ZONE_HOME_ID, szMesgScore[0], PA_LEFT, 0, 0,
                              PA_NO_EFFECT, PA_NO_EFFECT);
      display.displayZoneText(ZONE_SCORE, szMesgScore[1], PA_CENTER, 0, 0,
                              PA_NO_EFFECT, PA_NO_EFFECT);
      display.displayZoneText(ZONE_AWAY_ID, szMesgScore[2], PA_RIGHT, 0, 0,
                              PA_NO_EFFECT, PA_NO_EFFECT);

      info_has_changed = false;
    }
  }

  // Temporarily disable auto-resetting of time, so that the final score remains displayed indefinitely
  // timer_reset();

  delay(57);
}
