/*
 * DistanceCalc.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef SRC_DISTANCECALC_H_
#define SRC_DISTANCECALC_H_

#include <cstdint>
#include "RawSignalCondition.h"
#include "SlotTracker.h"

class DistanceCalc
{
  public:
    DistanceCalc(RawSignalCondition* rawCond, MedianFiltering* median,
                 SlotTracker* slotTracker)
        : m_rawCond(rawCond), m_median(median), m_slotTracker(slotTracker)
    {
    }

    // In 0.1mm unit.
	static const int wheelDiameter = 22340;
	static const constexpr int spokes = 36;
	static const constexpr std::uint16_t spokeAngle = 0x10000 / spokes;

    ~DistanceCalc() = default;

    // Depending on available data, return best guess wheel angle
    // angle is 0-0xffff mapped to the wheel.
    uint16_t wheelAngle() const;

    // A new tick point have been calculated. Update.
    void newTickPoint();
    void newSysTick(uint32_t systick);

    // Return the last passed spoke distance in um.
    int spokeDistance() const;

    // Return the last passed spoke speed in mm/sec.
    int spokeSpeed() const;

    RawSignalCondition* m_rawCond = nullptr;
    MedianFiltering* m_median = nullptr;
    SlotTracker* m_slotTracker = nullptr;

    // Total distance in micrometer.
    uint64_t m_totalDistance = 0;

    uint32_t m_lastUpdate = 0;
};

#endif /* SRC_DISTANCECALC_H_ */
