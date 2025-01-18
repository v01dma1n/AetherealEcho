#ifndef TOUCH_TIMER_H
#define TOUCH_TIMER_H

#include "timer_millis.h"

class TouchTimer {

protected:
  int touchThreshold{40};
  uint8_t pin;

  unsigned shortTouchMillis{100};
  unsigned longTouchMillis{2000};

  unsigned touchStartMillis;

public:
  TouchTimer(unsigned long interval, uint8_t pin,
             unsigned shortTouchMillis = 100, unsigned longTouchMillis = 2000)
      : timerMillis(interval) {
    this->pin = pin;
    this->shortTouchMillis = shortTouchMillis;
    this->longTouchMillis = longTouchMillis;
  };
  void setup();
  bool loop();

  void reset();

  TimerMillis timerMillis{0};

  bool isShortTouch{false};
  bool isLongTouch{false};

private:
};

#endif // TOUCH_TIMER_H
