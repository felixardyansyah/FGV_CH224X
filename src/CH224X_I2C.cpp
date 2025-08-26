#include "CH224X_I2C.h"

CH224X_I2C::CH224X_I2C(TwoWire& wire, uint8_t address)
  : _wire(&wire), _address(address), _hasPG(false) {}

CH224X_I2C::CH224X_I2C(TwoWire& wire, uint8_t address, uint8_t pg)
  : _wire(&wire), _address(address), _pg(pg), _hasPG(true) {}

bool CH224X_I2C::begin() {
  _wire->begin();
  if (_hasPG) pinMode(_pg, INPUT_PULLUP);
  return true;
}

bool CH224X_I2C::isPowerGood() {
  if (!_hasPG) return false;      // no PG pin → assume power is not good
  return !digitalRead(_pg);       // PG connected → read normally
}
// CH224 PG (Power Good) is active low, if voltage is set and stable then is goes low if is bad then is goes high
// If PG pin is not used CH224X_I2C(TwoWire& wire, uint8_t address); this function automatically returns false
// voltage good = 0, voltage bad = 1


int CH224X_I2C::setVoltage(uint8_t sel) {
  _wire->beginTransmission(_address);
  _wire->write(0x0A);  // Assume voltage register
  _wire->write(sel);
  return _wire->endTransmission() == 0;
}
// 8 bit 
// 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)



int CH224X_I2C::getCurrentProfile() {
  _wire->beginTransmission(_address);
  _wire->write(0x50);  // Register address for current
  if (_wire->endTransmission(false) != 0) return -1;

  _wire->requestFrom(_address, (uint8_t)2);
  if (_wire->available() < 1) return -1;

  uint16_t raw = _wire->read();
  return raw * 50;  // Each unit = 50mA
}
// 50mA * value of in the bit = max current 
// for example, if you read address 0x50 and you get value 0x002D or 45 in decimal for 45w charger then 45 * 50 = 2250 ma or 2,25 A 
// "Indicates the maximum current value available under the current PD gear. This register is only valid during the handshake PD protocol."
// the information of the maximum current only at the voltage is set at for example in 15V it maybe can reach 3A but in 20V it only can reach 2,25A

int CH224X_I2C::getPowerProtocol() {
  _wire->beginTransmission(_address);
  _wire->write(0x09);  // Protocol register
  if (_wire->endTransmission(false) != 0) return 0;

  _wire->requestFrom(_address, (uint8_t)1);
  if (_wire->available()) {
    return _wire->read();  // Bitmask
  }
  return 0;
}
// | Bit | Name           |                    |
// |-----|----------------|--------------------|
// | 7   | Reserved       |                    |
// | 6   | AVS exist      | 1 = exist, 0 = nah |
// | 5   | EPR exist      | 1 = exist, 0 = nah |
// | 4   | EPR activation | 1 = exist, 0 = nah |
// | 3   | PD activation  | 1 = exist, 0 = nah |
// | 2   | QC3 activation | 1 = exist, 0 = nah |
// | 1   | QC2 activation | 1 = exist, 0 = nah |
// | 0   | BC activation  | 1 = exist, 0 = nah |
