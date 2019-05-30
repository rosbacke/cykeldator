/*
 * israccess.h
 *
 *  Created on: May 30, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISRACCESS_H_
#define MCU_SRC_ISRACCESS_H_

#include <mcuaccess.h>

/**
 * Set up platform neutral access functions for the ISR subsystem.
 * The core_cm3.h/c file defines a number of processor specific functions.
 *
 * These are ordinary decl/def separated so we could use link time seam to
 * split between the target/host platforms.
 * Bur at least for the interrupt part we want inlining so we duplicate a few
 * functions here.
 */

namespace israccess
{

// Note: enable/disable interrupt globally handled in mcuaccess.h


// Replace get/set_BASEPRI for masking interrupts on priority.
#ifdef __linux__
extern uint32_t g_currentCortexIsrLevel;

inline uint32_t get_BASEPRI(void)
{
	return g_currentCortexIsrLevel;
}

inline void set_BASEPRI(uint32_t basePri)
{
	g_currentCortexIsrLevel = basePri;
}

// Replace core_cm3.h version, now with fake area pointers.
extern "C" inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)(IRQn) < 0)
    {
        SCB->SHP[(((uint32_t)(int32_t)IRQn) & 0xFUL) - 4UL] =
            (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
    }
    else
    {
        NVIC->IP[((uint32_t)(int32_t)IRQn)] =
            (uint8_t)((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
    }
}

// Replace core_cm3.h version, now with fake area pointers.
extern "C" inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
  NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Replace core_cm3.h version, now with fake area pointers.
extern "C" inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
  NVIC->ICER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

#else
inline uint32_t get_BASEPRI(void)
{
  uint32_t result=0;

  __ASM volatile ("MRS %0, basepri_max" : "=&r" (result) );
  return(result);
}

inline void set_BASEPRI(uint32_t value)
{
  __ASM volatile ("MSR basepri, %0" : : "r" (value) );
}

#endif


// Translate a user level interrupt priority to cortex-m interrupt register value.
// Note: For setting interrupt priority it is straight lower level -> higher priority.
// For BASEPRI, this is the same, but the value '0' is special 'no masking active.'.

// The model is '0' for threads and >= 1 for isr. -> Interrupt priorities should be
//
// Input '0' means all levels can run.
// This implies cortex value '0' is not maskable with BASEPRI.
template <int irqLevel>
constexpr int irq2BasepriLevel()
{
    return irqLevel == 0
               ? 0
               : ((1 << __NVIC_PRIO_BITS) - irqLevel) << (8 - __NVIC_PRIO_BITS);
}

template <int irqLevel>
constexpr int irq2CortexLevel()
{
	static_assert(irqLevel > 0);
    return ((1 << __NVIC_PRIO_BITS) - irqLevel) << (8 - __NVIC_PRIO_BITS);
}

// Set the interrupt mask level (in raw cortex register format).
template <int cortexPri>
inline void setCortexPri()
{
    set_BASEPRI(cortexPri);
}

inline void setCortexPri(int cortexPri)
{
    set_BASEPRI(cortexPri);
}

// Set the interrupt mask level (in raw cortex register format) and return the previous level.
template <int cortexPri>
inline int replaceCortexPri()
{
    int t = get_BASEPRI();
    set_BASEPRI(cortexPri);
    return t;
}


// Construct an object of this class to disable interrupt up to and
// including the level given by 'priority'. This will
// effectively disable this interrupt (and all lower level interrupts)
// Restore on destruction.
template<int priority>
class Protect
{
  public:
    Protect() : pri(replaceCortexPri<irq2BasepriLevel<priority>()>()) {}
    ~Protect()
    {
    	setCortexPri(pri);
    }

  private:
    int pri;
};


// NVIC controls interrupt priorities and can enable/disable individual
// Interrupts. It uses the IRQn_Type type to select individual sources.
// The NVIC is memory mapped so rely on device map pointer replacement
// to handle target/host difference.

// Set priority using compile time selection.
template <IRQn_Type IRQn, int priority>
void setIsrPriority()
{
    static_assert(priority > 0 && priority < (1 << __NVIC_PRIO_BITS));
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

template <>
inline void disableIrq<SysTick_IRQn>()
{
    ::hwports::systick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

} // israccess

#endif /* MCU_SRC_ISRACCESS_H_ */
