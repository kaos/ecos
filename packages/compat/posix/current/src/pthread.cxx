//==========================================================================
//
//      pthread.cxx
//
//      POSIX pthreads implementation
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg
// Contributors:        nickg, jlarmour
// Date:                2000-03-27
// Purpose:             POSIX pthread implementation
// Description:         This file contains the implementation of the POSIX pthread
//                      functions.
//              
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>
#include <pkgconf/isoinfra.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include "pprivate.h"                   // POSIX private header

#include <stdlib.h>                     // malloc(), free()

#include <cyg/kernel/sched.hxx>        // scheduler definitions
#include <cyg/kernel/thread.hxx>       // thread definitions
#include <cyg/kernel/mutex.hxx>        // mutex definitions
#include <cyg/kernel/clock.hxx>        // clock definitions

#include <cyg/kernel/sched.inl>        // scheduler inlines

//-----------------------------------------------------------------------------
// First check that the configuration contains the elements we need

#ifndef CYGPKG_KERNEL
#error POSIX pthread need eCos kernel
#endif

#ifndef CYGSEM_KERNEL_SCHED_MLQUEUE
#error POSIX pthreads need MLQ scheduler
#endif

#ifndef CYGSEM_KERNEL_SCHED_TIMESLICE
#error POSIX pthreads need timeslicing
#endif

#ifndef CYGVAR_KERNEL_THREADS_DATA
#error POSIX pthreads need per-thread data
#endif

//-----------------------------------------------------------------------------
// Internal definitions

// Handle entry to a pthread package function. 
#define PTHREAD_ENTRY() CYG_REPORT_FUNCTYPE( "returning %d" )

// Handle entry to a pthread package function with no args. 
#define PTHREAD_ENTRY_VOID() CYG_REPORT_FUNCTION()

// Do a pthread package defined return. This requires the error code to be
// returned as the result of the function. This also gives us a place to
// put any generic tidyup handling needed for things like signal delivery
// and cancellation.
#define PTHREAD_RETURN(err)                     \
CYG_MACRO_START                                 \
    CYG_REPORT_RETVAL( err );                   \
    return err;                                 \
CYG_MACRO_END

// A void variant of the above.
#define PTHREAD_RETURN_VOID                     \
CYG_MACRO_START                                 \
    CYG_REPORT_RETURN();                        \
    return;                                     \
CYG_MACRO_END

// Check that a pointer passed in as an argument is valid and return
// EINVAL if it is not. This should be used to check pointers that are
// required to be valid. Pointers that may optionally be NULL should
// be checked within the function.
#define PTHREAD_CHECK(ptr) if( (ptr) == NULL ) PTHREAD_RETURN(EINVAL);


//=============================================================================
// Internal data structures

// Mutex for controlling access to shared data structures
Cyg_Mutex pthread_mutex CYGBLD_POSIX_INIT;

// Array of pthread control structures. A pthread_t object is
// "just" an index into this array.
static pthread_info *thread_table[CYGNUM_POSIX_PTHREAD_THREADS_MAX];

// Count of number of threads in table.
static int pthread_count = 0;

// Count of number of threads that have exited and not been reaped.
static int pthreads_exited;

// Count of number of threads that are waiting to be joined
static int pthreads_tobejoined;

// Per-thread key allocation. This key map has a 1 bit set for each
// key that is free, zero if it is allocated.
#define KEY_MAP_TYPE cyg_uint32
#define KEY_MAP_TYPE_SIZE (sizeof(KEY_MAP_TYPE)*8) // in BITS!
static KEY_MAP_TYPE thread_key[PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE];
static void (*key_destructor[PTHREAD_KEYS_MAX]) (void *);
    
// Index of next pthread_info to allocate from thread_table array.
static int thread_info_next = 0;

// This is used to make pthread_t values unique even when reusing
// a table slot. This allows CYGNUM_POSIX_PTHREAD_THREADS_MAX to range
// up to 1024.
#define THREAD_ID_COOKIE_INC 0x00000400
#define THREAD_ID_COOKIE_MASK (THREAD_ID_COOKIE_INC-1)
static pthread_t thread_id_cookie = THREAD_ID_COOKIE_INC;

//-----------------------------------------------------------------------------
// Main thread.

static char main_stack[CYGNUM_POSIX_MAIN_DEFAULT_STACK_SIZE];

// Thread ID of main thread.
static pthread_t main_thread;

//=============================================================================
// Exported variables

int pthread_canceled;           // pointed to by PTHREAD_CANCELED

//=============================================================================
// Internal functions

//-----------------------------------------------------------------------------
// Private version of pthread_self() that returns a pointer to our internal
// control structure.

pthread_info *pthread_self_info(void)
{
    Cyg_Thread *thread = Cyg_Thread::self();

    CYG_CHECK_DATA_PTR(thread, "Illegal current thread");
    
    pthread_info *info = (pthread_info *)thread->get_data(CYGNUM_KERNEL_THREADS_DATA_POSIX);

//    CYG_CHECK_DATA_PTR(info, "Not a POSIX thread!!!");

    return info;
}

externC pthread_info *pthread_info_id( pthread_t id )
{
    pthread_t index = id & THREAD_ID_COOKIE_MASK;

    pthread_info *info = thread_table[index];

    // Check for a valid entry
    if( info == NULL )
        return NULL;
    
    // Check that this is a valid entry
    if ( info->state == PTHREAD_STATE_FREE ||
         info->state == PTHREAD_STATE_EXITED )
        return NULL;

    // Check that the entry matches the id
    if( info->id != id ) return NULL;

    // Return the pointer
    return info;
}

//-----------------------------------------------------------------------------
// new operator to allow us to invoke the Cyg_Thread constructor on the
// pthread_info.thread_obj array.

inline void *operator new(size_t size,  cyg_uint8 *ptr) { return (void *)ptr; };

