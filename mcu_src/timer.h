/*
 * timer.h
 *
 *  Created on: Sep 9, 2018
 *      Author: mikaelr
 */

#ifndef STM32_SRC_TIMER_H_
#define STM32_SRC_TIMER_H_

#include <mcuaccess.h>

#include "TickPoint.h"
#include <atomic>
#include <stdint.h>

#include "cover.h"
#include <delegate/delegate.hpp>

class OdoTimer
{
  public:
    OdoTimer(TIM_TypeDef* device);

    // Return current value of the running systick counter. (ms)
    uint32_t sysTick()
    {
        return m_sysTick;
    }

    using SleepCB = void (*)(void);

    // Delay for a number of ms.
    // Can take a template parameter with a callback to be repeatedly
    // called during the sleep.
    template <SleepCB = nullptr>
    void delay(int ms);

    // Called when a timer pulse can be fetched with getTP.
    delegate<void()> pulseCB;

    // Call with interrupts disabled.
    TickPoint getTP()
    {
        Cover<ShRes> c;
        return m_tp;
    }
    using ShRes = SharedResource<
        IrqList<IrqHandlers::systick, IrqHandlers::tim2, IrqHandlers::maxNo>>;

  private:
    void setupTimer();

    void sysTickIsr()
    {
        // Cover<ShRes, IrqHandlers::systick> c;
        m_sysTick++;
    }

    void tim2Isr();

    TIM_TypeDef* const m_dev = nullptr;

    uint16_t m_cntMsb{0};
    uint32_t m_cntMsb2{0};
    uint32_t m_count{0};

    uint32_t m_posEdgeTS{0};
    uint32_t m_negEdgeTS{0};

    TickPoint m_tp;
    std::atomic<uint32_t> m_sysTick{0};
};

template <OdoTimer::SleepCB cb>
void
OdoTimer::delay(int ms)
{
    const uint32_t base = [&]() { /*Cover<ShRes> c;*/
                                  return m_sysTick.load();
    }();
    auto done = [&]() { /* Cover<ShRes> c;*/
                        auto cnt = m_sysTick.load();
                        return int32_t(cnt - base) >= ms;
    };
    while (!done())
    {
        if (cb)
            cb();
    }
}

using TimerCB = void (*)(const TickPoint& tp, void* ctx);

#endif /* STM32_SRC_TIMER_H_ */
