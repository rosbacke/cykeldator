/*
 * interpreter.cpp
 *
 *  Created on: Apr 15, 2019
 *      Author: mikaelr
 */
#include <errno.h>
#include <fmt/format.h>
#include <string.h>

#include "SignalChain.h"

int
main(int argc, const char* argv[])
{
    std::array<uint32_t, 4> line;
    std::vector<std::array<uint32_t, 4>> data;
    int x;
    int y = 0;
    while ((x = scanf("%x %x %x %x", &line[0], &line[1], &line[2], &line[3])) ==
           4)
    {
        ++y;
        data.push_back(line);
    }
    fmt::print("errcode:{} errno:{}, y:{}\n", x, strerror(errno), y);

    struct El1
    {
        State state;
        RawSignalConditioning::Result res;
        MedianFiltering::State medState;
        uint32_t median;
        bool isAirVent;
    };

    std::vector<El1> data2;
    RawSignalConditioning rawCond;
    MedianFiltering medianCond;

    uint32_t tick = 0;
    for (uint32_t dataIndex = 0; dataIndex < data.size(); dataIndex++)
    {
        auto&& d = data[dataIndex];
        while (tick < d[3])
        {
            rawCond.addSystick(tick);
            tick++;
        }
        El1 el;
        rawCond.addTickPoint(TickPoint(d[0], d[1], d[2]));
        el.res = rawCond.m_result;
        el.state = rawCond.m_state;
        el.medState = MedianFiltering::State::INVALID;
        el.median = 0;
        el.isAirVent = false;
        if (el.state == State::VALID)
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
        double speed = el.medState == MedianFiltering::State::VALID
                           ? 3.6 * 2.2 / 36 * 1000000.0 / el.res.m_deltaRelease
                           : 0.0;
        fmt::print("{}\t{}\t{}\t{}\t{}\t{:>6}\t{:>6} {:>3}\t{}\t{:>7}\n",
                   (int)el.state, el.res.m_systick, el.res.m_count,
                   el.res.m_deltaAssert, el.res.m_deltaRelease,
                   el.res.m_timeAsserted, el.median, (int)el.medState,
                   el.isAirVent, speed);
    }
    fmt::print("{}\n", argc);
}
