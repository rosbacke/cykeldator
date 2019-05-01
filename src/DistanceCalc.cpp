/*
 * DistanceCalc.cpp
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#include "DistanceCalc.h"

uint16_t DistanceCalc::wheelAngle() const
{
	if (m_slotTracker->indexValid() && m_slotTracker->dataValid())
	{
		auto slot = m_slotTracker->index();
		return m_slotTracker->angle(m_slotTracker->index());
	}
	else if (m_slotTracker->indexValid())
	{
		auto slot = m_slotTracker->index();
		return slot == spokes || slot == 0 ? spokeAngle / 2 : spokeAngle;
	}
	else if (m_median->isValid())
	{
		// Bad estimate for slot before spoke.
		return m_median->isTopAirvent() ? spokeAngle/2 : spokeAngle;
	}
	else return spokeAngle;
}

int DistanceCalc::spokeDistance() const
{
	uint32_t angle = wheelAngle();

	// angle, 16 bits, wheeldiameter ~15bits.
	// *100 -> unit micrometer.
	return (((angle * wheelDiameter >> 8) * 100) >> 8);
}

int DistanceCalc::spokeSpeed() const
{
	uint32_t distance = spokeDistance(); // um. (About 5cm -> 50000 um)
	uint32_t time = m_rawCond->m_result.m_deltaRelease; // us. Up to 10e6.
	if (time >= 1 << 20)
		return 0;
	if (time == 0)
		return 0x7fffffff;
	return (1000 * distance) / time;
}

void DistanceCalc::newSysTick(uint32_t systick)
{
	if (m_lastUpdate == 0 || 100 <= (systick - m_lastUpdate))
	{
		m_lastUpdate = systick;

	}

}

void DistanceCalc::newTickPoint()
{
	m_totalDistance += spokeDistance();
}

