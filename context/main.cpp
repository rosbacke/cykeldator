#include "context.h"


static char stack1[ 500 ];

extern "C" void _init(void)
{
}

using namespace context::detail;

struct User
{
	fcontext_t (*fkn)(void* v);
};



static transfer_t context_exit( transfer_t)
{
	// Called 'onTop' of the caller stack. Clean up and return to main stack.
	return { nullptr, nullptr };
}

// Set up 
static void context_entry(transfer_t t) noexcept
{
	// Set up stack top.
	User u = *(User*)t.data;

	// Return back to our creator.
    t = jump_fcontext( t.fctx, nullptr);

    // This time, we actually start the function.
    t.fctx = (u.fkn)(t.data);

    // We are done with this fiber. This call should not return.
    // Pass on given context. After 'context_exit' it will be called
    // with nullptr as return value.
    ontop_fcontext( t.fctx, &u, context_exit);
}

int main()
{
  fcontext_t ctx;

  ctx = make_fcontext( stack1, sizeof stack1, context_entry);

  transfer_t t1;
  User u;
  t1 = jump_fcontext( ctx, (void*)&u);
}
