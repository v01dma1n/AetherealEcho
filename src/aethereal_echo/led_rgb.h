#ifndef LED_RGB_H
#define LED_RGB_H

#include "timer_millis.h"

const int RGB_PWM_CHANNEL = 0;
const int RGB_PWM_FREQ = 500;
const int RGB_PWM_RESOLUTION = 10;

const int RGB_MAX_DUTY_CYCLE = (int)(pow(2, RGB_PWM_RESOLUTION) - 1);

struct RGB {
  double r;
  double g;
  double b;
};

class PingRGB {

protected:
  double valOutput;
  unsigned pinR{0}, pinG{0}, pinB{0};
  double valR{0}, valG{0}, valB{0};

  double newValR{0}, newValG{0}, newValB{0};
  unsigned transitionsMs{0};
  double deltaR{0}, deltaG{0}, deltaB{0};

  double calcVal(double delta, double newVal, double val);

public:
  PingRGB(unsigned long interval, unsigned pinR, unsigned pinG, unsigned pinB)
      : timerOutputMillis(interval) {
    this->pinR = pinR;
    this->pinG = pinG;
    this->pinB = pinB;
  };
  void setup();
  void loop();
  // the output can be set from 0 to 1
  void setOutput(double valR, double valG, double valB);
  void setTransitionMs(unsigned transitionMs);
  TimerMillis timerOutputMillis{0};
  void off() {
    setTransitionMs(0);
    setOutput(0, 0, 0);
  };
  void fadeOut(unsigned transitionMs) {
    setTransitionMs(transitionMs);
    setOutput(0, 0, 0);
  };

private:
  void ledOutput();
};

#endif // LED_RGB_H
