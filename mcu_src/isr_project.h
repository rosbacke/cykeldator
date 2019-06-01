/*
 * isr_project.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_PROJECT_H_
#define MCU_SRC_ISR_PROJECT_H_

#include "isr.h"

enum class IrqSource
{
    systick,
    tim2,
    usart1,
    usart2,
    thread, // For use in 'cover'
    maxNo   // size of the interrupt array.
};

template <>
constexpr IrqData<IrqSource> irq2Data<IrqSource>(IrqSource irq) {
#define CASE(x,y,z) case IrqSource::x: return IrqData<IrqSource>(irq, y, z)
	switch(irq) {

	// 1: Enum value for interrupt.
	// 2: STM32 identifier for interrupt.
	// 3: Interrupt level. 1, lowest, 15 highest.
	CASE(systick, SysTick_IRQn, 1);
	CASE(tim2,    TIM2_IRQn, 3);
	CASE(usart1,  USART1_IRQn, 2);
	CASE(usart2,  USART2_IRQn, 2);
	CASE(thread,  NonMaskableInt_IRQn, 0);
	}
#undef CASE
	return IrqData<IrqSource>(IrqSource::maxNo, NonMaskableInt_IRQn, 0);
}

// Specialize on out irq enum. To be used for drivers to setup and
// control the interrupt
template<IrqSource irqSrc>
using IsrManager = InterruptSource<IrqSource, irqSrc>;

#endif /* MCU_SRC_ISR_PROJECT_H_ */
