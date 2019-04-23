/*
 * Lcd.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_LCD_H_
#define MCU_SRC_LCD_H_

#include <U8g2lib.h>

class OdoTimer;

class Lcd : U8G2
{
  public:
    Lcd(OdoTimer* timer)
    {
        m_timer = timer;
    }

    void setup();


    void write(int x);

  private:
    static OdoTimer* m_timer;
    static uint8_t u8g2_gpio_and_delay_stm32_2(U8X8_UNUSED u8x8_t* u8x8,
                                               U8X8_UNUSED uint8_t msg,
                                               U8X8_UNUSED uint8_t arg_int,
                                               U8X8_UNUSED void* arg_ptr);
};

#endif /* MCU_SRC_LCD_H_ */
