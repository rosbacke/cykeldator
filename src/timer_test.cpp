/*
 * timer_test.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */
#include <gtest/gtest.h>

#include <timer.h>


TEST(timer, construction)
{
	TIM_TypeDef timerFake;
	OdoTimer ot(&timerFake);

	EXPECT_EQ(ot.sysTick(), 0);
	IsrHandlers::callIsr(IsrHandlers::Handler::systick);

	EXPECT_EQ(ot.sysTick(), 1);
}



