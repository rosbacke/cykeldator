/*
 * SignalChain.cpp
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#include "SignalChain.h"
// #include <delegate/delegate.hpp>

// Working point: 10km/h -> ~3m/s. Spokes at ~6cm distance -> pulse freq ~3/0.06 ~50Hz, 20ms.
// Aim for 10Hz update frequency. Should work down to 2km/h.

/**
 * Some theory of calculations.  There will be incoming pulses from the odometer.
 * It needs to pass a number of stages:
 *
 *
 * - delta time with modular resolution.
 * - pre-filtering. (Time point are reasonable.)
 * - Unit conversion (float)
 * - Simple calculator. Does speed calculation based on assumption equal distance between spokes.
 * - Coarse 'missing spoke' reconstruction.
 * - Cycle identifier. Find the Air vent.
 * - Cycle mapper. Maps pulses to individual spokes (37 with air vent).
 * - Cycle profiler. Identify distance and width of each individual spoke.
 * - Accurate speed, distance measurement. Uses distance values from the Cycle profiler.
 * - Speed low pass filtering.
 */
SignalChain::SignalChain() {
}

SignalChain::~SignalChain() {
}

void SignalChain::newTickPoint(const TickPoint& tp) {
}
void SignalChain::newSysTick(uint32_t sysTick) {
}

void RawSignalConditioning::addTickPoint( const TickPoint& tp )
{
    switch ( m_state )
    {
    case State::NO_DATA:
        m_lastTick = tp;
        m_state = State::WAIT_NEXT;
        break;
    case State::WAIT_NEXT:
    case State::VALID:
        if ( tp.m_count != m_lastTick.m_count + 1 )
        {
            m_state = State::NO_DATA;
            break;
        }
        m_result.m_count = tp.m_count;
        m_result.m_deltaAssert =
            ( tp.m_failingEdge - m_lastTick.m_failingEdge ) / 72;
        m_result.m_deltaRelease =
            ( tp.m_raisingEdge - m_lastTick.m_raisingEdge ) / 72;
        m_result.m_timeAsserted = ( tp.m_raisingEdge - tp.m_failingEdge ) / 72;
        m_result.m_systick = m_lastSystick;
        m_state = State::VALID;
        if ( m_update )
            m_update( *this );
        m_lastTick = tp;
        break;
    }
}

void RawSignalConditioning::addSystick(uint32_t tick) {
	m_lastSystick = tick;
	if (m_state != State::NO_DATA) {
		if (int32_t(tick - m_result.m_systick) >= MAX_TP_AGE_MS) {
			m_state = State::NO_DATA;
		}
	}
}

