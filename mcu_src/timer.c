#include "timer.h"

#include "stm32f10x.h"
#include "usart.h"

#include <stdint.h>

volatile uint32_t systickCnt;
volatile uint16_t cntMsb;

volatile uint32_t posEdgeTS;
volatile uint32_t negEdgeTS;

// Input PA2, Tim2, Channel 3.


// A timestamp originating from the counters.
// Start counting at 0 at boot and the up.
struct CounterTS
{
	uint16_t lsb; // Value captured from HW counter.
	uint32_t msb; // Value captured from SW counter.
};

void delay()
{
	uint32_t base = systickCnt;
	while((systickCnt - base) < 500)
		;
}

uint32_t timer_counterU32()
{
	uint16_t cnt0_15;
	uint16_t cnt16_31;

	do {
		cnt0_15 = TIM2->CNT;
		cnt16_31 = cntMsb;
	} while( ((int16_t)(TIM2->CNT - cnt0_15)) < 0);

	return cnt16_31 << 16u | cnt0_15;
}

uint32_t timer_lastNegTP()
{
	return negEdgeTS;
}

uint32_t timer_lastPosTP()
{
	return posEdgeTS;
}

uint16_t timer_sysTickDelta()
{
	return cntMsb;
}

void setupTimer()
{
	SysTick_Config(72000);

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRH &= ~(0xfu << (0 * 4));
	GPIOA->CRH |= (2u << (0 * 4));

	// Enable counter, rest is default.
	TIM2->CR1 = TIM_CR1_CEN | TIM_CR1_URS;

	/* set Priority for Cortex-M0 System Interrupts */
	NVIC_SetPriority (TIM2_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Set up CC3/CC4 as input capture on IT3.
	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_1;

	// Enable capture on CC3 / CC4. CC4 neg flank.
	TIM2->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC4P;

	TIM2->DIER |= TIM_DIER_UIE | TIM_DIER_CC3IE | TIM_DIER_CC4IE;
}

void TIM2_IRQHandler(void)
{
	uint16_t sr = TIM2->SR;
	uint16_t cnt = TIM2->CNT;

	if (sr & TIM_SR_CC3IF)
	{
		posEdgeTS = TIM2->CCR3 | (cntMsb << 16u);
		sr &= ~TIM_SR_CC3IF;
	}
	if (sr & TIM_SR_CC4IF)
	{
		negEdgeTS = TIM2->CCR4 | (cntMsb << 16u);
		sr &= ~TIM_SR_CC4IF;
	}
	if (sr & TIM_SR_UIF)
	{
		cntMsb++;
		sr &= ~TIM_SR_UIF;
	}
	TIM2->SR = sr;
}

void SysTick_Handler(void)
{
	static bool sw;
	sw = !sw;

	if (sw)
		GPIOA->ODR &= ~0x100u;
	else
		GPIOA->ODR |= 0x100u;
	systickCnt++;
}
