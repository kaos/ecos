/* mips-stub.c - helper functions for stub, generic to all MIPS processors
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

#include "cyg/hal/hal_stub.h"

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include "cyg/hal/hal_arch.h"
#include <cyg/hal/mips-regs.h>
#include <cyg/hal/mips_opcode.h>

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
    case EXC_INT:
      /* External interrupt */
      return SIGINT;

    case EXC_RI:
      /* Reserved instruction */
    case EXC_CPU:
      /* Coprocessor unusable */
      return SIGILL;

    case EXC_BP:
      /* Break point */
      return SIGTRAP;

    case EXC_OVF:
      /* Arithmetic overflow */
    case EXC_TRAP:
      /* Trap exception */
    case EXC_FPE:
      /* Floating Point Exception */
      return SIGFPE;

    case EXC_IBE:
      /* Bus error (Ifetch) */
    case EXC_DBE:
      /* Bus error (data load or store) */
      return SIGBUS;

    case EXC_MOD:
      /* TLB modification exception */
    case EXC_TLBL:
      /* TLB miss (Load or Ifetch) */
    case EXC_TLBS:
      /* TLB miss (Store) */
    case EXC_ADEL:
      /* Address error (Load or Ifetch) */
    case EXC_ADES:
      /* Address error (Store) */
      return SIGSEGV;

    case EXC_SYS:
      /* System call */
      return SIGSYS;

    default:
      return SIGTERM;
    }
}


/* Return the trap number corresponding to the last-taken trap. */

int __get_trap_number (void)
{
  return (get_register (CAUSE) & CAUSE_EXCMASK) >> CAUSE_EXCSHIFT;
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

/* Saved instruction data for single step support.  */

typedef unsigned long t_inst;

static struct
{
  t_inst *targetAddr;
  t_inst savedInstr;
} instrBuffer;


/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */

void single_step (void)
{
  InstFmt inst;
  t_inst *pc = (t_inst *) get_register (PC);

  instrBuffer.targetAddr = pc + 1;		/* set default */

  inst.word = *pc;				/* read the next instruction  */

//diag_printf("pc %08x %08x\n",pc,inst.word);
 
  switch (inst.RType.op) {                    /* override default if branch */
    case OP_SPECIAL:
      switch (inst.RType.func) {
        case OP_JR:
        case OP_JALR:
          instrBuffer.targetAddr = (t_inst *) get_register (inst.RType.rs);
          break;
      };
      break;

    case OP_REGIMM:
      switch (inst.IType.rt) {
        case OP_BLTZ:
        case OP_BLTZL:
        case OP_BLTZAL:
        case OP_BLTZALL:
          if (get_register (inst.IType.rs) < 0 )
            instrBuffer.targetAddr =
              (t_inst *)(((signed short)inst.IType.imm<<2)
                                        + (get_register (PC) + 4));
          else
            instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
          break;
        case OP_BGEZ:
        case OP_BGEZL:
        case OP_BGEZAL:
        case OP_BGEZALL:
          if (get_register (inst.IType.rs) >= 0 )
            instrBuffer.targetAddr =
              (t_inst *)(((signed short)inst.IType.imm<<2)
                                        + (get_register (PC) + 4));
          else
            instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
          break;
      };
      break;

    case OP_J:
    case OP_JAL:
      instrBuffer.targetAddr =
        (t_inst *)((inst.JType.target<<2)
		   + ((get_register (PC) + 4)&0xf0000000));
      break;

    case OP_BEQ:
    case OP_BEQL:
      if (get_register (inst.IType.rs) == get_register (inst.IType.rt))
        instrBuffer.targetAddr =
          (t_inst *)(((signed short)inst.IType.imm<<2)
		     + (get_register (PC) + 4));
      else
        instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
      break;
    case OP_BNE:
    case OP_BNEL:
      if (get_register (inst.IType.rs) != get_register (inst.IType.rt))
        instrBuffer.targetAddr =
          (t_inst *)(((signed short)inst.IType.imm<<2)
		     + (get_register (PC) + 4));
      else
        instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
      break;
    case OP_BLEZ:
    case OP_BLEZL:
      if (get_register (inst.IType.rs) <= 0)
        instrBuffer.targetAddr =
          (t_inst *)(((signed short)inst.IType.imm<<2) + (get_register (PC) + 4));
      else
        instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
      break;
    case OP_BGTZ:
    case OP_BGTZL:
      if (get_register (inst.IType.rs) > 0)
        instrBuffer.targetAddr =
          (t_inst *)(((signed short)inst.IType.imm<<2) + (get_register (PC) + 4));
      else
        instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
      break;

#if 0	/* no floating point support at the moment */
    case OP_COP1:
      if (inst.RType.rs == OP_BC)
          switch (inst.RType.rt) {
            case COPz_BCF:
            case COPz_BCFL:
              if (get_register (FCSR) & CSR_C)
                instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
              else
                instrBuffer.targetAddr =
                  (t_inst *)(((signed short)inst.IType.imm<<2)
                                            + (get_register (PC) + 4));
              break;
            case COPz_BCT:
            case COPz_BCTL:
              if (get_register (FCSR) & CSR_C)
                instrBuffer.targetAddr =
                  (t_inst *)(((signed short)inst.IType.imm<<2)
                                            + (get_register (PC) + 4));
              else
                instrBuffer.targetAddr = (t_inst *)(get_register (PC) + 8);
              break;
          };
      break;
#endif

  }
}


/* Clear the single-step state. */

void clear_single_step (void)
{
//diag_printf("clear_ss ta %08x\n",instrBuffer.targetAddr);
  if (instrBuffer.targetAddr != NULL)
    {
      *instrBuffer.targetAddr = instrBuffer.savedInstr;
      instrBuffer.targetAddr = NULL;
    }
  instrBuffer.savedInstr = NOP_INSTR;
}


void install_breakpoints ()
{
//diag_printf("install_bpt ta %08x\n",instrBuffer.targetAddr);
  if (instrBuffer.targetAddr != NULL)
    {
      instrBuffer.savedInstr = *instrBuffer.targetAddr;
      *instrBuffer.targetAddr = __break_opcode ();
//diag_printf("ta %08x si %08x *ta %08x\n",
//            instrBuffer.targetAddr,instrBuffer.savedInstr,*instrBuffer.targetAddr);
    }
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
        extern int dbg_currthread_id(void); // FIXME: get from header instead

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
