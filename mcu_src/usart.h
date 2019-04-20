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

class RingBuffer
{
public:
	bool empty() const
	{
		return readIndex == writeIndex;
	}

	static inline void advance( int* cnt_p )
	{
	    if ( ++*cnt_p == BUF_SIZE )
	        *cnt_p = 0;
	}
	bool read( uint8_t* b );
	bool write( uint8_t b );

private:
    uint8_t buffer[ BUF_SIZE ];
    int readIndex=0;
    int writeIndex=0;
};

class Usart
{
public:
	Usart(USART_TypeDef* regs);
	void blockwrite( const char* str );

	static void setupUsart1(Usart& usart);


private:
	void isr();
	bool readByte( uint8_t* data );
	void checkRead();

    RingBuffer rx;
    RingBuffer tx;

	USART_TypeDef* m_regs;
};

#endif /* STM32_SRC_USART_H_ */
