/************************************
    WS2812ESPFast  rgb serial led driver

    Copyright 20201 Pierre HENRY net23@frdev.com

    WS2812ESPFast is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WS2812ESPFast is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with betaEvents.  If not, see <https://www.gnu.org/licenses/lglp.txt>.


    A reset is issued as early as at 9 µs???, contrary to the 50 µs mentioned in the data sheet. Longer delays between transmissions should be avoided.
    On tested componant 50µs reset is a mandatory
    The cycle time of a bit should be at least 1.25 µs, the value given in the data sheet, and at most ~50 µs, the shortest time for a reset.
    A “0” can be encoded with a pulse as short as 62.5 ns, but should not be longer than ~500 ns (maximum on WS2812).
    A “1” can be encoded with pulses almost as long as the total cycle time, but it should not be shorter than ~625 ns (minimum on WS2812B).
    from https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/

    https://roboticsbackend.com/arduino-fast-digitalwrite/

   V1.1 (05/11/2021)
   - Adjust for RVBW


*/

#include "ESP8266.h"
#include "WS2812ESPFast.h"
// 10 µsec pulse
/*  code pour nano
// 10 µsec pulse

#define MSK_WS2812 (1 << PIN_WS2812)

inline  void WS2812_LOW() __attribute__((always_inline));
inline  void WS2812_HIGH() __attribute__((always_inline));

void WS2812_LOW() {
  PORTD &= !MSK_WS2812;
}

void WS2812_HIGH() {
  PORTD |= MSK_WS2812;
}

#define ESP8266_REG(addr) *((volatile uint32_t *)(0x60000000+(addr)))
//#define GPI    ESP8266_REG(0x318) //GPIO_IN RO (Read Input Level)
//#define GPIP(p) ((GPI & (1 << ((p) & 0xF))) != 0)
#define GPO    ESP8266_REG(0x300) //GPIO_OUT R/W (Output Level)
#define GPOS   ESP8266_REG(0x304) //GPIO_OUT_SET WO
#define GPOC   ESP8266_REG(0x308) //GPIO_OUT_CLR WO
#define GPOP(p) ((GPO & (1 << ((p) & 0xF))) != 0)
*/
//#include "esp8266_peri.h"
//Code pour ESP
#define MSK_WS2812 (1 << (WS2812_PIN))  //ESP8266   N° pin 0 a 15 uniquement

inline void WS2812_LOW() {
  GPOC = MSK_WS2812;
}

inline void WS2812_HIGH() {
  GPOS = MSK_WS2812;
}


void WS2812rvbf_t::reset() {
  interrupts();
  WS2812_LOW();
  //delayMicroseconds(10);
}

//   Arduino Nano
//   Cycle = 1,7µs
//    0 = 0,3µs + 1,4µs
//    1 = 0,95µs + 0,75µs

//timing for nano
void WS2812rvbf_t::shift(uint8_t shift) {
  static volatile uint16_t delay1 = 0;


  
  //  A “0” can be encoded with a pulse as short as 62.5 ns, but should not be longer than ~500 ns (maximum on WS2812).
  //  A “1” can be encoded with pulses almost as long as the total cycle time, but it should not be shorter than ~625 ns (minimum on WS2812B).

  for (byte n = 8; n > 0; n--, shift = shift << 1) {
    if (shift & 0x80) {
      WS2812_HIGH();  //0,3µs
      delay1++;       //0,65µs
      delay1++;
      delay1++;
      delay1++;
      delay1++;
      WS2812_LOW();
    } else {
      WS2812_HIGH();  //0,3µs
      delay1++;
      WS2812_LOW();
      delay1++;
      delay1++;
      delay1++;
      delay1++;
    }
        delay1++;
        delay1++;
    /*
    delay1++;
    delay1++;
    delay1++;
    delay1++;
    */
  }
  
}






void WS2812rvbf_t::write() {
  noInterrupts();
  /*

  shift(0b01010101);
  shift(0b11111111);
  shift(0b00000000);
    */
  shift(map_color[color].green);    
  shift(map_color[color].red);  
  shift(map_color[color].blue);   

  //Serial.println(map_color[color].red);
}
