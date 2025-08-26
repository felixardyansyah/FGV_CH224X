#pragma once
#include "CH224X.h"
#include <Wire.h>


class CH224X_I2C : public CH224X {
public:
  CH224X_I2C(TwoWire& wire, uint8_t address);
  CH224X_I2C(TwoWire& wire, uint8_t address, uint8_t pg);

  bool begin() override;
  bool isPowerGood() override;
  int setVoltage(uint8_t sel) override;
  int getCurrentProfile();
  int getPowerProtocol();
  // uint8_t getVoltageSelection(float voltage) override;
  // void printProfiles(Stream& out) override;

  // Convenience check
  bool hasProtocol(uint8_t protocol) {
    return (getPowerProtocol() & protocol) != 0;
  }


  struct PowerProtocolInfo {
    bool BC, QC2, QC3, PD, EPR, EPRExist, AVSExist;
  };
  enum CH224X_Protocols {
    PROTOCOL_BC         = 0x01,
    PROTOCOL_QC2        = 0x02,
    PROTOCOL_QC3        = 0x04,
    PROTOCOL_PD         = 0x08,
    PROTOCOL_EPR        = 0x10,
    PROTOCOL_EPR_EXIST  = 0x20,
    PROTOCOL_AVS_EXIST  = 0x40
  };


private:
  TwoWire* _wire;
  uint8_t _address;
  uint8_t _pg;
  bool _hasPG;
};