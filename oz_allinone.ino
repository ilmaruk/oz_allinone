#include "config.h"
#include "secrets.h"
#include "timer.h"
#include "ota.h"
#include "display.h"

// #include <Arduino.h>

#include "oz_wifi.h"
#include "mqtt.h"
#include "clock.h"

WiFiClient espClient;
PubSubClient client(espClient);

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

  // Display initialisation
  display_init();

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

  display_update(&info_has_changed);

  // Temporarily disable auto-resetting of time, so that the final score remains displayed indefinitely
  // timer_reset();

  delay(57);
}
