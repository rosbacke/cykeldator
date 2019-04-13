/*
 * usart.c
 *
 *  Created on: Sep 4, 2018
 *      Author: mikaelr
 */

#include "usart.h"

#include "stm32f10x.h"

#include <stdbool.h>

#define BUF_SIZE 200

typedef struct RingBuffer
{
    uint8_t buffer[ BUF_SIZE ];
    int readIndex;
    int writeIndex;
} RingBuffer_t;

bool rbEmpty( RingBuffer_t* rb ) { return rb->readIndex == rb->writeIndex; }

static inline void advance( int* cnt_p )
{
    if ( ++*cnt_p == BUF_SIZE )
        *cnt_p = 0;
}

bool rbRead( RingBuffer_t* rb, uint8_t* b )
{
    if ( rbEmpty( rb ) )
        return false;
    *b = rb->buffer[ rb->readIndex ];
    advance( &rb->readIndex );
    return true;
}

bool rbWrite( RingBuffer_t* rb, uint8_t b )
{
    int next = rb->writeIndex;
    advance( &next );
    if ( next == rb->readIndex )
        return false;
    rb->buffer[ rb->writeIndex ] = b;
    rb->writeIndex = next;
    return true;
}

struct UsartDriver
{
    USART_TypeDef* regs;

    RingBuffer_t rx;
    RingBuffer_t tx;
};

static struct UsartDriver s_usart;


static void isr( struct UsartDriver* ud )
{
    USART_TypeDef* regs = ud->regs;
    uint16_t sr = regs->SR;
    if ( sr & USART_SR_RXNE )
    {
        uint8_t b = regs->DR;
        regs->DR = b;
        rbWrite( &ud->rx, b );
    }
    if ( sr & USART_SR_TXE )
    {
        uint8_t b;
        if ( rbRead( &ud->tx, &b ) )
        {
            regs->DR = b;
        }
        else
        {
            regs->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

bool usart_readByte( uint8_t* data ) { return rbRead( &s_usart.rx, data ); }

void usart_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

    // Pull up.
    GPIOA->ODR |= 0x200;
    USART1->CR1 |= USART_CR1_UE;
    USART1->BRR = 625;  // Should be 115200 baud.

    // TX: PA9, RX: PA10
    // GPIO mode alternate function.
    uint32_t t;
    t = GPIOA->CRH;
    t &= ~( 0xffu << 4 );
    t |= ( 0xa << 4 ) | ( 0x8 << 8 );
    GPIOA->CRH = t;

    NVIC_SetPriority( USART1_IRQn, ( 1 << __NVIC_PRIO_BITS ) - 1 );
    NVIC_EnableIRQ( USART1_IRQn );

    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;
}

void usart_checkRead()
{
    if ( USART1->SR & USART_SR_RXNE )
    {
        rbWrite( &s_usart.rx, USART1->DR );
    }
}

void usart_blockwrite( const char* str )
{
    while ( *str )
    {
        while ( ( USART1->SR & USART_SR_TXE ) == 0 )
            ;
        USART1->DR = *str++;
    }
    while ( ( USART1->SR & USART_SR_TC ) == 0 )
        ;
}

void USART1_IrqHandler( void ) { isr( &s_usart ); }
