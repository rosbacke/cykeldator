/*
 * usart.c
 *
 *  Created on: Sep 4, 2018
 *      Author: mikaelr
 */

#include "usart.h"

#include "mcuaccess.h"
#include "isr_project.h"

#include <stdbool.h>


bool RingBuffer::read( uint8_t* b )
{
    if ( empty() )
        return false;
    *b = buffer[ readIndex ];
    advance( &readIndex );
    return true;
}

bool RingBuffer::write( uint8_t b )
{
    int next = writeIndex;
    advance( &next );
    if ( next == readIndex )
        return false;
    buffer[ writeIndex ] = b;
    writeIndex = next;
    return true;
}

void Usart::isr()
{
	struct Usart* ud = this;
    USART_TypeDef* regs = m_regs;
    uint16_t sr = regs->SR;
    if ( sr & USART_SR_RXNE )
    {
        uint8_t b = regs->DR;
        regs->DR = b;
        rx.write( b );
    }
    if ( sr & USART_SR_TXE )
    {
        uint8_t b;
        if ( tx.read( &b ) )
        {
            regs->DR = b;
        }
        else
        {
            regs->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

bool Usart::readByte( uint8_t* data )
{
    return rx.read( data );
}

void Usart::checkRead()
{
    if ( m_regs->SR & USART_SR_RXNE )
    {
        rx.write( m_regs->DR );
    }
}

void Usart::blockwrite( const char* str )
{
	__disable_irq();
	while(*str)
		tx.write(*str++);
    m_regs->CR1 |= USART_CR1_TXEIE;
	__enable_irq();

}

Usart::Usart(USART_TypeDef* regs)
: m_regs(regs)
{}

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
