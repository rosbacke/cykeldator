/*
 * timer.h
 *
 *  Created on: Sep 9, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_TIMER_H_
#define STM32_SRC_TIMER_H_

#include <mcuaccess.h>

#include "TickPoint.h"
#include <stdint.h>
#include <atomic>

#include <delegate/delegate.hpp>

class OdoTimer
{
public:
	OdoTimer(TIM_TypeDef* device);
	uint32_t sysTick() { return m_sysTick; }

	void delay(int ms);

private:
    void sysTickIsr()
    {
    	m_sysTick++;
    }

	TIM_TypeDef* m_dev = nullptr;
	std::atomic<uint32_t> m_sysTick{0};
};

class IsrHandlers
{
public:
	using Del = delegate<void()>;
	enum class Handler
	{
		systick,
		tim2,
		maxNo
	};

	static IsrHandlers instance;

	static Del& del( Handler h)
    {
		return instance.m_isrCB[ static_cast<int>( h ) ];
    }

	static void setIsr( Handler h, Del del )
    {
        instance.m_isrCB[ static_cast<int>( h ) ]();
    }

    static void callIsr( Handler h ) { instance.m_isrCB[ static_cast<int>( h ) ](); }

  private:
	Del m_isrCB[ static_cast<int>(Handler::maxNo) ];
};

using TimerCB = void ( * )( const TickPoint& tp, void* ctx );

void setTimerCallback( TimerCB cb, void* ctx );

void setupTimer();

uint32_t timer_counterU32();

uint16_t timer_sysTickDelta();

uint32_t timer_lastNegTP();

uint32_t timer_lastPosTP();

void delay( int delay );

uint32_t timerSysTick();


#endif /* STM32_SRC_TIMER_H_ */