//-----------------------------------------------------------------------------
// Optional memory allocation functions for pthread stacks.
// If there is an implementation of malloc() available, define pthread_malloc()
// and pthread_free() to use it. Otherwise define them to do nothing.
// In the future we may want to add configuration here to permit thread stacks
// to be allocated in a nominated memory pool separate from the standard malloc()
// pool. Hence the (currently redundant) encapsulation of these functions.

#if CYGINT_ISO_MALLOC

static __inline__ CYG_ADDRWORD pthread_malloc( CYG_ADDRWORD size )
{
    return (CYG_ADDRWORD)malloc( size );
}

static __inline__ void pthread_free( CYG_ADDRWORD m )
{
    free( (void *)m );
}

#define PTHREAD_MALLOC

#else

#define pthread_malloc(_x_) (0)

#define pthread_free(_x_)

#endif

//-----------------------------------------------------------------------------
// pthread entry function.
// does some housekeeping and then calls the user's start routine.

static void pthread_entry(CYG_ADDRWORD data)
{
    pthread_info *self = (pthread_info *)data;

    void *retval = self->start_routine(self->start_arg);

    pthread_exit( retval );
}

//-----------------------------------------------------------------------------
// Main entry function.
// This is set as the start_routine of the main thread. It invokes main()
// and if it returns, shuts down the system.

externC void cyg_libc_invoke_main( void );

static void *call_main( void * )
{
    cyg_libc_invoke_main();
    return NULL; // placate compiler
}

//-----------------------------------------------------------------------------
// POSIX ASR
// This is installed as the ASR for all POSIX threads.

static void posix_asr( CYG_ADDRWORD data )
{
    pthread_info *self = (pthread_info *)data;

    // Call into timer subsystem to deliver any pending
    // timer expirations.
    cyg_posix_timer_asr(self);
    
    // Call signal subsystem to deliver any signals
    cyg_posix_signal_asr(self);
    
    // Check for cancellation
    if( self->cancelpending &&
        self->cancelstate == PTHREAD_CANCEL_ENABLE &&
        self->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS )
    {
        // If we have a pending cancellation, cancellations are
        // enabled and we are in asynchronous mode, then we can do the
        // cancellation processing.  Since pthread_exit() does
        // everything we need to do, we just call that here.
        
        pthread_exit(PTHREAD_CANCELED);
    }
}

//-----------------------------------------------------------------------------
// The (Grim) Reaper.
// This function is called to tidy up and dispose of any threads that have
// exited. This work must be done from a thread other than the one exiting.
// Note: this function _must_ be called with pthread_mutex locked.

static void pthread_reap()
{
    int i;

    // Loop over the thread table looking for exited threads. The
    // pthreads_exited counter springs us out of this once we have
    // found then all (and keeps us out if there are none to do).
   
    for( i = 0; pthreads_exited && i < CYGNUM_POSIX_PTHREAD_THREADS_MAX ; i++ )
    {
        pthread_info *thread = thread_table[i];

        if( thread != NULL && thread->state == PTHREAD_STATE_EXITED )
        {
            // The thread has exited, so it is a candidate for being
            // reaped. We have to make sure that the eCos thread has
            // also reached EXITED state before we can tidy it up.

            while( thread->thread->get_state() != Cyg_Thread::EXITED )
            {
                // The eCos thread has not yet exited. This is
                // probably because its priority is too low to allow
                // it to complete.  We fix this here by raising its
                // priority to equal ours and then yielding. This
                // should eventually get it into exited state.

                Cyg_Thread *self = Cyg_Thread::self();

                // Set thread's priority to our current dispatching priority.
                thread->thread->set_priority( self->get_current_priority() );

                // Yield, yield
                self->yield();
    
                // and keep looping until he exits.
            }

            // At this point we have a thread that we can reap.

            // destroy the eCos thread
            thread->thread->~Cyg_Thread();

            // destroy the joiner condvar
            thread->joiner->~Cyg_Condition_Variable();

            // Destroy signal handling fields
            cyg_posix_thread_sigdestroy( thread );
            
            // Free the stack if we allocated it
            if( thread->freestack )
                pthread_free( thread->stackmem );

            // Finally, set the thread table entry to NULL so that it
            // may be reused.
            thread_table[i] = NULL;

            pthread_count--;
            pthreads_exited--;
        }
    }
}

//=============================================================================
// Functions exported to rest of POSIX subsystem.

//-----------------------------------------------------------------------------
// Create the main() thread.

externC void cyg_posix_pthread_start( void )
{

    // Initialize the per-thread data key map.

    for( cyg_ucount32 i = 0; i < (PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE); i++ )
    {
        thread_key[i] = ~0;
    }
    
    // Create the main thread
    pthread_attr_t attr;
    struct sched_param schedparam;

    schedparam.sched_priority = CYGNUM_POSIX_MAIN_DEFAULT_PRIORITY;

    pthread_attr_init( &attr );
    pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    pthread_attr_setstackaddr( &attr, &main_stack[CYGNUM_POSIX_MAIN_DEFAULT_STACK_SIZE] );
    pthread_attr_setstacksize( &attr, CYGNUM_POSIX_MAIN_DEFAULT_STACK_SIZE );
    pthread_attr_setschedpolicy( &attr, SCHED_RR );
    pthread_attr_setschedparam( &attr, &schedparam );
    
    pthread_create( &main_thread, &attr, call_main, NULL );    
}

//-----------------------------------------------------------------------------
// Look for a thread that can accept delivery of any of the signals in
// the mask and release it from any wait it is in.  Since this may be
// called from a DSR, it cannot use any locks internally - any locking
// should be done before the call.

