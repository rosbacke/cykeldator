/*
 * mcuaccess.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */

#include "mcuaccess.h"

namespace hwports
{
MCU_ACCESS_PORT(TIM, TIM2, tim2);
MCU_ACCESS_PORT(GPIO, GPIOC, gpioc);
MCU_ACCESS_PORT(GPIO, GPIOA, gpioa);
}
