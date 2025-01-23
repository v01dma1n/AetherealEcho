#include "touch_timer.h"

void TouchTimer::setup() {}

bool TouchTimer::loop() {

  if (timerMillis.isRunning() && timerMillis.isTrigger()) {
    timerMillis.timerStart();

    touch_value_t val = touchRead(pin);
    if (val < touchThreshold) {    // touch detected
      if (touchStartMillis == 0) { // if we are not measuring the touch lenght
        // start measuring the touch length
        touchStartMillis = millis();
        isLongTouch = false;
        isShortTouch = false;
      } else { // we are measuring the touch length
        unsigned long currMillis = millis();
        if (currMillis - touchStartMillis >= longTouchMillis) {
          isLongTouch = true;
          isShortTouch = false;
        }
      }
    } else { // touch not detected (anymore)
      if (touchStartMillis > 0) { // if we are already measuring the touch length
        // determine touch length
        unsigned long currMillis = millis();
        if (currMillis - touchStartMillis >= longTouchMillis) {
          isLongTouch = true;
          isShortTouch = false;
        } else if (currMillis - touchStartMillis >= shortTouchMillis) {
          isLongTouch = false;
          isShortTouch = true;
        } else { // touch too short
          isLongTouch = false;
          isShortTouch = false;
        }
        touchStartMillis = 0;
      } else { // are are not measuring the touch length
        isLongTouch = false;
        isShortTouch = false;
      }
    }
    return isShortTouch || isLongTouch;
  }
  return false;
}

void TouchTimer::reset() { touchStartMillis = 0; }
