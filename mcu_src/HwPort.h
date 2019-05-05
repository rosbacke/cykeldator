/*
 * HwPort.h
 *
 *  Created on: 5 maj 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_HWPORT_H_
#define MCU_SRC_HWPORT_H_

#include <cstring>

namespace hwports
{

/**
 * Unit test friendly pointer to IO devices. By redefining the device
 * access pointers to this class, it is possible to redeclare the pointer to
 * a normal memory struct in unit tests.
 */
template <typename AccessStruct, unsigned realAddr, AccessStruct* fakeAddr>
class HwPort
{
  public:
    using AccessType = AccessStruct;
    HwPort() = default;
    AccessStruct* operator->()
    {
        return m_instance;
    }
    const AccessStruct* operator->() const
    {
        return m_instance;
    }

    AccessStruct* addr() const
    {
        return m_instance;
    }

    void setAddr(AccessStruct* addr)
    {
        m_instance = addr;
    }

  private:
#if defined(__linux__)
    AccessStruct* m_instance = fakeAddr;
#else
    AccessStruct* m_instance = reinterpret_cast<AccessStruct*>(realAddr);
#endif
};

/**
 * A unit testing wrapper for the fake memory block with helper functions
 * for testing.
 */
template<typename MemMap>
class FakeRegisters : public MemMap
{
public:
	FakeRegisters()
	{
		clear();
	}
	void clear()
	{
		::memset(static_cast<MemMap*>(this), 0, sizeof (MemMap));
	}
	MemMap* data() const
	{
		return static_cast<MemMap*>(this);
	}
};

} // namespace hwports

#endif /* MCU_SRC_HWPORT_H_ */
