/*
 * cover.h
 *
 *  Created on: Apr 21, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_COVER_H_
#define MCU_SRC_COVER_H_

#include "isr.h"

template <IrqHandlers handler, IrqHandlers... handlers>
class IrqList
{
  public:
    static const constexpr int maxPri =
        InterruptSource<handler>::priority > IrqList<handlers...>::maxPri
            ? InterruptSource<handler>::priority
            : IrqList<handlers...>::maxPri;

    constexpr static bool inSet(IrqHandlers queryVal)
    {
        return queryVal == handler || IrqList<handlers...>::inSet(queryVal);
    }
};

template <IrqHandlers handler>
class IrqList<handler>
{
  public:
    static const constexpr int maxPri = InterruptSource<handler>::priority;
    constexpr static bool inSet(IrqHandlers queryVal)
    {
        return queryVal == handler;
    }
};

// Represent a set of data shared by a set of interrupts and threads.
template <typename IrqList_>
class SharedResource
{
  public:
    using IrqList = IrqList_;
    static const constexpr int protectPri = IrqList::maxPri;
    constexpr static bool inSet(IrqHandlers queryVal)
    {
        return IrqList::inSet(queryVal);
    }
};

// Implement to cover a set of data.
//
template <typename SharedResource,
          IrqHandlers callingHandler = IrqHandlers::maxNo>
class Cover
{
  public:
    static const constexpr bool sameLevel =
        SharedResource::protectPri == InterruptSource<callingHandler>::priority;
    Cover()
    {
        static_assert(SharedResource::inSet(callingHandler), "");
        if (!sameLevel)
        {
            const int protectPri = SharedResource::protectPri;
            const int cortexPri = irq2CortexLevel<protectPri>();
            setPrio(cortexPri);
        }
        asm volatile("" : : : "memory");
    }
    ~Cover()
    {
        asm volatile("" : : : "memory");
        if (!sameLevel)
        {
            const int irqPri = InterruptSource<callingHandler>::priority;
            const int cortexPri = irq2CortexLevel<irqPri>();
            setPrio(cortexPri);
        }
    }
};

template <typename Data, typename SharedResource>
class SharedData
{
  public:
    template <typename... Args>
    SharedData(Args... args) : m_data(std::forward(args...))
    {
    }

    template <IrqHandlers callingHandler, typename R, typename DataOperator>
    auto access(DataOperator op) -> R
    {
        Cover<SharedResource, callingHandler> c;
        return op(m_data);
    }

  private:
    Data m_data;
};

#endif /* MCU_SRC_COVER_H_ */
