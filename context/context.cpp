#include "context.h"

using namespace context::detail;

extern "C" fcontext_t make_fcontext( void * sp, std::size_t size, void (* fn)( transfer_t) )
{

}


extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp)
{

}


extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) )
{

}
