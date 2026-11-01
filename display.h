#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <MD_Parola.h>

#include "config.h"
#include "clock_font.h"
#include "crests_font.h"
#include "timer.h"
#include "clock.h"
#include "board.h"

#define ZONES_COUNT 9

#define ZONE_CLOCK      0
#define ZONE_HOME_ID    1
#define ZONE_SCORE      2
#define ZONE_AWAY_ID    3
#define ZONE_DATE       4
#define ZONE_LEFT_MOST  5
#define ZONE_RIGHT_MOST 6
#define ZONE_GOAL_LEFT  7
#define ZONE_GOAL_RIGHT 8

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

char szMesgClock[10] = "BOOT...";
char szMesgDate[11] = "BOOT...";
char szMesgScore[3][8] = {"-", "0-0", "-"};
char* szMesgGoal = "GOL!";
char szMesgGoalId[2];

void display_set_intensity(size_t value);
void display_set_intensity();

void display_init() {
  display.begin(ZONES_COUNT);

  display.setZone(ZONE_CLOCK, 0, 3);
  display.setZone(ZONE_HOME_ID, 7, 7);
  display.setZone(ZONE_SCORE, 5, 6);
  display.setZone(ZONE_AWAY_ID, 4, 4);
  display.setZone(ZONE_DATE, 4, 7);
  display.setZone(ZONE_LEFT_MOST, 7, 7);
  display.setZone(ZONE_RIGHT_MOST, 0, 0);
  display.setZone(ZONE_GOAL_LEFT, 4, 6);
  display.setZone(ZONE_GOAL_RIGHT, 1, 3);

  display.setFont(ZONE_CLOCK, oz_score);
  display.setFont(ZONE_HOME_ID, font_crests);
  display.setFont(ZONE_SCORE, oz_score);
  display.setFont(ZONE_AWAY_ID, font_crests);
  display.setFont(ZONE_DATE, oz_score);
  display.setFont(ZONE_LEFT_MOST, font_crests);
  display.setFont(ZONE_RIGHT_MOST, font_crests);
  display.setFont(ZONE_GOAL_LEFT, oz_score_h6);
  display.setFont(ZONE_GOAL_RIGHT, oz_score_h6);

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

void display_clock();
void display_scoreboard();
void display_goal_carousel();

void display_update() {
  display_set_intensity();

  if (!display.displayAnimate()) {
    return;
  }

  size_t state = board_state();
  switch (state) {
    case BOARD_CLOCK:
      display_clock();
      break;
    case BOARD_TIMER:
      display_scoreboard();
      break;
    case BOARD_GOAL:
      display_goal_carousel();
      break;
  }
}

void display_clock() {
  display.setFont(ZONE_DATE, font_w3);
  display.setInvert(ZONE_DATE, 0);
  display.setFont(ZONE_CLOCK, oz_score);
  display.setInvert(ZONE_CLOCK, 0);

  clock_format(szMesgClock, szMesgDate);

  // Date
  display.displayZoneText(ZONE_DATE, szMesgDate, PA_CENTER, 0, 0, PA_NO_EFFECT);

  // Clock
  display.displayZoneText(ZONE_CLOCK, szMesgClock, PA_CENTER, 0, 0, PA_NO_EFFECT);
}

void display_scoreboard() {
  // Score
  display.setFont(ZONE_DATE, font_w3);
  display.setInvert(ZONE_DATE, 0);
  display.setFont(ZONE_CLOCK, oz_score);
  display.setInvert(ZONE_CLOCK, 0);

  if (timer_left() <= 0) {
    display.setFont(ZONE_CLOCK, oz_score_h6);
    display.setInvert(ZONE_CLOCK, 1);
  }
  timer_format(szMesgClock);

  // Timer
  display.displayZoneText(ZONE_CLOCK, szMesgClock, PA_CENTER, 0, 0, PA_NO_EFFECT);

  // Score
  display.displayZoneText(ZONE_HOME_ID, szMesgScore[0], PA_LEFT, 0, 0, PA_NO_EFFECT);
  display.displayZoneText(ZONE_SCORE, szMesgScore[1], PA_CENTER, 0, 0, PA_NO_EFFECT);
  display.displayZoneText(ZONE_AWAY_ID, szMesgScore[2], PA_RIGHT, 0, 0, PA_NO_EFFECT);
}

void display_goal_carousel() {
  display.displayZoneText(ZONE_LEFT_MOST, szMesgGoalId, PA_LEFT, 0, 0, PA_NO_EFFECT);
  display.displayZoneText(ZONE_RIGHT_MOST, szMesgGoalId, PA_RIGHT, 0, 0, PA_NO_EFFECT);

  if (!display.getZoneStatus(ZONE_GOAL_LEFT) || !display.getZoneStatus(ZONE_GOAL_RIGHT)) {
    return;
  }

  size_t i = board_goal_carousel_counter() % 2;
  size_t not_i = (i + 1) % 2;
  display.setInvert(ZONE_GOAL_LEFT, i);
  display.displayZoneText(ZONE_GOAL_LEFT, szMesgGoal, PA_CENTER, 0, GOAL_CAROUSEL_LENGTH, PA_NO_EFFECT);
  display.setInvert(ZONE_GOAL_RIGHT, not_i);
  display.displayZoneText(ZONE_GOAL_RIGHT, szMesgGoal, PA_CENTER, 0, GOAL_CAROUSEL_LENGTH, PA_NO_EFFECT);

  board_update_goal_carousel_counter();
}

#endif // _DISPLAY_H