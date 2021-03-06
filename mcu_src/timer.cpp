#include "timer.h"

#include "isr_project.h"
#include "mcuaccess.h"

#include "usart.h"
#include "TimeSource.h"

#include <delegate/delegate.hpp>

#include <atomic>
#include <stdint.h>

#include <cover.h>

OdoTimer::OdoTimer(TIM_TypeDef* device, TimeSource* ts) : m_dev(device)
{
    IsrHandlers<IrqSource>::del(IrqSource::tim2)
        .set<OdoTimer, &OdoTimer::tim2Isr>(*this);
    setupTimer(ts);
}

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

// Use unittest friendly pointer.
//#undef TIM2
//#define TIM2 m_dev
#undef RCC
#define RCC hwports::rcc

// Input PA2, Tim2, Channel 3.

/**
 * Set up PA2 as input to monitor, Timer2 to count up 0-0xffff,
 * CCR3 to detect positive flank and CCR4 for negative flank.
 */
void OdoTimer::setupTimer(TimeSource* ts)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // Input with pullup.
    GPIOA->CRL &= ~(uint32_t{0xf00});
    GPIOA->CRL |= GPIO_CRL_CNF2_1;

    // Pull resistor up.
    GPIOA->ODR |= 4;

    // Enable counter, rest is default.
    m_dev->CR1 = TIM_CR1_CEN | TIM_CR1_URS;

    /* set Priority for Cortex-M3 System Interrupts */
    IsrManager<IrqSource::tim2>::setup();
    IsrManager<IrqSource::tim2>::active(true);

    // Set up CC3/CC4 as input capture on IT3.
    m_dev->CCMR2 |= TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_1;

    // Enable capture on CC3 / CC4. CC4 neg flank.
    m_dev->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC4P;

    // Add 8:8 filtering. Still < 1us resolution so shoud be ok.
    m_dev->CCMR2 |= TIM_CCMR2_IC3F_0 * 9 + TIM_CCMR2_IC4F_0 * 9;

    m_dev->DIER |= TIM_DIER_UIE | TIM_DIER_CC3IE | TIM_DIER_CC4IE;
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
void OdoTimer::tim2Isr()
{
    uint16_t srMask = 0;
    uint16_t sr = m_dev->SR;
    uint16_t cnt = m_dev->CNT;
    uint16_t capture3 = m_dev->CCR3;
    uint16_t capture4 = m_dev->CCR4;
    const bool cntUpdate = sr & TIM_SR_UIF;

    uint16_t cntMsb_ = m_cntMsb;

    bool send = false;
    if (cntUpdate)
    {
        cntMsb_++;
        if (cntMsb_ == 0)
            m_cntMsb2++;

        srMask |= TIM_SR_UIF;
    }
    if (sr & TIM_SR_CC3IF)
    {
        uint32_t msb = (capture3 > 0xc000 && cnt < 0x3fff && cntUpdate)
                           ? uint32_t(cntMsb_) - 1u
                           : uint32_t(cntMsb_);
        m_posEdgeTS = capture3 | (msb << 16u);
        m_count++;
        send = true;
        srMask |= TIM_SR_CC3IF;
    }
    if (sr & TIM_SR_CC4IF)
    {
        uint32_t msb = (capture4 > 0xc000 && cnt < 0x3fff && cntUpdate)
                           ? uint32_t(cntMsb_) - 1u
                           : uint32_t(cntMsb_);
        m_negEdgeTS = capture4 | (msb << 16u);
        srMask |= TIM_SR_CC4IF;
    }
    m_cntMsb = cntMsb_;
    m_dev->SR &= ~(uint32_t)srMask;
    if (send)
    {
        Cover<ShRes, IrqSource::tim2> c;
        m_tp = TickPoint(m_count, m_negEdgeTS, m_posEdgeTS);
        m_newTp = true;
    }
    if (send && pulseCB)
        pulseCB();
}