externC void cyg_posix_pthread_release_thread( sigset_t *mask )
{
    int i;
    int count = pthread_count;
    
    // Loop over the thread table looking for a thread that has a
    // signal mask that does not mask all the signals in mask.
    // FIXME: find a more efficient way of doing this.
    
    for( i = 0; count > 0 && i < CYGNUM_POSIX_PTHREAD_THREADS_MAX ; i++ )
    {
        pthread_info *thread = thread_table[i];

        if( (thread != NULL) &&
            (thread->state == PTHREAD_STATE_RUNNING) &&
            ((*mask & ~thread->sigmask) != 0) )
        {
            // This thread can service at least one of the signals in
            // *mask. Knock it out of its wait and make its ASR pending.

            thread->thread->set_asr_pending();
            thread->thread->release();
            break;
        }

        if( thread->state != PTHREAD_STATE_FREE )
            count--;
    }
}

//=============================================================================
// General thread operations

//-----------------------------------------------------------------------------
// Thread creation and management.

// Create a thread.
externC int pthread_create ( pthread_t *thread,
                             const pthread_attr_t *attr,
                             void *(*start_routine) (void *),
                             void *arg)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK(thread);
    PTHREAD_CHECK(start_routine);

    pthread_info *self = pthread_self_info();
    
    pthread_attr_t use_attr;

    // Set use_attr to the set of attributes we are going to
    // actually use. Either those passed in, or the default set.
   
    if( attr == NULL )
        pthread_attr_init( &use_attr );
    else use_attr = *attr;

    // Adjust the attributes to cope with the setting of inheritsched.

    if( use_attr.inheritsched == PTHREAD_INHERIT_SCHED )
    {
        use_attr.schedpolicy = self->attr.schedpolicy;
        use_attr.schedparam  = self->attr.schedparam;
    }

    CYG_ADDRWORD stackbase, stacksize;
    cyg_bool freestack = false;
    CYG_ADDRWORD stackmem = 0;
    
    // If the stack size is not valid, we can assume that it is at
    // least PTHREAD_STACK_MIN bytes.
        
    if( use_attr.stacksize_valid )
        stacksize = use_attr.stacksize;
    else stacksize = PTHREAD_STACK_MIN;

    if( use_attr.stackaddr_valid )
    {
        // Set up stack base and size from supplied arguments.

        // Calculate stack base from address and size.
        // FIXME: Falling stack assumed in pthread_create().
        stackmem = stackbase = (CYG_ADDRWORD)use_attr.stackaddr-stacksize;
    }
    else
    {
#ifdef PTHREAD_MALLOC

        stackmem = stackbase = pthread_malloc( stacksize );

        if( stackmem == 0 )
            PTHREAD_RETURN( EAGAIN );

        freestack = true;
#else        
        PTHREAD_RETURN(EINVAL);
#endif        
        
    }

    // Get sole access to data structures
    
    pthread_mutex.lock();
    
    // Dispose of any dead threads
    pthread_reap();
    
    // Find a free slot in the thread table
    
    pthread_info *nthread;
    int thread_next = thread_info_next;
    
    while( thread_table[thread_next] != NULL )
    {
        thread_next++;
        if( thread_next >= CYGNUM_POSIX_PTHREAD_THREADS_MAX )
            thread_next = 0;

        // check for wrap, and return error if no slots left
        if( thread_next == thread_info_next )
        {
            pthread_mutex.unlock();
            if( freestack )
                pthread_free( stackmem );
            PTHREAD_RETURN(ENOMEM);
        }
    }

    nthread = (pthread_info *)stackbase;

    stackbase += sizeof(pthread_info);
    stacksize -= sizeof(pthread_info);
    
    thread_table[thread_next] = nthread;

    // Set new next index
    thread_info_next = thread_next;
    
    // step the cookie
    thread_id_cookie += THREAD_ID_COOKIE_INC;

    // Initialize the table entry
    nthread->state              = use_attr.detachstate == PTHREAD_CREATE_JOINABLE ?
                                  PTHREAD_STATE_RUNNING : PTHREAD_STATE_DETACHED;
    nthread->id                 = thread_next+thread_id_cookie;
    nthread->attr               = use_attr;
    nthread->retval             = 0;
    nthread->start_routine      = start_routine;
    nthread->start_arg          = arg;

    nthread->freestack          = freestack;
    nthread->stackmem           = stackmem;
    
    nthread->cancelstate        = PTHREAD_CANCEL_ENABLE;
    nthread->canceltype         = PTHREAD_CANCEL_DEFERRED;
    nthread->cancelbuffer       = NULL;
    nthread->cancelpending      = false;

    nthread->thread_data        = NULL;
    
#ifdef CYGVAR_KERNEL_THREADS_NAME    
    // generate a name for this thread

    char *name = nthread->name;
    static char *name_template = "pthread.00000000";
    pthread_t id = nthread->id;
    
    for( int i = 0; name_template[i]; i++ ) name[i] = name_template[i];

    // dump the id, in hex into the name.
    for( int i = 15; i >= 8; i-- )
    {
        name[i] = "0123456789ABCDEF"[id&0xF];
        id >>= 4;
    }

#endif

    // Initialize the joiner condition variable

    nthread->joiner = new(nthread->joiner_obj) Cyg_Condition_Variable( pthread_mutex );

    // Initialize signal specific fields.
    cyg_posix_thread_siginit( nthread );

    // create the underlying eCos thread

    nthread->thread = new(&nthread->thread_obj[0])
        Cyg_Thread ( PTHREAD_ECOS_PRIORITY(use_attr.schedparam.sched_priority),
                     pthread_entry,
                     (CYG_ADDRWORD)nthread,
                     name,
                     stackbase,
                     stacksize);

    // Put pointer to pthread_info into eCos thread's per-thread data.
    nthread->thread->set_data( CYGNUM_KERNEL_THREADS_DATA_POSIX, (CYG_ADDRWORD)nthread );

    // Set timeslice enable according to scheduling policy.
    if( use_attr.schedpolicy == SCHED_FIFO )
         nthread->thread->timeslice_disable();
    else nthread->thread->timeslice_enable();

    // set up ASR and data
    nthread->thread->set_asr( posix_asr, (CYG_ADDRWORD)nthread, NULL, NULL );    
    
    // return thread ID
    *thread = nthread->id;

    pthread_count++;
                
    pthread_mutex.unlock();

    // finally, set the thread going
    nthread->thread->resume();
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get current thread id.

