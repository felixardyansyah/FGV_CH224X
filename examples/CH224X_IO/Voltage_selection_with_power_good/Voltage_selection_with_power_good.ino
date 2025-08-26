/*
  Voltage_Selection_with_PowerGood

  Example using CH224X_IO to select a specific voltage to 12V via IO

  This example also uses the CH224X "Power Good" (PG) pin as an indicator.
  Note: PG is active-low, meaning LOW = power is good.

  Usage:
    - Upload this sketch to your board.
    - Open the Serial Monitor at 115200 baud to view results.
    - The built-in LED will turn ON if sufficient power is available.

  Copyright (c) 2025 Felix Ardyansyah (FGV)
  Released under the MIT License
*/


/*
  !!! reminder CH224A is 5V sensitive it can kill your ic

  arduino(atmega chip), LGT8F328, and other 5V IC logic you need 2KΩ resistor between your gpio and CH224X IC
  Wiring for my board CH224X module:

  CH224X         -> need   ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(IO1)       -> 2KΩ resistor -> A6 (arduino, LGT8F328) your pin will be different if you use other ic
  IO2(SD)        -> 2KΩ resistor -> A5 (arduino, LGT8F328) your pin will be different if you use other ic
  IO3(SL)        -> 2KΩ resistor -> A4 (arduino, LGT8F328) your pin will be different if you use other ic
  3V             -> 3V
  GND            -> GND

  ESP32, STM32, and other 3V IC logic
  Wiring for my board CH224X module:

  CH224X         ->  Board
  ------------------------------------------------------------------------------------------------------------------------------------------
  SDA(IO1)       -> 4 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  IO2(SD)        -> 6 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  IO3(SL)        -> 5 (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  3V             -> 3V (ESP32S3) you can use other pinout whatever you like (with safe pin of course :))
  GND            -> GND

  just uncomment and modify it below #include <CH224X_I2C.h>
  // #undef IO1
  // #undef IO2
  // #undef IO2
  // #define IO1 4
  // #define IO2 5
  // #define IO3 6
*/

#if defined(ARDUINO_ARCH_STM32)
  #ifndef LED_BUILTIN
    #define LED_BUILTIN PC13
  #endif
  #define PG_PIN   PB5
  #define LED_ON   LOW
  #define LED_OFF  HIGH
  #define IO1      A4
  #define IO2      A5
  #define IO3      A6

#elif defined(__LGT8FX8E__) || defined(__LGT8F328P__)
  #ifndef LED_BUILTIN
    #define LED_BUILTIN D13
  #endif
  #define PG_PIN   A7
  #define LED_ON   HIGH
  #define LED_OFF  LOW
  #define IO1      A4
  #define IO2      A5
  #define IO3      A6

#elif defined(CONFIG_IDF_TARGET_ESP32)
  #define LED_BUILTIN  2
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       4
  #define IO1      4
  #define IO2      5
  #define IO3      6

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  #define LED_BUILTIN  8
  #define LED_ON       LOW
  #define LED_OFF      HIGH
  #define IO1      4
  #define IO2      5
  #define IO3      6

#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  #define LED_BUILTIN  18
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       17
  #define IO1      4
  #define IO2      6
  #define IO3      5

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
  #define LED_BUILTIN  97
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define PG_PIN       7
  #define IO1      4
  #define IO2      6
  #define IO3      5

#else
  // Default fallback (Arduino Uno, Nano, generic boards)
  #ifndef LED_BUILTIN
    #define LED_BUILTIN 13
  #endif
  #define PG_PIN       A7
  #define LED_ON       HIGH
  #define LED_OFF      LOW
  #define IO1      A4
  #define IO2      A5
  #define IO3      A6
#endif


#include <CH224X_IO.h>

// But if you want to override: uncomment them via in windows *ctrl + /* in your keyboard
// #undef IO1
// #undef IO2
// #undef IO2
// #define IO1 4
// #define IO2 5
// #define IO3 6

CH224X_IO CH224X1(IO1, IO3, IO2, CH224_AQ, PG_PIN);// <-- change this for you cpecific CH224X IC
// with isPowerGood  then (IO1, IO3, IO2, CH224_AQ, PG_PIN)
// without isPowerGood then (IO1, IO3, IO2, CH224_AQ)
// PG is active low: LOW = power good, HIGH = power bad
// there is 2 chip type is CH224_AQ and CH224_K that have different logic for its io
// for CH224_AQ 0:5V, 1:9V, 2:12V, 3:20V, 4:28V
// for CH224_K  0:5V, 1:9V, 2:12V, 3:15V, 4:20V


void setup() {
  Serial.begin(115200);
  CH224X1.begin();   // Initialize pin modes

  Serial.println("");
  Serial.println("CH224X I2C Voltage select to 12V for QC both PD protocol");// reminder in IO mode you only can set voltage no current info
  delay(200); 
}

void loop() {
  Serial.println("----- Loop start -----");
  CH224X1.setVoltage(2);// we are using CH224_AQ 
  // for CH224_AQ 0:5V, 1:9V, 2:12V, 3:20V, 4:28V we are using this
  // for CH224_K  0:5V, 1:9V, 2:12V, 3:15V, 4:20V
  Serial.println("Negotiated: USB to 12V");
  if (CH224X1.isPowerGood()){
      Serial.println("✅ Power Good: Output stable.");
      digitalWrite(LED_BUILTIN, LED_ON); // turns on
    }else{
      Serial.println("❌ Voltage not supported by charger");
      digitalWrite(LED_BUILTIN, LED_OFF);  // turns off
    }
  delay(2000); // wait before repeating the full scan
  Serial.println("");
}