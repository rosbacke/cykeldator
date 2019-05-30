/*
 * cover.h
 *
 *  Created on: Apr 21, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_COVER_H_
#define MCU_SRC_COVER_H_

#include "isr.h"
#include <memory>

template <typename EnumType_, EnumType_ handler, EnumType_... handlers>
class IrqList
{
  public:
	using EnumType = EnumType_;
    static const constexpr int maxPri =
        InterruptSource<EnumType, handler>::priority > IrqList<EnumType, handlers...>::maxPri
            ? InterruptSource<EnumType, handler>::priority
            : IrqList<EnumType, handlers...>::maxPri;

    constexpr static bool inSet(EnumType queryVal)
    {
        return queryVal == handler || IrqList<EnumType, handlers...>::inSet(queryVal);
    }
};

template <typename EnumType_, EnumType_ handler>
class IrqList<EnumType_, handler>
{
  public:
	using EnumType = EnumType_;
    static const constexpr int maxPri = InterruptSource<EnumType, handler>::priority;
    constexpr static bool inSet(EnumType queryVal)
    {
        return queryVal == handler || queryVal == EnumType_::maxNo;
    }
};

// Represent a set of data shared by a set of interrupts and threads and
// share the same locking strategy. The analog of a mutex.
// @param IrqList_ A list of all the interrupts and threads that will
//                 access this resource.
template <typename IrqList_>
class SharedResource
{
  public:
    using IrqList = IrqList_;
    using EnumType = typename IrqList::EnumType;

    // Priority level to raise the irq level to during critical sections.
    static const constexpr int protectPri = IrqList::maxPri;

    // Return true if a particular thread/irq is part of of the given static set.
    constexpr static bool inSet(EnumType queryVal)
    {
        return IrqList::inSet(queryVal);
    }
};

// Implement to cover a set of data. The analog of a mutex_lock
//
template <typename SharedResource,
          typename SharedResource::EnumType callingHandler = SharedResource::EnumType::maxNo>
class Cover
{
  public:
    using EnumType = typename SharedResource::EnumType;
    static const constexpr int callLevel =
        InterruptSource<EnumType, callingHandler>::priority;
    static const constexpr int protectLevel = SharedResource::protectPri;
    static const constexpr bool sameLevel = callLevel == protectLevel;

    Cover()
    {
        // Default value will do a runtime check that we are really
        // in thread mode.
        if (callingHandler == EnumType::maxNo)
        {
            uint32_t result = 0;
            result = israccess::get_BASEPRI();
            if (result != 0)
            {
                while (1)
                    ;
            }
        }
        else
        {
            static_assert(SharedResource::inSet(callingHandler), "");
        }
        if (!sameLevel)
        {
            israccess::setCortexPri(israccess::irq2BasepriLevel<protectLevel>());
        }
        std::atomic_signal_fence( std::memory_order_seq_cst);
    }
    ~Cover()
    {
        std::atomic_signal_fence( std::memory_order_seq_cst);
        if (!sameLevel)
        {
            israccess::setCortexPri(israccess::irq2BasepriLevel<callLevel>());
        }
    }
};

#endif /* MCU_SRC_COVER_H_ */
