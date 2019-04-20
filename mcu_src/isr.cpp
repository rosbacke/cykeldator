/*
 * isr.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#include "isr.h"


IsrHandlers IsrHandlers::instance;

int basePri = 0;

MK_SYSIRQ_FKN(systick, SysTick)

MK_IRQ_FKN(tim2, TIM2)
MK_IRQ_FKN(usart1, USART1)
MK_IRQ_FKN(usart2, USART2)
