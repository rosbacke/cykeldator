/*
 * timer.h
 *
 *  Created on: Sep 9, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_TIMER_H_
#define STM32_SRC_TIMER_H_

#include <stdint.h>

struct TickPoint {
	TickPoint(uint32_t count, uint32_t fe, uint32_t re) :
			m_count(count), m_failingEdge(fe), m_raisingEdge(re) {
	}

	uint32_t m_count;
	uint32_t m_failingEdge;
	uint32_t m_raisingEdge;
};

using TimerCB = void (*)(const TickPoint& tp);

void setCallback(TimerCB cb);

void setupTimer();

uint32_t timer_counterU32();

uint16_t timer_sysTickDelta();

uint32_t timer_lastNegTP();

uint32_t timer_lastPosTP();

void delay(int delay);


#endif /* STM32_SRC_TIMER_H_ */
