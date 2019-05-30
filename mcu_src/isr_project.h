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
    maxNo
};

template <>
constexpr IRQn_Type irq2cmsis(IrqSource enumVal)
{
#define CASE(x, y) case IrqSource::x: return y
	switch(enumVal) {
	CASE(systick, SysTick_IRQn);
	CASE(tim2, TIM2_IRQn);
	CASE(usart1, USART1_IRQn);
	CASE(usart2, USART2_IRQn);
	}
#undef CASE
}

// Set up interrupt priorities for this particular application.

// Param 1 : Type of enum of the interrupt source.
// Param 2 : Enum value of the interrupt source to define.
// Param 3 : integral isr priority. 0-thread, higher value interrupts.
// A higher numerical value have higher precedence.
using IrqSource_Tim2 = InterruptSource<IrqSource, IrqSource::tim2, 3>;
using IrqSource_Usart1 = InterruptSource<IrqSource, IrqSource::usart1, 2>;
using IrqSource_SysTick = InterruptSource<IrqSource, IrqSource::systick, 1>;

using IrqSource_Thread = InterruptSource<IrqSource, IrqSource::thread, 0>;

#endif /* MCU_SRC_ISR_PROJECT_H_ */
