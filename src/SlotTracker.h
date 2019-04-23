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
    bool isValid() const
    {
    	return m_valid && m_state == State::READ_DATA;
    }

  private:
    void commitWheel();

    int m_commitCount = 0;

    // Circumference of wheel in 0.1mm.
    const int m_wheelDistance;

    bool m_valid = false;
    int m_index = 0;
    std::array<uint32_t, 37> m_temp;
    std::array<uint32_t, 37> m_permanent;
    State m_state = State::INVALID;
};

#endif /* SRC_SLOTTRACKER_H_ */
