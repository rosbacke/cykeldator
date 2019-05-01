/*
 * interpreter.cpp
 *
 *  Created on: Apr 15, 2019
 *      Author: mikaelr
 */
#include <errno.h>
#include <fmt/format.h>
#include <string.h>
#include <chrono>
#include <unistd.h>

#include "SignalChain.h"

static std::vector<std::array<uint32_t, 4>> readInput()
{
    std::array<uint32_t, 4> line;
    std::vector<std::array<uint32_t, 4>> data;
    int x;
    int y = 0;
    while ((x = scanf("%x %x %x %x", &line[0], &line[2], &line[1], &line[3])) ==
           4)
    {
        ++y;
        data.push_back(line);
    }
    fmt::print("errcode:{} errno:{}, y:{}\n", x, strerror(errno), y);
    return data;
}

struct El1
{
    State state;
    RawSignalCondition::Result res;
    State medState;
    uint32_t median;
    bool isAirVent;
};

void processSample(uint32_t index, SignalChain& sc)
{
	El1 el;
	el.isAirVent = sc.m_median.isTopAirvent();
	el.medState = sc.m_median.m_state;
	el.median = sc.m_median.m_median;
	el.res = sc.m_rawCond.m_result;
    el.state = sc.m_rawCond.m_state;

    int angle = sc.m_distanceCalc.wheelAngle();
    int speedi = sc.m_distanceCalc.spokeSpeed();
    double speed = el.medState == State::VALID
                       ? 3.6 * DistanceCalc::wheelDiameter * 0.0001 * angle / 65536 * 1000000.0 / el.res.m_deltaRelease
                       : 0.0;

    fmt::print("{}\t{}\t{}\t{}\t{}\t{}\t{:>6}\t{:>6} {:>3}\t{}\t{}\t{:>7}\t{:>.4}\t{:.7}\t{:.4}\t{:.4}\n",
    		index,
			   el.state, el.res.m_count, el.res.m_systick,
               el.res.m_deltaAssert, el.res.m_deltaRelease,
               el.res.m_timeAsserted, el.median, el.medState,
			   sc.m_slotTracker.index(),
               el.isAirVent, angle, speed, sc.m_slotTracker.m_state,
			   speedi*3.6*0.001, speed - speedi*3.6*0.001);
}

int
main(int argc, const char* argv[])
{
    std::vector<std::array<uint32_t, 4>> data = readInput();

    SignalChain signalChain;

    std::vector<El1> data2;
    bool realTime = false; //true;

    auto baseTime = std::chrono::system_clock::now();
    auto now = baseTime;

    auto baseTick = data[0][3];
    auto endTick = data.back()[3] + 1;

    uint32_t dataIndex=0;

    for (uint32_t tick = baseTick; tick < endTick; tick++)
    {
    	signalChain.addSysTick(tick);

        auto d_p = &data[dataIndex];
        while (realTime && (std::chrono::system_clock::now() - baseTime <
                            std::chrono::milliseconds(data[dataIndex][3] - baseTick)))
        {
            usleep(1000);
        }

        for (; dataIndex < data.size() && data[dataIndex][3] <= tick;
        		++dataIndex)
        {
        	const auto& d = data[dataIndex];
        	signalChain.addTickPoint(TickPoint(d[0], d[1], d[2]));
        	processSample(dataIndex, signalChain);
        }
    }

    for (auto i : signalChain.m_slotTracker.m_permanent)
        fmt::print("     {}\n", double(i)/65536.0);
    fmt::print("{}\n", argc);
}
