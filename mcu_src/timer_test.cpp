/*
 * timer_test.cpp
 *
 *  Created on: Apr 14, 2019
 *      Author: mikaelr
 */
#include <gtest/gtest.h>

#include <isr.h>
#include <timer.h>
#include <TimeSource.h>

static int s_delayCnt;

static void
delayTestCB()
{
    s_delayCnt++;
    IsrHandlers::callIsr(IrqHandlers::systick);
}

TEST(timer, construction)
{
    RCC_TypeDef rccFake;
    hwports::rcc.setAddr(&rccFake);
    SysTick_Type sysTickFake;
    hwports::systick.setAddr(&sysTickFake);
    GPIO_TypeDef gpioaFake;
    hwports::gpioa.setAddr(&gpioaFake);
    GPIO_TypeDef gpiocFake;
    hwports::gpioc.setAddr(&gpiocFake);

    TIM_TypeDef timerFake;

    TimeSource ts(&sysTickFake);

    EXPECT_EQ(ts.systick(), 0);
    IsrHandlers::callIsr(IrqHandlers::systick);

    EXPECT_EQ(ts.systick(), 1);
    IsrHandlers::callIsr(IrqHandlers::systick);

    EXPECT_EQ(ts.systick(), 2);

    // Got systick 2. A delay of 3 should mean ticking up to 5.
    s_delayCnt = 0;
    ts.delay<delayTestCB>(3);

    EXPECT_EQ(ts.systick(), 5);
    EXPECT_EQ(s_delayCnt, 3);
}
