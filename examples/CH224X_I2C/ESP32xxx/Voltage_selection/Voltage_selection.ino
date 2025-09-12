/*
  Voltage_Selection

  for use for MCU that use dynamic I2C pinout like ESP32 family of MCU

  Example using CH224X_I2C to select a specific voltage depending on protocol:
    - 20V if USB PD is available
    - 12V if QC is available

    Usage:
      - Upload this sketch to your board.
      - Open the Serial Monitor at 115200 baud to view results.

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
  #define SDA_PIN      21
  #define SCL_PIN      22

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define LED_BUILTIN  8
  #define LED_ON       LOW     // active LOW LED
  #define LED_OFF      HIGH
  #define SDA_PIN      4
  #define SCL_PIN      5

#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  #define LED_BUILTIN  18
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define SDA_PIN      33
  #define SCL_PIN      35

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define LED_BUILTIN  97
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define SDA_PIN      5
  #define SCL_PIN      6

#else
  // Fallback for generic Arduino AVR, STM32, etc.
  #define LED_BUILTIN  2
  #define LED_ON       HIGH
  #define LED_OFF      LOW
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

// CH224X I2C address: 0x22 or 0x23
CH224X_I2C CH224X1(Wire, 0x23);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.print("I2C pins: SDA=");
  Serial.print(SDA_PIN);
  Serial.print(", SCL=");
  Serial.println(SCL_PIN);
  Serial.println("CH224X I2C Voltage select 12V for QC and 20 for PD protocol");

  if (!CH224X1.begin()) {
    Serial.println("CH224 not responding!");
    while (1);
  }

  delay(200); // allow chip to stabilize
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
