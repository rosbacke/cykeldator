/*
 * mcuif_test.cpp
 *
 *  Created on: 5 maj 2019
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include "mcuaccess.h"

TEST(Test_mcuif, can_include_mcuaccess_without_errors)
{
	EXPECT_FALSE(false);
}

TEST(Test_mcuif, can_call_inline_assembly_fkns_in_host)
{
	EXPECT_FALSE(false);
	__enable_irq();
	__disable_irq();
	NOP();
	WFI();
	WFE();
}



