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

MK_SYSIRQ_FKN(IrqSource::systick, SysTick)

MK_IRQ_FKN(IrqSource::tim2, TIM2)
MK_IRQ_FKN(IrqSource::usart1, USART1)
MK_IRQ_FKN(IrqSource::usart2, USART2)
