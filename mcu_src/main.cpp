#include "timer.h"

#include "usart.h"

#include <stm32f10x.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Strings.h"

#define DEF_HANDLER( x )                                                       \
    extern "C" void x( void )                                                  \
    {                                                                          \
        while ( 1 )                                                            \
            ;                                                                  \
    }

DEF_HANDLER( NMI_Handler );
DEF_HANDLER( MemManage_Handler );
DEF_HANDLER( BusFault_Handler );
DEF_HANDLER( UsageFault_Handler );
DEF_HANDLER( SVC_Handler );
DEF_HANDLER( DebugMon_Handler );
DEF_HANDLER( PendSV_Handler );
DEF_HANDLER( WWDG_IRQHandler );

static char s_heap[ 10000 ];

static TickPoint s_tp;
static volatile bool s_newVal;

static char buffer[ 200 ];

static void timerCB( const TickPoint& tp, void* ctx )
{
    s_tp = tp;
    s_newVal = true;
}

static void write()
{
    __disable_irq();
    // uint2str(buffer, timerSysTick());
    //usart_blockwrite(buffer);
    //usart_blockwrite("\n");
    if ( !s_newVal )
    {
        __enable_irq();
        return;
    }
    TickPoint tp = s_tp;
    s_newVal = false;
    __enable_irq();

    // usart_blockwrite( "Hello." );

    char* p = buffer;
    p = uint2str(p, tp.m_count);
    *p++ = ' ';
    p = uint2str(p, tp.m_failingEdge);
    *p++ = ' ';
    p = uint2str(p, tp.m_raisingEdge);
    *p++ = ' ';
    p = uint2str(p, timerSysTick());
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';
    usart_blockwrite( buffer );
}

extern "C" void* _sbrk( intptr_t increment )
{
    static char* end = s_heap;
    if ( increment + end <= s_heap + sizeof s_heap )
    {
        end += increment;
        return end;
    }
    while ( 1 )
        ;
}

extern "C" void HardFault_Handler( void )
{
    while ( 1 )
        ;
}

void setLed( bool on )
{
    if ( on )
        GPIOC->ODR &= ~uint32_t( 1 << 13 );
    else
        GPIOC->ODR |= 1 << 13;
}

void setup()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH |= 0;
    usart_init();
    setupTimer();
    setTimerCallback( timerCB, nullptr );
}

int main()
{
    setup();
    __enable_irq();
    setLed( false );
    while ( 1 )
    {
    	write();
    	//usart_blockwrite( "Hello." );
        setLed( ( timerSysTick() & 0x200 ) != 0 );
        delay(1);
    }
}

extern "C" void _exit( int x )
{
    while ( 1 )
        ;
}

extern "C" void _init( int x )
{}

