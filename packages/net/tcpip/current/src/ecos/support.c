//==========================================================================
//
//      ecos/support.c
//
//      eCos wrapper and support functions
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
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


// Support routines, etc., used by network code

#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/kernel.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <net/netisr.h>

#include <machine/cpu.h>

#include <pkgconf/net.h>

#include <cyg/infra/diag.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/kernel/kapi.h>

#include <cyg/infra/cyg_ass.h>

#include <netdev.h>

// Used for system-wide "ticks per second"
int hz = 100;
int tick = 10000;  // usec per "tick"

volatile struct timeval mono_time;

// Low-level network debugging
int net_debug = 0;

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
static char netint_stack[STACK_SIZE];
static cyg_thread netint_thread_data;
static cyg_handle_t netint_thread_handle;

cyg_flag_t netint_flags;  
#define NETISR_ANY 0xFFFFFFFF  // Any possible bit...

extern void cyg_test_exit(void);  // TEMP
void
cyg_panic(const char *msg, ...)
{
    cyg_uint32 old_ints;
    HAL_DISABLE_INTERRUPTS(old_ints);
    diag_printf("PANIC: %s\n", msg);
    cyg_test_exit();  // FIXME
}


// ------------------------------------------------------------------------
#define SPLSOFTNET_IS_A_MUTEX 1
// ------------------------------------------------------------------------
#ifdef SPLSOFTNET_IS_A_MUTEX

#define MUTEX 0x50F750F7
#define HOLD  MUTEX+1

static cyg_mutex_t mutex;
static volatile cyg_handle_t owner;

#define SPLINIT() CYG_MACRO_START               \
    cyg_mutex_init( &mutex );                   \
    owner = 0;                                  \
CYG_MACRO_END

// use the scheduler lock for SPLX() and a mutex for softnet

static inline cyg_uint32
cyg_splin_softnet(void)
{
    if ( 0 == cyg_scheduler_read_lock() ) {
        cyg_handle_t self = cyg_thread_self();
        if ( self != owner ) {
            cyg_mutex_lock( &mutex ); // Perfectly OK for this to wait
            owner = self;
            return MUTEX; // release when restoring here
        }
        return HOLD; // do not release when restoring to this value
    }
    cyg_scheduler_lock();
    return cyg_scheduler_read_lock() - 1;
}

static inline cyg_uint32
cyg_splin(void)
{
    cyg_scheduler_lock();
    return cyg_scheduler_read_lock() - 1;
}

static inline void
cyg_splout(cyg_uint32 orig)
{
    if ( MUTEX == orig ) {
        CYG_ASSERT( cyg_thread_self() == owner, "Not owner release!" );
        cyg_scheduler_lock();
        owner = 0;
        cyg_mutex_unlock( &mutex );
        cyg_scheduler_unlock();
        return;
    }
    if ( HOLD == orig ) {
        CYG_ASSERT( cyg_thread_self() == owner, "Not owner hold!" );
        return;
    }
    CYG_ASSERT( 0 == (0xffff0000 & orig), "Scary splx value" );
    while ( cyg_scheduler_read_lock() > orig )
        cyg_scheduler_unlock();
}

#define SPL_ENTER(x)    (x) = cyg_splin()
#define SPL_SOFTNET(x)  (x) = cyg_splin_softnet()
#define SPL_EXIT(x)     cyg_splout((x))

#else
#if 1

// use the scheduler lock for SPLX()
static inline cyg_uint32
cyg_splin(void)
{
    cyg_scheduler_lock();
    return cyg_scheduler_read_lock() - 1;
}

static inline void
cyg_splout(cyg_uint32 orig)
{
    CYG_ASSERT( 0 == (0xffff0000 & orig), "Scary splx value" );
    while ( cyg_scheduler_read_lock() > orig )
        cyg_scheduler_unlock();
}

#define SPL_ENTER(x)    (x) = cyg_splin()
#define SPL_SOFTNET(x)  (x) = cyg_splin()
#define SPL_EXIT(x)     cyg_splout((x))

#else

// Old interrupt based version
#define SPL_ENTER(x)    HAL_DISABLE_INTERRUPTS((x))
#define SPL_SOFTNET(x)  HAL_DISABLE_INTERRUPTS((x))
#define SPL_EXIT(x)     HAL_RESTORE_INTERRUPTS((x))

