/* ppc_stub.c - helper functions for stub, generic to all PowerPC processors
 * 
 * Copyright (c) 1998 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#ifdef CYG_HAL_POWERPC_SIM
#error "GDB Stub support not implemented for PPC SIM"
#endif

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/ppc_regs.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGIOT	6	/* IOT instruction */
#define	SIGABRT 6	/* used by abort, replace SIGIOT in the future */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */

/* Saved registers. */

HAL_SavedRegisters *_hal_registers;

target_register_t * _registers; /* A pointer to the current set of registers */
target_register_t registers[NUMREGS];
target_register_t alt_registers[NUMREGS] ; /* Thread or saved process state */


/* Given a trap value TRAP, return the corresponding signal. */

int __computeSignal (unsigned int trap_number)
{
    switch (trap_number)
    {
    case CYG_VECTOR_MACHINE_CHECK:
        /* Machine check */
    case CYG_VECTOR_DSI:
        /* Data access */
        return SIGSEGV;
      
    case CYG_VECTOR_ISI:
        /* Instruction access (Ifetch) */
    case CYG_VECTOR_ALIGNMENT:
        /* Data access */
        return SIGBUS;
                    
    case CYG_VECTOR_INTERRUPT:
        /* External interrupt */
      return SIGINT;

    case CYG_VECTOR_TRACE:
        /* Instruction trace */
        return SIGTRAP;
      
    case CYG_VECTOR_PROGRAM:
        switch ((get_register (PS) >> 17) & 0xf){
        case 1:                         /* trap */
            return SIGTRAP;
        case 2:                         /* privileged instruction */
        case 4:                         /* illegal instruction */
            return SIGILL;
        case 8:                         /* floating point */
            return SIGFPE;
        default:                        /* should never happen! */
            return SIGTERM;
        }            

    case CYG_VECTOR_RESERVED_A:
    case CYG_VECTOR_RESERVED_B:
        return SIGILL;

    case CYG_VECTOR_FP_UNAVAILABLE:
        /* FPU disabled */
    case CYG_VECTOR_FP_ASSIST:
        /* FPU assist */
        return SIGFPE;

    case CYG_VECTOR_DECREMENTER:
        /* Decrementer alarm */
        return SIGALRM;

    case CYG_VECTOR_SYSTEM_CALL:
        /* System call */
        return SIGSYS;

#ifdef CYG_HAL_POWERPC_MP860
    case CYG_VECTOR_SW_EMUL:
        /* A SW_EMUL is generated instead of PROGRAM for illegal
           instructions. */
        return SIGILL;

    case CYG_VECTOR_DATA_BP:
    case CYG_VECTOR_INSTRUCTION_BP:
    case CYG_VECTOR_PERIPHERAL_BP:
    case CYG_VECTOR_NMI:
        /* Developer port debugging exceptions. */
        return SIGTRAP;

    case CYG_VECTOR_ITLB_MISS:
    case CYG_VECTOR_DTLB_MISS:
        /* Software reload of TLB required. */
        return SIGTRAP;

    case CYG_VECTOR_ITLB_ERROR:
        /* Invalid instruction access. */
        return SIGBUS;

    case CYG_VECTOR_DTLB_ERROR:
        /* Invalid data access. */
        return SIGSEGV;
#endif
        
    default:
        return SIGTERM;
    }
}


/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
    // The vector is not not part of the GDB register set so get it
    // directly from the save context.
    return _hal_registers->vector >> 8;
}

// Return the currently-saved value corresponding to register REG of
// the exception context.
target_register_t get_register (regnames_t reg)
{
    return registers[reg];
}

// Store VALUE in the register corresponding to WHICH in the exception
// context.
void put_register (regnames_t which, target_register_t value)
{
    registers[which] = value;
}


/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */

void set_pc (target_register_t pc)
{
    put_register (PC, pc);
}


/*----------------------------------------------------------------------
 * Single-step support
 */

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */

static target_register_t irq_state;

void single_step (void)
{
    target_register_t msr = get_register (PS);

    // Set single-step flag in the exception context.
    msr |= (MSR_SE | MSR_BE);
    // Disable interrupts.
    irq_state = msr & MSR_EE;
    msr &= ~MSR_EE;

    put_register (PS, msr);
}

/* Clear the single-step state. */

void clear_single_step (void)
{
    target_register_t msr = get_register (PS);

    // Clear single-step flag in the exception context.
    msr &= ~(MSR_SE | MSR_BE);
    // Restore interrupt state.
    // FIXME: Should check whether the executed instruction changed the
    // interrupt state - or single-stepping a MSR changing instruction
    // may result in a wrong EE. Not a very likely scenario though.
    msr |= irq_state;

    put_register (PS, msr);
}


void install_breakpoints (void)
{
    /* NOP since single-step HW exceptions are used instead of
       breakpoints. */
}


/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */

int
__is_breakpoint_function ()
{
    return get_register (PC) == (target_register_t)&_breakinst;
}


/* Skip the current instruction.  Since this is only called by the
   stub when the PC points to a breakpoint or trap instruction,
   we can safely just skip 4. */

void __skipinst (void)
{
    put_register (PC, get_register (PC) + 4);
}


/* Write the 'T' packet in BUFFER. SIGVAL is the signal the program
   received. */

void __build_t_packet (int sigval, char *buf)
{
    target_register_t addr;
    char *ptr = buf;
    target_register_t *sp;

    sp = (target_register_t *) get_register (SP);

    *ptr++ = 'T';
    *ptr++ = __tohex (sigval >> 4);
    *ptr++ = __tohex (sigval);

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
    // Include thread ID if thread manipulation is required.
    {
        int id;
        
        *ptr++ = 't';
        *ptr++ = 'h';
        *ptr++ = 'r';
        *ptr++ = 'e';
        *ptr++ = 'a';
        *ptr++ = 'd';
        *ptr++ = ':';

        id = dbg_currthread_id ();
        ptr = __mem2hex((char *)&id, ptr, sizeof(id), 0);
        *ptr++ = ';';
    }
#endif


    *ptr++ = __tohex (PC >> 4);
    *ptr++ = __tohex (PC);
    *ptr++ = ':';
    addr = get_register (PC);
    ptr = __mem2hex((char *)&addr, ptr, sizeof(addr), 0);
    *ptr++ = ';';

    *ptr++ = __tohex (SP >> 4);
    *ptr++ = __tohex (SP);
    *ptr++ = ':';
    ptr = __mem2hex((char *)&sp, ptr, sizeof(sp), 0);
    *ptr++ = ';';
    
    *ptr++ = 0;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
