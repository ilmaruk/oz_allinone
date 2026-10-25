#ifndef _CLOCK_H
#define _CLOCK_H

#include "time.h"
#include "sntp.h"

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

// Callback function (get's called when time adjusts via NTP)
void time_available(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
}

void clock_format(char *clock_buffer, char* date_buffer) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available, yet");
    return;
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  // strftime(buffer, 9, "%H:%M", &timeinfo);

  // Simulate blinking colon
  // In order to work, '|' must be empty and of the same length of ':'
  char sep = ':';
  if (timeinfo.tm_sec % 2 == 0) {
    sep = '|';
  }
  sprintf(clock_buffer, "%02d%c%02d", timeinfo.tm_hour, sep, timeinfo.tm_min);

  char buf[9];
  strftime(buf, sizeof(buf), "%d-%m-%y", &timeinfo);
  strcpy(date_buffer, buf);
}


#endif //  _CLOCK_H