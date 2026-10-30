#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <MD_Parola.h>

#include "config.h"
#include "clock_font.h"
#include "crests_font.h"
#include "timer.h"
#include "clock.h"

#define ZONES_COUNT 5

#define ZONE_CLOCK   0
#define ZONE_HOME_ID 1
#define ZONE_SCORE   2
#define ZONE_AWAY_ID 3
#define ZONE_DATE    4

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char szMesgClock[10] = "BOOT...";
char szMesgDate[11] = "BOOT...";
char szMesgScore[3][8] = {"-", "0-0", "-"};

void display_set_intensity(size_t value);
void display_set_intensity();

void display_init() {
  display.begin(ZONES_COUNT);

  display.setZone(ZONE_CLOCK, 0, 3);
  display.setZone(ZONE_HOME_ID, 7, 7);
  display.setZone(ZONE_SCORE, 5, 6);
  display.setZone(ZONE_AWAY_ID, 4, 4);
  display.setZone(ZONE_DATE, 4, 7);

  display.setFont(ZONE_CLOCK, oz_score);
  display.setFont(ZONE_HOME_ID, font_crests);
  display.setFont(ZONE_SCORE, oz_score);
  display.setFont(ZONE_AWAY_ID, font_crests);
  display.setFont(ZONE_DATE, font_w3);

  display_set_intensity();
}

size_t display_intensity = 0;

void display_set_intensity(size_t value) {
  display_intensity = value;
}

void display_set_intensity() {
  for (size_t z = 0; z < ZONES_COUNT; z++) {
    display.setIntensity(z, display_intensity);
  }
}

void display_update(bool* info_has_changed) {
  display_set_intensity();

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

      *info_has_changed = false;
    }
  }
}

#endif // _DISPLAY_H