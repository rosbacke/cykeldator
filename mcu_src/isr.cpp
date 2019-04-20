/*
 * isr.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#include "isr.h"


IsrHandlers IsrHandlers::instance;

// Set up all interrupt functions.
#define MK_IRQ_FKN(handler, FknName) \
extern "C" void FknName##_IRQHandler(void) \
{ IsrHandlers::callIsr(IrqHandlers::handler); }

#define MK_SYSIRQ_FKN(handler, FknName) \
extern "C" void FknName##_Handler(void) \
{ IsrHandlers::callIsr(IrqHandlers::handler); }


MK_SYSIRQ_FKN(systick, SysTick)

MK_IRQ_FKN(tim2, TIM2)
