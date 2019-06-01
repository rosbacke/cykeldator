# Using C++ with microcontrollers

How to break legacy practices and use C++ with unit testing and dual target codebases.

By: Mikael Rosbacke

---

# Introduction

Inspired by a presonal MCU project, what could be done to modernize common practices 
in embedded to be more in line with modern C++ development.

- Unit tests on the host computer.
- Decent subset of C++ for most of the code.
- Use of support tools: git, cmake, clang-format, tidy etc.
- Dual target compile. As much of the code workable on the host. (incl. interrupt and driver code)

---

# Bike computer

Example project: A bike computer.

- Use photo barrier to count spokes, giving ~40 pulses/wheel lapse.
- Precision time measurement with hardware based time stamps.
- speed/distance measurement, update freq. ~5Hz with good accuracy.  
- Based on STM32F103 MCU (Bluepill breakout board)  
- I2C connected 1.3" OLed display.
- Uart serial device for logging data to external units.

---

# MCU architecture.

- MCU: STM32F103 (128kB flash, 20kB SRAM, NVIC interrupt controller)
- 32 bit core, ARM Cortex-M3 architecture. Memory mapped devices.
- Project devices: Usart, I2c, Timer (counter + capture), GPIO, Systick.
- Interrupts with several priority levels used.
- No RTOS will be used. (Can be added later if needed)

---

# Testing strategy

Aim for 5 different types of testing:

- Exploratory testing : Hacking around on target during development.
- Unit testing : Putting most of the code under unit test, buildable on the host.
- Device driver tests : Target based application exercising most drivers.
- Signal processing tests : Replay of sensor data to verify algorithms.
- System tests : The bike computer works as expected during use.

---

# Dual targeting from a code base

- Foundation for unit testing all code. Vital to get as much of the code buildable on the host as possible.
- Needed for the signal processing tests. Should run with a thin wrapper supplying sensor data from a file.

## Challenges

- Embedded toolchains differs significantly from host systems. Solved by using gcc as both cross and host compiler.
- Interrupt code model that works on the host. 
- MCU support library. Need to identify an interface between target specific code and common code.
- Convince cmake to build for both targets.

---

# STM32 peripherial support library

An ST supplied library in C with hardware description header files and C drivers.
It has 5 main parts:

- c-startup assember code for the MCU:s. (startup_stm32f10x_md.s)
- Linker file.
- ST supplied register definitions for all peripherial devices. (stm32f10x.h)
- Arm supplied Cortex-M3 header files and drivers. (core_cm3.h)
- ST supplied drivers. (C and H files.)

We will use the first four. We skip the drivers since they do not offer much needed functionality and add complexity. By rewriting in C++ we gain type safety and get smaller drivers.

Startup and linker are target platform only. We need to handle core_cm3.h and stm32f10x.h.

---

# stm32f10x.h

Main components: 

- structs for each device type. Registers are struct members.
- Defines for bit definitions within the registers.
- Enum with all available interrupt sources.
- Defines for base pointers to device register maps.

Good news: This file can be used on the host. It uses standard C constructs.  
Bad news: The included core_cm3.h can not.  
Good news: The base pointers are macro defines and can be redefined.  

---

# core_cm3.h

Issues:

- Includes inline functions with inline cortex-m3 assembly instructions. Gcc can compile it, but
  the function can never be used.
- It is included by stm32f10x.h so we will get this included automatically.
- core_cm3.h and stm32f10x.h have cross dependencies. Main issue is IRQn_type enum, but there is more.
- The startup code uses these headers directly and are immune to any normal wrapping attempt.

---

# mcuaccess.h

Purpose: Wrap access to stm32f10x.h.  
- On target we just include it with minimal modifications.  
- On host we modify the supplied definitions so they are usable on the host.  

    #if defined(__linux__)
    
      #define __WFE __dummyWFE   // Turn definition into a dummy name.
    
      #include <DeviceSupport/ST/STM32F10x/stm32f10x.h>
    
      #undef __WFE               // And redefine.
      static __INLINE void __WFE()   { /* host specific code */ }

      extern USART_TypeDef g_fakeDeviceUsart1;

      #undef  USART1            // Make device pointer point to normal memory. 
      #define USART1 (&g_fakeDeviceUsart1)

    #else
      #include <DeviceSupport/ST/STM32F10x/stm32f10x.h>
    #endif

---

# Example unit test

    #include <gtest/gtest.h>
    #include "mcuaccess.h"

    TEST(Test_mcuif, can_call_inline_assembly_fkns_in_host)
    {
	    // Can call functions on host.
	    __enable_irq();
	    __disable_irq();
	    __WFE();
	    // ...
    }
    TEST(Test_mcuif, can_access_fake_memory_map_of_a_device)
    {
	    USART1->CR1 = 0x0;
	    EXPECT_EQ(g_fakeDeviceUsart1.CR1, 0x0);
	    USART1->CR1 = 0x1234;
	    EXPECT_EQ(g_fakeDeviceUsart1.CR1, 0x1234);
    }

