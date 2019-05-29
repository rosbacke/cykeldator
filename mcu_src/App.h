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


class App
{
  public:
    App();

    void setup();
    void write();
    void run();
    void setLed(bool on);

  private:
    void timerCB()
    {
        m_newVal = true;
    }

    std::atomic<bool> m_newVal{false};
    TimeSource m_ts;
    OdoTimer m_timer;
    Usart m_usart1;
    Lcd lcd;
};

#endif /* MCU_SRC_APP_H_ */
