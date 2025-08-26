#pragma once
#include "CH224X_Type.h"
#include <Arduino.h>

class CH224X {
public:
  virtual bool begin() = 0;
  virtual bool isPowerGood() = 0;
  virtual int setVoltage(uint8_t sel) = 0;

};
