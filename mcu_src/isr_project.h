/*
 * isr_project.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_PROJECT_H_
#define MCU_SRC_ISR_PROJECT_H_

#include "isr.h"

enum class Irq_e
{
    systick,
    tim2,
    usart1,
    usart2,
    thread, // For use in 'cover'
    maxNo   // size of the interrupt array.
};

#define CASE(x,y,z) case Irq_e::x: return IrqData<Irq_e>(irq, y, z)

template <>
constexpr IrqData<Irq_e> irq2Data<Irq_e>(Irq_e irq)
{
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
	return IrqData<Irq_e>(Irq_e::maxNo, NonMaskableInt_IRQn, 0);
}

#undef CASE

// Specialize on out irq enum. To be used for drivers to setup and
// control the interrupt
template<Irq_e irqSrc>
using IsrSource = InterruptSource<Irq_e, irqSrc>;

// Specialize on out irq enum. To be used for drivers to setup and
// control the interrupt
using IsrHandlers = IsrHandlerManager<Irq_e>;


#endif /* MCU_SRC_ISR_PROJECT_H_ */
