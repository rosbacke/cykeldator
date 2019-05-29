/*
 * Lcd.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_LCD_H_
#define MCU_SRC_LCD_H_


class TimeSource;

class Lcd
{
  public:
    Lcd(TimeSource* ts)
    {
        m_ts = ts;
    }

    virtual void setup() {};
    virtual void write(int x) {};
    virtual ~Lcd();

  protected:
    static TimeSource* m_ts;
};

#endif /* MCU_SRC_LCD_H_ */
