/*
 * RawSignalCondition.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef SRC_RAWSIGNALCONDITION_H_
#define SRC_RAWSIGNALCONDITION_H_

#include <cstdint>
#include "TickPoint.h"
#include <delegate/delegate.hpp>

#include <array>
#include <algorithm>

struct DeltaTPResult
{
    // 'Count' for the later timepoint this delta is calculated for.
    uint32_t m_count = 0;

    // Time duration (us) between 2 asserted pulses.
    uint32_t m_deltaAssert = 0;

    // Time duration (us) between 2 release pulses.
    uint32_t m_deltaRelease = 0;

    // Time duration (us) the last time point was asserted.
    uint32_t m_timeAsserted = 0;

    // Systick when the last time pulse was calculated.
    uint32_t m_systick = 0;
};

/**
 * Take time points and systick calls. Transform the incoming stream
 * of time points and calculate delta values. Use systick to invalidate
 * time points that take to long to get.
 *
 * in -> TickPoints, systick.
 * out -> Delta values and valid state.
 */
class RawSignalCondition
{
  public:
	using Result = DeltaTPResult;
    RawSignalCondition() = default;
    static const constexpr uint32_t MAX_TP_AGE_MS =
        10000; // Arbitrary guess, 10sec.

    void addTickPoint(const TickPoint& tp);
    void addSystick(uint32_t tick);

    bool isValid() const
    { return m_state == State::VALID; }

    State m_state = State::INVALID;
    TickPoint m_cachedTP;
    uint32_t m_lastTPSystick = 0;

    Result m_result; // Contain a valid calculation when m_state == VALID.

    uint32_t m_cachedSystick = 0; // Value of systick at last call.

    using RscCB = delegate<void(RawSignalCondition)>;
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
    static const constexpr int bucketNo = 5;

    State m_state = State::INVALID;
    uint32_t m_median = 0;

    struct Element
    {
        Element() = default;
        Element(uint32_t dt, uint32_t asserted, uint32_t count)
            : m_deltaRelease(dt), m_asserted(asserted), m_count(count)
        {
        }
        uint32_t m_deltaRelease = 0;
        uint32_t m_asserted = 0;
        uint32_t m_count = 0;

        bool operator<(const Element& el)
        {
            return m_deltaRelease < el.m_deltaRelease;
        }
    };
    std::array<Element, bucketNo> m_store;
    std::array<Element, bucketNo> m_calc;

    int m_index = 0; // Next position to write to.

    bool isValid() const
    {
    	return m_state == State::VALID;
    }
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

    void addDelta(uint32_t deltaRelease, uint32_t asserted, uint32_t count);
    bool isTopAirvent() const;
};

#endif /* SRC_RAWSIGNALCONDITION_H_ */
