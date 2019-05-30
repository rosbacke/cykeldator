/*
 * mcuaccess.h
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_MCUACCESS_H_
#define MCU_SRC_MCUACCESS_H_

#include "HwPort.h"
#include <assert.h>

#if defined(__linux__)

#define MCU_ACCESS_PORT(drv, device, objName) \
    extern drv##_TypeDef objName##Fake;       \
    extern hwports::HwPort<drv##_TypeDef, 0, &objName##Fake> objName;

#define MCU_ACCESS_PORT_DEF(drv, device, objName) \
    drv##_TypeDef objName##Fake;                  \
    hwports::HwPort<drv##_TypeDef, 0, &objName##Fake> objName;

#define MCU_ACCESS_CORTEX(drv, device, objName) \
    extern drv##_Type objName##Fake;            \
    extern hwports::HwPort<drv##_Type, 0, &objName##Fake> objName;

#define MCU_ACCESS_CORTEX_DEF(drv, device, objName) \
    drv##_Type objName##Fake;                       \
    hwports::HwPort<drv##_Type, 0, &objName##Fake> objName;

#else

#define MCU_ACCESS_PORT(drv, device, objName) \
    extern hwports::HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

#define MCU_ACCESS_PORT_DEF(drv, device, objName) \
    hwports::HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

#define MCU_ACCESS_CORTEX(drv, device, objName) \
    extern hwports::HwPort<drv##_Type, device##_BASE, nullptr> objName;

#define MCU_ACCESS_CORTEX_DEF(drv, device, objName) \
    hwports::HwPort<drv##_Type, device##_BASE, nullptr> objName;

#endif

#if defined(__linux__)

// Workaround to get rid of inline assembly functions in host environment.
#define __enable_irq __dummy__enable_irq
#define __disable_irq __dummy__disable_irq
#define __NOP __dummyNOP
#define __WFI __dummyWFI
#define __WFE __dummyWFE

// Inline functions with hard coded access to hardware, difficult to remove.
#define NVIC_SetPriority __dummyNVIC_SetPriority
#define NVIC_DisableIRQ __dummyNVIC_DisableIRQ
#define NVIC_EnableIRQ __dummyNVIC_EnableIRQ
#define SysTick_Config __dummySysTick_Config

#include <DeviceSupport/ST/STM32F10x/stm32f10x.h>

#undef __enable_irq
#undef __disable_irq
#undef __NOP
#undef __WFI
#undef __WFE

#undef NVIC_SetPriority
#undef NVIC_DisableIRQ
#undef NVIC_EnableIRQ
#undef SysTick_Config

static __INLINE void __enable_irq(){};
static __INLINE void __disable_irq(){};
static __INLINE void __NOP(){};
static __INLINE void __WFI(){};
static __INLINE void __WFE(){};

static __INLINE uint32_t SysTick_Config(uint32_t ticks)
{
    return 0;
};

#else

#include <DeviceSupport/ST/STM32F10x/stm32f10x.h>

#endif

namespace hwports
{

MCU_ACCESS_CORTEX(SCB, SCB, scb);
MCU_ACCESS_CORTEX(NVIC, NVIC, nvic);
MCU_ACCESS_CORTEX(SysTick, SysTick, systick);

MCU_ACCESS_PORT(TIM, TIM2, tim2);
MCU_ACCESS_PORT(GPIO, GPIOA, gpioa);
MCU_ACCESS_PORT(GPIO, GPIOB, gpiob);
MCU_ACCESS_PORT(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT(RCC, RCC, rcc);
MCU_ACCESS_PORT(I2C, I2C1, i2c1);
MCU_ACCESS_PORT(USART, USART1, usart1);
} // namespace hwports

#undef SCB
#define SCB ::hwports::scb
#undef NVIC
#define NVIC ::hwports::nvic
#undef SysTick
#define SysTick ::hwports::systick

#undef GPIOA
#define GPIOA ::hwports::gpioa
#undef GPIOB
#define GPIOB ::hwports::gpiob
#undef GPIOC
#define GPIOC ::hwports::gpioc
#undef TIM2
#define TIM2 ::hwports::tim2
#undef I2C1
#define I2C1 ::hwports::i2c1
#undef USART1
#define USART1 ::hwports::usart1

// Note: RCC is used before main is called in clock setup etc.
// This will only work for later code.
#undef RCC
#define RCC ::hwports::rcc

#endif /* MCU_SRC_MCUACCESS_H_ */
