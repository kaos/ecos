//==========================================================================
//
//        philo.cxx
//
//        A test of the dining philosophers problem
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-24
// Description:   A test of the dining philosophers problem
//####DESCRIPTIONEND####
// 

#include <cyg/kernel/kernel.hxx>

#include <cyg/hal/hal_io.h>

// -------------------------------------------------------------------------
// Data for the philosophers problem

#define PHILOSOPHERS    15               // number of philosophers
#define STACKSIZE       (2*1024)        // size of thread stack

// array of stacks for philosopher threads
char thread_stack[PHILOSOPHERS][STACKSIZE];

// array of threads. We use the new operator defined
// below to make these into real threads.
char thread[PHILOSOPHERS][sizeof(Cyg_Thread)];

inline void *operator new(size_t size, void *ptr) { return ptr; };

// array of chopsticks
Cyg_Binary_Semaphore chopstick[PHILOSOPHERS];


//cyg_thread_entry Philosopher;

// -------------------------------------------------------------------------
// State recording and display

static char pstate[PHILOSOPHERS+1];     // state vector showing what each
                                        // philosopher is doing

Cyg_Mutex state_mutex;

#ifdef CYG_HAL_MN10300_MN103002
static cyg_count8 eaters = 0;
#endif

void change_state(int id, char newstate)
{
    CYG_INSTRUMENT_USER( 1, 0, 0);
    state_mutex.lock();
    CYG_INSTRUMENT_USER( 2, 0, 0);

#ifdef CYG_HAL_MN10300_MN103002
    if( pstate[id] == 'E' ) eaters--;
    if( newstate == 'E' ) eaters++;
//    led(eaters);
#endif
    
    pstate[id] = newstate;

    diag_write_string(pstate);
#if 0
    diag_write_char(' ');
    diag_write_dec(Cyg_Scheduler::get_thread_switches());
#endif    
    diag_write_char('\n');

    CYG_INSTRUMENT_USER( 3, 0, 0);
    state_mutex.unlock();
    CYG_INSTRUMENT_USER( 4, 0, 0);    
    
}

char get_state( int id)
{
    state_mutex.lock();
    
    char s = pstate[id];

    state_mutex.unlock();

    return s;
}

// -------------------------------------------------------------------------
// Thread to behave like a philosopher

void Philosopher( CYG_ADDRESS id )
{
    Cyg_Thread *self = Cyg_Thread::self();
    Cyg_Binary_Semaphore *first_stick = &chopstick[id];
    Cyg_Binary_Semaphore *second_stick = &chopstick[(id+1)%PHILOSOPHERS];
#ifdef CYGPKG_INFRA_DEBUG
    int left_philo = ((id==0)?PHILOSOPHERS:id)-1;
    int right_philo = (id==PHILOSOPHERS-1)?0:(id+1);
#endif
    
    CYG_ASSERT( id >= 0 && id < PHILOSOPHERS, "Bad id");

    // Deadlock avoidance. The easiest way to make the philosophers
    // behave is to make each pick up the lowest numbered stick
    // first. This is how it works out anyway for all the philosophers
    // except the last, who must have his sticks swapped.
    
    if( id == PHILOSOPHERS-1 )
    {
        Cyg_Binary_Semaphore *t = first_stick;
        first_stick = second_stick;
        second_stick = t;
    }
    
    // The following variable is shared by all philosophers.
    // It is incremented unprotected, but this does not matter
    // since it is only present to introduce a little variability
    // into the think and eat times.
    
    static int cycle = 0;
    
    for(;;)
    {
        // Think for a bit

        self->delay((id+cycle++)%12);    // Cogito ergo sum...

        // I am now hungry, try to get the chopsticks

        change_state(id,'H');
        
        // Get the first stick
    CYG_INSTRUMENT_USER( 5, 0, 0);
        first_stick->wait();
    CYG_INSTRUMENT_USER( 6, 0, 0);
    
        // Get the second stick
    CYG_INSTRUMENT_USER( 7, 0, 0);
        second_stick->wait();
    CYG_INSTRUMENT_USER( 8, 0, 0);
    
        // Got them, now eat

        change_state(id,'E');
                
        // Check that the world is as I think it is...
        CYG_ASSERT( !first_stick->posted(), "Not got first stick");
        CYG_ASSERT( !second_stick->posted(), "Not got second stick");
        CYG_ASSERT( get_state(left_philo) != 'E', "Left neighbour also eating!!");
        CYG_ASSERT( get_state(right_philo) != 'E', "Right neighbour also eating!!");
        
        self->delay((id+cycle++)%6);    // munch munch

        // Finished eating, put down sticks.

        change_state(id,'T');   

    CYG_INSTRUMENT_USER( 9, 0, 0);        
        first_stick->post();
    CYG_INSTRUMENT_USER( 10, 0, 0);        
        second_stick->post();
    CYG_INSTRUMENT_USER( 11, 0, 0);

//    Cyg_Scheduler::lock();
//    Cyg_Scheduler::unlock();
    CYG_INSTRUMENT_USER( 12, 0, 0);
    
    }
}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    diag_init();

    diag_write_string("Philosophers\n");
    diag_write_string("Started\n");

    // Zero last element in state so it acts like
    // a string.
    pstate[PHILOSOPHERS] = 0;

#if 1
    for( int i = 0; i < PHILOSOPHERS; i++ )
    {
        change_state(i,'T');            // starting state

        // Start the philosopher
        Cyg_Thread *t = new((void *)&thread[i]) Cyg_Thread(Philosopher,
                                                   i,
                                                   STACKSIZE,
                                                   CYG_ADDRESS(&thread_stack[i])
            );

        // resume it
        t->resume();

        // and make the matching chopstick present
        chopstick[i].post();
    }
#endif
    
    // Get the world going
    Cyg_Scheduler::scheduler.start();

}

// -------------------------------------------------------------------------
// EOF philo.cxx
