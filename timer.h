#ifndef _TIMER_H
#define _TIMER_H

#define TIMER_STOPPED 0
#define TIMER_RUNNING 1
#define TIMER_PAUSED  2

size_t duration = 0;
size_t elapsed = 0;
size_t started_at = 0;
size_t reset_at = 0;

void timer_set(size_t d) {
  duration = d;
  elapsed = 0;
  started_at = 0;
  reset_at = 0;
}

bool timer_is_set() {
  return duration > 0;
}

void timer_reset() {
  if (reset_at > 0 && millis() >= reset_at) {
    timer_set(0);    
  }
}

size_t timer_status() {
  if (started_at > 0) {
    return TIMER_RUNNING;
  } else if (elapsed > 0) {
    return TIMER_PAUSED;
  }
  return TIMER_STOPPED;
}

void timer_start() {
  if (timer_status() == TIMER_RUNNING) {
    return;
  }

  started_at = millis();
}

void timer_stop() {
  if (timer_status() != TIMER_RUNNING) {
    return;
  }

  elapsed += millis() - started_at;
  started_at = 0;
}

signed int timer_left() {
  signed int left = duration - elapsed;
  if (started_at > 0) {
    left -= millis() - started_at;
  }
  if (left < 0) {
    if (reset_at == 0) {
      reset_at = millis() + 10000; // Wait 10 seconds before resetting the timer
    }
    return 0;
  }
  return left;
}

void timer_format(char* buffer) {
  signed int left = timer_left();
  size_t minutes = left / 60000;
  size_t seconds = (left / 1000) % 60;
  if (minutes > 0) {
    sprintf(buffer, "%02d:%02d", minutes, seconds);
  } else {
    size_t decimals = (left % 1000) / 10;
    sprintf(buffer, "%02d.%02d", seconds, decimals);
  }
}

#endif //  _TIMER_H