---

# Interrupt code on the host

- C/C++ have no notion of interrupts. This is fully a system defined extension. It relies on compiler support for specific hardware. 
- We need an interrupt programming model on the target, that can be ported to the host.
The model should be device agnostic. Written code should be able to outlive its hardware.
- Model the interrupts from threads. Assume interrupts will be modeled with threads on the host system.
- Interrupts priorities should be possible to model on the host using e.g. posix real-time thread extensions.

---

# Asymmetric, run-to-completion

- Interrupts are fully asymmetric. Once started, it must run to completion before a lower level interrupt can run. It can be interrupted by higher level interrupts.
- A collarly, we can't use mutexes to protect shared data, we need to prevent interrupts from starting, before accessing shared data. It is done using enable/disable interrupts.
- Code compiled both for host and target must use a protection primitive that turn into a mutex
on the host and disables interrupts on target.
- On target we still need the analog of 'acquire' and 'release' events connecting. Depending on target hardware compiler barriers might suffice.
- Do note on target, anything that can block a thread will risk to deadlock an interrupt. Atomics are fine if they are lock free. Watch out for compiler generated locks.

---

# Interrupt priorities

- Assume a model where main/threads have interrupt priority 0 and interrupts have a static level of 1 or above. Higher numerical level interrupt lower.
- Assume all interrupts are assigned levels at compile time.
- Assume all possible protected state know what interrupts will access it at compile time.
- Given this, we can determine max interrupt priority that will access a specific shared resource. Hence masking interrupts up to this level will protect the resource, without blocking higher interrupts.
- When protecting a resource all other potential accessors are blocked. Hence when restoring masking level we know we can restore it to level 0.

---

# Cortex-M3 interrupt control.

ARM uses a different interrupt priority scheme. 

- Based on an 8 bit value. But vendors can implement a subset, removing bits from LSB side. ST uses 4 bits.
- Higher numerical values implies lower interrupt priority. 

Hence values written to registers needs a conversion. It can be compile time calculated.

    // Functions from core_cm3.h

    // Read/Write interrupt priority mask.
    inline uint32_t __get_BASEPRI(void);
	inline void __set_BASEPRI(uint32_t basePri);

	// Set interrupt priority level.
	static __INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);

	// Enable/disable individual interrupts.
	static __INLINE void NVIC_EnableIRQ(IRQn_Type IRQn);
	static __INLINE void NVIC_DisableIRQ(IRQn_Type IRQn);

---

# Project specific setup.

    // Enum listing all used interrupts.
    enum class IrqS 
    {
        systick, tim2, usart1, 
        thread,                 // For use in 'cover'.
        maxNo                   // size of the interrupt array.
    };

    #define CASE(x,y,z) case x: return IrqData<IrqS>(x,y,z)
    template <>
    constexpr IrqData<IrqS> irq2Data<IrqS>(IrqS irq) 
    {
        switch(irq) {
        // 1: Enum value for interrupt.
        // 2: STM32 identifier for interrupt.
        // 3: Interrupt level. 1, lowest, 15 highest.
        CASE(IrqS::tim2,    Tim2_IRQn,           3);
        CASE(IrqS::usart1,  Usart1_IRQn,         2);
        CASE(IrqS::systick, SysTick_IRQn,        1);
        CASE(IrqS::thread,  NonMaskableInt_IRQn, 0); // Dummy IRQ value.
        }
    }

---

# SharedResource

Implement the analog of a mutex. Take a compile time list of interrupts that can lock this resource.

    template <typename EnumType_, EnumType_ handler, EnumType_... handlers>
    class IrqList;

    template <typename IrqList_>
    class SharedResource
    {
      public:
        using IrqList = IrqList_;
        using EnumType = typename IrqList::EnumType;

        // Priority level to raise the irq level to during critical sections.
        static const constexpr int protectPri = IrqList::maxPri;

        // Return true if a particular thread/irq is part of of the given 
        // static set.
        constexpr static bool inSet(EnumType queryVal) {
            return IrqList::inSet(queryVal);
        }
    };

---

# Cover and example usage

Use a class 'Cover' as analog of 'mutex_lock'.  
Protect on construction and restore on destruction.
Template argument ties to shared resource and indicate what is calling it.

    // Example from systick timer. Protect a sharead systick counter.
    using ShRes =
      SharedResource<IrqList<IrqS, IrqS::systick, IrqS::thread>>;
    uint32_t m_systick{0};    

    // Access functions, 
    void TimeSource::systickIsr()
    {
        Cover<ShRes, IrqSource::systick> c;
        m_systick++;
    }
    uint32_t TimeSource::systick() const
    {
        Cover<ShRes, IrqSource::thread> c;
        return m_systick;
    }

---

