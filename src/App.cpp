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
    : m_ts(hwports::systick.addr()), m_timer(hwports::tim2.addr(), &m_ts),
      m_usart1(hwports::usart1.addr()), lcd(&m_ts)
{
    setup();
}

void App::writeRawUsart(const TickPoint& tp)
{
	static char buffer[200];
    char* p = buffer;
    p = uint2str(p, tp.m_count);
    *p++ = ' ';
    p = uint2str(p, tp.m_failingEdge);
    *p++ = ' ';
    p = uint2str(p, tp.m_raisingEdge);
    *p++ = ' ';
    p = uint2str(p, m_ts.systick());
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';
    m_usart1.blockwrite(buffer);
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
	bool rawUsart = true;
	uint32_t lastTick = 0;
	uint32_t lastLed = 0;
	uint32_t lastDisplayUpdate = 0;
    TickPoint tp;

    while (1)
    {
    	uint32_t tick = m_ts.systick();
    	bool gotTP = m_timer.getTP(tp);

    	while (tick != lastTick)
    	{
        	m_calc.addSysTick(lastTick++);
    	}
        if (gotTP)
        {
        	if (rawUsart)
        	{
        		writeRawUsart(tp);
        	}
        	m_calc.addTickPoint(tp);
            lcd.write(tp.m_count);
        }
        if (int32_t(lastTick - lastLed) >= 500)
        {
        	setLed(true);
        }
        if (int32_t(lastTick - lastLed) >= 1000)
        {
        	setLed(false);
        	lastLed = lastTick;
        }
        m_ts.delay<__WFI>(0);
    }
}