externC pthread_t pthread_self ( void )
{
    PTHREAD_ENTRY();
    
    pthread_info *info = pthread_self_info();

    CYG_CHECK_DATA_PTR(info, "Not a POSIX thread!!!");
    
    return info->id;
}

//-----------------------------------------------------------------------------
// Compare two thread identifiers.

externC int pthread_equal (pthread_t thread1, pthread_t thread2)
{
    PTHREAD_ENTRY();
    
    return thread1 == thread2;
}

//-----------------------------------------------------------------------------
// Terminate current thread.

externC void exit(int) CYGBLD_ATTRIB_NORET;

externC void pthread_exit (void *retval)
{
    PTHREAD_ENTRY();
    
    pthread_info *self = pthread_self_info();

    // Call cancellation handlers. We eat up the buffers as we go in
    // case any of the routines calls pthread_exit() itself.
    while( self->cancelbuffer != NULL )
    {
        struct pthread_cleanup_buffer *buffer = self->cancelbuffer;

        self->cancelbuffer = buffer->prev;

        buffer->routine(buffer->arg);
    }

    if( self->thread_data != NULL )
    {
        // Call per-thread key destructors.
        // The specification of this is that we must continue to call the
        // destructor functions until all the per-thread data values are NULL or
        // we have done it PTHREAD_DESTRUCTOR_ITERATIONS times.
    
        cyg_bool destructors_called;
        int destructor_iterations = 0;

        do
        {
            destructors_called = false;
        
            for( cyg_ucount32 key = 0; key < PTHREAD_KEYS_MAX; key++ )
            {
                // Skip unallocated keys
                if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
                    continue;

                // Skip NULL destructors
                if( key_destructor[key] == NULL ) continue;

                // Skip NULL data values
                if( self->thread_data[key] == NULL ) continue;

                // If it passes all that, call the destructor
                key_destructor[key]((void *)(self->thread_data[key]));

                // Record that we called a destructor
                destructors_called = true;
            }

            // Count the iteration
            destructor_iterations++;
        
        } while( destructors_called &&
                 (destructor_iterations <= PTHREAD_DESTRUCTOR_ITERATIONS));

    }
    
    pthread_mutex.lock();

    // Set the retval for any joiner
    self->retval = retval;

    // If we are already detached, go to EXITED state, otherwise
    // go into JOIN state.
    
    if ( PTHREAD_STATE_DETACHED == self->state ) {
        self->state = PTHREAD_STATE_EXITED;
        pthreads_exited++;
    } else {
        self->state = PTHREAD_STATE_JOIN;
        pthreads_tobejoined++;
    }

    // Kick any waiting joiners
    self->joiner->broadcast();

    cyg_bool call_exit=false;

    // if this is the last thread (other than threads waiting to be joined)
    // then we need to call exit() later
    if ( pthreads_exited + pthreads_tobejoined == pthread_count )
        call_exit=true;

    pthread_mutex.unlock();
    
    // Finally, call the exit function; this will not return.
    if ( call_exit )
        ::exit(0);
    else
        self->thread->exit();

    // This loop keeps some compilers happy. pthread_exit() is marked
    // with the noreturn attribute, and without this they generate a
    // call to abort() here in case Cyg_Thread::exit() returns. 
    
    for(;;) continue;
}

//-----------------------------------------------------------------------------
// Wait for the thread to terminate. If thread_return is not NULL then
// the retval from the thread's call to pthread_exit() is stored at
// *thread_return.

