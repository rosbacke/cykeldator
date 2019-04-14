/*
 * mcuaccess.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#include "mcuaccess.h"

namespace hwports
{
MCU_ACCESS_PORT_DEF(TIM, TIM2, tim2);
MCU_ACCESS_PORT_DEF(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT_DEF(GPIO, GPIOA, gpioa);
MCU_ACCESS_PORT_DEF(RCC, RCC, rcc);

#ifdef UNIT_TEST

SysTick_Type systickFake;
HwPort<SysTick_Type, 0, &systickFake> systick;

#else

HwPort<SysTick_Type, SysTick_BASE, nullptr> systick;

#endif


}
