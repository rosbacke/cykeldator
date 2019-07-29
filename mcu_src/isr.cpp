/*
 * isr.cpp
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#include "isr.h"
#include "isr_project.h"

namespace israccess {
uint32_t g_currentCortexIsrLevel = 0;
}

void isrInit()
{}

MK_SYSIRQ_FKN(Irq_e::systick, SysTick)

MK_IRQ_FKN(Irq_e::tim2, TIM2)
MK_IRQ_FKN(Irq_e::usart1, USART1)
MK_IRQ_FKN(Irq_e::usart2, USART2)
