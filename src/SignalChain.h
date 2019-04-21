/*
 * SignalChain.h
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#ifndef SRC_SIGNALCHAIN_H_
#define SRC_SIGNALCHAIN_H_

#include "TickPoint.h"

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
        static_cast<SignalChain*>(ctx)->newTickPoint(tp);
    }
    static void SystickCB(uint32_t tickCnt, void* ctx)
    {
        static_cast<SignalChain*>(ctx)->newSysTick(tickCnt);
    }

    void newTickPoint(const TickPoint& tp);
    void newSysTick(uint32_t sysTick);
};

/**
 * Take time points and systick calls. Transform the incoming stream
 * of time points and calculate delta values. Use systick to invalidate
 * time points that take to long to get.
 *
 * in -> TickPoints, systick.
 * out -> Delta values and valid state.
 */
class RawSignalConditioning
{
  public:
    RawSignalConditioning() = default;
    enum class State
    {
        NO_DATA,   // Initial state. No acceptable measurement available.
        WAIT_NEXT, // Got one measurement, recent enough to be usable.
        VALID      // Last 2 measurements were usable and delta values can be
                   // calculated.
    };
    static const constexpr uint32_t MAX_TP_AGE_MS =
        10000; // Arbitrary guess, 10sec.

    void addTickPoint(const TickPoint& tp);
    void addSystick(uint32_t tick);

    State m_state = State::NO_DATA;
    TickPoint m_cachedTP;
    uint32_t m_lastTPSystick = 0;

    struct Result
    {
        uint32_t m_count =
            0; // 'Count' for the later timepoint this delta is calculated for.
        uint32_t m_deltaAssert =
            0; // Time duration (us) between 2 asserted pulses.
        uint32_t m_deltaRelease =
            0; // Time duration (us) between 2 release pulses.
        uint32_t m_timeAsserted =
            0; // Time duration (us) the last time point was asserted.
        uint32_t m_systick =
            0; // Systick when the last time pulse was calculated.
    };
    Result m_result; // Contain a valid calculation when m_state == VALID.

    uint32_t m_cachedSystick = 0; // Value of systick at last call.

    using RscCB = delegate<void(RawSignalConditioning)>;
    RscCB m_update;
};

/**
 * Use the sequence of delta times and store them in a ring buffer. Calculate
 * Median time to get an idea about the average time.
 * It require consecutive valid delta values to remain in its own valid state.
 * Minimum size is 5 to handle 2 short periods for the air vent.
 * Use 'deltaRelease' for selection.
 *
 * in -> raw delta values.
 * out -> median period time, valid state.
 */
class MedianFiltering
{
  public:
    MedianFiltering() = default;
    static const constexpr int bucketNo = 7;

    enum class State
    {
        INVALID,
        VALID
    };
    State m_state = State::INVALID;
    uint32_t m_median = 0;

    struct Element
    {
        Element() = default;
        Element(uint32_t dt, uint32_t count)
            : m_deltaRelease(dt), m_count(count)
        {
        }
        uint32_t m_deltaRelease = 0;
        uint32_t m_count = 0;

        bool operator<(const Element& el)
        {
            return m_deltaRelease < el.m_deltaRelease;
        }
    };
    std::array<Element, bucketNo> m_store;
    std::array<Element, bucketNo> m_calc;

    int m_index = 0; // Next position to write to.

    static int prev(const int* curr_p)
    {
        return *curr_p == 0 ? bucketNo - 1 : *curr_p - 1;
    }
    static int next(const int* curr_p)
    {
        return *curr_p == bucketNo - 1 ? 0 : *curr_p + 1;
    }

    Element& at(int index)
    {
        return m_store[index];
    }
    const Element& at(int index) const
    {
        return m_store[index];
    }
    const Element& top() const
    {
        return m_store[prev(&m_index)];
    }

    void reset()
    {
        m_state = State::INVALID;
        m_index = 0;
    }
    void addDelta(uint32_t deltaRelease, uint32_t count)
    {
        if (m_state == State::VALID || m_index > 0)
        {
            if (at(prev(&m_index)).m_count + 1 != count)
            {
                reset();
            }
        }
        at(m_index) = Element(deltaRelease, count);
        m_index = next(&m_index);
        if (m_index == 0)
            m_state = State::VALID;

        if (m_state != State::VALID)
            return;

        std::copy(m_store.begin(), m_store.end(), m_calc.begin());
        auto half = bucketNo / 2;
        std::nth_element(m_calc.begin(), m_calc.begin() + half, m_calc.end());
        m_median = m_calc[half].m_deltaRelease;
    }

    // Given that state is valid, query if the top seems to be an airvent.
    bool isTopAirvent() const
    {
        if (m_state != State::VALID)
            return false;
        auto t1Index = prev(&m_index);
        auto t2Index = prev(&t1Index);
        auto delta = abs(int32_t(at(t1Index).m_deltaRelease +
                                 at(t2Index).m_deltaRelease - m_median));

        return (delta < 10 || m_median / delta > 10);
    }
};

class SlotTracker
{
  public:
  private:
    std::array<uint32_t, 37> m_slots;
};

#endif /* SRC_SIGNALCHAIN_H_ */
