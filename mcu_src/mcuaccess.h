/*
 * mcuaccess.h
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_MCUACCESS_H_
#define MCU_SRC_MCUACCESS_H_

#include <cassert>
#include "HwPort.h"

#if defined(__linux__)

  // Workaround to get rid of inline assembly functions in host environment.
  #define __enable_irq __dummy__enable_irq
  #define __disable_irq __dummy__disable_irq
  #define NOP __dummyNOP
  #define WFI __dummyWFI
  #define WFE __dummyWFE

  #include <DeviceSupport/ST/STM32F10x/stm32f10x.h>
  #include <CoreSupport/core_cm3.h>

  #undef __enable_irq
  #undef __disable_irq
  #undef NOP
  #undef WFI
  #undef WFE

  static __INLINE void __enable_irq() {};
  static __INLINE void __disable_irq() {};
  static __INLINE void NOP() {};
  static __INLINE void WFI() {};
  static __INLINE void WFE() {};


#define MCU_ACCESS_PORT(drv, device, objName) \
    extern drv##_TypeDef objName##Fake;       \
    extern hwports::HwPort<drv##_TypeDef, 0, &objName##Fake> objName;

#define MCU_ACCESS_PORT_DEF(drv, device, objName) \
    drv##_TypeDef objName##Fake;                  \
    hwports::HwPort<drv##_TypeDef, 0, &objName##Fake> objName;

#else

  #include <DeviceSupport/ST/STM32F10x/stm32f10x.h>

  #define MCU_ACCESS_PORT(drv, device, objName) \
    extern hwports::HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

  #define MCU_ACCESS_PORT_DEF(drv, device, objName) \
	hwports::HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

#endif


namespace hwports {

#if defined(__linux__)
    extern SysTick_Type systickFake;
    extern HwPort<SysTick_Type, 0, &systickFake> systick;
#else
    extern HwPort<SysTick_Type, SysTick_BASE, nullptr> systick;
#endif


MCU_ACCESS_PORT(TIM, TIM2, tim2);
MCU_ACCESS_PORT(GPIO, GPIOA, gpioa);
MCU_ACCESS_PORT(GPIO, GPIOB, gpiob);
MCU_ACCESS_PORT(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT(RCC, RCC, rcc);
MCU_ACCESS_PORT(I2C, I2C1, i2c1);
MCU_ACCESS_PORT(USART, USART1, usart1);
}

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
// Seems to be issues with replacing it in that code.
//#undef RCC
//#define RCC ::hwports::rcc


#endif /* MCU_SRC_MCUACCESS_H_ */
