/*
 * SlotTracker.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "SlotTracker.h"

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
			m_state = State::READ_DATA;
			m_index = 0;
			m_temp[ m_index++ ] = deltaTP.m_deltaRelease;
		}
		break;

	case State::READ_DATA:
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
		else if (!isAirVent && m_index != 37) // Lost track
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
	if (m_commitCount == 0)
	{
		std::copy(m_temp.begin(), m_temp.end(), m_permanent.begin());
	}
	else {
		int factor = m_commitCount < 32 ? 1 + ((m_commitCount + 3) / 2) : 16;

        std::transform(m_temp.begin(), m_temp.end(), m_permanent.begin(),
                       m_permanent.begin(),
                       [&](uint32_t lhs, uint32_t rhs) -> uint32_t {
                           return (lhs + rhs * (factor - 1)) / factor;
                       });
    }
	m_valid = true;
	m_commitCount++;
}
