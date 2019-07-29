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
#include <isr_project.h>
#include <cover.h>


class TimeSource
{
public:
	TimeSource(SysTick_Type* stDev);

    // Return current value of the running systick counter. (ms)
	// Require thread context.
    uint32_t systick() const;

    using SleepCB = void (*)(void);

    // Delay for a number of ms.
    // Can take a template parameter with a callback to be repeatedly
    // called during the sleep.
    template <SleepCB = nullptr>
    void delay(int ms);

    delegate<void(uint32_t)> m_systickCB;

private:
  using ShRes =
      SharedResource<IrqList<Irq_e, Irq_e::systick, Irq_e::thread>>;

  SysTick_Type* m_stDev = nullptr;
  void systickIsr();

  uint32_t m_systick{0};
};

template <TimeSource::SleepCB cb>
void TimeSource::delay(int ms)
{
    const uint32_t base = m_systick;
    auto done = [&]() {
		Cover<ShRes, Irq_e::thread> c;
        auto cnt = m_systick;
        return int32_t(cnt - base) >= ms;
    };
    do {
        if (cb)
            cb();
    } while(!done());
}

#endif /* MCU_SRC_TIMESOURCE_H_ */
