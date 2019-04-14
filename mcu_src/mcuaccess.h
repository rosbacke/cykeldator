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

template<typename AccessStruct, unsigned devAddr>
class HwPort
{
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
}

#endif /* MCU_SRC_MCUACCESS_H_ */
