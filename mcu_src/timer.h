/*
 * timer.h
 *
 *  Created on: Sep 9, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_TIMER_H_
#define STM32_SRC_TIMER_H_

#include "TickPoint.h"
#include <stdint.h>

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
