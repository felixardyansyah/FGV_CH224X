# FGV CH224X
### 🔌 Arduino library for the CH224X using the I2C or simple GPIO

The **CH224X** is a low cost usb trigger/decoy IC made by WCH that can work with I2C or GPIO using:
1) CH224X_I2C for using via I2C 
2) CH224X_IO for using via IO (GPIO) 

This library supports CH224A and CH224K, and CH224Q chips for negotiating USB Power Delivery (PD) and Quick charge (QC) voltages in **I2C**(CH224A/Q) or **GPIO**(CH224A/Q/K).
the reason why this library included 2 ways is to make learning and easier to be use by many people with dumb logic or using I2C

This library use the Arduino standard I2C/Two-Wire Library for its I2C communication. soo it should be able to work with most MCU that is supported by arduino.

# *⚠️ !!! reminder CH224A is 5V sensitive it can kill your IC*

## **Features**

### *🔍 Detect supported protocols*

### *📈 Read current output in milliamps*

### *⚡ Control USB PD/QC output voltage*

***with extra feature coming soon, assuming i can get hand of CH224Q :)***


## instalation

as the time of the writing the README.md this libraries is not officially in arduino libraries soo you need to install manually, i am so sorry :(

Go to your GitHub repo.

Click the green Code button → Download ZIP.

Open Arduino IDE.

Go to Sketch → Include Library → Add .ZIP Library…

Select the ZIP you just downloaded.

<hr>

If you want to build your own board or buy one (I’m working with local seller **Freelab Jakarta**, only available in Indonesia):
### [GITHUB MODULE BOARD CH224A LINK](https://github.com/felixardyansyah/CH224A_MODULE_FGV)
### [BUY MODULE BOARD CH224A LINK](https://www.tokopedia.com/freelab-jakarta)

## Arduino I2C Library Commands
&#8211;&nbsp;&nbsp; `isPowerGood() read data type bool`
<br>the power good indicator from CH224A/Q active low mean voltage good = 0, voltage bad = 1
<br />&#8211;&nbsp;&nbsp; `setVoltage() write data type int`
<br>0:5V, 1:9V, 2:12V, 3:15V, 4:20V, 5:28V, 6:PPS mode(CH224Q), 7:AVS mode(CH224Q)
<br>**setVoltage(4)**//  trigger the usb to output 20V (only available in USB PD protocol)
<br />&#8211;&nbsp;&nbsp; `getCurrentProfile() read data type int`
<br>it output in int for example charger/powerbank that output 65W it will say 3250 
<br />&#8211;&nbsp;&nbsp; `hasProtocol() write data type bool`
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_BC)** mean it using USB BC protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC2)** mean it using USB QC2 protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_QC3)** mean it using USB QC3 protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_PD)** mean it using USB BC protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_EPR)** mean it using USB EPR protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_EPR_EXIST)** mean the charger support EPR/Extended Power Range protocol
<br>**CH224X1.hasProtocol(CH224X_I2C::PROTOCOL_AVS_EXIST)** mean the charger support AVS/Adjustable Voltage Supply protocol


        
for using CH224A/Q with power good
`CH224X_I2C CH224X1(Wire, 0x23, PG_PIN)` or
`CH224X_I2C CH224X1(2, 3, 0x23, PG_PIN)` depending of your pinout

for using CH224A/Q without power good
`CH224X_I2C CH224X1(Wire, 0x23)` or
`CH224X_I2C CH224X1(2, 3, 0x23)` depending of your pinout

and make sure in the setup you insert `CH224X1.begin();`

## I2C Wiring with static pinout Example


###⚠️ NOTE using 5V logic MCU must use 5L and 5D pin for to use built in I2C ttl voltage converter


### Arduino / ATmega / LGT8F328 (5V logic)
| CH224X Pin     | Board Pin Example |
|----------------|-------------------|
| SDA (5D)       | A5 (Arduino, LGT8F328) → *may differ on other MCUs* |
| SCL (5L)       | A4 (Arduino, LGT8F328) → *may differ on other MCUs* |
| PG (Power Good)| A7 |
| 5V             | 5V |
| 3V             | 3.3V |
| GND            | GND |

---

### STM32 (3.3V logic)

| CH224X Pin     | Board Pin Example |
|----------------|-------------------|
| SDA (SD)       | PB7 (STM32F103xxx) → *may differ on other MCUs* |
| SCL (SL)       | PB6 (STM32F103xxx) → *may differ on other MCUs* |
| PG (Power Good)| PB5 |
| 3V             | 3.3V |
| GND            | GND |
<hr>

### ESP32xxx (3.3V logic)

| CH224X Pin     | Board Pin Example |
|----------------|-------------------|
| SDA (SD)       | GPIO 5 (ESP32-S3) → *you can choose other safe pins* |
| SCL (SL)       | GPIO 6 (ESP32-S3) → *you can choose other safe pins* |
| PG (Power Good)| GPIO 7 (ESP32-S3) → *you can choose other safe pins* |
| 3V             | 3.3V |
| GND            | GND |

<hr>

###  ⚠️ NOTE

