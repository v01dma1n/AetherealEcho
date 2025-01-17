#ifndef TIMER_MILLIS_H
#define TIMER_MILLIS_H

#include <Arduino.h>

class TimerMillis {

protected:
  bool running = false;
  bool canRestart = false;
  unsigned long intervalMillis;
  unsigned long startMillis = 0;

public:
  TimerMillis(unsigned long intervalMillis)
      : running(false), canRestart(false), intervalMillis(intervalMillis){};

  void setCanRestart(bool val);

  bool isTrigger();
  bool isRunning() { return running; };

  void setInterval(unsigned long intervalMillis);
  unsigned long getInterval() { return intervalMillis; };

  void timerStart();
  void timerStop();
};

#endif // TIMER_MILLIS_H
