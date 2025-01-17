#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "meter_output.h"

void MeterOutput::meterOutput() {

  double deltaVal =
      valOutput -
      curValOutput; // difference between current value and target value
  if (deltaVal != 0) {
    double transitionSpeed =
        abs(deltaVal / 2); // transition speed depends on the log10(delta)
    double duration = abs(deltaVal) / transitionSpeed;
    double increment = deltaVal * interval / 1000 / duration;
    curValOutput += increment;
    // DBGLOG(Debug, "interval=%ld, transitionSpeed=%f deltaVal=%f, duration=%f,
    // increment=%f, curValOutput=%f\n",
    //                interval, transitionSpeed, deltaVal, duration, increment,
    //                curValOutput);
  };

  curValOutput = valOutput;
  int valDuty = MAX_DUTY_CYCLE_ADJ * log10(curValOutput) /
                3; // the range is from 1 to 1000 so log10(1000)==3
  ledcWrite(pinOut, valDuty);
}

double MeterOutput::lowPassFilter(double input, double previousOutput,
                                  double alpha) {
  // Check for invalid alpha values
  if (alpha < 0.0 || alpha > 1.0) {
    return previousOutput;
  }
  return (alpha * input) + ((1.0 - alpha) * previousOutput);
}

// this is specific to the meter - non-linearity correction
double MeterOutput::correctionFunction(double input) {

  struct LinearSegment {
    float inputFrom;
    float inputTo;
    float adjustedSlope;
    float adjustedIntercept;
  };

  // const int numSegments = 12;
  // LinearSegment segments[numSegments] = {
  //   {0, 1, 1.0, 0},
  //   {1, 2, 1.0, 0.0},
  //   {2, 4, 1.0, 1.1},
  //   {4, 8, 1.0, 0.4},
  //   {8, 16, 1.0, 0.8},
  //   {16, 32, 1.0, 7},
  //   {32, 64, 1.0, 50},
  //   {64, 128, 1.0, 90},
  //   {128, 256, 1.0, 180},
  //   {256, 512, 1.0, 220},
  //   {512, 1000, 1.0, 115},
  //   {1000, 10000, 1.0, 0},
  // };

  const int numSegments = 35;
  LinearSegment segments[numSegments] = {
      {0, 1, 1.0, 0.5},       {1, 2, 1.0, 1.5},        {2, 3, 1.0, 1.0},
      {3, 4, 1.0, 0.7},       {4, 5, 1.0, 0.5},        {5, 6, 1.0, 0.4},
      {6, 7, 1.0, 0.4},       {7, 8, 1.0, 0.5},        {8, 9, 1.0, 0.6},
      {9, 10, 1.0, 1.0},      {10, 20, 1.0, 1.8},      {20, 30, 1.0, 15.0},
      {30, 40, 1.0, 25.0},    {40, 50, 1.0, 30.0},     {50, 60, 1.0, 28.0},
      {60, 70, 1.0, 35.0},    {70, 80, 1.0, 40.0},     {80, 90, 1.0, 50.0},
      {90, 100, 1.0, 60.0},   {100, 200, 1.0, 70.0},   {200, 300, 1.0, 130.0},
      {300, 400, 1.0, 180.0}, {400, 500, 1.0, 220.0},  {500, 600, 1.0, 220.0},
      {600, 700, 1.0, 110.0}, {700, 800, 1.0, 100.0},  {800, 900, 1.0, 100.0},
      {900, 1000, 1.0, 50.0}, {1000, 10000, 1.0, 0.0},
  };

  // Iterate through the segments to find the correct range
  for (int i = 0; i < numSegments; i++) {
    if (input >= segments[i].inputFrom && input < segments[i].inputTo) {
      // Calculate the output using the linear equation
      return segments[i].adjustedSlope * input + segments[i].adjustedIntercept;
    }
  }

  // Handle cases where the input is outside the defined ranges
  if (input < segments[0].inputFrom) {
    DBGLOG(Error, "Input value is below the minimum defined range.");
    input = segments[0].inputFrom;
    return segments[0].adjustedSlope * input + segments[0].adjustedIntercept;
  } else if (input >= segments[numSegments - 1].inputTo) {
    DBGLOG(Error, "Input value is above the maximum defined range.");
    return segments[numSegments - 1].adjustedSlope * input +
           segments[numSegments - 1].adjustedIntercept;
  }

  return 0; // Should not reach here
};

void MeterOutput::setOutput(double val) {
  if (val < 0) // do not let it be negative
    val = 0;
  else if (val > MAX_OUTPUT) {
    DBGLOG(Error, "setOutput(%f), exceeded maxOutput %f", val, MAX_OUTPUT);
    val = MAX_OUTPUT;
  }

  // valOutput = correctionFunction(val);  // remember the target
  valOutput = val; // no correction function
  DBGLOG(Info, "setOutput(%0.2f), corrected valOutput %0.2f", val, valOutput);
};

void MeterOutput::setup() {

  pinMode(pinOut, OUTPUT);
  ledcAttach(pinOut, PWM_FREQ, PWM_RESOLUTION); // 12 kHz
}

void MeterOutput::loop() {

  if (timerOutputMillis.isRunning() && timerOutputMillis.isTrigger()) {
    timerOutputMillis.timerStart();
    meterOutput();
  }
}