#endif
#endif
// ------------------------------------------------------------------------

cyg_uint32
cyg_splimp(void)
{
    cyg_uint32 old_ints;
    SPL_ENTER(old_ints);
    return old_ints;
}

cyg_uint32
cyg_splnet(void)
{
    cyg_uint32 old_ints;
    SPL_ENTER(old_ints);
    return old_ints;
}

cyg_uint32
cyg_splclock(void)
{
    cyg_uint32 old_ints;
    SPL_ENTER(old_ints);
    return old_ints;
}

cyg_uint32
cyg_splsoftnet(void)
{
    cyg_uint32 old_ints;
    SPL_SOFTNET(old_ints);
    return old_ints;
}

void
cyg_splx(cyg_uint32 old_lev)
{
    SPL_EXIT(old_lev);
}

void
setsoftnet(void)
{
    diag_printf("setsoftnet\n");
    schednetisr(NETISR_SOFTNET);
}

// Round a number 'n' up to a multiple of 'm'
#define round(n,m) ((((n)+((m)-1))/(m))*(m))

#define NET_MEMPOOL_SIZE  round(CYGPKG_NET_MEM_USAGE/4,MSIZE)
#define NET_MBUFS_SIZE    round(CYGPKG_NET_MEM_USAGE/4,MSIZE)
#define NET_CLUSTERS_SIZE round(CYGPKG_NET_MEM_USAGE/2,MCLBYTES)

static unsigned char net_mempool_area[NET_MEMPOOL_SIZE];
static cyg_mempool_var net_mem_pool;
static cyg_handle_t    net_mem;
static unsigned char net_mbufs_area[NET_MBUFS_SIZE];
static cyg_mempool_fix net_mbufs_pool;
static cyg_handle_t    net_mbufs;
static unsigned char net_clusters_area[NET_CLUSTERS_SIZE];
static cyg_mempool_fix net_clusters_pool;
static cyg_handle_t    net_clusters;
static char            net_clusters_refcnt[(NET_CLUSTERS_SIZE/MCLBYTES)+1];

static struct net_stats  stats_malloc, stats_free, 
    stats_memcpy, stats_memset,
    stats_mbuf_alloc, stats_mbuf_free, stats_cluster_alloc;
extern struct net_stats stats_in_cksum;

// Display a number of ticks as microseconds
// Note: for improved calculation significance, values are kept in ticks*1000
static long rtc_resolution[] = CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION;
static long ns_per_system_clock;

static void
show_ticks_in_us(cyg_uint32 ticks)
{
    long long ns;
    ns_per_system_clock = 1000000/rtc_resolution[1];
    ns = (ns_per_system_clock * ((long long)ticks * 1000)) / 
        CYGNUM_KERNEL_COUNTERS_RTC_PERIOD;
    ns += 5;  // for rounding to .01us
    diag_printf("%7d.%02d", (int)(ns/1000), (int)((ns%1000)/10));
}

void
show_net_stats(struct net_stats *stats, const char *title)
{
    int ave;
    ave = stats->total_time / stats->count;
    diag_printf("%s:\n", title);
    diag_printf("  count: %6d", stats->count);
    diag_printf(", min: ");
    show_ticks_in_us(stats->min_time);
    diag_printf(", max: ");
    show_ticks_in_us(stats->max_time);
    diag_printf(", total: ");
    show_ticks_in_us(stats->total_time);
    diag_printf(", ave: ");
    show_ticks_in_us(ave);
    diag_printf("\n");
    // Reset stats
    memset(stats, 0, sizeof(*stats));
}

void
show_net_times(void)
{
    show_net_stats(&stats_malloc,        "Net malloc");
    show_net_stats(&stats_free,          "Net free");
    show_net_stats(&stats_mbuf_alloc,    "Mbuf alloc");
    show_net_stats(&stats_mbuf_free,     "Mbuf free");
    show_net_stats(&stats_cluster_alloc, "Cluster alloc");
    show_net_stats(&stats_in_cksum,      "Checksum");
    show_net_stats(&stats_memcpy,        "Net memcpy");
    show_net_stats(&stats_memset,        "Net memset");
}

void *
cyg_net_malloc(u_long size, int type, int flags)
{
    void *res;
    START_STATS();
    if (flags & M_NOWAIT) {
        res = cyg_mempool_var_try_alloc(net_mem, size);
    } else {
        res = cyg_mempool_var_alloc(net_mem, size);
    }
    FINISH_STATS(stats_malloc);
    return (res);
}

