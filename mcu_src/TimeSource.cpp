/*
 * TimeSource.cpp
 *
 *  Created on: May 29, 2019
 *      Author: mikaelr
 */
#include "TimeSource.h"

#include "isr_project.h"

TimeSource::TimeSource(SysTick_Type* stDev)
	: m_stDev(stDev)
{
    IsrHandlers<IrqSource>::del(IrqSource::systick)
        .set<TimeSource, &TimeSource::systickIsr>(*this);
}

void TimeSource::systickIsr()
{
    // Cover<ShRes, IrqHandlers::systick> c;
    m_systick++;
    m_systickCB(m_systick);
}
