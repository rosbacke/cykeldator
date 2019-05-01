/*
 * RawSignalCondition.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "RawSignalCondition.h"

void
RawSignalCondition::addTickPoint(const TickPoint& tp)
{
    m_lastTPSystick = m_cachedSystick;
    switch (m_state)
    {
    case State::INVALID:
        m_cachedTP = tp;
        m_state = State::RD_DATA;
        m_result.m_count = tp.m_count;
        break;
    case State::RD_DATA:
    case State::VALID:
        if (tp.m_count != m_cachedTP.m_count + 1)
        {
            m_state = State::INVALID;
            break;
        }
        m_result.m_count = tp.m_count;
        m_result.m_deltaAssert =
            (tp.m_failingEdge - m_cachedTP.m_failingEdge) / 72;
        m_result.m_deltaRelease =
            (tp.m_raisingEdge - m_cachedTP.m_raisingEdge) / 72;
        m_result.m_timeAsserted = (tp.m_raisingEdge - tp.m_failingEdge) / 72;
        m_result.m_systick = m_cachedSystick;
        m_state = State::VALID;
        if (m_update)
            m_update(*this);
        m_cachedTP = tp;
        break;
    }
}

void
RawSignalCondition::addSystick(uint32_t tick)
{
    m_cachedSystick = tick;
    if (m_state != State::INVALID)
    {
        if (int32_t(tick - m_lastTPSystick) >= MAX_TP_AGE_MS)
        {
            m_state = State::INVALID;
        }
    }
}

void MedianFiltering::addDelta(uint32_t deltaRelease, uint32_t asserted, uint32_t count)
{
    if (m_state == State::VALID || m_index > 0)
    {
        if (at(prev(&m_index)).m_count + 1 != count)
        {
            reset();
        }
    }
    at(m_index) = Element(deltaRelease, asserted, count);
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
bool MedianFiltering::isTopAirvent() const
{
    if (m_state != State::VALID)
        return false;
    auto t1Index = prev(&m_index);
    auto t2Index = prev(&t1Index);
    auto t3Index = prev(&t2Index);
    auto base = int32_t(at(t3Index).m_deltaRelease);
    auto delta = abs(int32_t(at(t1Index).m_deltaRelease) +
    		         int32_t(at(t2Index).m_deltaRelease) - base);


    bool res = delta < 10  //
    		|| (((base / delta) >= 2)
    				&& at(t2Index).m_asserted > 2 * at(t1Index).m_asserted);

#ifdef __linux__
//    fmt::print("{}\t{}\t{}\t{}\t{}\n", base, delta, at(t2Index).m_asserted, at(t1Index).m_asserted, res);
#endif

    return res;
}
