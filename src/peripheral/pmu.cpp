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
#include <Arduino.h>
#include <RTC.h>
#include <DeviceControlXMC.h>

namespace pmu
{
   static RTCClass rtc;
   static XMCClass xmc1100;

   static const byte initYear = 23;
   static const byte initMonth = 10;
   static const byte initDay = 1;
   static const byte initHours = 0;
   static const byte initMinutes = 0;
   static const byte initSeconds = 0;

   void init(void)
   {
      xmc1100.begin();
      xmc1100.configureSleepMode(SLEEP_MODE, USIC_ON, LEDT_OFF, CCU_ON, WDT_ON, FLASH_ON);

      rtc.begin();
      rtc.setDate(initDay, initMonth, initYear);
      rtc.setTime(initHours, initMinutes, initSeconds);

      rtc.setAlarmDate(initDay, initMonth, initYear);
      rtc.setAlarmTime(initHours, initMinutes, initSeconds + 1);
      rtc.enableAlarm(rtc.ALARM);
      rtc.attachInterrupt([]()
                          { xmc1100.enterActiveMode(); });
   }

   // timeout: in units of seconds
   void sleepWakeupTimeout(uint16_t timeout)
   {
      byte seconds = timeout % 60;
      timeout /= 60;
      byte minutes = timeout % 60;
      timeout /= 60;
      byte hours = timeout % 24;

      rtc.setDate(initDay, initMonth, initYear);
      rtc.setTime(initHours, initMinutes, initSeconds);

      rtc.setAlarmDate(initDay, initMonth, initYear);
      rtc.setAlarmTime(hours, minutes, seconds);
      rtc.enableAlarm(rtc.ALARM);
      xmc1100.enterSleepMode();
   }

   byte getRtcSecond(void)
   {
      return rtc.getSeconds();
   }

   // Die/Device Temperature control
   int32_t getDieTemperature(void)
   {
      return xmc1100.getTemperature();
   }

   size_t freeHeapRAM(void)
   {
      return xmc1100.freeHeapRAM();
   }

} // namespace pmu
