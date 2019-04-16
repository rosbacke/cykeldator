/*
 * SignalChain_test.cpp
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */
#include "SignalChain.h"

#include <gtest/gtest.h>


void sendTP(RawSignalConditioning& rsc, uint32_t cnt, uint32_t assert_us, uint32_t negate_us)
{
	TickPoint tp(cnt, assert_us * 72,negate_us * 72);
	rsc.addTickPoint(tp);
}

void sendSysTick(SignalChain& sc, uint32_t tick)
{
	SignalChain::SystickCB(tick, static_cast<void*>(&sc));
}


TEST(Buildtest, initial)
{
	SignalChain sc;

	ASSERT_FALSE(false);
}

TEST(RawSignalConditioning, construction)
{
	RawSignalConditioning rsc;
	using State = RawSignalConditioning::State;

	ASSERT_EQ(rsc.m_state, State::NO_DATA);
	ASSERT_EQ(rsc.m_cachedSystick, 0);

	rsc.addSystick(10);
	ASSERT_EQ(rsc.m_state, State::NO_DATA);
	ASSERT_EQ(rsc.m_cachedSystick, 10);

	sendTP(rsc, 1, 10, 200);
	ASSERT_EQ(rsc.m_state, State::WAIT_NEXT);
	ASSERT_EQ(rsc.m_cachedSystick, 10);

	sendTP(rsc, 2, 410, 700);
	ASSERT_EQ(rsc.m_state, State::VALID);
	ASSERT_EQ(rsc.m_cachedSystick, 10);
	ASSERT_EQ(rsc.m_result.m_count, 2);
	ASSERT_EQ(rsc.m_result.m_deltaAssert, 400);
	ASSERT_EQ(rsc.m_result.m_deltaRelease, 500);
	ASSERT_EQ(rsc.m_result.m_timeAsserted, 290);
	ASSERT_EQ(rsc.m_cachedSystick, 10);

	ASSERT_FALSE(false);
}
