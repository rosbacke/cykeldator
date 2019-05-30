#include "timer.h"
#include "App.h"

#include "isr_project.h"
#include "usart.h"

#include "mcuaccess.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <atomic>

#include "Strings.h"

#include <U8g2lib.h>

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

extern "C" void* _sbrk(intptr_t increment)
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

extern "C" void HardFault_Handler(void)
{
    while (1)
        ;
}
#if 0
void * operator new(std::size_t n)
{
  void * const p = std::malloc(n);
  // handle p == 0
  return p;
}
#endif

void operator delete(void * p) // or delete(void *, std::size_t)
{
}

void operator delete(void *, std::size_t)
{
}


int main()
{
    __enable_irq();
    App app;

    app.setLed(false);
    app.run();
}

extern "C" void _exit(int x)
{
    while (1)
        ;
}

extern "C" void _init(int x) {}
