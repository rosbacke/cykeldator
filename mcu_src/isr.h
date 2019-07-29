/*
 * isr.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_H_
#define MCU_SRC_ISR_H_

#include <mcuaccess.h>
#include <israccess.h>
#include <delegate/delegate.hpp>

void isrInit();

template<typename EnumType>
struct IrqData
{
	constexpr IrqData(EnumType irq_, IRQn_Type cmsis_, int level_)
	: irq(irq_), cmsis(cmsis_), level(level_)
	{}

	EnumType irq;
	IRQn_Type cmsis;
	int level;
};


template <typename EnumType>
constexpr IrqData<EnumType> irq2Data(EnumType irq);


/** User defined object for controlling interrupts
 *  @param handler An enum value indicating which interrupt to handle.
 *  @param Priority  A priority level for the interrupt. 0 is thread priority,
 *         higher numerical value mean priority over lower values.
 *         Default 1 is lowest interrupt priority.
 */
template <typename IrqSrc, IrqSrc handler>
class InterruptSource
{
  public:
    static const constexpr int priority = irq2Data(handler).level;
    static const constexpr IRQn_Type cmsisNo = irq2Data(handler).cmsis;

    // Enable/disable interrupt at NVIC level.
    static void active(bool state)
    {
        if (state)
            israccess::enableIrq<cmsisNo>();
        else
        	israccess::disableIrq<cmsisNo>();
    }

    // Run before first enable. Sets up priority.
    static void setup()
    {
        israccess::setIsrPriority<cmsisNo, priority>();
    }
};

template<typename IrqEnum>
class IsrHandlerManager
{
  public:
    using IrqFkn = delegate<void()>;
    static IsrHandlerManager instance;

    static inline IrqFkn& del(IrqEnum h)
    {
        return instance.m_isrCB[static_cast<int>(h)];
    }

    static void setIsr(IrqEnum h, IrqFkn d)
    {
        del(h) = d;
    }

    static inline void callIsr(IrqEnum h)
    {
        instance.m_isrCB[static_cast<int>(h)]();
    }

    IrqFkn m_isrCB[static_cast<int>(IrqEnum::maxNo)];
};

template<typename IrqEnum>
IsrHandlerManager<IrqEnum> IsrHandlerManager<IrqEnum>::instance;


/**
 * Macro to define interrupt functions for device interrupts.
 * @param handler enum value connected to this interrupt function.
 * @param FknName First part of the interrupt handler function name.
 */
#define MK_IRQ_FKN(handler, FknName)                      \
    extern "C" void FknName##_IRQHandler(void)            \
    {                                                     \
        using EnumType = decltype(handler);               \
        int pos = static_cast<int>(handler);              \
        IsrHandlerManager<EnumType>::instance.m_isrCB[pos]();   \
    }


/**
 * Macro to define interrupt functions for system interrupts.
 * @param handler enum value connected to this interrupt function.
 * @param FknName First part of the interrupt handler function name.
 */
#define MK_SYSIRQ_FKN(handler, FknName)                   \
    extern "C" void FknName##_Handler(void)               \
    {                                                     \
        using EnumType = decltype(handler);               \
        int pos = static_cast<int>(handler);              \
        IsrHandlerManager<EnumType>::instance.m_isrCB[pos]();   \
    }

#endif /* MCU_SRC_ISR_H_ */
