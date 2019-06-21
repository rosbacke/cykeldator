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
// It contains the place to return to on next jump. (LR)

extern "C" transfer_t topFkn( fcontext_t const to, void * vp)
{
	asm("ldr	r1, [r0, #4]");
	asm("ldr	r0, [r0, #0]");
	asm("bx r8");
}

extern "C" fcontext_t make_fcontext( void * sp, std::size_t size, void (* fn)( transfer_t) )
{
	// Initial context sets up a function that get the incoming values via parameters.
	// The normal 'jump_context' assumes they are passed via return value.
	// We use 'topFkn' as an adapter to move the returned values into arguments and
	// then call 'fn'.

	uint32_t* stackTop = (uint32_t*)((uint32_t)sp + size);
	uint32_t* currSP = stackTop;
	uint32_t* r0 = currSP + 1; // Set up r1 to point into the stack where r4 is stored. r4,r5 will be used for return value.
	currSP -= 7; // Reserve space where registers are saved.
	currSP[6] =(uint32_t)topFkn; // Need a small adapter to move return value into fkn arguments.
	currSP[5] =(uint32_t)fn; // Store fn function pointer as r8.
	currSP[0] = (uint32_t)r0;
	return fcontext_t{ currSP };
}

//extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp) __attribute__((naked));
extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp)
{
	// Save registers.
	// 	Save r0 to know where return value should be written to.
	asm("push	{r0, r4, r5, r6, r7, r8, lr}");

	// Switch stack.
	asm("mrs r3, msp");
	asm("msr msp, r1");

	// Write return values for the new function.

	// Restore registers.
	asm("pop	{r0, r4, r5, r6, r7, r8, lr}");

	asm("str	r3, [r0, #0]");
	asm("str	r2, [r0, #4]");

	// Return by jumping to link register.
	asm("bx lr");
	return transfer_t{0,0};
}

//extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) ) __attribute__((__noreturn__));
extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) )
{
	// Save registers.
	// 	Save r0 to know where return value should be written to.
	asm("push	{r0, r4, r5, r6, r7, r8, lr}");

	// Switch stack.
	asm("mrs r4, msp"); // TODO: switch to user space stack.
	asm("msr msp, r1"); // TODO: switch to user space stack.
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
