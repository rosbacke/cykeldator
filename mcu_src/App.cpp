/*
 * App.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "App.h"

#include "isr_project.h"
#include "mcuaccess.h"

App::App()
    : m_timer(hwports::tim2.addr()), m_usart1(hwports::usart1.addr()),
      lcd(&m_timer)
{
    setup();
}

void App::write()
{
    static char buffer[200];

    TickPoint tp;
    {
        if (!m_newVal)
        {
            return;
        }
        tp = m_timer.getTP();
        m_newVal = false;
    }
    char* p = buffer;
    p = uint2str(p, tp.m_count);
    *p++ = ' ';
    p = uint2str(p, tp.m_failingEdge);
    *p++ = ' ';
    p = uint2str(p, tp.m_raisingEdge);
    *p++ = ' ';
    p = uint2str(p, m_timer.sysTick());
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';
    m_usart1.blockwrite(buffer);

    lcd.write(tp.m_count);
}

void App::setLed(bool on)
{
    using hwports::gpioc;
    if (on)
        gpioc->ODR &= ~uint32_t(1 << 13);
    else
        gpioc->ODR |= 1 << 13;
}

void App::setup()
{
    using hwports::gpioc;
    hwports::rcc->APB2ENR |= RCC_APB2ENR_IOPCEN;
    gpioc->CRH |= GPIO_CRH_MODE13_1;
    gpioc->CRH |= 0;
    Usart::setupUsart1(m_usart1);
    m_timer.pulseCB.set<App, &App::timerCB>(*this);
    lcd.setup();
}

void App::run()
{
    while (1)
    {
        write();
        setLed((m_timer.sysTick() & 0x200) != 0);
        m_timer.delay<__WFI>(0);
    }
}
