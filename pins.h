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

// external interrupt pins of XMC1302
#define EXT0 14 // 14/P0.12
#define EXT1 15 // 15/P0.13

// unused pins
#define UNUSED_P0_2 26
#define UNUSED_P0_5 32
#define UNUSED_P0_10 16
#define UNUSED_P0_11 17
#define UNUSED_P1_4 19
#define UNUSED_P1_5 18
#define UNUSED_P2_10 6
#define UNUSED_P2_11 7

#define PIN_CO2_SENSOR_INT EXT1 // pressure sensor interrupt pin

#define PIN_I2C_SCL 10 // 10/P2.0
#define PIN_I2C_SDA 11 // 11/P2.1

#define PIN_RF_MOSI 22   // 22/P1.1
#define PIN_RF_MISO 23   // 23/P1.0
#define PIN_RF_NSS 29    // 29/P0.6
#define PIN_RF_SCK 30    // 30/P0.7
#define PIN_RF_BUSY 31   // 31/P0.4/rfBUSY
#define PIN_RF_DIO1 EXT0 // 14/P0.12/rfDIO1
#define PIN_RF_RST 33    // 33/P0.3

//  XMC103 Boot Kit on-board peripherals
#define PIN_POTENTIOMETER 1 // P2.5
