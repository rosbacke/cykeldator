#include "context.h"
#include "../mcu_src/mcuaccess.h"

extern "C" void _init(void)
{
}

using namespace context::detail;

struct User
{
	void (*fkn)(void* v);
};

static transfer_t context_exit( transfer_t)
{
	// Called 'onTop' of the caller stack. Clean up and return to main stack.
	return { nullptr, nullptr };
}

// Set up a function to be called initially on the new stack.
// Note, this function should never exit.
// t.fctx : context of the old stack we were called from.
// t.data : void ptr passed from the previous stack.
static void context_entry(transfer_t t) noexcept
{
	// Set up stack top.
	User u = *(User*)t.data;

	// Return back to our creator.
    t = jump_fcontext( t.fctx, nullptr);

    // This time, we actually start the function.
    (u.fkn)(t.data);

    // We are done with this fiber. This call should not return.
    // Pass on given context. After 'context_exit' it will be called
    // with nullptr as return value.
    ontop_fcontext( t.fctx, &u, context_exit);
}

static fcontext_t createContext(void* stack, uint32_t stackSize, void (*fkn)(void* v))
{
	User u;
	u.fkn = fkn;
	fcontext_t ctx = make_fcontext( stack, stackSize, context_entry);
	transfer_t t1 = jump_fcontext( ctx, (void*)&u);
	return t1.fctx;
}

static fcontext_t switchContext(fcontext_t ctx)
{
	transfer_t t1 = jump_fcontext( ctx, nullptr);
	return t1.fctx;
}


static void testFiber(void* v)
{
	while(1);
}

void setLed(bool on)
{
    if (on)
        GPIOC->ODR &= ~uint32_t(1 << 13);
    else
        GPIOC->ODR |= 1 << 13;
}




static void pass()
{
	GPIOB->ODR = 0xf00;
	while(1);
}

static void fail()
{
	GPIOB->ODR = 0xa500;
	while(1);
}

#define VERIFY( x ) do { if (!(x)) fail(); } while(0)

static void setup()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRH = 0x11111111;
	GPIOB->ODR = 0x0;
}

static int testData = 0;
static int testData2 = 0;

void dummyEntry( transfer_t t)
{
	VERIFY(t.data == &testData);
	testData++;
	jump_fcontext(t.fctx, &testData2);
	while(1)
		;
	fail();
}

void can_set_up_a_stack()
{
	static char stack[ 64 ];
	fcontext_t fct = make_fcontext( stack, sizeof stack, dummyEntry);
	VERIFY(true);
	VERIFY(fct >= stack && fct < stack + sizeof stack);

	uint32_t* t = (uint32_t*)fct;
	VERIFY(t[5] == (uint32_t)dummyEntry);

}

void can_jump_to_stack()
{
	testData = 0;
	static char stack[ 64 ];
	fcontext_t fct = make_fcontext( stack, sizeof stack, dummyEntry);

	auto t = jump_fcontext(fct, &testData);
	VERIFY(testData == 1);
	VERIFY(t.data == &testData2);
	//fcontext_t fct = make_fcontext( stack, sizeof stack, dummyEntry);
}

static char stack1[ 500 ];

int main()
{
	setup();

	can_set_up_a_stack();
	can_jump_to_stack();
	pass();
	while(1)
		;

  fcontext_t ctx = createContext(&stack1, sizeof stack1, testFiber);

  // Actually run the function.
  ctx = switchContext( ctx );
}
