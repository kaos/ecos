//=================================================================
//
//        malloc4.cxx
//
//        Stress test malloc(), calloc(), realloc() and free()
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-05-30
// Description:   Contains a rigorous multithreaded test for malloc(),
//                calloc(), realloc() and free() functions
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/system.h>
#include <pkgconf/memalloc.h> // config header
#ifdef CYGPKG_ISOINFRA
# include <pkgconf/isoinfra.h>
# include <stdlib.h>
#endif
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
# include <cyg/kernel/thread.hxx>
# include <cyg/kernel/thread.inl>
# include <cyg/kernel/sched.hxx>
# include <cyg/kernel/sched.inl>
#endif
#include <cyg/infra/testcase.h>

#if !defined(CYGPKG_KERNEL)
# define NA_MSG "Requires kernel"
#elif !defined(CYGFUN_KERNEL_THREADS_TIMER)
# define NA_MSG "Requires thread timers"
#elif !defined(CYGPKG_ISOINFRA)
# define NA_MSG "Requires isoinfra package"
#elif !CYGINT_ISO_MALLOC
# define NA_MSG "Requires malloc"
#elif !CYGINT_ISO_MALLINFO
# define NA_MSG "Requires mallinfo"
#elif !CYGINT_ISO_RAND
# define NA_MSG "Requires rand"
#elif defined(CYGIMP_MEMALLOC_MALLOC_DLMALLOC) && \
      !defined(CYGIMP_MEMALLOC_ALLOCATOR_DLMALLOC_THREADAWARE)
# define NA_MSG "Requires thread-safe dlmalloc"
#elif defined(CYGIMP_MEMALLOC_MALLOC_VARIABLE_SIMPLE) && \
      !defined(CYGSEM_MEMALLOC_ALLOCATOR_VARIABLE_THREADAWARE)
# define NA_MSG "Requires thread-safe variable block allocator"
#endif

#ifdef NA_MSG

externC void
cyg_start()
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
    CYG_TEST_FINISH("Done");
}
#else

#define NTHREADS 4
#include "testaux.hxx"

#include <cyg/infra/diag.h>

volatile int gonow = 0;
volatile int stopnow = 0;

struct ptr {
    volatile char *p;
    volatile size_t size;
    volatile unsigned char busy;
};

#define STRINGIFY1( _x_ ) #_x_
#define STRINGIFY( _x_ ) STRINGIFY1( _x_ )

#define NUM_PTRS 100
#define WAITFORMEMDELAYMAX (cyg_test_is_simulator ? 1 : 3)
#define LOOPDELAYMAX       (cyg_test_is_simulator ? 1 : 3)
#define ITERATIONS         (cyg_test_is_simulator ? 10 : 200)
#define OUTPUTINTERVAL     (cyg_test_is_simulator ? 1 : 10)

int iterations = ITERATIONS;

static struct ptr ptrs[ NUM_PTRS ];

static __inline__ int
myrand(int limit, unsigned int *seed)
{
    int r;
    double l=(double)limit;
    r=(int)( l*rand_r(seed) / RAND_MAX );
    return r;
}

size_t memsize;

static void
fill_with_data( struct ptr *p )
{
    unsigned int i, j;
    for (i=0; i < (p->size/4); i++)
        ((unsigned int *)p->p)[i] = (unsigned int)p;
    for ( j=i*4; j < p->size ; j++ )
        p->p[j] = ((char *)p)[j-i*4];
}

static void
check_data( struct ptr *p )
{
    unsigned int i, j;
    for (i=0; i < (p->size/4); i++)
        CYG_TEST_CHECK( ((unsigned int *)p->p)[i] == (unsigned int)p,
                        "Data didn't compare correctly");
    for ( j=i*4; j < p->size ; j++ )
        CYG_TEST_CHECK( p->p[j] == ((char *)p)[j-i*4],
                        "Data didn't compare correctly");
}

static void
check_zeroes( struct ptr *p )
{
    unsigned int i, j;
    for (i=0; i < (p->size/4); i++)
        CYG_TEST_CHECK( ((int *)p->p)[i] == 0,
                        "Zeroed data didn't compare correctly");
    for ( j=i*4; j < p->size ; j++ )
        CYG_TEST_CHECK( p->p[j] == 0,
                        "Zeroed data didn't compare correctly");
}


static void
thrmalloc( CYG_ADDRWORD data )
{
    int r, i;
    void *mem;
    unsigned int seed;

    while (!gonow)
        Cyg_Thread::self()->delay(1);
        
    while (!stopnow) {
        r = myrand( NUM_PTRS-1, &seed );
        
        for (i=r+1; ; i++) {
            Cyg_Scheduler::lock();
            if (i == NUM_PTRS)
                i=0;
            if (!ptrs[i].busy && (ptrs[i].p == NULL) )
                break;
            Cyg_Scheduler::unlock();
            if ( i==r ) {
                Cyg_Thread::self()->delay( myrand(WAITFORMEMDELAYMAX, &seed) );
            }
        }
        ptrs[i].busy = 1;
        Cyg_Scheduler::unlock();
        r = myrand(memsize, &seed);
        mem = malloc(r);
        ptrs[i].p = (char *)mem;
        ptrs[i].size = r;
        if ( NULL != mem ) {
//            diag_printf("malloc=%08x size=%d\n", mem, r);
            fill_with_data( &ptrs[i] );
        }
        ptrs[i].busy = 0;        
        Cyg_Thread::self()->delay( myrand(LOOPDELAYMAX, &seed) );
    }
}

