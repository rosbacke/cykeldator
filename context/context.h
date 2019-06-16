#pragma once

#include <cstdint>


namespace context {
namespace detail {

typedef void*   fcontext_t;

struct transfer_t {
    fcontext_t  fctx;
    void    *   data;
};



/**
 * Given a fcontext_t pointer 'to', jump over to it and resume execution 
 * on it. On the receiving side, return 'transfer_t' describing the 
 * stack we came from.
 */ 
extern "C" transfer_t jump_fcontext( fcontext_t const to, void * vp);

/**
 * Given a pointer to a stack buffer, prepare it so that it can be 
 * switched to. Upon switch, 'fn' will be called.
 */
extern "C" fcontext_t make_fcontext( void * sp, std::size_t size, void (* fn)( transfer_t) );


/**
 * Given a specific context_t 'to' to start, switch over to that function,
 * but run 'fn'. Fn receives a transfer_t representing the current stack.
 * once 'fn' returns we end up in the original return point of the 'to'
 * context. The 'transfer_t' returned by 'fn' will be the return value
 * from the latest jump_context call.
 */
extern "C" transfer_t ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) );

}}
