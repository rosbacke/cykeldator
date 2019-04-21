/*
 * I2c.cpp
 *
 *  Created on: Apr 16, 2019
 *      Author: mikaelr
 */

#include "I2c.h"

void
I2c::init()
{
    using hwports::gpiob;
    using hwports::rcc;

    hwports::rcc->APB1ENR |= RCC_APB1ENR_I2C1EN;
    hwports::rcc->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // PB6 : SCL, PB7 : SDA : remapping 0.
    // Alternate function open drain output.
    hwports::gpiob->CRL |= GPIO_CRL_MODE6_0 * 2 | GPIO_CRL_CNF6_0 * 3 |
                           GPIO_CRL_MODE7_0 * 2 | GPIO_CRL_CNF7_0 * 3;

    m_dev->CR1 |= I2C_CR1_PE;
    m_dev->CR2 |= I2C_CR2_FREQ_0 * 50;
}
