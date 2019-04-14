/*
 * mcuaccess.h
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_MCUACCESS_H_
#define MCU_SRC_MCUACCESS_H_

#include <DeviceSupport/ST/STM32F10x/stm32f10x.h>

namespace hwports
{

/**
 * Unit test friendly pointer to IO devices. By redefining the device
 * access pointers to this, it is possible to redeclare the pointer to
 * a normal memory struct in unit tests.
 */
template<typename AccessStruct, unsigned devAddr>
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
	AccessStruct* m_instance = reinterpret_cast<AccessStruct*>(devAddr);
};


#define MCU_ACCESS_PORT(drv, device, objName) \
	HwPort<drv##_TypeDef, device##_BASE> objName;

extern MCU_ACCESS_PORT(TIM, TIM2, tim2);
extern MCU_ACCESS_PORT(GPIO, GPIOA, gpioa);
extern MCU_ACCESS_PORT(GPIO, GPIOC, gpioc);
}

#endif /* MCU_SRC_MCUACCESS_H_ */
