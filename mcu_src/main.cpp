#include "timer.h"

#include "isr_project.h"
#include "usart.h"

#include "mcuaccess.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <atomic>

#include "Strings.h"

#define DEF_HANDLER(x)      \
    extern "C" void x(void) \
    {                       \
        while (1)           \
            ;               \
    }

DEF_HANDLER(NMI_Handler);
DEF_HANDLER(MemManage_Handler);
DEF_HANDLER(BusFault_Handler);
DEF_HANDLER(UsageFault_Handler);
DEF_HANDLER(SVC_Handler);
DEF_HANDLER(DebugMon_Handler);
DEF_HANDLER(PendSV_Handler);
DEF_HANDLER(WWDG_IRQHandler);

static char s_heap[10000];

extern "C" void*
_sbrk(intptr_t increment)
{
    static char* end = s_heap;
    if (increment + end <= s_heap + sizeof s_heap)
    {
        void* oldEnd = end;
        end += increment;
        return oldEnd;
    }
    while (1)
        ;
}

extern "C" void
HardFault_Handler(void)
{
    while (1)
        ;
}

class App
{
  public:
    App() : m_timer(hwports::tim2.addr()), m_usart1(hwports::usart1.addr())
    {
        setup();
    }

    void setup();
    void write();
    void run();
    void setLed(bool on);

  private:
    void timerCB()
    {
        m_newVal = true;
    }

    std::atomic<bool> m_newVal{false};
    OdoTimer m_timer;
    Usart m_usart1;
};

void
App::write()
{
    static char buffer[200];

    TickPoint tp;
    {
        if (!m_newVal)
        {
            return;
        }
        tp = m_timer.getTP();
        m_newVal = false;
    }
    char* p = buffer;
    p = uint2str(p, tp.m_count);
    *p++ = ' ';
    p = uint2str(p, tp.m_failingEdge);
    *p++ = ' ';
    p = uint2str(p, tp.m_raisingEdge);
    *p++ = ' ';
    p = uint2str(p, m_timer.sysTick());
    *p++ = '\r';
    *p++ = '\n';
    *p++ = '\0';
    m_usart1.blockwrite(buffer);
}

void
App::setLed(bool on)
{
    using hwports::gpioc;
    if (on)
        gpioc->ODR &= ~uint32_t(1 << 13);
    else
        gpioc->ODR |= 1 << 13;
}

void
App::setup()
{
    using hwports::gpioc;
    hwports::rcc->APB2ENR |= RCC_APB2ENR_IOPCEN;
    gpioc->CRH |= GPIO_CRH_MODE13_1;
    gpioc->CRH |= 0;
    Usart::setupUsart1(m_usart1);
    m_timer.pulseCB.set<App, &App::timerCB>(*this);
}

void
App::run()
{
    while (1)
    {
        write();
        setLed((m_timer.sysTick() & 0x200) != 0);
        m_timer.delay<__WFI>(0);
    }
}

int
main()
{
    SysTick_Config(72000);
    App app;

    __enable_irq();
    app.setLed(false);
    app.run();
}

extern "C" void
_exit(int x)
{
    while (1)
        ;
}

extern "C" void
_init(int x)
{
}
