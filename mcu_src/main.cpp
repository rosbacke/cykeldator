#include <stm32f10x.h>
#include "timer.h"
#include "usart.h"

#define DEF_HANDLER(x) \
extern "C" void x(void) \
{	while(1); }

DEF_HANDLER(NMI_Handler);
DEF_HANDLER(MemManage_Handler);
DEF_HANDLER(BusFault_Handler);
DEF_HANDLER(UsageFault_Handler);
DEF_HANDLER(SVC_Handler);
DEF_HANDLER(DebugMon_Handler);
DEF_HANDLER(PendSV_Handler);
DEF_HANDLER(WWDG_IRQHandler);

extern "C" void HardFault_Handler(void)
{
	while(1)
		;
}


void setLed(bool on)
{
	if (on)
		GPIOC->ODR &= ~uint32_t(1 << 13);
	else
		GPIOC->ODR |= 1 << 13;
}

void setup()
{
	SysTick_Config(72000);
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13_1;
	GPIOC->CRH |= 0;
	usart_init();
}


int main()
{
	setup();
	__enable_irq();
	setLed(false);
	while(1)
	{
		usart_blockwrite("Test.");
		delay(300)
			;
		setLed(true);
		delay(200)
			;
		setLed(false);
	}
}

extern "C" void _exit(int x)
{
	while(1)
		;
}

extern "C" void _init(int x)
{
}
