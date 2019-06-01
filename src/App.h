/*
 * App.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_APP_H_
#define MCU_SRC_APP_H_

#include "timer.h"

#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <atomic>

#include "Strings.h"
#include "Lcd.h"
#include "TimeSource.h"
#include "SignalChain.h"

class TickPoint;

class App
{
  public:
    App();

    void setup();
    void write();
    void run();
    void setLed(bool on);

    void writeRawUsart(const TickPoint& tp);

  private:
    void timerCB()
    {
    }

    TimeSource m_ts;
    OdoTimer m_timer;
    Usart m_usart1;
    Lcd lcd;
    SignalChain m_calc;
};

#endif /* MCU_SRC_APP_H_ */
