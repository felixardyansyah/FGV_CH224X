/*
  Voltage_Selection_with_PowerGood

  for use for MCU that use specific I2C pinout like arduino(atmega chip), LGT8F328, and STM32 family

  Example using CH224X_I2C to select a specific voltage depending on protocol:
    - 20V if USB PD is available
    - 12V if QC is available

  This example also uses the CH224X "Power Good" (PG) pin as an indicator.
  Note: PG is active-low, meaning LOW = power is good.

  Usage:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 9600 baud to view results.
    - The built-in LED will turn ON if sufficient power is available.

  note
  With USB PD protocol it is possible to read the maximum available current,
  but with QC protocol this information is not available.

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
  PG(Power Good) -> PB5
  3V             -> 3V
  GND            -> GND
*/


// Define built-in LED and CH224X Power-Good (PG) pin
// PG is active low: LOW = power good, HIGH = power bad

#if defined(ARDUINO_ARCH_STM32)
  // STM32 boards — default to PA5 for LED if not already defined
  #ifndef LED_BUILTIN
    #define LED_BUILTIN PC13
  #endif
  #define PG_PIN  PB5   // adjust if using a custom board

  // Many STM32 boards have active-low LEDs
  #define LED_ON   LOW
  #define LED_OFF  HIGH

#elif defined(__LGT8FX8E__) || defined(__LGT8F328P__)
  // LGT8F boards — default to D13 for LED
  #ifndef LED_BUILTIN
    #define LED_BUILTIN D13
  #endif
  #define PG_PIN  A7

  #define LED_ON   HIGH
  #define LED_OFF  LOW

#else
  // Default Arduino AVR boards (Uno, Nano, etc.)
  #ifndef LED_BUILTIN
    #define LED_BUILTIN D13
  #endif
  #define PG_PIN  A7

  #define LED_ON   HIGH
  #define LED_OFF  LOW
#endif

#include <Wire.h>
#include <CH224X_I2C.h>

// CH224A I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23, PG_PIN);
// with isPowerGood  then (Wire, 0x23, PG_PIN) or (4(SDA), 5(SCL), 0x23, PG_PIN);
// without isPowerGood then (Wire, 0x23) or (4(SDA), 5(SCL), 0x23);
// PG is active low: LOW = power good, HIGH = power bad

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Wire.begin();

  Serial.println("");
  Serial.println("CH224A I2C Voltage select 12V for QC and 20 for PD protocol with power indicator");

  if (!CH224X1.begin()) {
    Serial.println("CH224 not responding!");
    while (1);
  }

  delay(200); // allow chip to stabilize
}

void loop() {
  Serial.println("----- Loop start -----");

  if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_PD)) { // checking if it use USB PD protocol
    CH224X1.setVoltage(4); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
    Serial.println("Negotiated: PD to 20V");
    Serial.print("maximum current : ");
    Serial.print(CH224X1.getCurrentProfile()); // with USB PD protocol is possible to grab maximum charger/powerbank current can handle
    Serial.print(" ma or ");
    Serial.print((float)CH224X1.getCurrentProfile() / 1000.0);
    Serial.println(" A");
    CH224X1.setVoltage(4); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
      if (CH224X1.isPowerGood()){
        Serial.println("✅ Power Good: Output stable.");
        digitalWrite(LED_BUILTIN, LED_ON); // turns on
      }else{
        Serial.println("❌ Voltage not supported by charger");
        digitalWrite(LED_BUILTIN, LED_OFF);  // turns off

      }
  } else if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC2) || CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC3)) { // checking if it use USB QC2/3 protocol
    CH224X1.setVoltage(2); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
    CH224X1.setVoltage(2); // 0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode (CH224Q)
    // with USB QC2/3 protocol you cannot grab maximum charger/powerbank current can handle reason the reason QC talk via Resistor not via a logic like USB PD
      if (CH224X1.isPowerGood()){
        Serial.println("✅ Power Good: Output stable.");
        digitalWrite(LED_BUILTIN, LED_ON); // turns on
      }else{
        Serial.println("❌ Voltage not supported by charger");
        digitalWrite(LED_BUILTIN, LED_OFF);  // turns off
      }
  } else {
    Serial.println("there is no protocol that this charger/powerbank use");
    digitalWrite(LED_BUILTIN, LED_OFF);  // turns off
  }
  delay(2000); // wait before repeating the full scan
  Serial.println("");

}
