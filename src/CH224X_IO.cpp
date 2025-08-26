#pragma once
#include "CH224X_IO.h"

CH224X_IO::CH224X_IO(uint8_t cfg1, uint8_t cfg2, uint8_t cfg3, CH224_Chip_Type type)
  : _cfg1(cfg1), _cfg2(cfg2), _cfg3(cfg3), _type(type), _hasPG(false) {}

CH224X_IO::CH224X_IO(uint8_t cfg1, uint8_t cfg2, uint8_t cfg3, CH224_Chip_Type type, uint8_t pg)
  : _cfg1(cfg1), _cfg2(cfg2), _cfg3(cfg3), _pg(pg), _type(type), _hasPG(true) {}

bool CH224X_IO::begin() {
  pinMode(_cfg1, OUTPUT);
  pinMode(_cfg2, OUTPUT);
  pinMode(_cfg3, OUTPUT);
  if (_hasPG) pinMode(_pg, INPUT_PULLUP);
  return true;
}

bool CH224X_IO::isPowerGood() {
  if (!_hasPG) return false;      // no PG pin → assume power is not good
  return !digitalRead(_pg);       // PG connected → read normally
}
// CH224 PG (Power Good) is active low, if voltage is set and stable then is goes low if is bad then is goes high
// If PG pin is not used CH224X_I2C(uint8_t cfg1, uint8_t cfg2, uint8_t cfg3, CH224_Chip_Type type); this function automatically returns false
// voltage good = 0, voltage bad = 1



int CH224X_IO::setVoltage(uint8_t sel) {
  bool c1 = 0, c2 = 0, c3 = 0;
  switch (_type) {
    case CH224_AQ:
      switch (sel) {
        case 0: c1 = 1; c2 = 0; c3 = 0; break; // 5V
        case 1: c1 = 0; c2 = 0; c3 = 0; break; // 9V
        case 2: c1 = 0; c2 = 0; c3 = 1; break; // 12V
        case 3: c1 = 0; c2 = 1; c3 = 1; break; // 20V
        case 4: c1 = 0; c2 = 1; c3 = 0; break; // 28V
        default: return -1; // error
      }
      break;
    case CH224_K:
      switch (sel) {
        case 0: c1 = 1; c2 = 0; c3 = 0; break; // 5V
        case 1: c1 = 0; c2 = 0; c3 = 0; break; // 9V
        case 2: c1 = 0; c2 = 0; c3 = 1; break; // 12V
        case 3: c1 = 0; c2 = 1; c3 = 1; break; // 15V
        case 4: c1 = 0; c2 = 1; c3 = 0; break; // 20V
        default: return -1;
      }
      break;
  }
  digitalWrite(_cfg1, c1);
  digitalWrite(_cfg2, c2);
  digitalWrite(_cfg3, c3);
  return sel;  // return the selected profile index
}
