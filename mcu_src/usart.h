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

void usart_init();

void usart_blockwrite( const char* str );


bool usart_readByte( uint8_t* data );

void usart_checkRead();


#endif /* STM32_SRC_USART_H_ */
