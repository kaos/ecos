//=============================================================================
//
//      synth_intr.c
//
//      Interrupt and clock code for the Linux synthetic target.
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    bartv
// Contributors: bartv, asl
// Date:         2001-03-30
// Purpose:      Implement the interrupt subsystem for the synthetic target
//####DESCRIPTIONEND####
//=============================================================================

// sigprocmask handling.
//
// In the synthetic target interrupts and exceptions are based around
// POSIX sighandlerals. When the clock ticks a SIGALRM signal is raised.
// When the I/O auxiliary wants to raise some other interrupt, it
// sends a SIGIO signal. When an exception occurs this results in
// signals like SIGILL and SIGSEGV. This implies an implementation
// where the VSR is the signal handler. Disabling interrupts then
// means using sigprocmask() to block certain signals, and enabling
// interrupts means unblocking those signals.
//
// However there are a few problems. One of these is performance: some
// bits of the system such as buffered tracing make very extensive use
// of enabling and disabling interrupts, so making a sigprocmask
// system call each time adds a lot of overhead. More seriously, there
// is a subtle discrepancy between POSIX signal handling and hardware
// interrupts. It is expected that signal handlers return, and then
// the system automatically passes control back to the foreground thread.
// In the process, the sigprocmask is manipulated before invoking the
// signal handler and restored afterwards. Interrupt handlers are
// different: it is quite likely that an interrupt results in another
// eCos thread being activated, so the signal handler does not actually
// return until the interrupted thread gets another chance to run.
//
// The second problem can be addressed by making the sigprocmask part of
// the thread state, saving and restoring it as part of a context switch.
// This matches quite nicely onto typical real hardware, where there might
// be a flag inside some control register that controls whether or not
// interrupts are enabled. However this adds further to the overhead.
//
// The alternative approach is to implement interrupt enabling and
// disabling in software. The sigprocmask is manipulated only once,
// during initialization, such that certain signals are allowed
// through and others are blocked. When a signal is raised the signal
// handler will always be invoked, but it will decide in software
// whether or not the signal should be processed immediately. This
// alternative approach does not correspond particularly well with
// real hardware: effectively the VSR is always allowed to run.
// However for typical applications this will not really matter, and
// the performance gains outweigh the discrepancy.
//
// Nested interrupts and interrupt priorities can be implemented in
// software, specifically by manipulating the current mask of blocked
// interrupts. This is not currently implemented.
//
// At first glance it might seem that an interrupt stack could be
// implemented trivially using sigaltstack. This does not quite work.
// The problem is that signal handlers do not always return
// immediately, so the system does not get a chance to clean up the
// signal handling stack. A separate interrupt stack is possible but
// would have to be implemented here, in software, e.g. by having the
// signal handler invoke the VSR on that stack. Unfortunately the
// system may have pushed quite a lot of state on to the current stack
// already when raising the signal, so things could get messy.

// ----------------------------------------------------------------------------
#include <pkgconf/hal.h>
#include <pkgconf/hal_synth.h>

// There are various dependencies on the kernel, e.g. how exceptions
// should be handled.
#include <pkgconf/system.h>
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>          // Assertions are safe in the synthetic target

// ----------------------------------------------------------------------------
// Statics.

// Are interrupts currently enabled?
volatile cyg_bool_t hal_interrupts_enabled = false;

// These flags are updated by the signal handler when a signal comes in
// and interrupts are disabled.
static volatile cyg_bool_t  synth_sigio_pending         = false;
static volatile cyg_bool_t  synth_sigalrm_pending       = false;

// The current VSR, to be invoked by the signal handler. This allows
// application code to install an alternative VSR, without that VSR
// having to check for interrupts being disabled and updating the
// pending flags. Effectively, the VSR is only invoked when interrupts
// are enabled.
static void (*synth_VSR)(void)                          = (void (*)(void)) 0;