externC int pthread_join (pthread_t thread, void **thread_return)
{
    PTHREAD_ENTRY();
    
    pthread_mutex.lock();
    
    // Dispose of any dead threads
    pthread_reap();
    
    pthread_info *self = pthread_self_info();
    pthread_info *joinee = pthread_info_id( thread );

    if( joinee == NULL )
    {
        pthread_mutex.unlock();
        PTHREAD_RETURN(ESRCH);
    }

    if( joinee == self )
    {
        pthread_mutex.unlock();
        PTHREAD_RETURN(EDEADLK);
    }

    switch ( joinee->state )
    {
    case PTHREAD_STATE_RUNNING:
        // The thread is still running, we must wait for it.
        while( joinee->state == PTHREAD_STATE_RUNNING )
            joinee->joiner->wait();

        // check that the thread is still joinable
        if( joinee->state == PTHREAD_STATE_JOIN )
            break;

        // The thread has become unjoinable while we waited, so we
        // fall through to complain.

    case PTHREAD_STATE_FREE:
    case PTHREAD_STATE_DETACHED:
    case PTHREAD_STATE_EXITED:
        // None of these may be joined.
        pthread_mutex.unlock();
        PTHREAD_RETURN(EINVAL);
        
    case PTHREAD_STATE_JOIN:
        break;
    }

    // here, we know that joinee is a thread that has exited and is
    // ready to be joined.

    // Get the retval

    if( thread_return != NULL )
        *thread_return = joinee->retval;

    // set state to exited.
    joinee->state = PTHREAD_STATE_EXITED;
    pthreads_exited++;
    pthreads_tobejoined--;
    
    // Dispose of any dead threads
    pthread_reap();
    
    pthread_mutex.unlock();

    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set the detachstate of the thread to "detached". The thread then does not
// need to be joined and its resources will be freed when it exits.

externC int pthread_detach (pthread_t thread)
{
    PTHREAD_ENTRY();
    
    int ret = 0;
    
    pthread_mutex.lock();

    pthread_info *detachee = pthread_info_id( thread );
    
    if( detachee == NULL )
        ret = ESRCH;                    // No such thread
    else if( detachee->state == PTHREAD_STATE_DETACHED )
        ret = EINVAL;                   // Already detached!
    else
    {
        // Set state to detached and kick any joinees to
        // make them return.
        detachee->state = PTHREAD_STATE_DETACHED;
        detachee->joiner->broadcast();
    }
    
    // Dispose of any dead threads
    pthread_reap();
    
    pthread_mutex.unlock();

    PTHREAD_RETURN(ret);
}


//-----------------------------------------------------------------------------
// Thread attribute handling.

//-----------------------------------------------------------------------------
// Initialize attributes object with default attributes:
// detachstate          == PTHREAD_CREATE_JOINABLE
// scope                == PTHREAD_SCOPE_SYSTEM
// inheritsched         == PTHREAD_INHERIT_SCHED
// schedpolicy          == SCHED_OTHER
// schedparam           == unset
// stackaddr            == unset
// stacksize            == 0
// 

externC int pthread_attr_init (pthread_attr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);
    
    attr->detachstate                 = PTHREAD_CREATE_JOINABLE;
    attr->scope                       = PTHREAD_SCOPE_SYSTEM;
    attr->inheritsched                = PTHREAD_INHERIT_SCHED;
    attr->schedpolicy                 = SCHED_OTHER;
    attr->schedparam.sched_priority   = 0;
    attr->stackaddr_valid             = 0;    
    attr->stackaddr                   = NULL;
    attr->stacksize_valid             = 0;    
    attr->stacksize                   = 0;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy thread attributes object

externC int pthread_attr_destroy (pthread_attr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Nothing to do here...
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set the detachstate attribute

externC int pthread_attr_setdetachstate (pthread_attr_t *attr,
                                         int detachstate)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( detachstate == PTHREAD_CREATE_JOINABLE ||
        detachstate == PTHREAD_CREATE_DETACHED )
    {
        attr->detachstate = detachstate;
        PTHREAD_RETURN(0);
    }
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get the detachstate attribute
externC int pthread_attr_getdetachstate (const pthread_attr_t *attr,
                                         int *detachstate)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( detachstate != NULL )
        *detachstate = attr->detachstate;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set scheduling contention scope

externC int pthread_attr_setscope (pthread_attr_t *attr, int scope)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( scope == PTHREAD_SCOPE_SYSTEM ||
        scope == PTHREAD_SCOPE_PROCESS )
    {
        if( scope == PTHREAD_SCOPE_PROCESS )
            PTHREAD_RETURN(ENOTSUP);

        attr->scope = scope;

        PTHREAD_RETURN(0);
    }
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling contention scope

externC int pthread_attr_getscope (const pthread_attr_t *attr, int *scope)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( scope != NULL )
        *scope = attr->scope;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set scheduling inheritance attribute

externC int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( inherit == PTHREAD_INHERIT_SCHED ||
        inherit == PTHREAD_EXPLICIT_SCHED )
    {
        attr->inheritsched = inherit;

        PTHREAD_RETURN(0);
    }

    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling inheritance attribute

externC int pthread_attr_getinheritsched (const pthread_attr_t *attr,
                                          int *inherit)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( inherit != NULL )
        *inherit = attr->inheritsched;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set scheduling policy

externC int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( policy == SCHED_OTHER ||
        policy == SCHED_FIFO ||
        policy == SCHED_RR )
    {
        attr->schedpolicy = policy;

        PTHREAD_RETURN(0);
    }
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling policy

externC int pthread_attr_getschedpolicy (const pthread_attr_t *attr,
                                         int *policy)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( policy != NULL )
        *policy = attr->schedpolicy;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set scheduling parameters
externC int pthread_attr_setschedparam (pthread_attr_t *attr,
				        const struct sched_param *param)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);
    PTHREAD_CHECK(param);

    attr->schedparam = *param;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get scheduling parameters

externC int pthread_attr_getschedparam (const pthread_attr_t *attr,
                                        struct sched_param *param)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( param != NULL )
        *param = attr->schedparam;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set starting address of stack. Whether this is at the start or end of
// the memory block allocated for the stack depends on whether the stack
// grows up or down.

externC int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    attr->stackaddr       = stackaddr;
    attr->stackaddr_valid = 1;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get any previously set stack address.

externC int pthread_attr_getstackaddr (const pthread_attr_t *attr,
                                       void **stackaddr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( stackaddr != NULL )
    {
        if( attr->stackaddr_valid )
        {
            *stackaddr = attr->stackaddr;
            PTHREAD_RETURN(0);
        }
        // Stack address not set, return EINVAL.
        else PTHREAD_RETURN(EINVAL);
    }

    PTHREAD_RETURN(0);
}


//-----------------------------------------------------------------------------
// Set minimum creation stack size.

externC int pthread_attr_setstacksize (pthread_attr_t *attr,
                                       size_t stacksize)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    CYG_ASSERT( stacksize > PTHREAD_STACK_MIN, "Inadequate stack size supplied");
    
    // Reject inadequate stack sizes
    if( stacksize < PTHREAD_STACK_MIN )
        PTHREAD_RETURN(EINVAL);
        
    attr->stacksize_valid = 1;    
    attr->stacksize = stacksize;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get current minimal stack size.

externC int pthread_attr_getstacksize (const pthread_attr_t *attr,
                                       size_t *stacksize)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Reject attempts to get a stack size when one has not been set.
    if( !attr->stacksize_valid )
        PTHREAD_RETURN(EINVAL);
    
    if( stacksize != NULL )
        *stacksize = attr->stacksize;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Thread scheduling controls

//-----------------------------------------------------------------------------
// Set scheduling policy and parameters for the thread

externC int pthread_setschedparam (pthread_t thread_id,
                                   int policy,
                                   const struct sched_param *param)
{
    PTHREAD_ENTRY();

    if( policy != SCHED_OTHER &&
        policy != SCHED_FIFO &&
        policy != SCHED_RR )
        PTHREAD_RETURN(EINVAL);

    PTHREAD_CHECK(param);

    // The parameters seem OK, change the thread...
    
    pthread_mutex.lock();

    pthread_info *thread = pthread_info_id( thread_id );

    if( thread == NULL )
    {
        pthread_mutex.unlock();
        PTHREAD_RETURN(ESRCH);
    }
    
    thread->attr.schedpolicy = policy;
    thread->attr.schedparam = *param;

    if ( policy == SCHED_FIFO )
         thread->thread->timeslice_disable();
    else thread->thread->timeslice_enable();

    thread->thread->set_priority( PTHREAD_ECOS_PRIORITY( param->sched_priority ));

    pthread_mutex.unlock();
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get scheduling policy and parameters for the thread

externC int pthread_getschedparam (pthread_t thread_id,
                                   int *policy,
                                   struct sched_param *param)
{
    PTHREAD_ENTRY();

    pthread_mutex.lock();

    pthread_info *thread = pthread_info_id( thread_id );

    if( thread == NULL )
    {
        pthread_mutex.unlock();
        PTHREAD_RETURN(ESRCH);
    }

    if( policy != NULL )
        *policy = thread->attr.schedpolicy;

    if( param != NULL )
        *param = thread->attr.schedparam;
    
    pthread_mutex.unlock();
    
    PTHREAD_RETURN(0);
}


//=============================================================================
// Mutexes

//-----------------------------------------------------------------------------
// Mutex attributes manipulation functions

//-----------------------------------------------------------------------------
// Initialize attribute object

externC int pthread_mutexattr_init ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    attr->protocol      = PTHREAD_PRIO_NONE;
    attr->prioceiling   = 0;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy attribute object

externC int pthread_mutexattr_destroy ( pthread_mutexattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Nothing to do here...
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Optional functions depending on priority inversion protection options.

#if defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT)

// Set priority inversion protection protocol
externC int pthread_mutexattr_setprotocol ( pthread_mutexattr_t *attr,
                                            int protocol)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    switch( protocol )
    {
    case PTHREAD_PRIO_NONE:
#if defined(_POSIX_THREAD_PRIO_INHERIT)        
    case PTHREAD_PRIO_INHERIT:
#endif
#if defined(_POSIX_THREAD_PRIO_PROTECT)
    case PTHREAD_PRIO_PROTECT:
#endif        
        attr->protocol = protocol;
        PTHREAD_RETURN(0);

    default:
        PTHREAD_RETURN(EINVAL);
    }
    
    PTHREAD_RETURN(0);
}

// Get priority inversion protection protocol
externC int pthread_mutexattr_getprotocol ( pthread_mutexattr_t *attr,
                                            int *protocol)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( protocol != NULL )
        *protocol = attr->protocol;
    
    PTHREAD_RETURN(0);
}

#if defined(_POSIX_THREAD_PRIO_PROTECT)

// Set priority for priority ceiling protocol
externC int pthread_mutexattr_setprioceiling ( pthread_mutexattr_t *attr,
                                               int prioceiling)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    
    attr->prioceiling = prioceiling;
    
    PTHREAD_RETURN(0);
}

