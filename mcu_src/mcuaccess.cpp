/*
 * mcuaccess.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#include "mcuaccess.h"

namespace hwports
{

MCU_ACCESS_CORTEX_DEF(SCB, SCB, scb);
MCU_ACCESS_CORTEX_DEF(NVIC, NVIC, nvic);
MCU_ACCESS_CORTEX_DEF(SysTick, SysTick, systick);

MCU_ACCESS_PORT_DEF(TIM, TIM2, tim2);
MCU_ACCESS_PORT_DEF(GPIO, GPIOA, gpioa);
MCU_ACCESS_PORT_DEF(GPIO, GPIOB, gpiob);
MCU_ACCESS_PORT_DEF(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT_DEF(RCC, RCC, rcc);
MCU_ACCESS_PORT_DEF(I2C, I2C1, i2c1);
MCU_ACCESS_PORT_DEF(USART, USART1, usart1);

} // namespace hwports