void 
cyg_net_free(caddr_t addr, int type)
{
    START_STATS();
    cyg_mempool_var_free(net_mem, addr);
    FINISH_STATS(stats_free);
}

void *
cyg_net_mbuf_alloc(int type, int flags)
{
    void *res;    
    START_STATS();
    mbstat.m_mbufs++;
    if (flags & M_NOWAIT) {
        res = cyg_mempool_fix_try_alloc(net_mbufs);
    } else {
        res = cyg_mempool_fix_alloc(net_mbufs);
    }
    FINISH_STATS(stats_mbuf_alloc);
    // Check that this nastiness works OK
    CYG_ASSERT( dtom(res) == res, "dtom failed, base of mbuf" );
    CYG_ASSERT( dtom((char *)res + MSIZE/2) == res, "dtom failed, mid mbuf" );
    return (res);
}

void 
cyg_net_mbuf_free(caddr_t addr, int type)
{
    START_STATS();
    mbstat.m_mbufs--;
    cyg_mempool_fix_free(net_mbufs, addr);
    FINISH_STATS(stats_mbuf_free);
}

void *
cyg_net_cluster_alloc(void)
{
    void *res;
    START_STATS();
    res = cyg_mempool_fix_try_alloc(net_clusters);
    FINISH_STATS(stats_cluster_alloc);
    return res;
}

static void
cyg_kmem_init(void)
{
    unsigned char *p;
    diag_printf("Network stack using %d bytes for misc space\n", NET_MEMPOOL_SIZE);
    diag_printf("                    %d bytes for mbufs\n", NET_MBUFS_SIZE);
    diag_printf("                    %d bytes for mbuf clusters\n", NET_CLUSTERS_SIZE);
    cyg_mempool_var_create(&net_mempool_area,
                           NET_MEMPOOL_SIZE,
                           &net_mem,
                           &net_mem_pool);
    // Align the mbufs on MSIZE boudaries so that dtom() can work.
    p = (unsigned char *)(((long)(&net_mbufs_area) + MSIZE - 1) & ~(MSIZE-1));
    cyg_mempool_fix_create(p,
                           ((&(net_mbufs_area[NET_MBUFS_SIZE])) - p) & ~(MSIZE-1),
                           MSIZE,
                           &net_mbufs,
                           &net_mbufs_pool);
    cyg_mempool_fix_create(&net_clusters_area,
                           NET_CLUSTERS_SIZE,
                           MCLBYTES,
                           &net_clusters,
                           &net_clusters_pool);
    mbutl = (struct mbuf *)&net_clusters_area;
    mclrefcnt = net_clusters_refcnt;
}


void cyg_kmem_print_stats( void )
{
    cyg_mempool_info info;

    diag_printf( "Network stack mbuf stats:\n" );
    diag_printf( "   mbufs %d, clusters %d, free clusters %d\n",
                 mbstat.m_mbufs,	/* mbufs obtained from page pool */
                 mbstat.m_clusters,	/* clusters obtained from page pool */
                 /* mbstat.m_spare, */	/* spare field */
                 mbstat.m_clfree	/* free clusters */
        );
    diag_printf( "   Failed to get %d times\n"
                 "   Waited to get %d times\n"
                 "   Drained queues to get %d times\n",
                 mbstat.m_drops,	/* times failed to find space */
                 mbstat.m_wait, 	/* times waited for space */
                 mbstat.m_drain         /* times drained protocols for space */
                 /* mbstat.m_mtypes[256]; type specific mbuf allocations */
        );

    cyg_mempool_var_get_info( net_mem, &info );
    diag_printf( "Misc mpool: total %7d, free %7d, max free block %d\n",
                 info.totalmem,
                 info.freemem,
                 info.maxfree
        );

    cyg_mempool_fix_get_info( net_mbufs, &info );
    diag_printf( "Mbufs pool: total %7d, free %7d, blocksize %4d\n",
                 info.totalmem,
                 info.freemem,
                 info.blocksize
        );


    cyg_mempool_fix_get_info( net_clusters, &info );
    diag_printf( "Clust pool: total %7d, free %7d, blocksize %4d\n",
                 info.totalmem,
                 info.freemem,
                 info.blocksize
        );
}

