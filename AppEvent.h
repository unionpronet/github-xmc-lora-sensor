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
#include <Arduino.h>

class AppEvent
{
public:
    static const uint16_t REPORT_DATA = 0x0001;
    static const uint16_t LORA_ISR = 0x0010;

    AppEvent() : _eventFlags(0)
    {
    }

    void setEventFlag(uint16_t flag)
    {
        __disable_irq();
        _eventFlags |= flag;
        __enable_irq();
    }
    void clearEventFlag(uint16_t flag)
    {
        __disable_irq();
        _eventFlags &= ~flag;
        __enable_irq();
    }
    uint16_t getEventFlag(void)
    {
        __disable_irq();
        uint16_t flags = _eventFlags;
        __enable_irq();
        return flags;
    }

private:
    uint16_t _eventFlags;
};
