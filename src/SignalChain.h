/*
 * SignalChain.h
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#ifndef SRC_SIGNALCHAIN_H_
#define SRC_SIGNALCHAIN_H_

#include "TickPoint.h"

#include <delegate/delegate.hpp>

class SignalChain {
public:
	SignalChain();
	~SignalChain();

	static void tickPointCB(const TickPoint& tp, void* ctx)
	{
		static_cast<SignalChain*>(ctx)->tickPoint(tp);
	}
	static void SystickCB(uint32_t tickCnt, void* ctx)
	{
		static_cast<SignalChain*>(ctx)->sysTick(tickCnt);
	}

	void tickPoint(const TickPoint& tp);
	void sysTick(uint32_t sysTick);
};

class RawSignalConditioning
{
public:
	RawSignalConditioning() = default;
	enum class State
	{
		NO_DATA, // Initial state. No acceptable measurement available.
		WAIT_NEXT, // Got one measurement, recent enough to be usable.
		VALID      // Last 2 measurements were usable and delta values can be calculated.
	};
	static const constexpr uint32_t MAX_TP_AGE_MS = 10000; // Arbitrary guess, 10sec.

	void reportTickPoint(const TickPoint& tp);
	void reportSystick(uint32_t tick);

	State m_state = State::NO_DATA;
	TickPoint m_lastTick;
	uint32_t m_lastTickSystick = 0;

	struct Result {
		uint32_t m_count = 0; // Count for the later timepoint this delta is calculated for.
		uint32_t m_deltaAssert = 0; // Time (us) between 2 asserted pulses.
		uint32_t m_deltaRelease = 0; // Time (us) between 2 release pulses.
		uint32_t m_timeAsserted = 0; // Time (us) the last time point was asserted.
		uint32_t m_systick = 0;      // Systick when the last time pulse was calculated.
	};
	Result m_result;             // Contain a valid calculation when m_state == VALID.

	uint32_t m_lastSystick = 0;      // Value of systick at last call.

	using RscCB = delegate<void(RawSignalConditioning)>;
	RscCB m_update;
};

#endif /* SRC_SIGNALCHAIN_H_ */
