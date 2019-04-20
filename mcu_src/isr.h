/*
 * isr.h
 *
 *  Created on: Apr 20, 2019
 *      Author: mikaelr
 */

#ifndef MCU_SRC_ISR_H_
#define MCU_SRC_ISR_H_

#include <mcuaccess.h>

enum class IrqHandlers
{
	systick,
	tim2,
	maxNo
};

template<IrqHandlers handler>
class IrqMap;

#define MAP_DATA(handler, IrqN) \
template<> class IrqMap<IrqHandlers::handler> \
{ static const constexpr int cmsisIrq = IrqN; }; \


MAP_DATA(systick, SysTick_IRQn);


// User
template<IrqHandlers handler>
class InterruptSource
{
	// Enable/disable interrupt at NVIQ level.
	void active(bool state);

	// Raise global interrupt level to guarantee nothing below or same level
	// as us runs.
	void protect(bool protectActive);
};


// Set priority using compile time selection.
template<IRQn_Type IRQn>
void setIsrPriority(uint32_t priority);

// Enable interrupt using compile time selection.
template<IRQn_Type IRQn>
void enableIrq();

// Set priority using compile time selection.
template<IRQn_Type IRQn>
void disableIrq();

#ifdef UNIT_TEST

template<IRQn_Type IRQn>
inline void setIsrPriority(uint32_t priority)
{}

template<IRQn_Type IRQn>
inline void enableIrq()
{
}

template<IRQn_Type IRQn>
inline void disableIrq()
{
}

#else

template<IRQn_Type IRQn>
inline void setIsrPriority(uint32_t priority)
{
	NVIC_SetPriority( IRQn, priority );
}

template<IRQn_Type IRQn>
inline void enableIrq()
{
	static_assert(IRQn >= 0, "");
	NVIC_EnableIRQ( IRQn );
}

template<>
inline void enableIrq<SysTick_IRQn>()
{
	::hwports::systick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

template <IRQn_Type IRQn>
inline void disableIrq()
{
    static_assert( IRQn >= 0, "" );
    NVIC_DisableIRQ( IRQn );
}

template <>
inline void disableIrq<SysTick_IRQn>()
{
    ::hwports::systick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}
#endif


// Enable/disable interrupt at NVIQ level.
template <IrqHandlers handler>
void InterruptSource<handler>::active( bool state )
{
    const constexpr IRQn_Type irqNo = IrqMap<handler>::cmsisIrq;

    if ( state )
        enableIrq<irqNo>();
    else
        disableIrq<irqNo>();
}

class IsrHandlers
{
public:
	using Del = delegate<void()>;
	static IsrHandlers instance;

	static Del& del( IrqHandlers h)
    {
		return instance.m_isrCB[ static_cast<int>( h ) ];
    }

	static void setIsr( IrqHandlers h, Del d )
    {
		del(h) = d;
    }

    static void callIsr( IrqHandlers h )
    {
    	del(h)();
    }

  private:
	Del m_isrCB[ static_cast<int>(IrqHandlers::maxNo) ];
};

#endif /* MCU_SRC_ISR_H_ */
