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
#pragma once

#define RADIOLIB_GODMODE
#define RADIOLIB_STATIC_ONLY
#include <RadioLib.h>

#include "../../../pins.h"
#include "../../../AppLog.h"

typedef struct _RfConfig
{
    float freq;
    float bw;
    uint8_t sf;
    uint8_t cr;
    uint8_t syncWord;
    int8_t power;
    uint16_t preambleLength;
    float tcxoVoltage;
    bool useLDO;
} RfConfig;

#define LORA_SPI SPI

class RfLora : public LLCC68
{
public:
    RfLora() : module(PIN_RF_NSS, PIN_RF_DIO1, PIN_RF_RST, PIN_RF_BUSY, LORA_SPI),
               LLCC68(&module)
    {
    }

    bool init(const RfConfig &rf, void (*loraIsr)(void) = nullptr)
    {
        LOG_TRACE("loraIsr=(hex)", (uint32_t)loraIsr);

        LORA_SPI.begin();

        int16_t result = reset();
        if (result != RADIOLIB_ERR_NONE)
        {
            LOG_ERROR("reset() returns ", result);
            return false;
        }

        LOG_TRACE("rfConfig.freq=", rf.freq, ", bw=", rf.bw, ", sf=", rf.sf, ", cr=", rf.cr,
                  ", syncWord=", rf.syncWord, ", power=", rf.power, ", preambleLength=", rf.preambleLength,
                  ", tcxoVoltage=", rf.tcxoVoltage, ", useLDO=", rf.useLDO);
        result = begin(rf.freq, rf.bw, rf.sf, rf.cr,
                       rf.syncWord, rf.power, rf.preambleLength,
                       rf.tcxoVoltage, rf.useLDO);
        if (result != RADIOLIB_ERR_NONE)
        {
            LOG_ERROR("begin() returns ", result);
            return false;
        }
        switch (result)
        {
        case RADIOLIB_ERR_NONE:
            // success => move forward
            break;
        case RADIOLIB_ERR_UNKNOWN:
            LOG_ERROR("RADIOLIB_ERR_UNKNOWN (", RADIOLIB_ERR_UNKNOWN, ")");
            return false;
        case RADIOLIB_ERR_CHIP_NOT_FOUND:
            LOG_ERROR("RADIOLIB_ERR_CHIP_NOT_FOUND (", RADIOLIB_ERR_CHIP_NOT_FOUND, ")");
            return false;
        case RADIOLIB_ERR_INVALID_BANDWIDTH:
            LOG_ERROR("RADIOLIB_ERR_INVALID_BANDWIDTH (", RADIOLIB_ERR_INVALID_BANDWIDTH, ")");
            return false;
        case RADIOLIB_ERR_INVALID_SPREADING_FACTOR:
            LOG_ERROR("RADIOLIB_ERR_INVALID_SPREADING_FACTOR (", RADIOLIB_ERR_INVALID_SPREADING_FACTOR, ")");
            return false;
        case RADIOLIB_ERR_INVALID_CODING_RATE:
            LOG_ERROR("RADIOLIB_ERR_INVALID_CODING_RATE (", RADIOLIB_ERR_INVALID_CODING_RATE, ")");
            return false;
        case RADIOLIB_ERR_INVALID_BIT_RANGE:
            LOG_ERROR("RADIOLIB_ERR_INVALID_BIT_RANGE (", RADIOLIB_ERR_INVALID_BIT_RANGE, ")");
            return false;
        case RADIOLIB_ERR_INVALID_FREQUENCY:
            LOG_ERROR("RADIOLIB_ERR_INVALID_FREQUENCY (", RADIOLIB_ERR_INVALID_FREQUENCY, ")");
            return false;
        case RADIOLIB_ERR_INVALID_OUTPUT_POWER:
            LOG_ERROR("RADIOLIB_ERR_INVALID_OUTPUT_POWER (", RADIOLIB_ERR_INVALID_OUTPUT_POWER, ")");
            return false;
        default:
            LOG_ERROR("unsupported begin() return: ", result);
            return false;
        }

        result = setDio2AsRfSwitch(true);
        if (result != RADIOLIB_ERR_NONE)
        {
            LOG_ERROR("setDio2AsRfSwitch() returns ", result);
            return false;
        }

        // set the function that will be called when new packet is received
        if (loraIsr)
        {
            setDio1Action(loraIsr);
        }

        result = standby();
        if (result != RADIOLIB_ERR_NONE)
        {
            LOG_ERROR("sleep()/standby() returns ", result);
            return false;
        }

        return true;
    }

    virtual int16_t startTransmit(uint8_t *data, size_t len, uint8_t addr = (uint8_t)0U)
    {
        LORA_SPI.begin();
        return LLCC68::startTransmit(data, len, addr);
    }

private:
    Module module;
};