// The current ISR status and mask registers, or rather software
// emulations there of. These are not static since
// application-specific VSRs may want to examine/manipulate these.
// They are also not exported in any header file, forcing people
// writing such VSRs to know what they are doing.
volatile cyg_uint32 synth_pending_isrs      = 0;
volatile cyg_uint32 synth_masked_isrs       = 0xFFFFFFFF;

// The vector of interrupt handlers.
typedef struct synth_isr_handler {
    cyg_ISR_t*          isr;
    CYG_ADDRWORD        data;
    CYG_ADDRESS         obj;
    cyg_priority_t      pri;
} synth_isr_handler;
static synth_isr_handler synth_isr_handlers[CYGNUM_HAL_ISR_COUNT];

static void  synth_alrm_sighandler(int);
static void  synth_io_sighandler(int);

// ----------------------------------------------------------------------------
// Basic ISR and VSR handling.

// The default ISR handler. The system should never receive an interrupt it
// does not know how to handle.
static cyg_uint32
synth_default_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    CYG_UNUSED_PARAM(cyg_vector_t, vector);
    CYG_UNUSED_PARAM(cyg_addrword_t, data);
    CYG_FAIL("Default isr handler should never get invoked");
    return CYG_ISR_HANDLED;
}

// The VSR is invoked under the following circumstances:
//
//  1) interrupts were globally enabled, but are now disabled.
//  2) there are pending ISRs which are not masked off
//
// The implementation is as per the HAL specification, where applicable.

static void
synth_default_vsr(void)
{
    int         isr_vector;
    cyg_uint32  isr_result;

    CYG_ASSERT(!hal_interrupts_enabled, "VSRs should only be invoked when interrupts are disabled");
    CYG_ASSERT(0 != (synth_pending_isrs & ~synth_masked_isrs), "VSRs should only be invoked when an interrupt is pending");

    // No need to save the cpu state. Either we are in a signal
    // handler and the system has done that for us, or we are called
    // synchronously via enable_interrupts.

    // Increment the kernel scheduler lock, if the kernel is present.
    // This prevents any attempts at context switching while interrupt
    // handling is in progress.
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    cyg_scheduler_lock();
#endif

    // Do not switch to an interrupt stack - functionality is not
    // implemented

    // Do not allow nested interrupts - functionality is not
    // implemented.

    // Decode the actual external interrupt being delivered. This is
    // determined from the pending and masked variables. Only one isr
    // source can be handled here, since interrupt_end must be invoked
    // with details of that interrupt.
    HAL_LSBIT_INDEX(isr_vector, (synth_pending_isrs & ~synth_masked_isrs));
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= isr_vector) && (isr_vector <= CYGNUM_HAL_ISR_MAX), "ISR vector must be valid");
    
    isr_result = (*synth_isr_handlers[isr_vector].isr)(isr_vector, synth_isr_handlers[isr_vector].data);

    // Do not switch back from the interrupt stack, there isn't one.
    
    // Interrupts were not enabled before, so they must be enabled
    // now. This may result in a recursive invocation if other IRQs
    // are still pending.
    hal_enable_interrupts();

    // Now call interrupt_end() with the result of the isr and the
    // ISR's object This may return straightaway, or it may result in
    // a context switch to another thread. In the latter case, when
    // the current thread is reactivated we end up back here. The
    // third argument should be a pointer to the saved state, but that
    // is only relevant for thread-aware debugging which is not
    // supported by the synthetic target.
    {
        extern void interrupt_end(cyg_uint32, CYG_ADDRESS, HAL_SavedRegisters*);
        interrupt_end(isr_result, synth_isr_handlers[isr_vector].obj, (HAL_SavedRegisters*) 0);
    }

    // Restore machine state and return to the interrupted thread.
    // That requires no effort here.
}