In **I2C** mode, The default address is `0x22` or `0x23`, for USB PD you can grab current data becouse USB PD talk via in both direction via CC line, while with USB QC2/3 protocol you cannot grab maximum charger/powerbank current can handle reason the reason QC talk via Resistor not via a logic like USB PD.

# ⚠️ The wiring assuming the board have it own TTL Level Coverter if not you  need to make simple TTL level Coverter

## Arduino IO Library Commands
&#8211;&nbsp;&nbsp; `isPowerGood()`
<br>the power good indicator from CH224A/Q active low mean voltage good = 0, voltage bad = 1
<br />&#8211;&nbsp;&nbsp; `setVoltage()` 
<br>for CH224_AQ 0:5V, 1:9V, 2:12V, 3:20V, 4:28V
<br>for CH224_K  0:5V, 1:9V, 2:12V, 3:15V, 4:20V

for using CH224A/Q with power good
<br>`CH224X_IO CH224X1(IO1, IO2, IO3, CH224_AQ, PG_PIN);`

for using CH224A/Q without power good
<br>`CH224X_IO CH224X1(IO1, IO2, IO3, CH224_AQ);`

for using CH224K with power good
<br>`CH224X_IO CH224X1(IO1, IO2, IO3, CH224_K, PG_PIN);`

for using CH224K without power good
<br>`CH224X_IO CH224X1(IO1, IO2, IO3, CH224_K);`

and make sure in the setup you insert `CH224X1.begin();`

## IO Wiring with static pinout Example

### Arduino / ATmega / LGT8F328 (5V logic, with resistors)

| CH224X Pin     | Need       | Board Pin Example |
|----------------|-------------------|-------------------|
| SDA (IO1)      | 2 kΩ resistor     | A6 (Arduino, LGT8F328) → *may differ on other MCUs* |
| IO2 (SD)       | 2 kΩ resistor     | A5 (Arduino, LGT8F328) → *may differ on other MCUs* |
| IO3 (SL)       | 2 kΩ resistor     | A4 (Arduino, LGT8F328) → *may differ on other MCUs* |
| 3V             | —                 | 3.3V |
| GND            | —                 | GND |

---

### ESP32 / STM32 (3.3V logic)

| CH224X Pin     | Board Pin Example |
|----------------|-------------------|
| SDA (IO1)      | GPIO 4 (ESP32-S3) → *you can choose other safe pins* |
| IO2 (SD)       | GPIO 6 (ESP32-S3) → *you can choose other safe pins* |
| IO3 (SL)       | GPIO 5 (ESP32-S3) → *you can choose other safe pins* |
| 3V             | 3.3V |
| GND            | GND |

<hr>

In **IO** mode, you cannot do that much again it is the limitation of simple GPIO logic of CH224X but in the bright side is kinda simple and use less memory. with 3V logic MCU like STM32 or ESP32 you dont need any additional component, but if you use 5V logic MCU you need additional resistor

# ⚠️ Reminder if you use 5V logic MCU you need 2K resistor if not your CH224X will release the magic smoke


<hr>


Also included in the repository are:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch that use all the function with specific wiring of your board the the module board or IC themself
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of CH224X Datasheet in english and chinese in the Documents folder.

## Folder structure


# CH224X Library Folder Structure

- CH224X/
  - document/
    - CH224X_English_2.1.pdf
    - CH224X_Chinese_2.1.pdf
  - src/
    - CH224X.h
    - CH224X_I2C.h
    - CH224X_I2C.cpp
    - CH224X_IO.h
    - CH224X_IO.cpp
    - CH224X_Type.h
  - examples/
    - CH224X_I2C/
      - Arduino_LGT8F32_ATMEGA_STM32/
        - Minimum_power_delivery/
          - Minimum_power_delivery.ino
        - Simple_USB_Protocol_tester/
          - Simple_USB_Protocol_tester.ino
        - Voltage_selection/
          - Voltage_selection.ino
        - Voltage_selection_with_power_good/
          - Voltage_selection_with_power_good.ino
      - ESP32xxx/
        - Minimum_power_delivery/
          - Minimum_power_delivery.ino
        - Simple_USB_Protocol_tester/
          - Simple_USB_Protocol_tester.ino
        - Voltage_selection/
          - Voltage_selection.ino
        - Voltage_selection_with_power_good/
          - Voltage_selection_with_power_good.ino
    - CH224X_IO/
      - Voltage_selection/
        - Voltage_selection.ino
      - Voltage_selection_with_power_good/
        - Voltage_selection_with_power_good.ino
      - Simple_voltage_tester/
        - Simple_voltage_tester.ino
  - library.properties
  - keywords.txt
  - README.md
  - .gitattributes


## License
- **Library code (src/)** is licensed under the MIT License.  
- **Board design files (hardware/)** are licensed under the Creative Commons Attribution 4.0 International License (CC BY 4.0).  

This means:
- You can use the library freely in commercial or non-commercial projects.  
- If you manufacture or sell boards based on my design, you **must credit me AS Felix Ardyansyah OR FGV** somewhere in your documentation or product page.

## Changelog
- v1.0.0 — First public release
