#define ESP32DEBUGGING
#include <ESP32Logger.h>

#include "led_rgb.h"

double PingRGB::calcVal(double delta, double newVal, double val) {
  double retVal = val;
  if (delta != 0) {
    if (newVal > val) {
      if (delta > 0)
        retVal += delta;
      else
        retVal = newVal; // end value
    } else if (newVal < val) {
      if (delta < 0)
        retVal += delta;
      else
        retVal = newVal; // end value
    }
  } // delta == 0
  else {
    retVal = newVal;
  }
  return retVal;
}

void PingRGB::ledOutput() {

  valR = calcVal(deltaR, newValR, valR);
  valG = calcVal(deltaG, newValG, valG);
  valB = calcVal(deltaB, newValB, valB);

  // DBGLOG(Debug, "valRGB=(%f, %f, %f)", valR, valG, valB);

  ledcWrite(pinR, RGB_MAX_DUTY_CYCLE * valR);
  ledcWrite(pinG, RGB_MAX_DUTY_CYCLE * valG);
  ledcWrite(pinB, RGB_MAX_DUTY_CYCLE * valB);
}

void PingRGB::setTransitionMs(unsigned transitionMs) {
  this->transitionsMs = transitionMs;
};

void PingRGB::setOutput(double valR, double valG, double valB) {
  if (valR < 0.0)
    valR = 0.0;
  else if (valR > 1.0)
    valR = 1.0;

  if (valG < 0.0)
    valG = 0.0;
  else if (valG > 1.0)
    valG = 1.0;

  if (valB < 0.0)
    valB = 0.0;
  else if (valB > 1.0)
    valB = 1.0;

  newValR = valR;
  newValG = valG;
  newValB = valB;

  if (transitionsMs == 0) {
    deltaR = 0;
    deltaG = 0;
    deltaB = 0;
    this->valR = newValR;
    this->valG = newValG;
    this->valB = newValB;
  } else {
    deltaR = (newValR - this->valR) * timerOutputMillis.getInterval() /
             transitionsMs;
    deltaG = (newValG - this->valG) * timerOutputMillis.getInterval() /
             transitionsMs;
    deltaB = (newValB - this->valB) * timerOutputMillis.getInterval() /
             transitionsMs;
  };
};

void PingRGB::setup() {

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  ledcAttach(pinR, RGB_PWM_FREQ, RGB_PWM_RESOLUTION);
  ledcAttach(pinG, RGB_PWM_FREQ, RGB_PWM_RESOLUTION);
  ledcAttach(pinB, RGB_PWM_FREQ, RGB_PWM_RESOLUTION);
}

void PingRGB::loop() {

  if (timerOutputMillis.isRunning() && timerOutputMillis.isTrigger()) {
    timerOutputMillis.timerStart();
    ledOutput();
  }
}