// Get priority for priority ceiling protocol
externC int pthread_mutexattr_getprioceiling ( pthread_mutexattr_t *attr,
                                               int *prioceiling)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( prioceiling != NULL )
        *prioceiling = attr->prioceiling;
    
    PTHREAD_RETURN(0);
}

// Set priority ceiling of given mutex, returning old ceiling.
externC int pthread_mutex_setprioceiling( pthread_mutex_t *mutex,
                                          int prioceiling,
                                          int *old_ceiling)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(mutex);

    pthread_mutex_lock( mutex );

    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;

    if( old_ceiling != NULL )
        *old_ceiling = mx->get_ceiling();
    
    mx->set_ceiling( prioceiling );
    
    pthread_mutex_unlock( mutex );

    PTHREAD_RETURN(0);
}

// Get priority ceiling of given mutex
externC int pthread_mutex_getprioceiling( pthread_mutex_t *mutex,
                                          int *prioceiling)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(mutex);

    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;

    if( prioceiling != NULL )
        *prioceiling = mx->get_ceiling();
        
    PTHREAD_RETURN(0);
}

#endif // defined(_POSIX_THREAD_PRIO_PROTECT)

#endif // defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT)

//-----------------------------------------------------------------------------
// Mutex functions

//-----------------------------------------------------------------------------
// Initialize mutex. If mutex_attr is NULL, use default attributes.

externC int pthread_mutex_init (pthread_mutex_t *mutex,
                                const pthread_mutexattr_t *mutex_attr)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
    
    pthread_mutexattr_t use_attr;

    // Set up the attributes we are going to use
    if( mutex_attr == NULL )
        pthread_mutexattr_init( &use_attr );
    else use_attr = *mutex_attr;

    // Now translate the POSIX protocol identifier into the eCos one.
    Cyg_Mutex::cyg_protcol protocol;
    
    switch( use_attr.protocol )
    {
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING        
    case PTHREAD_PRIO_PROTECT:
        protocol = Cyg_Mutex::CEILING;
        break;
#endif
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_INHERIT        
    case PTHREAD_PRIO_INHERIT:
        protocol = Cyg_Mutex::INHERIT;
        break;
#endif        
    case PTHREAD_PRIO_NONE:
        protocol = Cyg_Mutex::NONE;
        break;

    default:
        PTHREAD_RETURN(EINVAL);
    }

    Cyg_Mutex *mx = new((cyg_uint8 *)mutex) Cyg_Mutex(  protocol );

    if ( protocol == Cyg_Mutex::CEILING )
        mx->set_ceiling( use_attr.prioceiling );
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy mutex.

externC int pthread_mutex_destroy (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY();

    int err = ENOERR;
    
    PTHREAD_CHECK( mutex );

    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;
    
    if( mx->get_owner() != NULL )
        err = EBUSY;
    else mx->~Cyg_Mutex();
    
    PTHREAD_RETURN(err);
}

