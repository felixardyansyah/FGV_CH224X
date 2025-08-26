/*
  Voltage_Selection

  for use for MCU that use specific I2C pinout like arduino(atmega chip), LGT8F328, and STM32 family

  Example using CH224X_I2C to select a specific voltage depending on protocol:
    - 20V if USB PD is available
    - 12V if QC is available

    Usage:
      - Upload this sketch to your board.
      - Open the Serial Monitor at 9600 baud to view results.

  Copyright (c) 2025 Felix Ardyansyah (FGV)
  Released under the MIT License
*/

/*
  !!! reminder CH224A is 5V sensitive it can kill your ic

  arduino(atmega chip), LGT8F328, and other 5V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(5D)        -> A5 (arduino, LGT8F328) your pin will be different if you use other ic
  SCL(5L)        -> A4 (arduino, LGT8F328) your pin will be different if you use other ic
  PG(Power Good) -> A7
  5V             -> 5V
  3V             -> 3V
  GND            -> GND
*/

/*
  STM32 and other 3V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(SD)        -> PB7 (STM32F103xxx) your pin will be different if you use other ic
  SCL(SL)        -> PB6 (STM32F103xxx) your pin will be different if you use other ic
  3V             -> 3V
  GND            -> GND
*/

#include <Wire.h>
#include <CH224X_I2C.h>

// CH224X I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23);


void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("");
  Serial.println("CH224X I2C Voltage select 12V for QC and 20 for PD protocol");

  if (!CH224X1.begin()) {
    Serial.println("CH224X not responding!");
    while (1);
  }

  delay(200); 
}

void loop() {
  Serial.println("----- Loop start -----");
  if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_PD)) {
    CH224X1.setVoltage(4); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
    Serial.println("Negotiated: PD to 20V");
    Serial.print("maximum current : ");
    Serial.print(CH224X1.getCurrentProfile()); // with USB PD protocol is possible to grab maximum charger/powerbank current can handle
    Serial.print(" ma or ");
    Serial.print((float)CH224X1.getCurrentProfile() / 1000.0);
    Serial.println(" A");
    CH224X1.setVoltage(4); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
  } else if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC2) || CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC3)) {
    CH224X1.setVoltage(2); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
    Serial.println("Negotiated: QC to 12V");
    // with USB QC2/3 protocol you cannot grab maximum charger/powerbank current can handle reason the reason QC talk via Resistor not via a logic like USB PD
  } else {
    Serial.println("there is no protocol that this charger/powerbank use");
  }
  delay(2000); // wait before repeating the full scan
  Serial.println("");
}
