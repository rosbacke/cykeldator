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
    IsrHandlers::del(Irq_e::systick)
        .set<TimeSource, &TimeSource::systickIsr>(*this);
    IsrSource<Irq_e::systick>::setup();
    SysTick_Config(72000);
}

void TimeSource::systickIsr()
{
	Cover<ShRes, Irq_e::systick> c;
	m_systick++;
}

uint32_t TimeSource::systick() const
{
	Cover<ShRes, Irq_e::thread> c;
    return m_systick;
}

extern "C" void *memset(void *s, int c, size_t n)
{
	uint8_t* idx = static_cast<uint8_t*>(s);
	if ((uintptr_t(idx) & 3) || (n & 3))
	{
		uint8_t* end8 = idx + n;
		while(idx < end8)
			*idx++ = c;
	}
	else
	{
		uint32_t val16 = c + (c << 8);
		uint32_t val = val16 + (val16 << 16);
		uint32_t* end32 = (uint32_t*)(idx + n);
		uint32_t* idx32 = (uint32_t*)idx;
		while(idx32 < end32)
			*idx32++ = val;
	}
	return s;
}
