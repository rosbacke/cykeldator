/*
 * TickPoint.h
 *
 *  Created on: Apr 7, 2019
 *      Author: mikaelr
 */

#ifndef SRC_TICKPOINT_H_
#define SRC_TICKPOINT_H_

#include <stdint.h>

struct TickPoint
{
    TickPoint() = default;
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
