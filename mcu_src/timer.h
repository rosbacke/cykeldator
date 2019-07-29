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
#include "isr_project.h"
#include <delegate/delegate.hpp>

class OdoTimer
{
  public:
    OdoTimer(TIM_TypeDef* device, TimeSource* ts);

    // Called when a timer pulse can be fetched with getTP.
    delegate<void()> pulseCB;

    // CAll in thread or timer mode.
    bool getTP(TickPoint& tp)
    {
        Cover<ShRes, Irq_e::thread> c;
        tp = m_tp;
        bool newTp = m_newTp;
        m_newTp = false;
        return newTp;
    }
    using ShRes = SharedResource<IrqList<Irq_e, Irq_e::systick,
                                         Irq_e::tim2, Irq_e::thread>>;

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
    bool m_newTp = false;
};

#endif /* STM32_SRC_TIMER_H_ */