// Enabling interrupts. If a SIGALRM or SIGIO arrived at an inconvenient
// time, e.g. when already interacting with the auxiliary, then these
// will have been left pending and must be servied now. Next, enabling
// interrupts means checking the interrupt pending and mask registers
// and seeing if the VSR should be invoked.
void
hal_enable_interrupts(void)
{
    hal_interrupts_enabled = true;
    if (synth_sigalrm_pending) {
        synth_sigalrm_pending = false;
        synth_alrm_sighandler(CYG_HAL_SYS_SIGALRM);
    }
    if (synth_sigio_pending) {
        synth_sigio_pending = false;
        synth_io_sighandler(CYG_HAL_SYS_SIGIO);
    }

    // The interrupt mask "register" may have been modified while
    // interrupts were disabled. If there are pending interrupts,
    // invoke the VSR. The VSR must be invoked with interrupts
    // disabled, and will return with interrupts enabled.
    // An alternative implementation that might be more accurate
    // is to raise a signal, e.g. SIGUSR1. That way all interrupts
    // come in via the system's signal handling mechanism, and
    // it might be possible to do something useful with saved contexts
    // etc.
    if (0 != (synth_pending_isrs & ~synth_masked_isrs)) {
        hal_interrupts_enabled = false;
        (*synth_VSR)();
        CYG_ASSERT( hal_interrupts_enabled, "Interrupts should still be enabled on return from the VSR");
    }
}

// ----------------------------------------------------------------------------
// Other interrupt-related routines. Mostly these just involve
// updating some of the statics, but they may be called while
// interrupts are still enabled so care has to be taken.

cyg_bool_t
hal_interrupt_in_use(cyg_vector_t vec)
{
    CYG_ASSERT( (CYGNUM_HAL_ISR_MIN <= vec) && (vec <= CYGNUM_HAL_ISR_MAX), "Can only attach to valid ISR vectors");
    return synth_default_isr != synth_isr_handlers[vec].isr;
}

void
hal_interrupt_attach(cyg_vector_t vec, cyg_ISR_t* isr, CYG_ADDRWORD data, CYG_ADDRESS obj)
{
    CYG_ASSERT( (CYGNUM_HAL_ISR_MIN <= vec) && (vec <= CYGNUM_HAL_ISR_MAX), "Can only attach to valid ISR vectors");
    CYG_CHECK_FUNC_PTR( isr, "A valid ISR must be supplied");
    // The object cannot be validated, it may be NULL if chained
    // interrupts are enabled.
    CYG_ASSERT( synth_isr_handlers[vec].isr == &synth_default_isr, "Only one ISR can be attached to a vector at the HAL level");
    CYG_ASSERT( (false == hal_interrupts_enabled) || (0 != (synth_masked_isrs & (0x01 << vec))), "ISRs should only be attached when it is safe");

    // The priority will have been installed shortly before this call.
    synth_isr_handlers[vec].isr     = isr;
    synth_isr_handlers[vec].data    = data;
    synth_isr_handlers[vec].obj     = obj;
}

void
hal_interrupt_detach(cyg_vector_t vec, cyg_ISR_t* isr)
{
    CYG_ASSERT( (CYGNUM_HAL_ISR_MIN <= vec) && (vec <= CYGNUM_HAL_ISR_MAX), "Can only detach from valid ISR vectors");
    CYG_CHECK_FUNC_PTR( isr, "A valid ISR must be supplied");
    CYG_ASSERT( isr != &synth_default_isr, "An ISR must be attached before it can be detached");
    CYG_ASSERT( (false == hal_interrupts_enabled) || (0 != (synth_masked_isrs & (0x01 << vec))), "ISRs should only be detached when it is safe");

    // The Cyg_Interrupt destructor does an unconditional detach, even if the
    // isr is not currently attached.
    if (isr == synth_isr_handlers[vec].isr) {
        synth_isr_handlers[vec].isr     = &synth_default_isr;
        synth_isr_handlers[vec].data    = (CYG_ADDRWORD) 0;
        synth_isr_handlers[vec].obj     = (CYG_ADDRESS) 0;
    }
    
    // The priority is not updated here. This should be ok, if another
    // isr is attached then the appropriate priority will be installed
    // first.
}

