/*
 * interpreter.cpp
 *
 *  Created on: Apr 15, 2019
 *      Author: mikaelr
 */
#include <fmt/format.h>
#include <string.h>
#include <errno.h>

#include "SignalChain.h"

int main(int argc, const char* argv[])
{
	std::array<uint32_t,4> line;
	std::vector<std::array<uint32_t, 4>> data;
	int x;
	int y = 0;
	while( (x = scanf("%x %x %x %x", &line[0], &line[1], &line[2], &line[3])) == 4)
	{
		++y;
		data.push_back(line);
	}
	fmt::print("errcode:{} errno:{}, y:{}\n", x, strerror(errno), y);

	struct El1
	{
		RawSignalConditioning::State state;
		RawSignalConditioning::Result res;
		MedianFiltering::State medState;
		uint32_t median;
		bool isAirVent;
	};

	std::vector<El1> data2;
	RawSignalConditioning rawCond;
	MedianFiltering medianCond;
	for (auto&& d : data)
	{
		El1 el;
		rawCond.addTickPoint(TickPoint(d[0], d[1], d[2]));
		el.res = rawCond.m_result;
		el.state = rawCond.m_state;
		el.medState = MedianFiltering::State::INVALID;
		el.median = 0;
		el.isAirVent = false;
		if (el.state == RawSignalConditioning::State::VALID)
		{
			medianCond.addDelta(el.res.m_deltaRelease, el.res.m_count);
			if (medianCond.m_state == MedianFiltering::State::VALID)
			{
				el.medState = medianCond.m_state;
				el.median = medianCond.m_median;
				el.isAirVent = medianCond.isTopAirvent();
			}

		}
		data2.push_back(el);
        fmt::print( "{} {} {} {} {} {} {} {} {}\n", ( int )el.state, el.res.m_systick,
                    el.res.m_count, el.res.m_deltaAssert, el.res.m_deltaRelease,
                    el.res.m_timeAsserted, el.median, ( int )el.medState, el.isAirVent );
    }
    fmt::print("{}\n", argc);
}


