/*
  Minimum_Power_Delivery

  for use for MCU that use dynamic I2C pinout like ESP32 family of MCU

  Example using CH224X_I2C to check available power from a PD/QC charger
  and compare it against the minimum power required for your circuit.
  LED indicates if sufficient power is available.

    how to use:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 115200 baud to view results.
    - The built-in LED indicates if sufficient power is available.

  Copyright (c) 2025 Felix Ardyansyah OR FGV
  Released under the MIT License
*/

/*
  ESP32 and other 3V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(SD)        -> 5 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  SCL(SL)        -> 6 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  PG(Power Good) -> 7(VESP32S3) you can use other pinout whatever you like (with safe pin of course :))
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



// CH224A I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23, PG_PIN); // we use A7 as isPowerGood pin
// with isPowerGood  then (Wire, 0x23, PG_PIN) or (4(SDA), 5(SCL), 0x23, PG_PIN);
// without isPowerGood then (Wire, 0x23) or (4(SDA), 5(SCL), 0x23);
// PG is active low: LOW = power good, HIGH = power bad


float minimum_power = 20; //minimum power that our circuit need in this case 20W

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.print("I2C pins: SDA=");
  Serial.print(SDA_PIN);
  Serial.print(", SCL=");
  Serial.println(SCL_PIN);

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