//-----------------------------------------------------------------------------
// Lock mutex, waiting for it if necessary.

externC int pthread_mutex_lock (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );

    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;

    if( mx->get_owner() == Cyg_Thread::self() )
        PTHREAD_RETURN(EDEADLK);
    
    // Loop here until we acquire the mutex. Even if we are kicked out
    // of the wait by a signal or release we must retry.
    while( !mx->lock() )
        continue;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Try to lock mutex.

externC int pthread_mutex_trylock (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
    
    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;

    if( mx->get_owner() == Cyg_Thread::self() )
        PTHREAD_RETURN(EDEADLK);
    
    if( mx->trylock() )
        PTHREAD_RETURN(0);

    PTHREAD_RETURN(EBUSY);
}


//-----------------------------------------------------------------------------
// Unlock mutex.

externC int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( mutex );
    
    Cyg_Mutex *mx = (Cyg_Mutex *)mutex;

    mx->unlock();
    
    PTHREAD_RETURN(0);
}


//=============================================================================
// Condition Variables

//-----------------------------------------------------------------------------
// Attribute manipulation functions
// We do not actually support any attributes at present, so these do nothing.

//-----------------------------------------------------------------------------
// Initialize condition variable attributes

externC int pthread_condattr_init (pthread_condattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // There are no condition variable attributes at present
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy condition variable attributes

externC int pthread_condattr_destroy (pthread_condattr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // nothing to do here...
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Condition variable functions

//-----------------------------------------------------------------------------
// Initialize condition variable.

externC int pthread_cond_init (pthread_cond_t *cond,
                               const pthread_condattr_t *attr)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );

    Cyg_Condition_Variable *cv =
        new((cyg_uint8 *)cond) Cyg_Condition_Variable();

    cv = cv;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy condition variable.

externC int pthread_cond_destroy (pthread_cond_t *cond)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );

    ((Cyg_Condition_Variable *)cond)->~Cyg_Condition_Variable();
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Wake up one thread waiting for condition variable

externC int pthread_cond_signal (pthread_cond_t *cond)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );

    ((Cyg_Condition_Variable *)cond)->signal();
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Wake up all threads waiting for condition variable

externC int pthread_cond_broadcast (pthread_cond_t *cond)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );

    ((Cyg_Condition_Variable *)cond)->broadcast();
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Block on condition variable until signalled. The mutex is
// assumed to be locked before this call, will be unlocked
// during the wait, and will be re-locked on wakeup.

externC int pthread_cond_wait (pthread_cond_t *cond,
                               pthread_mutex_t *mutex)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );
    PTHREAD_CHECK( mutex );    

    ((Cyg_Condition_Variable *)cond)->wait( *(Cyg_Mutex *)mutex );
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Block on condition variable until signalled, or the timeout expires.

externC int pthread_cond_timedwait (pthread_cond_t *cond,
                                    pthread_mutex_t *mutex,
                                    const struct timespec *abstime)
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( cond );
    PTHREAD_CHECK( mutex );    
    PTHREAD_CHECK( abstime );    

    cyg_tick_count ticks;
    struct Cyg_Clock::converter ns_converter, sec_converter;
    
    Cyg_Clock::real_time_clock->get_other_to_clock_converter( 1, &ns_converter );
    Cyg_Clock::real_time_clock->get_other_to_clock_converter( 1000000000, &sec_converter );

    ticks = Cyg_Clock::convert( abstime->tv_sec, &sec_converter );
    ticks += Cyg_Clock::convert( abstime->tv_nsec, &ns_converter );
    
    ((Cyg_Condition_Variable *)cond)->wait( *(Cyg_Mutex *)mutex, ticks );
    
    PTHREAD_RETURN(0);
}

//=============================================================================
// Dynamic package initialization
// Call init_routine just the once per control variable.

externC int pthread_once (pthread_once_t *once_control,
                          void (*init_routine) (void))
{
    PTHREAD_ENTRY();

    PTHREAD_CHECK( once_control );
    PTHREAD_CHECK( init_routine );

    pthread_once_t old;

    // Do a test and set on the once_control object.
    pthread_mutex.lock();

    old = *once_control;
    *once_control = 1;

    pthread_mutex.unlock();

    // If the once_control was zero, call the init_routine().
    if( !old ) init_routine();
    
    PTHREAD_RETURN(0);
}


//=============================================================================
//Thread specific data

//-----------------------------------------------------------------------------
// Create a key to identify a location in the thread specific data area.
// Each thread has its own distinct thread-specific data area but all are
// addressed by the same keys. The destructor function is called whenever a
// thread exits and the value associated with the key is non-NULL.

