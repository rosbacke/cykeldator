/*
 * mcuaccess.h
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_MCUACCESS_H_
#define MCU_SRC_MCUACCESS_H_

#include <cassert>

#include <DeviceSupport/ST/STM32F10x/stm32f10x.h>
#include <delegate/delegate.hpp>

namespace hwports
{

/**
 * Unit test friendly pointer to IO devices. By redefining the device
 * access pointers to this class, it is possible to redeclare the pointer to
 * a normal memory struct in unit tests.
 */
template<typename AccessStruct, unsigned realAddr, AccessStruct* fakeAddr>
class HwPort
{
public:
	using AccessType = AccessStruct;
	HwPort() = default;
	AccessStruct* operator->()
	{
		return m_instance;
	}
	const AccessStruct* operator->() const
	{
		return m_instance;
	}

	AccessStruct* addr() const
	{
		return m_instance;
	}

	void setAddr(AccessStruct* addr)
	{
		m_instance = addr;
	}

private:
#ifdef UNIT_TEST
	AccessStruct* m_instance = fakeAddr;
#else
	AccessStruct* m_instance = reinterpret_cast<AccessStruct*>(realAddr);
#endif
};

#ifdef UNIT_TEST

#define MCU_ACCESS_PORT(drv, device, objName) \
	extern drv##_TypeDef objName##Fake; \
	extern HwPort<drv##_TypeDef, 0, &objName##Fake> objName;

#define MCU_ACCESS_PORT_DEF(drv, device, objName) \
	drv##_TypeDef objName##Fake; \
	HwPort<drv##_TypeDef, 0, &objName##Fake> objName;


extern SysTick_Type systickFake;
extern HwPort<SysTick_Type, 0, &systickFake> systick;

#else

#define MCU_ACCESS_PORT(drv, device, objName) \
	extern HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

#define MCU_ACCESS_PORT_DEF(drv, device, objName) \
	HwPort<drv##_TypeDef, device##_BASE, nullptr> objName;

extern HwPort<SysTick_Type, SysTick_BASE, nullptr> systick;

#endif

MCU_ACCESS_PORT(TIM, TIM2, tim2);
MCU_ACCESS_PORT(GPIO, GPIOA, gpioa);
MCU_ACCESS_PORT(GPIO, GPIOB, gpiob);
MCU_ACCESS_PORT(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT(RCC, RCC, rcc);
MCU_ACCESS_PORT(I2C, I2C1, i2c1);
MCU_ACCESS_PORT(USART, USART1, usart1);


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
}

#endif /* MCU_SRC_MCUACCESS_H_ */
