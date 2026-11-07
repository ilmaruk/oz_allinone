#ifndef _BOARD_H
#define _BOARD_H

#include "config.h"
#include "timer.h"

#define BOARD_CLOCK 0
#define BOARD_TIMER 1
#define BOARD_GOAL  2

size_t goal_carousel_counter = 0;

size_t board_state() {
  if (timer_is_set()) {
    if (goal_carousel_counter > 0) {
      return BOARD_GOAL;
    } else {
      return BOARD_TIMER;
    }
  }
  return BOARD_CLOCK;
}

void board_activate_goal_carousel() {
  goal_carousel_counter = GOAL_CAROUSEL_ITERATIONS;
}

size_t board_goal_carousel_counter() {
  return goal_carousel_counter;
}

void board_update_goal_carousel_counter() {
  if (goal_carousel_counter > 0) {
    goal_carousel_counter--;
  }
}

#endif // _BOARD_H