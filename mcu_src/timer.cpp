#include "timer.h"

#include "mcuaccess.h"
#include "usart.h"

#include <stdint.h>
#include <atomic>

/**
 * Timer1 is set up to count crystal clock pulses (8 mHz) and then use interrupt
 * to extend that count. The low 16 bits are given by the hardware counter and
 * upper 16 bits by interrupts.
 *
 * In addition time stamps are taken for each positive and negative pulse coming
 * in from the odometer sensor. The output are pair-wise 32 bit time stamps of
 * pulse begin/pulse end.
 *
 * Input Pin: PA2
 * Timer TIM2.
 * Capture register: CAP3.
 * Capture register: CAP4.
 */

static std::atomic<uint32_t> systickCnt;

static std::atomic<uint16_t> cntMsb;
static std::atomic<uint32_t> cntMsb2;

static std::atomic<uint32_t> posEdgeTS;
static std::atomic<uint32_t> negEdgeTS;
static std::atomic<uint32_t> count;

static TimerCB s_timerCB = nullptr;
static void* s_timerCBCtx = nullptr;

void setTimerCallback( TimerCB cb, void* ctx )
{
    s_timerCB = cb;
    s_timerCBCtx = ctx;
}

// Input PA2, Tim2, Channel 3.


void delay( int ms )
{
    uint32_t base = systickCnt;
    uint32_t cnt;
    do {
    	cnt = systickCnt;

    } while (( cnt - base ) < ms);
}

uint32_t timer_counterU32()
{
    uint16_t cnt0_15;
    uint16_t cnt16_31;

    do
    {
        cnt0_15 = TIM2->CNT;
        cnt16_31 = cntMsb;
    } while ( ( ( int16_t )( TIM2->CNT - cnt0_15 ) ) < 0 );

    return cnt16_31 << 16u | cnt0_15;
}

uint32_t timer_lastNegTP() { return negEdgeTS; }

uint32_t timer_lastPosTP() { return posEdgeTS; }

uint16_t timer_sysTickDelta() { return cntMsb; }

uint32_t timerSysTick() { return systickCnt; }

/**
 * Set up PA2 as input to monitor, Timer2 to count up 0-0xffff,
 * CCR3 to detect positive flank and CCR4 for negative flank.
 */
void setupTimer()
{
    SysTick_Config( 72000 );

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // Input with pullup.
    GPIOA->CRL &= ~(uint32_t{0xf00});
    GPIOA->CRL |= GPIO_CRL_CNF2_1;

    // Pull resistor up.
    GPIOA->ODR |= 4;

    // Enable counter, rest is default.
    TIM2->CR1 = TIM_CR1_CEN | TIM_CR1_URS;

    /* set Priority for Cortex-M3 System Interrupts */
    NVIC_SetPriority( TIM2_IRQn, ( 1 << __NVIC_PRIO_BITS ) - 1 );
    NVIC_EnableIRQ( TIM2_IRQn );

    // Set up CC3/CC4 as input capture on IT3.
    TIM2->CCMR2 |= TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_1;

    // Enable capture on CC3 / CC4. CC4 neg flank.
    TIM2->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC4P;

    TIM2->DIER |= TIM_DIER_UIE | TIM_DIER_CC3IE | TIM_DIER_CC4IE;

    // Add 8:8 filtering. Still < 1us resolution so shoud be ok.
    TIM2->CCMR2 |= TIM_CCMR2_IC3F_0 * 9 + TIM_CCMR2_IC4F_0 * 9;
}

/* Case 1:
 * - CC taken (high value) start irq processing.
 * - Timer overflow and UIF set. (cnt low, cntUpdate = true)
 * -> Need cntMsb - 1.
 *
 * Case 2:
 * - CC taken (high value) start irq processing.
 * - Timer overflow between SR & CNT. (cnt low, cntUpdate = false)
 * -> Need cntMsb.
 *
 * Case 3:
 * - CC taken (high value) start irq processing.
 * - Timer overflow after SR & CNT. (cnt high, cntUpdate = false)
 * -> Need cntMsb.
 *
 * Case 4:
 * - Timer overflow, start irq processing.  (cnt low, cntUpdate = true)
 * - CC taken (low value)
 * - SR read -> cnt low.
 * -> Need cntMsb.
 *
 * Case 5:
 * - Timer overflow. (cnt low, cntUpdate = true)
 * - SR read -> Not seeing CCR.
 * - CC taken (low value)
 * -> No update of CCR.
 */
extern "C" void TIM2_IRQHandler( void )
{
    uint16_t srMask = 0;
    uint16_t sr = TIM2->SR;
    uint16_t cnt = TIM2->CNT;
    uint16_t cntMsb_ = cntMsb;

    bool send = false;
    const bool cntUpdate = sr & TIM_SR_UIF;
    if ( cntUpdate )
    {
        cntMsb_++;
        if ( cntMsb_ == 0 )
            cntMsb2++;

        srMask |= TIM_SR_UIF;
    }
    if ( sr & TIM_SR_CC3IF )
    {
        uint16_t capture = TIM2->CCR3;
        uint32_t msb = ( capture > 0xc000 && cnt < 0x3fff && cntUpdate )
                           ? uint32_t(cntMsb_) - 1u
                           : uint32_t(cntMsb_);
        posEdgeTS = capture | ( msb << 16u );
        send = true;
        count++;
        srMask |= TIM_SR_CC3IF;
    }
    if ( sr & TIM_SR_CC4IF )
    {
        uint16_t capture = TIM2->CCR4;
        uint32_t msb = ( capture > 0xc000 && cnt < 0x3fff && cntUpdate )
                           ? uint32_t(cntMsb_) - 1u
                           : uint32_t(cntMsb_);
        negEdgeTS = capture | ( msb << 16u );
        srMask |= TIM_SR_CC4IF;
    }
    cntMsb = cntMsb_;
    TIM2->SR &= ~( uint32_t )srMask;

    if ( send && s_timerCB )
        s_timerCB( TickPoint( count, negEdgeTS, posEdgeTS ), s_timerCBCtx );
}

extern "C" void SysTick_Handler( void )
{
    systickCnt++;
}
