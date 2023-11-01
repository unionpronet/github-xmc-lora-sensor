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
#include <Dps3xx.h>

#include "pressure.h"
#include "../../../../pins.h"
#include "../../../../AppLog.h"

namespace sensor
{
    namespace pressure
    {
        static Dps3xx Dps3xxPressureSensor = Dps3xx();

        bool init(TwoWire &wire)
        {
            return init(wire, (uint8_t)DPS__STD_SLAVE_ADDRESS);
        }

        bool init(TwoWire &wire, uint8_t slaveAddress)
        {
            // Call begin to initialize Dps3xxPressureSensor
            // The parameter 0x76 is the bus address. The default address is 0x77 and does not need to be given.
            // Dps3xxPressureSensor.begin(Wire, 0x76);
            // Use the commented line below instead to use the default I2C address.
            Dps3xxPressureSensor.begin(wire, slaveAddress);

            return true;
        }

        // return
        //  true: temperature
        //  false: error
        bool getTemperature(float &temperature, uint8_t oversamplingRate)
        {
            return (Dps3xxPressureSensor.measureTempOnce(temperature, oversamplingRate) == DPS__SUCCEEDED);
        }

        // return
        //  true: pressure
        //  false: error
        bool getPressure(float &pressure, uint8_t oversamplingRate)
        {
            return (Dps3xxPressureSensor.measurePressureOnce(pressure, oversamplingRate) == DPS__SUCCEEDED);
        }

        // returns the Product ID of the connected Dps3xx sensor
        uint8_t getProductId(void)
        {
            return Dps3xxPressureSensor.getProductId();
        }

        // returns the Revision ID of the connected Dps3xx sensor
        uint8_t getRevisionId(void)
        {
            return Dps3xxPressureSensor.getRevisionId();
        }

    }

} // namespace sensor