void (*hal_vsr_get(cyg_vector_t vec))(void)
{
    CYG_ASSERT( (CYGNUM_HAL_VSR_MIN <= vec) && (vec <= CYGNUM_HAL_VSR_MAX), "Can only get valid VSR vectors");
    return synth_VSR;
}

void
hal_vsr_set(cyg_vector_t vec, void (*new_vsr)(void), void (**old_vsrp)(void))
{
    cyg_bool_t  old;

    CYG_ASSERT( (CYGNUM_HAL_VSR_MIN <= vec) && (vec <= CYGNUM_HAL_VSR_MAX), "Can only get valid VSR vectors");
    CYG_CHECK_FUNC_PTR( new_vsr, "A valid VSR must be supplied");

    // There is a theoretical possibility of two hal_vsr_set calls at
    // the same time. The old and new VSRs must be kept in synch.
    HAL_DISABLE_INTERRUPTS(old);
    if (0 != old_vsrp) {
        *old_vsrp = synth_VSR;
    }
    synth_VSR = new_vsr;
    HAL_RESTORE_INTERRUPTS(old);
}

void
hal_interrupt_mask(cyg_vector_t which)
{
    CYG_PRECONDITION( !hal_interrupts_enabled, "Interrupts should be disabled on entry to hal_interrupt_mask");
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= which) && (which <= CYGNUM_HAL_ISR_MAX), "A valid ISR vector must be supplied");
    synth_masked_isrs |= (0x01 << which);
}

void
hal_interrupt_unmask(cyg_vector_t which)
{
    CYG_PRECONDITION( !hal_interrupts_enabled, "Interrupts should be disabled on entry to hal_interrupt_unmask");
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= which) && (which <= CYGNUM_HAL_ISR_MAX), "A valid ISR vector must be supplied");
    synth_masked_isrs &= ~(0x01 << which);
}

void
hal_interrupt_acknowledge(cyg_vector_t which)
{
    cyg_bool_t old;
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= which) && (which <= CYGNUM_HAL_ISR_MAX), "A valid ISR vector must be supplied");

    // Acknowledging an interrupt means clearing the bit in the
    // interrupt pending "register".
    // NOTE: does the auxiliary need to keep track of this? Probably
    // not, the auxiliary can just raise SIGIO whenever a device wants
    // attention. There may be a trade off here between additional
    // communication and unnecessary SIGIOs.
    HAL_DISABLE_INTERRUPTS(old);
    synth_pending_isrs &= ~(0x01 << which);
    HAL_RESTORE_INTERRUPTS(old);
}

void
hal_interrupt_configure(cyg_vector_t which, cyg_bool_t level, cyg_bool_t up)
{
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= which) && (which <= CYGNUM_HAL_ISR_MAX), "A valid ISR vector must be supplied");
    // The synthetic target does not currently distinguish between
    // level and edge interrupts. Possibly this information will have
    // to be passed on to the auxiliary in future.
    CYG_UNUSED_PARAM(cyg_vector_t, which);
    CYG_UNUSED_PARAM(cyg_bool_t, level);
    CYG_UNUSED_PARAM(cyg_bool_t, up);
}

void
hal_interrupt_set_level(cyg_vector_t which, cyg_priority_t level)
{
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= which) && (which <= CYGNUM_HAL_ISR_MAX), "A valid ISR vector must be supplied");
    // The legal values for priorities are not defined at this time.
    // Manipulating the interrupt priority level currently has no
    // effect. The information is stored anyway, for future use.
    synth_isr_handlers[which].pri = level;
}

// ----------------------------------------------------------------------------
// Exception handling. Typically this involves calling into the kernel,
// translating the POSIX signal number into a HAL exception number. In
// practice these signals will generally be caught in the debugger and
// will not have to be handled by eCos itself.

