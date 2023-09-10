## Solar power Lora sensor platform
Power supply is one of the main challenges for engineers working on IoT products. On one hand, it is difficult to access electricity in rural areas. On the other hand, battery capacity is limited and cannot support long operations without an external power supply.

This project creates a LoRa sensor platform that is powered by a solar panel. Unlike existing products that require an external power supply or battery capacity that is limited and cannot support long operations without an external power supply, the solar panel system provides 24x7 operation to the platform
This platform can be extended with different features by integrating it with different sensors.

The solar panel charges the power bank during the daytime, and the power bank provides energy for the platform. If the charging energy is greater than the consuming energy on a daily or weekly basis, then the platform can theoretically operate 24x7.
On the platform, the “XMC1300 Boot Kit” obtains pressure and CO2 data from the "S2GO PRESSURE DPS368" and "EVAL PASCO2 MINIBOARD" sensors, respectively. It then sends this data to a concentrator via LoRa every hour.

---
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/LICENSE)

<a href="https://www.buymeacoffee.com/unionpro" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 38px !important;width: 168px !important;" ></a>

---
## System block diagram
```
        Solar Power Lora Sensor                                            Lora receiver                     Android logger

   +-------------+   +----------+  +----------+                           +----------------+
   | Solar panel |   | Pressure |  |   CO2    |                           |   Power bank   |
   +-------------+   |  sensor  |  |  sensor  |                           +----------------+
          |          +----------+  +----------+                                   |
          |               |             |                                 +----------------+                 +-----------+
   +-------------+   +------------------------+ +------+         +------+ |   ESP32 Lora   |   (Bluetooth)   |  mobile   |
   | Power bank  |---|         XMC1300        |-| Lora |   ...   | Lora |-|    adapter     |       ...       |  device   |
   +-------------+   +------------------------+ +------+         +------+ +----------------+                 +-----------+
```
note: Please refer to the GitHub repository https://github.com/unionpronet/github-esp32-lora-adapter for information on setting up the “Lora receiver” and “Android logger” portions.


