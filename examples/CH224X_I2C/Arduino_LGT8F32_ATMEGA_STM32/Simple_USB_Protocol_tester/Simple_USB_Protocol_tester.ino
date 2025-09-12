/*
  Simple_USB_Protocol_Tester

  for use for MCU that use specific I2C pinout like arduino(atmega chip), LGT8F328, and STM32 family

  Example using CH224X_I2C to check which protocol your charger/powerbank supports.
  With USB PD protocol it is possible to read the maximum available current,
  but with QC protocol this information is not available.

    how to use:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 9600 baud to view results.
    - The built-in LED indicates if the power ouput is Good .


  Copyright (c) 2025 Felix Ardyansyah (FGV)
  Released under the MIT License
*/

/*
  !!! reminder CH224A is 5V sensitive it can kill your ic

  arduino(ATMEGA328 chip), LGT8F328, and other 5V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(5D)        -> A5 (ATMEGA328, LGT8F328) your pin will be different if you use other ic
  SCL(5L)        -> A4 (ATMEGA328, LGT8F328) your pin will be different if you use other ic
  PG(Power Good) -> A7
  5V             -> 5V
  3V             -> 3V
  GND            -> GND
*/

/*
  STM32 and other 3V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ---------------------------------------------------------------------------------------------------------------------------------
  SDA(SD)        -> PB7 (STM32F103xxx) your pin will be different if you use other ic
  SCL(SL)        -> PB6 (STM32F103xxx) your pin will be different if you use other ic
  3V             -> 3V
  GND            -> GND
*/

// Define built-in LED and CH224X Power-Good (PG) pin
// PG is active low: LOW = power good, HIGH = power bad
#if defined(ARDUINO_ARCH_STM32)
  // STM32 boards — default to PC13 for LED if not already defined
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

#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_SAM_DUE)
  // Arduino Mega and Arduino Due
  #ifndef LED_BUILTIN
    #define LED_BUILTIN 13
  #endif
  #define PG_PIN  17   // Use D17 for PG pin

  #define LED_ON   HIGH
  #define LED_OFF  LOW

#else
  // Default Arduino AVR boards (Uno, Nano, etc.)
  #ifndef LED_BUILTIN
    #define LED_BUILTIN 13
  #endif
  #define PG_PIN  A7

  #define LED_ON   HIGH
  #define LED_OFF  LOW
#endif



#include <Wire.h>
#include <CH224X_I2C.h>

// CH224X I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23, PG_PIN); 
// with isPowerGood  then (Wire, 0x23, PG_PIN) or (4(SDA), 5(SCL), 0x23, PG_PIN);
// without isPowerGood then (Wire, 0x23) or (4(SDA), 5(SCL), 0x23);
// PG is active low: LOW = power good, HIGH = power bad

// Voltage profiles (8-bit codes)
const uint8_t voltageProfiles[] = {0, 1, 2, 3, 4, 5};
const float voltageValues[]     = {5.0, 9.0, 12.0, 15.0, 20.0, 28.0};
const uint8_t numProfiles       = sizeof(voltageProfiles) / sizeof(voltageProfiles[0]);

void printPowerProtocols(CH224X_I2C &chip) {
  int proto = chip.getPowerProtocol();

  // First, print capability info
  if (proto & CH224X_I2C::PROTOCOL_AVS_EXIST) {
    Serial.println("AVS/Adjustable Voltage Supply supported");
  }
  if (proto & CH224X_I2C::PROTOCOL_EPR_EXIST) {
    Serial.println("EPR/Extended Power Range supported");
  }

  // Then print active protocols (bitmask)
  Serial.print("Active Protocols: ");

  bool first = true;
  struct Protocol {
    const char* name;
    uint8_t mask;
  } protocols[] = {
    {"BC",  CH224X_I2C::PROTOCOL_BC},
    {"QC2", CH224X_I2C::PROTOCOL_QC2},
    {"QC3", CH224X_I2C::PROTOCOL_QC3},
    {"PD",  CH224X_I2C::PROTOCOL_PD},
    {"EPR", CH224X_I2C::PROTOCOL_EPR}
  };

  for (auto &p : protocols) {
    if (proto & p.mask) {
      if (!first) Serial.print(", ");
      Serial.print(p.name);
      first = false;
    }
  }

  if (first) Serial.print("None");
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  // LED off at start

  Serial.println("CH224X I2C Voltage/Current Scan");

  if (!CH224X1.begin()) {
    Serial.println("CH224X not responding!");
    while (1) {
      digitalWrite(LED_BUILTIN, LED_ON); // blink LED fast to show error
      delay(200);
      digitalWrite(LED_BUILTIN, LED_OFF);
      delay(200);
    }
  }

  delay(200); // allow chip to stabilize
}

void loop() {
  // show supported protocols
  Serial.println("----- Loop start -----");
  printPowerProtocols(CH224X1);

  for (uint8_t i = 0; i < numProfiles; i++) {
    uint8_t profile = voltageProfiles[i];
    float voltage = voltageValues[i];

    // Request voltage
    CH224X1.setVoltage(profile);
    delay(400); // wait for PD negotiation

    // Read max current for this voltage
    int current_mA = CH224X1.getCurrentProfile(); 

    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" V | Max Current: ");
    if (current_mA < 0) {
      Serial.println("N/A");
    } else {
      Serial.print(current_mA);
      Serial.println(" mA");
    }

    if (CH224X1.isPowerGood()) {
      Serial.println("✅ Power Good: Output stable.");
      digitalWrite(LED_BUILTIN, LED_ON);  // LED on when PG = good
    } else {
      Serial.println("❌ Voltage not supported by charger");
      digitalWrite(LED_BUILTIN, LED_OFF);   // LED off when PG = bad
    }
    delay(400); // small delay before next profile
    Serial.println("");
  }

  Serial.println("----- Loop complete -----\n");
  delay(2000); // wait before repeating the full scan
}