static void
synth_exception_sighandler(int sig)
{
    CYG_WORD    ecos_exception_number = 0;
    cyg_bool_t  old;

    // There is no need to save state, that will have been done by the
    // system as part of the signal delivery process.
    
    // Disable interrupts. Performing e.g. an interaction with the
    // auxiliary after a SIGSEGV is dubious.
    HAL_DISABLE_INTERRUPTS(old);

    // Now decode the signal and turn it into an eCos exception.
    switch(sig) {
      case CYG_HAL_SYS_SIGILL:
        ecos_exception_number = CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION;
        break;
      case CYG_HAL_SYS_SIGBUS:
      case CYG_HAL_SYS_SIGSEGV:
        ecos_exception_number = CYGNUM_HAL_EXCEPTION_DATA_ACCESS;
        break;
      case CYG_HAL_SYS_SIGFPE:
        ecos_exception_number = CYGNUM_HAL_EXCEPTION_FPU;
        break;
      default:
        CYG_FAIL("Unknown signal");
        break;
    }

#ifdef CYGPKG_KERNEL_EXCEPTIONS
    // Deliver the signal, usually to the kernel, possibly to the
    // common HAL. The second argument should be the current
    // savestate, but that is not readily accessible.
    cyg_hal_deliver_exception(ecos_exception_number, (CYG_ADDRWORD) 0);

    // It is now necessary to restore the machine state, including
    // interrupts. In theory higher level code may have manipulated
    // the machine state to prevent any recurrence of the exception.
    // In practice the machine state is not readily accessible.
    HAL_RESTORE_INTERRUPTS(old);
#else
    CYG_FAIL("Exception!!!");
    for (;;);
#endif    
}

// ----------------------------------------------------------------------------
// The clock support. This can be implemented using the setitimer()
// and getitimer() calls. The kernel will install a suitable interrupt
// handler for CYGNUM_HAL_INTERRUPT_RTC, but it depends on the HAL
// for low-level manipulation of the clock hardware.
//
// There is a problem with HAL_CLOCK_READ(). The obvious implementation
// would use getitimer(), but that has the wrong behaviour: it is intended
// for fairly coarse intervals and works in terms of system clock ticks,
// as opposed to a fine-grained implementation that actually examines the
// system clock. Instead it is necessary to use gettimeofday(). 

static struct cyg_hal_sys_timeval synth_clock   = { 0, 0 };

void
hal_clock_initialize(cyg_uint32 period)
{
    struct cyg_hal_sys_itimerval    timer;

    // Needed for hal_clock_read()
    cyg_hal_sys_gettimeofday(&synth_clock, (struct cyg_hal_sys_timezone*) 0);
    
    // The synthetic target clock resolution is in microseconds. A typical
    // value for the period will be 10000, corresponding to one timer
    // interrupt every 10ms. Set up a timer to interrupt in period us,
    // and again every period us after that.
    CYG_ASSERT( period < 1000000, "Clock interrupts should happen at least once per second");
    timer.hal_it_interval.hal_tv_sec    = 0;
    timer.hal_it_interval.hal_tv_usec   = period;
    timer.hal_it_value.hal_tv_sec       = 0;
    timer.hal_it_value.hal_tv_usec      = period;
    
    if (0 != cyg_hal_sys_setitimer(CYG_HAL_SYS_ITIMER_REAL, &timer, (struct cyg_hal_sys_itimerval*) 0)) {
        CYG_FAIL("Failed to initialize the clock itimer");
    }
}

