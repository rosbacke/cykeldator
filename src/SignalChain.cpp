/*
 * SignalChain.cpp
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#include "SignalChain.h"

// Working point: 10km/h -> ~3m/s. Spokes at ~6cm distance -> pulse freq ~3/0.06
// ~50Hz, 20ms. Aim for 10Hz update frequency. Should work down to 2km/h.

/**
 * Some theory of calculations.  There will be incoming pulses from the
 * odometer. It needs to pass a number of stages:
 *
 *
 * - delta time with modular resolution.
 * - pre-filtering. (Time point are reasonable.)
 * - Unit conversion (float)
 * - Simple calculator. Does speed calculation based on assumption equal
 * distance between spokes.
 * - Coarse 'missing spoke' reconstruction.
 * - Cycle identifier. Find the Air vent.
 * - Cycle mapper. Maps pulses to individual spokes (37 with air vent).
 * - Cycle profiler. Identify distance and width of each individual spoke.
 * - Accurate speed, distance measurement. Uses distance values from the Cycle
 * profiler.
 * - Speed low pass filtering.
 */
SignalChain::SignalChain()
: m_slotTracker(DistanceCalc::wheelDiameter),
  m_distanceCalc(&m_rawCond, &m_median, &m_slotTracker)
{}

SignalChain::~SignalChain() {}

void
SignalChain::addTickPoint(const TickPoint& tp)
{
	m_rawCond.addTickPoint(tp);
	if (m_rawCond.isValid())
	{
		auto& res = m_rawCond.m_result;
		m_median.addDelta(res.m_deltaRelease, res.m_timeAsserted, res.m_count);
	}
	else m_median.reset();

	if (m_rawCond.isValid() && m_median.isValid())
	{
		m_slotTracker.addData(m_rawCond.m_result, m_median.isTopAirvent());
	}
	m_distanceCalc.newTickPoint();
}

void
SignalChain::addSysTick(uint32_t sysTick)
{
	m_rawCond.addSystick(sysTick);
	m_distanceCalc.newSysTick(sysTick);
}
