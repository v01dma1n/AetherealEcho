#include "timer_millis.h"
#include <Arduino.h>

bool TimerMillis::isTrigger() {
  if (!running)
    return false;
  if (millis() - startMillis >= intervalMillis) {
    // reached end of the timer interval: trigger
    if (canRestart)
      timerStart();
    else
      running = false;
    return true;
  } else
    return false;
};

void TimerMillis::setInterval(unsigned long intervalMillis) {
  this->intervalMillis = intervalMillis;
};

void TimerMillis::setCanRestart(bool val) { canRestart = val; }

void TimerMillis::timerStart() {
  startMillis = millis();
  running = true;
};

void TimerMillis::timerStop() { running = false; };