static void
synth_alrm_sighandler(int sig)
{
    CYG_PRECONDITION((CYG_HAL_SYS_SIGALRM == sig), "Only SIGALRM should be handled here");
    
    if (!hal_interrupts_enabled) {
        synth_sigalrm_pending = true;
        return;
    }

    // Interrupts were enabled, but must be blocked before any further processing.
    hal_interrupts_enabled = false;

    // Update the cached value of the clock for hal_clock_read()
    cyg_hal_sys_gettimeofday(&synth_clock, (struct cyg_hal_sys_timezone*) 0);

    // Update the interrupt status "register" to match pending interrupts
    // A timer signal means that IRQ 0 needs attention.
    synth_pending_isrs |= 0x01;

    // If any of the pending interrupts are not masked, invoke the VSR
    if (0 != (synth_pending_isrs & ~synth_masked_isrs)) {
        (*synth_VSR)();
    } else {
        hal_interrupts_enabled = true;
    }

    // The VSR will have invoked interrupt_end() with interrupts
    // enabled, and they should still be enabled.
    CYG_ASSERT( hal_interrupts_enabled, "Interrupts should still be enabled on return from the VSR");
}

// Implementing hal_clock_read(). gettimeofday() in conjunction with
// synth_clock gives the time since the last clock tick in
// microseconds, the correct unit for the synthetic target.
cyg_uint32
hal_clock_read(void)
{
    int elapsed;
    struct cyg_hal_sys_timeval  now;
    cyg_hal_sys_gettimeofday(&now, (struct cyg_hal_sys_timezone*) 0);

    elapsed = (1000000 * (now.hal_tv_sec - synth_clock.hal_tv_sec)) + (now.hal_tv_usec - synth_clock.hal_tv_usec);
    return elapsed;
}

// ----------------------------------------------------------------------------
// The signal handler for SIGIO. This can also be invoked by
// hal_enable_interrupts() to catch up with any signals that arrived
// while interrupts were disabled. SIGIO is raised by the auxiliary
// when it requires attention, i.e. when one or more of the devices
// want to raise an interrupt. Finding out exactly which interrupt(s)
// are currently pending in the auxiliary requires communication with
// the auxiliary.
//
// If interrupts are currently disabled then the signal cannot be
// handled immediately. In particular SIGIO cannot be handled because
// there may already be ongoing communication with the auxiliary.
// Instead some volatile flags are used to keep track of which signals
// were raised while interrupts were disabled. 

static void
synth_io_sighandler(int sig)
{
    CYG_PRECONDITION((CYG_HAL_SYS_SIGIO == sig), "Only SIGIO should be handled here");
    
    if (!hal_interrupts_enabled) {
        synth_sigio_pending = true;
        return;
    }
    
    // Interrupts were enabled, but must be blocked before any further processing.
    hal_interrupts_enabled = false;

    // Update the interrupt status "register" to match pending interrupts
    // Contact the auxiliary to find out what interrupts are currently pending there.
    CYG_FAIL("SIGIO should not occur until the auxiliary is implemented");

    // If any of the pending interrupts are not masked, invoke the VSR
    if (0 != (synth_pending_isrs & ~synth_masked_isrs)) {
        (*synth_VSR)();
    } else {
        hal_interrupts_enabled = true;
    }

    // The VSR will have invoked interrupt_end() with interrupts
    // enabled, and they should still be enabled.
    CYG_ASSERT( hal_interrupts_enabled, "Interrupts should still be enabled on return from the VSR");
}

// ----------------------------------------------------------------------------
// SIGPIPE and SIGCHLD are special, related to the auxiliary process.
static void
synth_pipe_sighandler(int sig)
{
    CYG_ASSERT(CYG_HAL_SYS_SIGPIPE == sig, "The right signal handler should be invoked");
    CYG_FAIL("The auxiliary is not yet implemented");
}

static void
synth_chld_sighandler(int sig)
{
    CYG_ASSERT(CYG_HAL_SYS_SIGCHLD == sig, "The right signal handler should be invoked");
    CYG_FAIL("The auxiliary is not yet implemented");
}

