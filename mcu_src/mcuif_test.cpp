/*
 * mcuif_test.cpp
 *
 *  Created on: 5 maj 2019
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include "mcuaccess.h"
#include <cstring>

TEST(Test_mcuif, can_include_mcuaccess_without_errors)
{
	EXPECT_FALSE(false);
}

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
