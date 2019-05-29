/*
 * isr.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_H_
#define MCU_SRC_ISR_H_

#include <mcuaccess.h>
#include <delegate/delegate.hpp>

enum class IrqHandlers
{
    systick,
    tim2,
    usart1,
    usart2,
    thread, // For use in 'cover'
    maxNo
};

template <IrqHandlers handler>
class IrqMap;

#define MAP_DATA(handler, IrqN)                           \
    template <>                                           \
    class IrqMap<IrqHandlers::handler>                    \
    {                                                     \
      public:                                             \
        static const constexpr IRQn_Type cmsisIrq = IrqN; \
    };

MAP_DATA(systick, SysTick_IRQn);
MAP_DATA(tim2, TIM2_IRQn);
MAP_DATA(usart1, USART1_IRQn);

template <int cortexPri>
inline int replacePrio();

void setPrio(int cortexPrio);

template <int irqLevel>
constexpr int irq2CortexLevel()
{
    return irqLevel == 0
               ? 0
               : ((1 << __NVIC_PRIO_BITS) - irqLevel) << (8 - __NVIC_PRIO_BITS);
}

/** User defined object for controlling interrupts
 *  @param handler An enum value indicating which interrupt to handle.
 *  @param Priority  A priority level for the interrupt. 0 is thread prioriy,
 *         higher numerical value mean priority over lower values.
 *         Default 1 is lowest interrupt priority.
 */
template <IrqHandlers handler, int pri_ = 1>
class InterruptSource
{
  public:
    static const constexpr int priority = pri_;
    // Construct an object of this class to disable interrupt up to and
    // including the level for this particular interrupt. This will
    // effectively disable this interrupt (and all lower level interrupts)
    class Protect
    {
      public:
        static const constexpr int cortexPri = irq2CortexLevel<priority>();
        Protect() : pri(replacePrio<cortexPri>()) {}
        ~Protect()
        {
            setPrio(pri);
        }

      private:
        int pri;
    };

    // Enable/disable interrupt at NVIQ level.
    static void active(bool state);

    // Rub before first enable. Sets up priority.
    static void setup();
};

using IrqSource_Thread = InterruptSource<IrqHandlers::thread, 0>;

// Set priority using compile time selection.
template <IRQn_Type IRQn, int priority>
void setIsrPriority();

// Enable interrupt using compile time selection.
template <IRQn_Type IRQn>
void enableIrq();

// Disable interrupt using compile time selection.
template <IRQn_Type IRQn>
void disableIrq();

#ifdef __linux__

static inline uint32_t get_BASEPRI(void)
{
    return 0;
}

template <IRQn_Type IRQn, int prio>
inline void setIsrPriority()
{
}

template <IRQn_Type IRQn>
inline void enableIrq()
{
}

template <IRQn_Type IRQn>
inline void disableIrq()
{
}

extern int basePri;

template <int newPri>
inline int replacePrio()
{
    int t = basePri;
    basePri = newPri;
    return t;
}

inline void setPrio(int newPrio)
{
    basePri = newPrio;
}

#else

static inline uint32_t get_BASEPRI(void)
{
    uint32_t result = 0;
    __ASM volatile("MRS %0, basepri_max" : "=&r"(result));
    return (result);
}

static inline void set_BASEPRI(uint32_t value)
{
    __ASM volatile("MSR basepri, %0" : : "r"(value));
}

template <IRQn_Type IRQn, int priority>
inline void setIsrPriority()
{
    // Long story: Cortex-M register  values are in range 0 - 0xff where 0x0 is
    // highest priority. Device vendors can chose to implement a subset given by
    // __NVIC_PRIO_BITS where the upper bits are used.
    // Fkn NVIC_SetPriority does the shifting but still use 0 as highest
    // priority.
    static_assert(priority >= 0 && priority < __NVIC_PRIO_BITS);
    const int cortexPrio = (1 << __NVIC_PRIO_BITS) - priority;
    NVIC_SetPriority(IRQn, cortexPrio);
}

template <IRQn_Type IRQn>
inline void enableIrq()
{
    static_assert(IRQn >= 0, "");
    NVIC_EnableIRQ(IRQn);
}

template <>
inline void enableIrq<SysTick_IRQn>()
{
	hwports::systick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

template <IRQn_Type IRQn>
inline void disableIrq()
{
    static_assert(IRQn >= 0, "");
    NVIC_DisableIRQ(IRQn);
}

template <int cortexPrio>
inline int replacePrio()
{
    int t = get_BASEPRI();
    set_BASEPRI(cortexPrio);
    return t;
}

inline void setPrio(int cortexPrio)
{
    set_BASEPRI(cortexPrio);
}

template <>
inline void disableIrq<SysTick_IRQn>()
{
    ::hwports::systick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}
#endif

// Enable/disable interrupt at NVIQ level.
template <IrqHandlers handler, int priority>
void InterruptSource<handler, priority>::active(bool state)
{
    const constexpr IRQn_Type irqNo = IrqMap<handler>::cmsisIrq;

    if (state)
        enableIrq<irqNo>();
    else
        disableIrq<irqNo>();
}

// Setup priority.
template <IrqHandlers handler, int priority>
void InterruptSource<handler, priority>::setup()
{
    const constexpr IRQn_Type irqNo = IrqMap<handler>::cmsisIrq;
    setIsrPriority<irqNo, priority>();
}

class IsrHandlers
{
  public:
    using Del = delegate<void()>;
    static IsrHandlers instance;

    static inline Del& del(IrqHandlers h)
    {
        return instance.m_isrCB[static_cast<int>(h)];
    }

    static void setIsr(IrqHandlers h, Del d)
    {
        del(h) = d;
    }

    static inline void callIsr(IrqHandlers h)
    {
        instance.m_isrCB[static_cast<int>(h)]();
    }

    Del m_isrCB[static_cast<int>(IrqHandlers::maxNo)];
};


// Set up all interrupt functions.
#define MK_IRQ_FKN(handler, FknName)                      \
    extern "C" void FknName##_IRQHandler(void)            \
    {                                                     \
        int pos = static_cast<int>(IrqHandlers::handler); \
        IsrHandlers::instance.m_isrCB[pos]();             \
    }

#define MK_SYSIRQ_FKN(handler, FknName)                   \
    extern "C" void FknName##_Handler(void)               \
    {                                                     \
        int pos = static_cast<int>(IrqHandlers::handler); \
        IsrHandlers::instance.m_isrCB[pos]();             \
    }

#endif /* MCU_SRC_ISR_H_ */