// ----------------------------------------------------------------------------
// Here we define an action to do in the idle thread. For the
// synthetic architecture it makes no sense to spin eating processor
// time that other processes could make use of. Instead we call
// select. The itimer will still go off and kick the scheduler back
// into life so giving up an escape path from the select. There is one
// problem: in some configurations, e.g. when preemption is disabled,
// the idle thread must yield continuously rather than blocking.
void
hal_idle_thread_action(cyg_uint32 loop_count)
{
#ifndef CYGIMP_IDLE_THREAD_YIELD
    cyg_hal_sys__newselect(0,
                           (struct cyg_hal_sys_fd_set*) 0,
                           (struct cyg_hal_sys_fd_set*) 0,
                           (struct cyg_hal_sys_fd_set*) 0,
                           (struct cyg_hal_sys_timeval*) 0);
#endif
    CYG_UNUSED_PARAM(cyg_uint32, loop_count);
}
// ----------------------------------------------------------------------------
// Initialization

void
hal_synthetic_target_init(void)
{
    struct cyg_hal_sys_sigaction action;
    struct cyg_hal_sys_sigset_t  blocked;
    int i;

    // Set up a sigprocmask to block all signals except the ones we
    // particularly want to handle. However do not block the tty
    // signals - the ability to ctrl-C a program is important.
    CYG_HAL_SYS_SIGFILLSET(&blocked);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGILL);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGBUS);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGFPE);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGSEGV);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGPIPE);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGCHLD);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGALRM);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGIO);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGHUP);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGINT);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGQUIT);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGTERM);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGCONT);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGSTOP);
    CYG_HAL_SYS_SIGDELSET(&blocked, CYG_HAL_SYS_SIGTSTP);
        
    if (0 != cyg_hal_sys_sigprocmask(CYG_HAL_SYS_SIG_SETMASK, &blocked, (cyg_hal_sys_sigset_t*) 0)) {
        CYG_FAIL("Failed to initialize sigprocmask");
    }

    // Now set up the VSR and ISR statics
    synth_VSR = &synth_default_vsr;
    for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++) {
        synth_isr_handlers[i].isr       = &synth_default_isr;
        synth_isr_handlers[i].data      = (CYG_ADDRWORD) 0;
        synth_isr_handlers[i].obj       = (CYG_ADDRESS) 0;
        synth_isr_handlers[i].pri       = CYGNUM_HAL_ISR_COUNT;
    }

    // Install signal handlers for SIGIO and SIGALRM, the two signals
    // that may cause the VSR to run. SA_NODEFER is important: it
    // means that the current signal will not be blocked while the
    // signal handler is running. Combined with a mask of 0, it means
    // that the sigprocmask does not change when a signal handler is
    // invoked, giving eCos the flexibility to switch to other threads
    // instead of having the signal handler return immediately.
    action.hal_mask     = 0;
    action.hal_flags    = CYG_HAL_SYS_SA_NODEFER;
    action.hal_bogus    = (void (*)(int)) 0;
    action.hal_handler  = &synth_alrm_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGALRM, &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGALRM");
    }
    action.hal_handler  = &synth_io_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGIO, &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGIO");
    }

    // Now install handlers for the various exceptions. For now these
    // also operate with unchanged sigprocmasks, allowing nested
    // exceptions. It is not clear that this is entirely a good idea,
    // but in practice these exceptions will usually be handled by gdb
    // anyway.
    action.hal_handler  = &synth_exception_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGILL,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGILL");
    }
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGBUS,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGBUS");
    }
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGFPE,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGFPE");
    }
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGSEGV,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGSEGV");
    }

    // Also cope with SIGCHLD and SIGPIPE. SIGCHLD indicates that the
    // auxiliary has terminated, which is a bad thing. SIGPIPE
    // indicates that a write to the auxiliary has terminated, but
    // the error condition was caught at a different stage.
    action.hal_handler = &synth_pipe_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGPIPE,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGPIPE");
    }
    action.hal_handler = &synth_chld_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGCHLD,  &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGCHLD");
    }
    
    // All done. At this stage interrupts are still disabled, no ISRs
    // have been installed, and the clock is not yet ticking.
    // Exceptions can come in and will be processed normally. SIGIO
    // and SIGALRM could come in, but nothing has yet been done
    // to make that happen.
}
