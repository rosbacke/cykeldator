/*
 * mcuif_test.cpp
 *
 *  Created on: 5 maj 2019
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include "israccess.h"
#include <cstring>


TEST(Test_israccess, can_use_access_on_host)
{
	israccess::g_currentCortexIsrLevel = 13;
	EXPECT_EQ(israccess::get_BASEPRI(), 13);

	israccess::set_BASEPRI(12);
	EXPECT_EQ(israccess::g_currentCortexIsrLevel, 12);
	EXPECT_EQ(israccess::get_BASEPRI(), 12);
}

using namespace israccess;

TEST(Test_israccess, isr_pri_conversion)
{
	// Value '0' should static assert.
	EXPECT_EQ(irq2CortexLevel<1>() , 0xf0);
	EXPECT_EQ(irq2CortexLevel<2>() , 0xe0);
	EXPECT_EQ(irq2CortexLevel<15>() , 0x10);

	EXPECT_EQ(irq2BasepriLevel<0>() , 0);
	EXPECT_EQ(irq2BasepriLevel<1>() , 0xf0);
	EXPECT_EQ(irq2BasepriLevel<2>() , 0xe0);
	EXPECT_EQ(irq2BasepriLevel<15>() , 0x10);
}

TEST(Test_israccess, protect_class)
{
	// Value '0' should static assert.
	g_currentCortexIsrLevel = 0;
	EXPECT_EQ(g_currentCortexIsrLevel, 0);

	{
		Protect<2> p;
		EXPECT_EQ(g_currentCortexIsrLevel, 0xe0);
	}
	EXPECT_EQ(g_currentCortexIsrLevel, 0);
}

TEST(Test_israccess, access_to_nvic_functions)
{
	hwports::nvic->IP[ (int)USART1_IRQn ] = 0;
	setIsrPriority<USART1_IRQn, 1>();
	EXPECT_EQ(hwports::nvic->IP[ (int)USART1_IRQn ], 0xf0);

	hwports::nvic->ISER[ 1 ] = 0xaaaaaaaa;
	enableIrq<USART1_IRQn>();
	EXPECT_EQ(hwports::nvic->ISER[ 1 ], 32);

	hwports::nvic->ICER[ 0 ] = 0xaaaaaaaa;
	disableIrq<TIM2_IRQn>();
	EXPECT_EQ(hwports::nvic->ICER[ 0 ], (1 << 28));
}

#if 0
TEST(Test_mcuif, can_call_inline_assembly_fkns_in_host)
{
	__enable_irq();
	__disable_irq();
	__NOP();
	__WFI();
	__WFE();
}

TEST(Test_mcuif, can_access_fake_memory_map_of_a_device)
{
	USART1->CR1 = 0x0;
	EXPECT_EQ(hwports::usart1Fake.CR1, 0x0);
	USART1->CR1 = 0x1234;
	EXPECT_EQ(hwports::usart1Fake.CR1, 0x1234);
	EXPECT_EQ(hwports::usart1->CR1, 0x1234);
	hwports::usart1->CR1 = 0x4321;
	EXPECT_EQ(hwports::usart1Fake.CR1, 0x4321);
}

TEST(Test_mcuif, can_setup_an_usart_irq)
{

}
#endif
