/*
  Minimum_Power_Delivery

  for use for MCU that use specific I2C pinout like arduino(atmega chip), LGT8F328, and STM32 family

  Example using CH224X_I2C to check available power from a PD/QC charger
  and compare it against the minimum power required for your circuit.
  LED indicates if sufficient power is available.

    how to use:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 9600 baud to view results.
    - The built-in LED indicates if sufficient power is available.

  Copyright (c) 2025 Felix Ardyansyah OR FGV
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
  SDA            -> PB7 (STM32F103xxx) your pin will be different if you use other ic
  SCL            -> PB6 (STM32F103xxx) your pin will be different if you use other ic
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
CH224X_I2C CH224X1(Wire, 0x23, PG_PIN); // we use A7 as isPowerGood pin
// with isPowerGood  then (Wire, 0x23, PG_PIN) or (4(SDA), 5(SCL), 0x23, PG_PIN);
// without isPowerGood then (Wire, 0x23) or (4(SDA), 5(SCL), 0x23);
// PG is active low: LOW = power good, HIGH = power bad


float minimum_power = 20; //minimum power that our circuit need in this case 20W

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("");
  Wire.begin();

  Serial.println("CH224A I2C Checking available vs required power");

  if (!CH224X1.begin()) {
    Serial.println("CH224 not responding!");
    while (1);
  }

  delay(200); // allow chip to stabilize
}

void loop() {
  float current = 0;
  float power = 0;
  float voltage = 0;

  Serial.println("");
  Serial.println("Checking against minimum power requirement of 20W...");

  if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_PD)) { // USB PD detected
    CH224X1.setVoltage(4); // request 20V profile
    voltage = 20;
    current = CH224X1.getCurrentProfile(); // PD lets us query max current
    Serial.println("Requesting 20V profile...");
    Serial.print("Maximum current : ");
    Serial.print(current, 0);
    Serial.print(" mA (");
    Serial.print(current / 1000.0);
    Serial.println(" A)");
    power = (current / 1000) * voltage;
    Serial.print("Available power : ");
    Serial.print(power);
    Serial.println(" W");

    if (CH224X1.isPowerGood()) { // PG LOW = good, HIGH = bad
      Serial.println("Power Good: Output stable.");
      if (power > minimum_power) { 
        Serial.println("✅ Power is enough for your circuit");
        digitalWrite(LED_BUILTIN, LED_ON); 
      } else {
        Serial.println("❌ Available power is NOT enough for your circuit");
        digitalWrite(LED_BUILTIN, LED_OFF);
      }
    } else {
      Serial.println("Voltage not supported by charger/powerbank");
      digitalWrite(LED_BUILTIN, LED_OFF);
    }

  } else if (CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC2) ||
             CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC3)) {
    CH224X1.setVoltage(0); // fallback to 5V
    Serial.println("❌ QC detected: limited to ~18W max, not sufficient");
    digitalWrite(LED_BUILTIN, LED_OFF);
  } else {
    Serial.println("❌ No supported protocol detected on charger/powerbank");
    digitalWrite(LED_BUILTIN, LED_OFF);
  }

  delay(2000); // wait before repeating
}