## Hardware requirement
1. Infineon XMC1300 Boot Kit (https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc13_boot_001/)
2. Infineon S2GO PRESSURE DPS368 (https://www.infineon.com/cms/en/product/evaluation-boards/kit_dps368_2go/)
3. Infineon EVAL PASCO2 MINIBOARD (https://www.infineon.com/cms/en/product/evaluation-boards/eval_pasco2_miniboard/)
4. XMC-Lora-sensor Adaptor board (https://unionpronet.wordpress.com/xmc-lora-sensor/)
5. Ai-Thinker Ra-01SC Lora module (https://docs.ai-thinker.com/_media/ble/docs/ra-01sc%E8%A7%84%E6%A0%BC%E4%B9%A620210413.pdf)
6. Solar panel
7. Power bank
8. Computer 

note: Please refer to the GitHub repository https://github.com/unionpronet/github-esp32-lora-adapter for information about the hardware for the “Lora receiver” and “Android logger” portions.

### image of "Solar Power Lora Sensor" portion 
[![XMC Lora Sensor (debug)](/images/xmc-lora-sensor-debug.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/xmc-lora-sensor-debug.jpg)


### image of whole system 
The "Solar Power Lora Sensor (XMC)" portion is shown on the bottom half.  
The "Lora Receiver (ESP32)" portion is shown on the upper half.
[![system image](/images/system-test.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/system-test.jpg)

### image of Lora data received on Android APP
[![ESP32 Lora Adapter](/images/bluetooth-serial-screen.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/bluetooth-serial-screen.jpg) 

---
## Software setup
1. Install [Arduino IDE 2.0+ for Arduino](https://www.arduino.cc/en/Main/Software)
2. Install [Infineon's XMC Microcontroller Boards for Arduino](https://github.com/Infineon/XMC-for-Arduino) for [XMC1300 Boot Kit]
3. Install [XENSIV™ Digital Pressure Sensor Arduino Library](https://github.com/Infineon/arduino-xensiv-dps3xx)
4. Install [XENSIV™ PAS CO2 Sensor Arduino Library](https://github.com/Infineon/arduino-pas-co2-sensor)
5. Install [Arduino DebugLog lib](https://www.arduino.cc/reference/en/libraries/debuglog/)
6. Install [Arduino RadioLib](https://www.arduino.cc/reference/en/libraries/radiolib/)
7. Install [Arduino MemStream lib](https://github.com/Apollon77/MemStream)


## Modify Arduino XMC package files
a. comment line 87 of <Arduino folder>/Arduino15/packages/Infineon/hardware/xmc/2.2.0/cores/wiring_constants.h, as below
```
// #define map( x, in_min, in_max, out_min, out_max)   ( ( (x - in_min) * (out_max - out_min) ) / (in_max - in_min) + out_min)
```
b. insert "__WEAK" on line 298 of <Arduino folder>/Arduino15/packages/Infineon/hardware/xmc/2.2.0/cores/wiring_time.c, as below
```
__WEAK void SysTick_Handler( void )
```
c. change line 255 of <Arduino folder>/Arduino15/packages/Infineon/hardware/xmc/2.2.0/libraries/RTC/src/RTC.cpp
```
	if(alarm == XMC_RTC_EVENT_ALARM)
to    
	if (alarm == ALARM)
```
d. insert "__WEAK" on line 67 of <Arduino folder>/Arduino15/packages/Infineon/hardware/xmc/2.2.0/libraries/DeviceControlXMC/src/utility/timer.c, as below
```
__WEAK void CCU40_0_IRQHandler( void )
```

## Modify Arduino library for XMC board
modify header files for XMC1300_Boot_Kit (line 26 of MemStream.h)

From
```
#include <inttypes.h>
#include <Stream.h>
#include <avr/pgmspace.h>
```
To
```
#include <inttypes.h>
#include <Stream.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#elif defined(XMC1300_Boot_Kit)
#undef memcpy_P
#define memcpy_P memcpy
#else
#define pgm_read_byte(addr) \
  (*(const unsigned char *)(addr)) ///< PROGMEM workaround for non-AVR
#endif
```

## Download and extract the github-xmc-lora-sensor source code from github 
1. Launch Arduino IDE and open "github-xmc-lora-sensor.ino"
2. Click menu "Sketch" -> "Verify/Compile". If everything goes smooth, you should see the followings
[![Build success](/images/build-success.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/build-success.jpg)

3. Connect the "XMC-lora-sensor" board to PC by an USB cable
6. On the Arduino IDE, click menu "Sketch" -> "Upload". You should see the followings
[![Upload success](/images/upload-success.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/upload-success.jpg)

7. Launch "Serial Monitor" in the Arduino IDE by clicking menu "Tools" -> "Serial Monitor". You should see the followings
[![Serial monitor](/images/serial-monitor.jpg)](https://github.com/unionpronet/github-xmc-lora-sensor/blob/main/images/serial-monitor.jpg)


---

## Code Example

### pin assignment
pin assignment is defined in "pins.h"
```
// pins.h
#define PIN_I2C_SCL 10 // 10/P2.0
#define PIN_I2C_SDA 11 // 11/P2.1

#define PIN_RF_MOSI 22   // 22/P1.1
#define PIN_RF_MISO 23   // 23/P1.0
#define PIN_RF_NSS 29    // 29/P0.6
#define PIN_RF_SCK 30    // 30/P0.7
#define PIN_RF_BUSY 31   // 31/P0.4/rfBUSY
#define PIN_RF_DIO1 EXT0 // 14/P0.12/rfDIO1
#define PIN_RF_RST 33 // 33/P0.3

```
### In "setup()", it initiates debug logger, pressure and CO2 sensors, lora module and XMC device controller (for deepSleep) sequentially.
```
static void initPeripheral(void)
{
      /////////////////////////////////////////////////////////////////////////////
      // init I2C
      Wire.begin();
      Wire.setClock(I2C_FREQ_HZ);

      /////////////////////////////////////////////////////////////////////////////
      // init pressure sensor
      bool bInitPressureSensor = sensor::pressure::init(Wire);
      uint8_t productID = sensor::pressure::getProductId();
      uint8_t revID = sensor::pressure::getRevisionId();
      LOG_TRACE("sensor::pressure::init() returns ", bInitPressureSensor, ", getProductId()=", productID, ", getRevisionId()=", revID);

      /////////////////////////////////////////////////////////////////////////////
      // init CO2 sensor 
      bool bInitCo2Sensor = sensor::co2::init();
      LOG_TRACE("sensor::co2::init() returns ", bInitCo2Sensor);

      uint8_t prodId, revId;
      bool rstGetDevID = sensor::co2::getDeviceID(prodId, revId);
      LOG_TRACE("sensor::co2::getDeviceID() returns ", rstGetDevID, ", product id=", prodId, ", revision id=", revId);

      /////////////////////////////////////////////////////////////////////////////
      // init Lora module
      bool initLora = rfLora.init(rfConfig, &loraIsr);
      LOG_TRACE("rfLora.init() returns ", initLora);
}

void setup()
{
      /////////////////////////////////////////////////////////////////////////////
      Serial.begin(115200);

      // set log_leval by following macro
      LOG_SET_LEVEL(DefaultLogLevel);
      LOG_SET_DELIMITER("");
      LOG_ATTACH_SERIAL(Serial);
      /////////////////////////////////////////////////////////////////////////////

      delay(3000);  // Provides time for the PC to initialize the serial port for the XMC 1300 boot kit during power up.

      initPeripheral();
      pmu::init();
}
```

### A pseudo event driven approach is used in the "loop()" function. 
When there is a status change in the Lora module, a loraIsr() callback is invoked, which sets an “AppEvent::LORA_ISR” flag in the event flags.
In the "loop()" function, it checks the event flags. 
If the "AppEvent::LORA_ISR" flag is set, it processes the Lora status by calling "handlerLoraDio1()".
If the "AppEvent::REPORT_DATA" flag is set, it get sensors' data and send the data via the Lora module.
If there is no event, the system sets a wakeup alarm with a duration of "SENSOR_REPORT_INTERVAL" seconds. After that, it goes to deep sleep to save power. The "AppEvent::REPORT_DATA" flag is set after exiting from deep sleep to trigger the process of sending sensor data.”

```
static void loraIsr(void)
{
      appEvent.setEventFlag(AppEvent::LORA_ISR);
}

void handlerLoraDio1(void)
{
      uint16_t irqStatus = rfLora.getIrqStatus();
      rfLora.clearIrqStatus(RADIOLIB_SX126X_IRQ_ALL);

      ...
}

void loop()
{
      ledMcu.on();

      uint16_t flag = appEvent.getEventFlag();
      LOG_TRACE("appEvent.getEventFlag() returns (hex)", DebugLogBase::HEX, flag);
      if (flag & AppEvent::LORA_ISR)
      {
            appEvent.clearEventFlag(AppEvent::LORA_ISR);
            handlerLoraDio1();
      }
      else if (flag & AppEvent::REPORT_DATA)
      {
            appEvent.clearEventFlag(AppEvent::REPORT_DATA);

            int16_t co2ppm = 0;
            float temperature = 0.0;
            float pressure = 0.0;
            getSensorData(co2ppm, temperature, pressure);
            sendSensorData(co2ppm, temperature, pressure);
      }
      else
      {
            /////////////////////////////////////////////////////////////////////////
            // set MCU into deepSleep mode
            /////////////////////////////////////////////////////////////////////////
            ledMcu.off();
            LOG_TRACE("pmu::sleepWakeupTimeout() enter");
            pmu::sleepWakeupTimeout(SENSOR_REPORT_INTERVAL);
            LOG_TRACE("pmu::sleepWakeupTimeout() exit");
            appEvent.setEventFlag(AppEvent::REPORT_DATA);
      }

      /////////////////////////////////////////////////////////////////////////
}
```
### Please refer to "github-xmc-lora-sensor.ino" for complete code
## *** source code will be available in B/Nov ***
---

### Video demo is available on  
[xmc-lora-sensor video](https://youtu.be/yTs0bI0mfuo)  
Setup Lora Receiver and Android APP during 0 - 8s.  
Power on the "Solar Power Lora Sensor" on 9s.  
The first sensors' data is received at 28s, and repeatedly receive every 5 seconds.

Example of Lora data in json format  
  {"event":"eventSensor","arg0":664,"arg1":32.25,"arg2":99268.41}  
where  
  arg0 = CO2 ppm value  
  arg1 = temperature value in degree C  
  arg2 = pressure value in Pascal



---
### Debug
Enable or disable log be modifying macro on "AppLog.h"

Debug is disabled by "#define DEBUGLOG_DISABLE_LOG"
Enable trace debug by "#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE"

Example of AppLog.h
```
// Disable Logging Macro (Release Mode)
// #define DEBUGLOG_DISABLE_LOG
// You can also set default log level by defining macro (default: INFO)
// #define DEBUGLOG_DEFAULT_LOG_LEVEL_WARN // for release version
#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE // for debug version
#include <DebugLog.h>                    // https://github.com/hideakitai/DebugLog
```
---
### Troubleshooting
If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the project will only work if you update the board core to the latest version because I am using newly added functions.

---
### Issues
Submit issues to: [XMC Lora sensor issues](https://github.com/unionpronet/github-xmc-lora-sensor/issues) 

---
### TO DO
1. Search for bug and improvement.
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this project.
---

### Contributing
If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this project
---

### License
- The library is licensed under GNU LESSER GENERAL PUBLIC LICENSE Version 3
---

### Copyright
- Copyright 2023 unionpro.net@gmail.com. All rights reserved.

