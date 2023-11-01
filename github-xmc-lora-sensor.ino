/* Copyright 2023 unionpro.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <Wire.h>
#include <MemStream.h>

#include "./src/peripheral/pmu.h"
#include "./src/peripheral/sensor/pressure/pressure.h"
#include "./src/peripheral/sensor/co2/co2.h"
#include "./src/peripheral/led/LedMcu.h"
#include "./src/peripheral/led/LedPressureSensor.h"
#include "./src/peripheral/led/LedCo2Sensor.h"
#include "./src/peripheral/led/LedLora.h"
#include "./src/peripheral/lora/RfLora.h"
#include "./src/JsonMessage.h"

#include "./AppEvent.h"
#include "./AppLog.h"
#include "./pins.h"

///////////////////////////////////////////////////////////////////////////////
#define SENSOR_REPORT_INTERVAL 5 // in units of seconds
#define MessageJsonDocSize 512

///////////////////////////////////////////////////////////////////////////////
#define I2C_FREQ_HZ 400000

static const RfConfig rfConfig = {
    915.0, // freq
    125.0, // bw
    9,     // sf
    7,     // cr
    18,    // syncWord (RADIOLIB_SX126X_SYNC_WORD_PRIVATE = 0x12)
    0,     // power
    8,     // preambleLength
    0.0,   // tcxoVoltage
    false  // useLDO
};

///////////////////////////////////////////////////////////////////////////////
static LedMcu ledMcu;
static LedPressureSensor ledPressure;
static LedCo2Sensor ledCo2;
static LedLora ledLora;

static RfLora rfLora;
static AppEvent appEvent;

///////////////////////////////////////////////////////////////////////////////
static void loraIsr(void)
{
    appEvent.setEventFlag(AppEvent::LORA_ISR);
}

void handlerLoraDio1(void)
{
    uint16_t irqStatus = rfLora.getIrqStatus();
    rfLora.clearIrqStatus(RADIOLIB_SX126X_IRQ_ALL);

    if (irqStatus & RADIOLIB_SX126X_IRQ_TX_DONE)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_TX_DONE (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_TX_DONE, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_RX_DONE)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_RX_DONE (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_RX_DONE, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_RADIOLIB_PREAMBLE_DETECTED)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_RADIOLIB_PREAMBLE_DETECTED (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_RADIOLIB_PREAMBLE_DETECTED, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_SYNC_WORD_VALID)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_SYNC_WORD_VALID (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_SYNC_WORD_VALID, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_HEADER_VALID)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_HEADER_VALID (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_HEADER_VALID, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_HEADER_ERR)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_HEADER_ERR (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_HEADER_ERR), ")";
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_CRC_ERR)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_CRC_ERR (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_CRC_ERR, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_CAD_DONE)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_CAD_DONE (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_CAD_DONE, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_CAD_DETECTED)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_CAD_DETECTED (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_CAD_DETECTED, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_TIMEOUT)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_TIMEOUT (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_TIMEOUT, ")");
    }
    if (irqStatus & RADIOLIB_SX126X_IRQ_LR_FHSS_HOP)
    {
        LOG_TRACE("RADIOLIB_SX126X_IRQ_LR_FHSS_HOP (0x", DebugLogBase::HEX, RADIOLIB_SX126X_IRQ_LR_FHSS_HOP, ")");
    }

    rfLora.standby();
}

///////////////////////////////////////////////////////////////////////////////
static void initUnusedPins(void)
{
    pinMode(UNUSED_P0_2, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P0_5, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P0_10, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P0_11, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P1_4, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P1_5, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P2_10, XMC_GPIO_MODE_INPUT_PULL_UP);
    pinMode(UNUSED_P2_11, XMC_GPIO_MODE_INPUT_PULL_UP);
}

static void initPeripheral(void)
{
    /////////////////////////////////////////////////////////////////////////////
    // init I2C
    Wire.begin();
    Wire.setClock(I2C_FREQ_HZ);

    /////////////////////////////////////////////////////////////////////////////
    // init pressure sensor (test pass)
    bool bInitPressureSensor = sensor::pressure::init(Wire);
    uint8_t productID = sensor::pressure::getProductId();
    uint8_t revID = sensor::pressure::getRevisionId();
    LOG_TRACE("sensor::pressure::init() returns ", bInitPressureSensor, ", getProductId()=", productID, ", getRevisionId()=", revID);

    /////////////////////////////////////////////////////////////////////////////
    // init CO2 sensor (test pass)
    bool bInitCo2Sensor = sensor::co2::init();
    LOG_TRACE("sensor::co2::init() returns ", bInitCo2Sensor);

    uint8_t prodId, revId;
    bool rstGetDevID = sensor::co2::getDeviceID(prodId, revId);
    LOG_TRACE("sensor::co2::getDeviceID() returns ", rstGetDevID, ", product id=", prodId, ", revision id=", revId);

    /////////////////////////////////////////////////////////////////////////////
    // init Lora module (test pass)
    bool initLora = rfLora.init(rfConfig, &loraIsr);
    LOG_TRACE("rfLora.init() returns ", initLora);
}

static bool getTemperature(float &temperature)
{
    // poll temperature value from pressure sensor
    float value;
    bool rst = sensor::pressure::getTemperature(value);
    if (rst)
    {
        temperature = value;
    }
    return rst;
}

static bool getPressure(float &pressure)
{
    // poll pressure value
    float value;
    bool rst = sensor::pressure::getPressure(value);
    if (rst)
    {
        pressure = value;
    }
    return rst;
}

static bool getCo2(int16_t &co2ppm)
{
    // poll co2 value
    bool rst = sensor::co2::startMeasure();
    if (rst)
    {
        int16_t co2 = 0;
        do
        {
            rst = sensor::co2::getCO2(co2);
        } while (!rst && co2 == 0);
        co2ppm = co2;
        LOG_TRACE("co2=", co2ppm, " (ppm)");
        return true;
    }
    else
    {
        LOG_TRACE("sensor::co2::startMeasure() returns false");
    }
    return false;
}

static void getSensorData(int16_t &co2ppm, float &temperature, float &pressure)
{
    bool rst = false;

    Wire.begin();

    /////////////////////////////////////////////////////////////////////////
    // get pressure and temperature values from pressure sensor
    /////////////////////////////////////////////////////////////////////////
    ledPressure.on();
    rst = getTemperature(temperature);
    LOG_TRACE("getTemperature() returns ", rst, ", sensor::pressure:temperature=", temperature);

    rst = getPressure(pressure);
    LOG_TRACE("getPressure() returns ", rst, ", sensor::pressure:pressure=", pressure);
    ledPressure.off();

    /////////////////////////////////////////////////////////////////////////
    // get CO2 value (test pass)
    /////////////////////////////////////////////////////////////////////////
    ledCo2.on();
    rst = getCo2(co2ppm);
    LOG_TRACE("getCo2() returns ", rst, ", sensor::co2=", co2ppm, " (ppm)");
    ledCo2.off();
    /////////////////////////////////////////////////////////////////////////
}

static const char *toJsonString(int16_t co2ppm, float temperature, float pressure)
{
    static uint8_t memBuffer[MessageJsonDocSize];
    static MemStream memStream(memBuffer, sizeof(memBuffer));
    static JsonMessage<MessageJsonDocSize> jsonMessage(&memStream);

    memStream.flush();
    jsonMessage.clear();

    int16_t co2 = co2ppm;
    uint16_t temperatureValue = (uint16_t)roundf(temperature * 100.0);
    uint32_t pressureValue = (uint32_t)roundf(pressure * 100);
    jsonMessage["event"] = "eventSensor";
    jsonMessage["arg0"] = co2;
    jsonMessage["arg1"] = temperatureValue;
    jsonMessage["arg2"] = pressureValue;

    jsonMessage.serialize();

    return (const char *)memStream;
}

static void sendSensorData(int16_t co2ppm, float temperature, float pressure)
{
    /////////////////////////////////////////////////////////////////////////
    // prepare json string from sensor's values
    // send json string via lora
    /////////////////////////////////////////////////////////////////////////
    const char *jsonString = toJsonString(co2ppm, temperature, pressure);
    if (jsonString)
    {
        LOG_TRACE("jsonString=", jsonString);

        ledLora.on();
        int16_t rst = rfLora.startTransmit((uint8_t *)jsonString, strlen(jsonString));
        LOG_TRACE("rfLora.startTransmit() returns ", rst);
        delay(100);
        ledLora.off();
    }
    else
    {
        LOG_TRACE("toJsonString() returns null");
    }
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

    delay(5000);

    initUnusedPins();
    initPeripheral();
    pmu::init();
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
        // delay(5000);
    }
}
