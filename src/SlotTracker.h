/*
 * SlotTracker.h
 *
 *  Created on: 22 apr. 2019
 *      Author: mikaelr
 */

#ifndef SRC_SLOTTRACKER_H_
#define SRC_SLOTTRACKER_H_

#include <array>
#include "RawSignalCondition.h"

class SlotTracker
{
  public:
    SlotTracker(int wheelCirc /* 0.1mm */);
    ~SlotTracker() = default;

    void addData(DeltaTPResult const& deltaTP, bool isAirVent);
    bool indexValid() const
    {
    	return m_state == State::RD_DATA;
    }
    bool dataValid() const
    {
    	return m_commitCount >= 1;
    }

    // For a given slot index, return the angle it spans.
    uint16_t angle(int index)
    {
    	return m_permanent[index];
    }

    // Return the index of the last read slot. Median::isAirWent is
    // true for index zero.
    int index() const
    {
    	return m_index != 0 ? m_index - 1 : 36;
    }

    State m_state = State::INVALID;

    // Contain an angle for each slot+.
    std::array<uint32_t, 37> m_permanent;

  private:
    void commitWheel();

    int m_commitCount = 0;

    // Circumference of wheel in 0.1mm.
    const int m_wheelDistance;
    int m_index = 0;

    // Contain time for each slot in us.
    std::array<uint32_t, 37> m_temp;

};

#endif /* SRC_SLOTTRACKER_H_ */