externC int pthread_key_create (pthread_key_t *key,
                                void (*destructor) (void *))
{
    PTHREAD_ENTRY();

    pthread_key_t k = -1;
    
    pthread_mutex.lock();

    // Find a key to allocate
    for( cyg_ucount32 i = 0; i < (PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE); i++ )
    {
        if( thread_key[i] != 0 )
        {
            // We have a table slot with space available

            // Get index of ls set bit.
            HAL_LSBIT_INDEX( k, thread_key[i] );

            // clear it
            thread_key[i] &= ~(1<<k);

            // Add index of word
            k += i * KEY_MAP_TYPE_SIZE;

            // Install destructor
            key_destructor[k] = destructor;
            
            // break out with key found
            break;
        }
    }

    if( k != -1 )
    {
        // plant a NULL in all the valid thread data slots for this
        // key in case we are reusing a key we used before.
        
        for( cyg_ucount32 i = 0; i < CYGNUM_POSIX_PTHREAD_THREADS_MAX ; i++ )
        {
            pthread_info *thread = thread_table[i];

            if( thread != NULL && thread->thread_data != NULL )
                thread->thread_data[k] = NULL;
        }
    }
    
    pthread_mutex.unlock();    

    if( k == -1 ) PTHREAD_RETURN(EAGAIN);

    *key = k;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Delete key.

externC int pthread_key_delete (pthread_key_t key)
{
    PTHREAD_ENTRY();

    pthread_mutex.lock();

    // Set the key bit to 1 to indicate it is free.
    thread_key[key/KEY_MAP_TYPE_SIZE] |= 1<<(key%(KEY_MAP_TYPE_SIZE));

    pthread_mutex.unlock();        
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Store the pointer value in the thread-specific data slot addressed
// by the key.

externC int pthread_setspecific (pthread_key_t key, const void *pointer)
{
    PTHREAD_ENTRY();

    if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
        PTHREAD_RETURN(EINVAL);

    pthread_info *self = pthread_self_info();

    if( self->thread_data == NULL )
    {
        // Allocate the per-thread data table
        self->thread_data =
            (void **)self->thread->increment_stack_limit(
                PTHREAD_KEYS_MAX * sizeof(void *) );

        // Clear out all entries
        for( int i  = 0; i < PTHREAD_KEYS_MAX; i++ )
            self->thread_data[i] = NULL;
    }
    
    self->thread_data[key] = (void *)pointer;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Retrieve the pointer value in the thread-specific data slot addressed
// by the key.

externC void *pthread_getspecific (pthread_key_t key)
{
    void *val;
    PTHREAD_ENTRY();

    if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
        PTHREAD_RETURN(NULL);

    pthread_info *self = pthread_self_info();

    if( self->thread_data == NULL )
        val = NULL;
    else val = self->thread_data[key];

    PTHREAD_RETURN(val);
}

//=============================================================================
// Thread Cancellation Functions

//-----------------------------------------------------------------------------
// Set cancel state of current thread to ENABLE or DISABLE.
// Returns old state in *oldstate.

externC int pthread_setcancelstate (int state, int *oldstate)
{
    PTHREAD_ENTRY();

    if( state != PTHREAD_CANCEL_ENABLE &&
        state != PTHREAD_CANCEL_DISABLE )
        PTHREAD_RETURN(EINVAL);
    
    pthread_mutex.lock();

    pthread_info *self = pthread_self_info();

    if( oldstate != NULL ) *oldstate = self->cancelstate;
    
    self->cancelstate = state;
    
    pthread_mutex.unlock();
    
    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set cancel type of current thread to ASYNCHRONOUS or DEFERRED.
// Returns old type in *oldtype.

externC int pthread_setcanceltype (int type, int *oldtype)
{
    PTHREAD_ENTRY();

    if( type != PTHREAD_CANCEL_ASYNCHRONOUS &&
        type != PTHREAD_CANCEL_DEFERRED )
        PTHREAD_RETURN(EINVAL);
    
    pthread_mutex.lock();

    pthread_info *self = pthread_self_info();
        
    if( oldtype != NULL ) *oldtype = self->canceltype;

    self->canceltype = type;
    
    pthread_mutex.unlock();   

    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Cancel the thread.

externC int pthread_cancel (pthread_t thread)
{
    PTHREAD_ENTRY();

    pthread_mutex.lock();

    pthread_info *th = pthread_info_id(thread);

    if( th == NULL )
    {
        pthread_mutex.unlock();
        PTHREAD_RETURN(ESRCH);
    }

    th->cancelpending = true;

    if( th->cancelstate == PTHREAD_CANCEL_ENABLE &&
        th->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS )
    {
        // If the thread has cancellation enabled, and it is in
        // asynchronous mode, set the eCos thread's ASR pending to
        // deal with it when the thread wakes up. We also release the
        // thread out of any current wait to make it wake up.
        
        th->thread->set_asr_pending();
        th->thread->release();
    }
    // Otherwise the thread either has cancellation disabled, or is
    // in deferred mode. In either case it is up to the thread to
    // get into a state to deal with the pending cancellation.
    
    pthread_mutex.unlock();   
   
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Test for a pending cancellation for the current thread and terminate
// the thread if there is one.

externC void pthread_testcancel (void)
{
    PTHREAD_ENTRY_VOID();

    pthread_info *self = pthread_self_info();

    // Check whether there is a cancel pending and if so, whether
    // cancellations are enabled. We do it in this order to reduce the
    // number of tests in the common case - when no cancellations are
    // pending.

    if( self->cancelpending &&
        self->cancelstate == PTHREAD_CANCEL_ENABLE )
        
    {
        // If we have cancellation enabled, and there is a cancellation
        // pending, then go ahead and do the deed. 
        
        // Exit now with special retval. pthread_exit() calls the
        // cancellation handlers implicitly.
        pthread_exit(PTHREAD_CANCELED);
    }
        
    PTHREAD_RETURN_VOID;
}

//-----------------------------------------------------------------------------
// These two functions actually implement the cleanup push and pop functionality.

externC void pthread_cleanup_push_inner (struct pthread_cleanup_buffer *buffer,
                                         void (*routine) (void *),
                                         void *arg)
{
    PTHREAD_ENTRY();

    pthread_info *self = pthread_self_info();

    buffer->routine     = routine;
    buffer->arg         = arg;
    
    buffer->prev        = self->cancelbuffer;

    self->cancelbuffer  = buffer;

    return;
}

externC void pthread_cleanup_pop_inner (struct pthread_cleanup_buffer *buffer,
                                        int execute)
{
    PTHREAD_ENTRY();

    pthread_info *self = pthread_self_info();
    
    CYG_ASSERT( self->cancelbuffer == buffer, "Stacking error in cleanup buffers");
    
    if( self->cancelbuffer == buffer )
    {
        // Remove the buffer from the stack
        self->cancelbuffer = buffer->prev;
    }
    else
    {
        // If the top of the stack is not the buffer we expect, do not
        // execute it.
        execute = 0;
    }

    if( execute ) buffer->routine(buffer->arg);
    
    return;
}


// -------------------------------------------------------------------------
// EOF pthread.cxx