static void
thrcalloc( CYG_ADDRWORD data )
{
    int r, i;
    void *mem;
    unsigned int seed;

    while (!gonow)
        Cyg_Thread::self()->delay(1);
        
    while (!stopnow) {
        r = myrand( NUM_PTRS-1, &seed );
        
        for (i=r+1; ; i++) {
            Cyg_Scheduler::lock();
            if (i == NUM_PTRS)
                i=0;
            if (!ptrs[i].busy && (ptrs[i].p == NULL) )
                break;
            Cyg_Scheduler::unlock();
            if ( i==r ) {
                Cyg_Thread::self()->delay( myrand(WAITFORMEMDELAYMAX, &seed) );
            }
        }
        ptrs[i].busy = 1;
        Cyg_Scheduler::unlock();
        r = myrand(memsize, &seed);
        mem = calloc( 1, r );
        ptrs[i].p = (char *)mem;
        ptrs[i].size = r;
        if ( NULL != mem ) {
//            diag_printf("calloc=%08x size=%d\n", mem, r);
            check_zeroes( &ptrs[i] );
            fill_with_data( &ptrs[i] );
        }
        ptrs[i].busy = 0;        
        Cyg_Thread::self()->delay( myrand(LOOPDELAYMAX, &seed) );
    }
}

static void
thrrealloc( CYG_ADDRWORD data )
{
    int r, i;
    void *mem;
    unsigned int seed;

    while (!gonow)
        Cyg_Thread::self()->delay(1);
        
    while (!stopnow) {
        r = myrand( NUM_PTRS-1, &seed );
        
        for (i=r+1; ; i++) {
            Cyg_Scheduler::lock();
            if (i == NUM_PTRS)
                i=0;
            if (!ptrs[i].busy && (ptrs[i].p != NULL) )
                break;
            Cyg_Scheduler::unlock();
            if ( i==r ) {
                Cyg_Thread::self()->delay( myrand(WAITFORMEMDELAYMAX, &seed) );
            }
        }
        ptrs[i].busy = 1;
        Cyg_Scheduler::unlock();
        check_data( &ptrs[i] );
        r = myrand(memsize, &seed);
        mem = realloc( (void *)ptrs[i].p, r );
        if ( NULL != mem ) {
//            diag_printf("realloc=%08x oldsize=%d newsize=%d\n", mem, ptrs[i].size, r);
            ptrs[i].size = r;
            ptrs[i].p = (char *)mem;
            fill_with_data( &ptrs[i] );
        }
        ptrs[i].busy = 0;        
        Cyg_Thread::self()->delay( myrand(LOOPDELAYMAX, &seed) );
    }
}

static void
thrfree( CYG_ADDRWORD data )
{
    int r, i;
    int iter = 0;
    struct mallinfo minfo;
    unsigned int seed;

    minfo = mallinfo();
    memsize = (unsigned long) minfo.maxfree;
    diag_printf("INFO:<Iteration 0, arenasize=%d, space free=%d, maxfree=%d>\n",
                minfo.arena, minfo.fordblks, minfo.maxfree );
    gonow++;
        
    Cyg_Thread::self()->delay(1);

    while (1) {
        if ( (iter > 0) && (0 == (iter % OUTPUTINTERVAL)) ) {
            minfo = mallinfo();
            diag_printf("INFO:<Iteration %d, arenasize=%d, "
                        "space free=%d, maxfree=%d>\n",
                        iter, minfo.arena, minfo.fordblks, minfo.maxfree );
        }

        if ( iterations == iter++ )
            stopnow++;

        r = myrand( NUM_PTRS-1, &seed );
        
        for (i=r+1; ; i++) {
            Cyg_Scheduler::lock();
            if (i >= NUM_PTRS)
                i=0;
            if (!ptrs[i].busy && (ptrs[i].p != NULL) )
                break;
            Cyg_Scheduler::unlock();
            if ( i==r ) {
                if ( stopnow ) {
                    // we may have gone round all the ptrs even though one
                    // or more of them was busy, so check again just for that
                    int j;
                    for (j=0; j<NUM_PTRS; j++)
                        if (ptrs[j].busy)
                            break;
                    if ( j<NUM_PTRS )
                        continue;
                    struct mallinfo minfo;

                    minfo = mallinfo();
                    diag_printf("INFO:<Iteration %d, arenasize=%d, "
                                "space free=%d, maxfree=%d>\n",
                                iter, minfo.arena, minfo.fordblks,
                                minfo.maxfree );
                    CYG_TEST_PASS_FINISH("malloc4 test completed successfully");
                } else {
                    Cyg_Thread::self()->delay( 
                        myrand(WAITFORMEMDELAYMAX, &seed) );
                }
            }
        }
        ptrs[i].busy = 1;
        Cyg_Scheduler::unlock();
        check_data( &ptrs[i] );
//        diag_printf("about to free %08x\n", ptrs[i].p);
        free( (void *)ptrs[i].p );
        ptrs[i].p = NULL;
        ptrs[i].busy = 0;
        Cyg_Thread::self()->delay( myrand(LOOPDELAYMAX, &seed) );
    }
}


externC void
cyg_start()
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Starting malloc4 test");

    new_thread(thrmalloc, 0);
    new_thread(thrcalloc, 1);
    new_thread(thrrealloc, 2);
    new_thread(thrfree, 3);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
} // main()

#endif // !NA_MSG

// EOF malloc4.cxx
