#pragma once
#include "CH224X.h"

class CH224X_IO : public CH224X {
public:
  CH224X_IO(uint8_t cfg1, uint8_t cfg2, uint8_t cfg3, CH224_Chip_Type type);
  CH224X_IO(uint8_t cfg1, uint8_t cfg2, uint8_t cfg3, CH224_Chip_Type type, uint8_t pg);

  
  bool begin() override;
  bool isPowerGood() override;
  int setVoltage(uint8_t sel) override;


private:
  uint8_t _cfg1, _cfg2, _cfg3;
  CH224_Chip_Type _type;
  uint8_t _pg;
  bool _hasPG;
};