/*
 * TimeSource.h
 *
 *  Created on: May 29, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_TIMESOURCE_H_
#define MCU_SRC_TIMESOURCE_H_

#include <cstdint>
#include <atomic>

#include <delegate/delegate.hpp>
#include <mcuaccess.h>

class TimeSource
{
public:
	TimeSource(SysTick_Type* stDev);

    // Return current value of the running systick counter. (ms)
    uint32_t systick()
    {
        return m_systick;
    }

    using SleepCB = void (*)(void);

    // Delay for a number of ms.
    // Can take a template parameter with a callback to be repeatedly
    // called during the sleep.
    template <SleepCB = nullptr>
    void delay(int ms);

    delegate<void(uint32_t)> m_systickCB;

private:
    SysTick_Type* m_stDev = nullptr;
    void systickIsr();

    std::atomic<uint32_t> m_systick{0};
};

template <TimeSource::SleepCB cb>
void TimeSource::delay(int ms)
{
    const uint32_t base = m_systick.load();
    auto done = [&]() {
        auto cnt = m_systick.load();
        return int32_t(cnt - base) >= ms;
    };
    do {
        if (cb)
            cb();
    } while(!done());
}

#endif /* MCU_SRC_TIMESOURCE_H_ */
