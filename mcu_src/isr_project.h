/*
 * isr_project.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_PROJECT_H_
#define MCU_SRC_ISR_PROJECT_H_

#include "isr.h"

// Set up interrupt priorities for this particular application.
// Param 1 : Enum value of the interrupt source to define.
// Param 2 : integral isr priority. 0-thread, higher value interupts lower values.

using IrqSource_Tim2 = InterruptSource<IrqHandlers::tim2, 2>;
using IrqSource_Usart1 = InterruptSource<IrqHandlers::usart1, 1>;



#endif /* MCU_SRC_ISR_PROJECT_H_ */
