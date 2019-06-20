#include "context.h"

using namespace context::detail;

// Stack switching happens in 'jump' and 'ontop' functions. The 'make' only needs to prepare the stack
// for an upcoming switch.

// Reference C ARM calling convention: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf


// Both 'jump' and 'ontop' return a composite type. In Cortex-M calling convention this result in
// the caller allocating stack space and a pointer is passed in R0. Hence return values should
// be written to the address stored in 'R0'.

// Upon entry to a new function, register LR contain the return address, PC the current address.
// -> Storing registers, switching stack and restoring registers (Including LR) means we get the correct
// return address.


// Invariants for valid sleeping contexts: (work in progress)
// ctx points to lowest occupied stack entry.
// It contains the function to call on next invocation.

extern "C" fcontext_t make_fcontext( void * sp, std::size_t size, void (* fn)( transfer_t) )
{
	uint32_t* stackTop = (uint32_t*)((uint32_t)sp + size);
	uint32_t* currSP = stackTop;
	currSP -= 7; // Reserve space where registers are saved.
	currSP[1] =(uint32_t)fn; // Store fn function pointer as new LR to be restored.
	return fcontext_t{ currSP };
}

//extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp) __attribute__((naked));
extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp)
{
	// Save registers.
	// 	Save r0 to know where return value should be written to.
	asm("push	{r0, r4, r5, r6, r7, r8, lr}");

	// Switch stack.
	asm("mrs r3, psp");
	asm("msr psp, r1");

	// Write return values for the new function.

	// Restore registers.
	asm("pop	{r0, r4, r5, r6, r7, r8, lr}");

	asm("str	r3, [r0], #0");
	asm("str	r2, [r0], #4");

	// Return by jumping to link register.
	asm("bx lr");
	return transfer_t{0,0};

#if 0
	// Save registers.
	uint32_t* newSP = (uint32_t*)to;
	uint32_t* currSP = /* getSP */0; // Must be stored in register.
	/* setSP( newSP ); */
	// Restore registers.
	// Populate return values to R0.
	return transfer_t{ currSP, vp };
#endif
}

//extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) ) __attribute__((__noreturn__));
extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) )
{
	// Save registers.
	// 	Save r0 to know where return value should be written to.
	asm("push	{r0, r4, r5, r6, r7, r8, lr}");

	// Switch stack.
	asm("mrs r4, psp");
	asm("msr psp, r1");
	asm("mov r4, r1");

	// Restore registers.
	asm("pop	{r0, r4, r5, r6, r7, r8, lr}");

	// Now, the transfer_t is passed on to the new function.
	// r2 already correct transfer_t.data.
	// r1 already correct transfer_t.ctx.
	// r0 already correct for return value.
	// Need prev. context written into transfer_t.ctx.
	asm("bx  r3");

	asm("nop");
	return transfer_t{0,0};
}
