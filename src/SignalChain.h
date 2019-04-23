/*
 * SignalChain.h
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#ifndef SRC_SIGNALCHAIN_H_
#define SRC_SIGNALCHAIN_H_

#include "TickPoint.h"

#include "RawSignalCondition.h"
#include "SlotTracker.h"
#include "DistanceCalc.h"

#include <algorithm>
#include <array>
#include <delegate/delegate.hpp>


class SignalChain
{
  public:
    SignalChain();
    ~SignalChain();

    static void tickPointCB(const TickPoint& tp, void* ctx)
    {
        static_cast<SignalChain*>(ctx)->addTickPoint(tp);
    }
    static void SystickCB(uint32_t tickCnt, void* ctx)
    {
        static_cast<SignalChain*>(ctx)->addSysTick(tickCnt);
    }

    void addTickPoint(const TickPoint& tp);
    void addSysTick(uint32_t sysTick);

    RawSignalCondition m_rawCond;
    MedianFiltering m_median;
    SlotTracker m_slotTracker;

    DistanceCalc m_distanceCalc;
};


#endif /* SRC_SIGNALCHAIN_H_ */
