#ifndef METER_OUTPUT_H
#define METER_OUTPUT_H

#include "timer_millis.h"

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 500;
const int PWM_RESOLUTION = 10;

const double MAX_OUTPUT = 1500; // this is meter specific

const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);
const int MAX_DUTY_CYCLE_ADJ = (int)(MAX_DUTY_CYCLE - MAX_DUTY_CYCLE / 20.0);
// const double transitionSpeed = log10(1000)/3;  // change of 1000 in 3 seconds
const double LOW_PASS_FILTER_ALPHA = 0.1; // 0.05;

class MeterOutput {

protected:
  double valOutput{0};
  double curValOutput{0}; // current output on its way to valOutput

  unsigned long interval;
  unsigned pinOut;

  double lowPassFilter(double input, double previousOutput, double alpha);
  double correctionFunction(double input);

public:
  MeterOutput(unsigned long interval, unsigned pinOut)
      : timerOutputMillis(interval) {
    this->interval = interval;
    this->pinOut = pinOut;
  };
  void setup();
  void loop();
  void setOutput(double val);
  TimerMillis timerOutputMillis{0};

private:
  void meterOutput();
};

#endif // METER_OUTPUT_H