/*
 * usart.h
 *
 *  Created on: Sep 4, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_USART_H_
#define STM32_SRC_USART_H_

#include <stdbool.h>
#include <stdint.h>
#include <mcuaccess.h>

#define BUF_SIZE 200

typedef struct RingBuffer
{
    uint8_t buffer[ BUF_SIZE ];
    int readIndex;
    int writeIndex;
} RingBuffer_t;

class Usart
{
public:
	Usart(USART_TypeDef* regs);

	static void setupUsart1(Usart& usart);

private:
	void isr();
	bool usart_readByte( uint8_t* data );
	bool rbWrite( RingBuffer_t* rb, uint8_t b );
	void usart_checkRead();

    RingBuffer_t rx;
    RingBuffer_t tx;

	USART_TypeDef* m_regs;
};


void usart_init();

void usart_blockwrite( const char* str );


bool usart_readByte( uint8_t* data );

void usart_checkRead();


#endif /* STM32_SRC_USART_H_ */