int
cyg_mtocl(u_long x)
{
    int res;
    res = (((u_long)(x) - (u_long)mbutl) >> MCLSHIFT);
    return res;
}

struct mbuf *
cyg_cltom(u_long x)
{
    struct mbuf *res;
    res = (struct mbuf *)((caddr_t)((u_long)mbutl + ((u_long)(x) << MCLSHIFT)));
    return res;
}

void 
net_memcpy(void *d, void *s, int n)
{
    START_STATS();
    memcpy(d, s, n);
    FINISH_STATS(stats_memcpy);
}

void 
net_memset(void *s, int v, int n)
{
    START_STATS();
    memset(s, v, n);
    FINISH_STATS(stats_memset);
}

// Rather than bring in the whole BSD 'random' code...
int
arc4random(void)
{
    cyg_uint32 res;
    static unsigned long seed = 0xDEADB00B;
    HAL_CLOCK_READ(&res);  // Not so bad...
    seed = ((seed & 0x07F00FF) << 7) ^
        (seed & 0xF80FF00) ^ 
        (res << 13);
    return (int)seed;
}

void 
get_random_bytes(void *buf, size_t len)
{
    unsigned long ranbuf, *lp;
    lp = (unsigned long *)buf;
    while (len > 0) {
        ranbuf = arc4random();
        *lp++ = ranbuf;
        len -= sizeof(ranbuf);
    }
}

void 
microtime(struct timeval *tp)
{
    panic("microtime");
}

void
get_mono_time(void)
{
    panic("get_mono_time");
}

void 
csignal(pid_t pgid, int signum, uid_t uid, uid_t euid)
{
    panic("csignal");
}

int
bcmp(const void *_p1, const void *_p2, size_t len)
{
    int res = 0;
    unsigned char *p1 = (unsigned char *)_p1;
    unsigned char *p2 = (unsigned char *)_p2;
    while (len-- > 0) {
        res = *p1++ - *p2++;
        if (res) break;
    }
    return res;
}

int
copyout(const void *s, void *d, size_t len)
{
    memcpy(d, s, len);
    return 0;
}

int
copyin(const void *s, void *d, size_t len)
{
    memcpy(d, s, len);
    return 0;
}

void
ovbcopy(const void *s, void *d, size_t len)
{
    memcpy(d, s, len);
}

//
// Structure used to keep track of 'tsleep' style events
//
struct wakeup_event {
    void *chan;
    cyg_sem_t sem;
};
static struct wakeup_event wakeup_list[CYGPKG_NET_NUM_WAKEUP_EVENTS];

//
// Signal an event
void
cyg_wakeup(void *chan)
{
    int i;
    struct wakeup_event *ev;
    cyg_scheduler_lock();  // Ensure scan is safe
    for (i = 0, ev = wakeup_list;  i < CYGPKG_NET_NUM_WAKEUP_EVENTS;  i++, ev++) {
        if (ev->chan == chan) {
            cyg_semaphore_post(&ev->sem);
            ev->chan = 0;
        }
    }
    cyg_scheduler_unlock();
}

//
// Wait for an event with timeout
//   tsleep(event, priority, state, timeout)
//     event - the thing to wait for
//     priority - unused
//     state    - a descriptive message
//     timeout  - max time (in ticks) to wait
//   returns:
//     0         - event was "signalled"
//     ETIMEDOUT - timeout occurred
//
int       
cyg_tsleep(void *chan, int pri, char *wmesg, int timo)
{
    int i, res = 0;
    struct wakeup_event *ev;    
    cyg_tick_count_t sleep_time;
#ifdef SPLSOFTNET_IS_A_MUTEX
    int olock; // this does the same as safe_lock() but keeping the old state
    cyg_scheduler_lock(); // ...around.
    olock = cyg_scheduler_read_lock();
    if ( olock > 1 )
        cyg_scheduler_unlock();
#else
    cyg_scheduler_safe_lock();  // Ensure safe scan
#endif

    for (i = 0, ev = wakeup_list;  i < CYGPKG_NET_NUM_WAKEUP_EVENTS;  i++, ev++) {
        if (ev->chan == 0) {
            ev->chan = chan;
            break;
        }
    }
    if (i == CYGPKG_NET_NUM_WAKEUP_EVENTS) {
        panic("no sleep slots");
    }
    CYG_ASSERT( 1 == cyg_scheduler_read_lock(), "Sleep won't!" );

#ifdef SPLSOFTNET_IS_A_MUTEX
    {   // Then we must release the mutex when we wait - if we have it
        cyg_handle_t self = cyg_thread_self();
        if ( self == owner ) {
            owner = 0;
            cyg_mutex_unlock( &mutex );
        } else {
            self = 0; // Flag no need to reclaim
        }
#endif

        // This part actually does the wait:
        cyg_scheduler_unlock();
        if (timo) {
            sleep_time = cyg_current_time() + timo;
            if (!cyg_semaphore_timed_wait(&ev->sem, sleep_time)) {
                res = ETIMEDOUT;
                ev->chan = 0;  // Free slot
            }
        } else {
            cyg_semaphore_wait(&ev->sem);
        }

#ifdef SPLSOFTNET_IS_A_MUTEX
        if ( self ) { // return to previous state
            cyg_mutex_lock( &mutex ); // this might wait
            owner = self; // got it now...
        }
        if ( olock > 1 )
            cyg_scheduler_lock();
    }
#endif

    return res;
}

