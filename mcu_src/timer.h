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

	// Return current value of the running systick counter. (ms)
	uint32_t sysTick() { return m_sysTick; }

	using SleepCB = void (*)(void);

	// Delay for a number of ms.
	// Can take a template parameter with a callback to be repeatedly
	// called during the sleep.
	template<SleepCB = nullptr>
	void delay(int ms);

	// Called when a timer pulse can be fetched with getTP.
	delegate<void()> pulseCB;

	// Call with interrupts disabled.
	TickPoint getTP() const
	{
		return m_tp;
	}
private:
	void setupTimer();

    void inline sysTickIsr()
    {
    	m_sysTick++;
    }

    void tim2Isr();

	TIM_TypeDef* m_dev = nullptr;
	std::atomic<uint32_t> m_sysTick{0};
	// volatile uint32_t m_sysTick{0};

	std::atomic<uint16_t> m_cntMsb{0};
	std::atomic<uint32_t> m_cntMsb2{0};
	std::atomic<uint32_t> m_count{0};

	std::atomic<uint32_t> m_posEdgeTS{0};
	std::atomic<uint32_t> m_negEdgeTS{0};

	TickPoint m_tp;
};

template<OdoTimer::SleepCB cb>
void OdoTimer::delay( int ms )
{
    uint32_t base = m_sysTick;
    auto done = [&](uint32_t cnt) { return int32_t(cnt - base) >= ms; } ;
    do {
    	if (cb)
    		cb();
    } while(!done(m_sysTick));

}

using TimerCB = void ( * )( const TickPoint& tp, void* ctx );

uint16_t timer_sysTickDelta();

uint32_t timer_lastNegTP();

uint32_t timer_lastPosTP();


#endif /* STM32_SRC_TIMER_H_ */
