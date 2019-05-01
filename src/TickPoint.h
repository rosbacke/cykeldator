/*
 * TickPoint.h
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#ifndef SRC_TICKPOINT_H_
#define SRC_TICKPOINT_H_

#include <stdint.h>

enum class State
{
	INVALID,
	VALID,
    RD_DATA, // Got one measurements, recent enough to be usable.
    NO_DATA,   // Initial state. No acceptable measurement available.
};

inline const char* str(const State& s)
{
#define CASE(x) case State::x: return #x
	switch(s)
	{
	CASE(INVALID);
	CASE(VALID);
	CASE(RD_DATA);
	CASE(NO_DATA);
	}
	return "";
#undef CASE
}

#ifdef UNIT_TEST
#include <iostream>
#include <fmt/ostream.h>

inline std::ostream& operator<<(std::ostream& os, const State& s)
{
	return os << std::string(str(s));
}

#endif


struct TickPoint
{
    TickPoint() = default;
    ~TickPoint() = default;
    TickPoint(uint32_t count, uint32_t fe, uint32_t re)
        : m_count(count), m_failingEdge(fe), m_raisingEdge(re)
    {
    }
    TickPoint& operator=(const TickPoint&) = default;

    uint32_t m_count = 0;
    uint32_t m_failingEdge = 0;
    uint32_t m_raisingEdge = 0;
};

#endif /* SRC_TICKPOINT_H_ */
