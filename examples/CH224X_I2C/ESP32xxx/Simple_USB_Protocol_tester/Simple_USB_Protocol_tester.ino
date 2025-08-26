/*
  Simple_USB_Protocol_Tester

  for use for MCU that use dynamic I2C pinout like ESP32 family of MCU

  Example using CH224X_I2C to check which protocol your charger/powerbank supports.
  With USB PD protocol it is possible to read the maximum available current,
  but with QC protocol this information is not available.

  This example also uses the CH224X "Power Good" (PG) pin as an indicator.
  Note: PG is active-low, meaning LOW = power is good.

    how to use:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 115200 baud to view results.
    - The built-in LED indicates if the power ouput is Good .


  Copyright (c) 2025 Felix Ardyansyah (FGV)
  Released under the MIT License
*/

/*
  ESP32 and other 3V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(SD)        -> 5 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  SCL(SL)        -> 6 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  PG(Power Good) -> 7 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  3V             -> 3V
  GND            -> GND

  just uncomment and modify it below #include <CH224X_I2C.h>
  // #undef SDA_PIN
  // #undef SCL_PIN
  // #define SDA_PIN 16
  // #define SCL_PIN 17
*/


// Define built-in LED and CH224X Power-Good (PG) pin
// PG is active low: LOW = power good, HIGH = power bad
// !!! dont forget to jumper RGB or LED jumper on certain dev board
// !!! reminder is that RGB or LED pinout is sometime is different from every board
#if defined(CONFIG_IDF_TARGET_ESP32)
  #define LED_BUILTIN  2
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       4       // safe GPIO
  #define SDA_PIN      21
  #define SCL_PIN      22

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define LED_BUILTIN  8
  #define LED_ON       LOW     // active LOW LED
  #define LED_OFF      HIGH
  #define PG_PIN       3       // safe GPIO
  #define SDA_PIN      4
  #define SCL_PIN      5

#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  #define LED_BUILTIN  18
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       17      // safe GPIO
  #define SDA_PIN      33
  #define SCL_PIN      35

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define LED_BUILTIN  97
  #define LED_ON       HIGH   
  #define LED_OFF      LOW
  #define PG_PIN       7      // safe GPIO
  #define SDA_PIN      5
  #define SCL_PIN      6

#else
  // Fallback for generic Arduino AVR, STM32, etc.
  #define LED_BUILTIN  13
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       4
  #define SDA_PIN      SDA     // use core-defined
  #define SCL_PIN      SCL
#endif

#include <Wire.h>
#include <CH224X_I2C.h>


// But if you want to override: uncomment them via in windows *ctrl + /* in your keyboard
// #undef SDA_PIN
// #undef SCL_PIN
// #define SDA_PIN 16
// #define SCL_PIN 17

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


// CH224A I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23, PG_PIN); // we use A7 as isPowerGood pin
// with isPowerGood  then (Wire, 0x23, PG_PIN) or (4(SDA), 5(SCL), 0x23, PG_PIN);
// without isPowerGood then (Wire, 0x23) or (4(SDA), 5(SCL), 0x23);
// PG is active low: LOW = power good, HIGH = power bad



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.print("I2C pins: SDA=");
  Serial.print(SDA_PIN);
  Serial.print(", SCL=");
  Serial.println(SCL_PIN);
  Serial.println("CH224X I2C Voltage/Current Scan");

  if (!CH224X1.begin()) {
    Serial.println("CH224 not responding!");
    while (1);
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