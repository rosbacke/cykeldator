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
#include "TimeSource.h"

#include <atomic>
#include <stdint.h>

#include "cover.h"
#include <delegate/delegate.hpp>
#include "isr_project.h"

class OdoTimer
{
  public:
    OdoTimer(TIM_TypeDef* device, TimeSource* ts);

    // Called when a timer pulse can be fetched with getTP.
    delegate<void()> pulseCB;

    // Call with interrupts disabled.
    TickPoint getTP()
    {
        Cover<ShRes> c;
        return m_tp;
    }
    using ShRes = SharedResource<
        IrqList<IrqSource, IrqSource::systick, IrqSource::tim2, IrqSource::thread>>;

  private:
    void setupTimer(TimeSource*);

    void tim2Isr();

    TIM_TypeDef* const m_dev = nullptr;

    uint16_t m_cntMsb{0};
    uint32_t m_cntMsb2{0};
    uint32_t m_count{0};

    uint32_t m_posEdgeTS{0};
    uint32_t m_negEdgeTS{0};

    TickPoint m_tp;
};

#endif /* STM32_SRC_TIMER_H_ */
