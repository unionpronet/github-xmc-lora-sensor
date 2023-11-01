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
#include <pas-co2-ino.hpp>
#include "../../led/LedCo2Sensor.h"

#include "../../../../pins.h"
#include "../../../../AppLog.h"

#include "co2.h"

namespace sensor
{
    namespace co2
    {
        static PASCO2Ino co2(&Wire, PIN_CO2_SENSOR_INT);

        bool init(void)
        {
            return (co2.begin() == XENSIV_PASCO2_OK);
        }

        bool getDeviceID(uint8_t &prodID, uint8_t &revID)
        {
            return (co2.getDeviceID(prodID, revID) == XENSIV_PASCO2_OK);
        }

        bool startMeasure(int16_t periodInSec, int16_t alarmTh, void (*cback)(void *), bool earlyNotification)
        {
            return (co2.startMeasure(periodInSec, alarmTh, cback, earlyNotification) == XENSIV_PASCO2_OK);
        }

        bool getCO2(int16_t &ppm)
        {
            return (co2.getCO2(ppm) == XENSIV_PASCO2_OK);
        }

        uint8_t getIntrPin(void)
        {
            return PIN_CO2_SENSOR_INT;
        }
    }
}