# Example assembly.

    08000eb0 <TimeSource::systickIsr()>:
     8000eb0:	68c3      	ldr	r3, [r0, #12]
     8000eb2:	3301      	adds	r3, #1
     8000eb4:	60c3      	str	r3, [r0, #12]
     8000eb6:	4770      	bx	lr

    08000ebe <TimeSource::systick() const>:
     8000ebe:	23f0      	movs	r3, #240	; 0xf0
     8000ec0:	f383 8811 	msr	BASEPRI, r3
     8000ec4:	68c0      	ldr	r0, [r0, #12]
     8000ec6:	2300      	movs	r3, #0
     8000ec8:	f383 8811 	msr	BASEPRI, r3
     8000ecc:	4770      	bx	lr

---

# Cover implementation

    template <typename SharedResource, IrqS callIrq>
    class Cover
    {
      public:
        static const constexpr int callLevel = irq2Data(callIrq).level;
        static const constexpr int protectLevel = SharedResource::protectLevel;
        Cover() 
        {
            static_assert(SharedResource::inSet(callIrq), "");
            if (callLevel != protectLevel) 
                setCortexPri(irq2BasepriLevel<protectLevel>());
            std::atomic_signal_fence(std::memory_order_seq_cst);
        }
        ~Cover()
        {
            std::atomic_signal_fence(std::memory_order_seq_cst);
            if (callLevel != protectLevel)
                setCortexPri(irq2BasepriLevel<0>());
        }
    };

---

# Notes on atomics

Standard atomics can be used for primitive types _if_ they are lock free. It is true
for Cortex-M3 but not for e.g. Cortex-M0. 
But looking at the assembly from atomic accesses, it is very verbose.
The solution above is much more compact.

---

# Interrupt handler functions

The entry functions for interrupts are a highly system/compiler dependent.

    // Example: Handler function for the systick interrupt. 
    // (For Cortex-M, a normal C function)
    extern "C" void SysTick_IRQHandler(void) { /* ... */ }


- From the C++ standard point of view, these functions are root-nodes in in calling tree. We must
  convince the compiler that they can be called, even though no other function calls them.
With link-time optimization the toolchain gets a whole-program view and just hiding it in a .cpp 
is not enough anymore. 
- After C++11, the compiler must be ready for other threads to call externally visible functions so it should suffice to show it is externally accessible.
- In our case, they are refered in the startup assembly file in the interrupt vector table and
it seems to be enough for gcc/Cortex-M.

---

# Interrupt handler functions.

Calling e.g. a member function require some kind of data pointer, making the raw handler awkward.
We let the handler call a 'delegate' and then drivers can register their function callbacks as needed.

    // Heavy slideware...
    delegate<void()> isrTable[ IrqS::maxNo ];
    
    #define MK_IRQ_FKN(handler, FknName)                      \
        extern "C" void FknName##_IRQHandler(void)            \
        {                                                     \
            isrTable[ static_cast<int>(handler) ]();          \
        }

    MK_IRQ_FKN(IrqS::systick, SysTick)
    MK_IRQ_FKN(IrqS::tim2,    TIM2)
    MK_IRQ_FKN(IrqS::usart1,  USART1)

    // Example registration in systick driver.
    TimeSource::TimeSource() {
        isrTable[ int(IrqS::systick) ].set<&TimeSource::systickIsr>(*this);
    }

---

# Assembly for systick irq call

    08000f84 <SysTick_Handler>:
    
     8000f84: 4801        ldr r0, [pc, #4]  ; (8000f8c <SysTick_Handler+0x8>)
     8000f86: f850 3c04   ldr.w r3, [r0, #-4]
     8000f8a: 4718        bx  r3
     8000f8c: 20000004  andcs r0, r0, r4


    08000ebc <void details::common<void ()>::doMemberCB<TimeSource,
      &TimeSource::systickIsr>(details::common<void ()>::DataPtr const&)>:
    
     8000ebc: 6800        ldr r0, [r0, #0]
     8000ebe: f7ff bff9   b.w 8000eb4 <TimeSource::systickIsr()>


    08000eb4 <TimeSource::systickIsr()>:

     8000eb4: 68c3        ldr r3, [r0, #12]
     8000eb6: 3301        adds  r3, #1
     8000eb8: 60c3        str r3, [r0, #12]
     8000eba: 4770        bx  lr

---

# Summary so far

- It is possible to get the ST supplied header files to compile on the host.
  The code needed is far from ideal, but get the job done and is localized to one file.
- You can unit test all the way down to driver level. However, at some point the tests
  gets trivial so a testing boundary needs to be established.
- A model for interrupt code is presented that can be turned into thread based code on the host.
  It also lifts us from outdated practices using volatile into a more common mutex like workflow.
- With interrupt priorities set at compile time, it is possible to get the shared data protection to automatically adapt to needed protection measures.
- Using delegates for interrupt handlers opens up much design freedom with a very modest runtime cost.


