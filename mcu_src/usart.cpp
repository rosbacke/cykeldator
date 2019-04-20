/*
 * usart.c
 *
 *  Created on: Sep 4, 2018
 *      Author: mikaelr
 */

#include "usart.h"

#include "mcuaccess.h"
#include "isr.h"

#include <stdbool.h>


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

bool Usart::rbWrite( RingBuffer_t* rb, uint8_t b )
{
    int next = rb->writeIndex;
    advance( &next );
    if ( next == rb->readIndex )
        return false;
    rb->buffer[ rb->writeIndex ] = b;
    rb->writeIndex = next;
    return true;
}

#if 0
struct UsartDriver
{
    USART_TypeDef* regs;

    RingBuffer_t rx;
    RingBuffer_t tx;
};
#endif

static struct Usart s_usart(hwports::usart1.addr());

void Usart::isr()
{
	struct Usart* ud = this;
    USART_TypeDef* regs = m_regs;
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

bool Usart::usart_readByte( uint8_t* data ) { return rbRead( &s_usart.rx, data ); }

void usart_init()
{
	Usart::setupUsart1(s_usart);
}

void Usart::usart_checkRead()
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


Usart::Usart(USART_TypeDef* regs)
{
	m_regs = regs;
}

void Usart::setupUsart1(Usart& usart)
{
	using hwports::rcc;
	using hwports::gpioa;
	using hwports::usart1;

	IsrHandlers::del(IrqHandlers::usart1).set<Usart, &Usart::isr>(usart);

	rcc->APB2ENR |= RCC_APB2ENR_IOPAEN |  RCC_APB2ENR_USART1EN;

	rcc->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	rcc->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

    // Pull up.
	gpioa->ODR |= 0x200;
    usart1->CR1 |= USART_CR1_UE;
    usart1->BRR = 625;  // Should be 115200 baud.

    // TX: PA9, RX: PA10
    // GPIO mode alternate function.
    uint32_t t;
    t = gpioa->CRH;
    t &= ~( 0xffu << 4 );
    t |= ( 0xa << 4 ) | ( 0x8 << 8 );
    gpioa->CRH = t;

    IrqSource_Usart1::setup();
    IrqSource_Usart1::active(true);

    usart1->CR1 |= USART_CR1_RE | USART_CR1_TE;
}