// Called to initialize structures used by timeout functions
static void
cyg_timeout_init(void)
{
    int i;
    struct wakeup_event *ev;
    // Create list of "wakeup event" semaphores
    for (i = 0, ev = wakeup_list;  i < CYGPKG_NET_NUM_WAKEUP_EVENTS;  i++, ev++) {
        ev->chan = 0;
        cyg_semaphore_init(&ev->sem, 0);
    }
}

//
// Network software interrupt handler
//   This function is run as a separate thread to allow
// processing of network events (mostly incoming packets)
// at "user level" instead of at interrupt time.
//
static void
cyg_netint(cyg_addrword_t param)
{
    cyg_flag_value_t curisr;
    while (true) {
        curisr = cyg_flag_wait(&netint_flags, NETISR_ANY, 
                               CYG_FLAG_WAITMODE_OR|CYG_FLAG_WAITMODE_CLR);
        cyg_scheduler_lock();  // This code should not be preempted
#ifdef INET
        if (curisr & (1 << NETISR_ARP)) {
            // Pending ARP requests
            arpintr();
        }
        if (curisr & (1 << NETISR_IP)) {
            // Pending IPv4 input
            ipintr();
        }
#endif
#ifdef INET6
        if (curisr & (1 << NETISR_IPV6)) {
            // Pending IPv6 input
            ip6intr();
        }
#endif
        cyg_scheduler_unlock();
    }
}

//
// Network initialization
//   This function is called during system initialization to setup the whole
// networking environment.
//
extern void cyg_do_net_init(void);  // Linker magic to execute this function as 'init'
extern void ifinit(void);
extern void loopattach(int);

void
cyg_net_init(void)
{
    static int _init = false;
    cyg_netdevtab_entry_t *t;

    if (_init) return;
    cyg_do_net_init();  // Just forces the linking in of the initializer/constructor
    // Initialize interrupt "flags"
    cyg_flag_init(&netint_flags);
    // Anthing else needed?
#ifdef SPLINIT
    SPLINIT();
#endif
    // Create network background thread
    cyg_thread_create(CYGPKG_NET_THREAD_PRIORITY, // Priority
                      cyg_netint,               // entry
                      0,                        // entry parameter
                      "Network support",        // Name
                      &netint_stack[0],         // Stack
                      STACK_SIZE,               // Size
                      &netint_thread_handle,    // Handle
                      &netint_thread_data       // Thread data structure
            );
    cyg_thread_resume(netint_thread_handle);    // Start it
    // Initialize timeout support
    cyg_timeout_init();
    // Initialize network memory system
    cyg_kmem_init();
    mbinit();
    // Initialize all network devices
    for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
        diag_printf("Init device '%s'\n", t->name);
        if (t->init(t)) {
            t->status = CYG_NETDEVTAB_STATUS_AVAIL;
        } else {
            // What to do if device init fails?
            t->status = 0;  // Device not [currently] available
        }
    }
    // And attack the loopback interface
#ifdef CYGPKG_NET_NLOOP
#if 0 < CYGPKG_NET_NLOOP
    loopattach(0);
#endif
#endif
    // Start up the network processing
    ifinit();
    domaininit();
    // Done
    _init = true;
}
