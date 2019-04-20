/*
 * I2c.h
 *
 *  Created on: Apr 16, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_I2C_H_
#define MCU_SRC_I2C_H_

#include <mcuaccess.h>

class I2c
{
public:
	I2c(I2C_TypeDef* i2c) : m_dev(i2c) {}
	void init();
private:
	I2C_TypeDef* m_dev;
};


#endif /* MCU_SRC_I2C_H_ */
