/*
 * SlotTracker.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "SlotTracker.h"

#include <numeric>

SlotTracker::SlotTracker(int wheelCirc)
	: m_wheelDistance(wheelCirc)
{}

void SlotTracker::addData(DeltaTPResult const& deltaTP, bool isAirVent)
{
	switch(m_state)
	{
	case State::INVALID:
		if (isAirVent)
		{
			m_state = State::RD_DATA;
			m_index = 0;
			m_temp[ m_index++ ] = deltaTP.m_deltaRelease;
		}
		break;

	case State::RD_DATA:
		if (isAirVent && m_index == 37)
		{
			commitWheel();
			m_index = 0;
			m_temp[ m_index++ ] = deltaTP.m_deltaRelease;
		}
		else if (isAirVent && m_index != 37)
		{
			m_index = 0;
			m_temp[ m_index++ ] = deltaTP.m_deltaRelease;
		}
		else if (!isAirVent && m_index == 37) // Lost track
		{
			m_index = 0;
			m_state = State::INVALID;
		}
		else {
			m_temp[ m_index++ ] = deltaTP.m_deltaRelease;
		}
		break;
	};
}

void SlotTracker::commitWheel()
{
    uint32_t totalTime = std::accumulate(m_temp.begin(), m_temp.end(), 0);

    // Require about 2m/s speed to start collecting samples. For normal
    // wheel that is about 1 rev / second. Use that as limit.
    // Limit is that our time (us) fit in 20 bits.
    // Follows that parts do the same.

    const uint32_t maxTime = (1 << 20); // us.
    if (totalTime >= maxTime)
    	return;

    // First, full assignemnt, then gradual decline.
    int factor = m_commitCount < 30 ? ((m_commitCount + 3) / 2) : 16;

	uint32_t whole = totalTime >> 4;
    std::transform(m_temp.begin(), m_temp.end(), m_permanent.begin(),
                   m_permanent.begin(),
                   [&](uint32_t lhs, uint32_t rhs) -> uint32_t
				   {
    	               uint32_t angle = (lhs << 12) / whole;
                       return (angle + rhs * (factor - 1)) / factor;
                   });
    m_commitCount++;
}
