/*
 * usart.h
 *
 *  Created on: Sep 4, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_USART_H_
#define STM32_SRC_USART_H_

#include <mcuaccess.h>
#include <stdbool.h>
#include <stdint.h>

#include "cover.h"
#include "RingBuffer.h"

class Usart
{
  public:
    Usart(USART_TypeDef* regs);
    void blockwrite(const char* str);

    static void setupUsart1(Usart& usart);

  private:
    using UsartCover =
        SharedResource<IrqList<IrqHandlers::usart1, IrqHandlers::thread>>;

    void isr();
    bool readByte(uint8_t* data);
    void checkRead();

    RingBuffer<200> rx;
    RingBuffer<200> tx;

    USART_TypeDef* m_regs;
};

#endif /* STM32_SRC_USART_H_ */
