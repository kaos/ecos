//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
//
//=============================================================================

// - pjo, 28 sep 1999
// - Copied ARM version for use with i386/pc.

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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas (based on the old ARM/AEB hal_stub.c)
// Contributors:gthomas, jskov, pjo, nickg
// Date:        1999-02-15
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros

#include <cyg/hal/plf_misc.h>

#ifdef CYGPKG_IO_SERIAL
#include <pkgconf/io_serial.h>
#endif

//-----------------------------------------------------------------------------
// Serial definitions.

// 38400, COM1
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB    0
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB    3

// This is the base address of COM1.
#define CYG_DEVICE_SERIAL_RS232_8250_BASE      0x3F8

// Define the serial registers.
#define CYG_DEVICE_SERIAL_RS232_8250_RBR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x00)
    // receiver buffer register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_8250_THR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x00)
    // transmitter holding register, write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_8250_DLL \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x00)
    // divisor latch (LS), read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_8250_IER \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x01)
    // interrupt enable register, read/write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_8250_DLM \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x01)
    // divisor latch (MS), read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_8250_IIR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x02)
    // interrupt identification register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_8250_FCR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x02)
    // fifo control register, write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_8250_LCR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x03)
    // line control register, read/write
#define CYG_DEVICE_SERIAL_RS232_8250_MCR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x4)
    // modem control register, read/write
#define CYG_DEVICE_SERIAL_RS232_8250_LSR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x5)
    // line status register, read
#define CYG_DEVICE_SERIAL_RS232_8250_MSR \
    (CYG_DEVICE_SERIAL_RS232_8250_BASE + 0x6)
    // modem status register, read

# define CYGNUM_HAL_INTERRUPT_COM1 (4 + 32)

// The interrupt enable register bits.
#define SIO_IER_ERDAI   0x01            // enable received data available irq
#define SIO_IER_ETHREI  0x02            // enable THR empty interrupt
#define SIO_IER_ELSI    0x04            // enable receiver line status irq
#define SIO_IER_EMSI    0x08            // enable modem status interrupt

// The interrupt identification register bits.
#define SIO_IIR_IP      0x01            // 0 if interrupt pending
#define SIO_IIR_ID_MASK 0x0e            // mask for interrupt ID bits

// The line status register bits.
#define SIO_LSR_DR      0x01            // data ready
#define SIO_LSR_OE      0x02            // overrun error
#define SIO_LSR_PE      0x04            // parity error
#define SIO_LSR_FE      0x08            // framing error
#define SIO_LSR_BI      0x10            // break interrupt
#define SIO_LSR_THRE    0x20            // transmitter holding register empty
#define SIO_LSR_TEMT    0x40            // transmitter register empty
#define SIO_LSR_ERR     0x80            // any error condition

// The modem status register bits.
#define SIO_MSR_DCTS  0x01              // delta clear to send
#define SIO_MSR_DDSR  0x02              // delta data set ready
#define SIO_MSR_TERI  0x04              // trailing edge ring indicator
#define SIO_MSR_DDCD  0x08              // delta data carrier detect
#define SIO_MSR_CTS   0x10              // clear to send
#define SIO_MSR_DSR   0x20              // data set ready
#define SIO_MSR_RI    0x40              // ring indicator
#define SIO_MSR_DCD   0x80              // data carrier detect

// The line control register bits.
#define SIO_LCR_WLS0   0x01             // word length select bit 0
#define SIO_LCR_WLS1   0x02             // word length select bit 1
#define SIO_LCR_STB    0x04             // number of stop bits
#define SIO_LCR_PEN    0x08             // parity enable
#define SIO_LCR_EPS    0x10             // even parity select
#define SIO_LCR_SP     0x20             // stick parity
#define SIO_LCR_SB     0x40             // set break
#define SIO_LCR_DLAB   0x80             // divisor latch access bit

// Interrupt Enable Register
#define SIO_IER_RCV 0x01
#define SIO_IER_XMT 0x02
#define SIO_IER_LS  0x04
#define SIO_IER_MS  0x08

// Modem Control Register
#define SIO_MCR_DTR 0x01
#define SIO_MCR_RTS 0x02
#define SIO_MCR_INT 0x08   // Enable interrupts

// FIFO control register
#define SIO_FCR_FE	0x01
#define SIO_FCR_RFR	0x02
#define SIO_FCR_TFR	0x04

#define SIO_FCR_ENABLE (SIO_FCR_FE | SIO_FCR_RFR | SIO_FCR_TFR)
#define SIO_FCR_DISABLE (0)

#if 0 //def CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
static cyg_interrupt gdb_interrupt;
static cyg_handle_t  gdb_interrupt_handle;
#endif

void hal_screen_putc(int c);

//-----------------------------------------------------------------------------

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

CYG_ADDRWORD hal_pc_break_pc = 0 ;

// This ISR is called only for serial receive interrupts.
int 
cyg_hal_gdb_isr(CYG_WORD32 vector, CYG_ADDRWORD data, CYG_ADDRWORD *regs)
{
    CYG_BYTE c;
    cyg_uint8 lsr;

    // Check for a character. If it has already been taken
    // just return with nothing to do.
    lsr = pc_inb(CYG_DEVICE_SERIAL_RS232_8250_LSR);
    if ((lsr & SIO_LSR_DR) == 0)
    {
        // Ensure that the interrupt will not reoccur.
        HAL_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_COM1);        
        return 0;
    }
 
    c = hal_pc_get_char();  // Fetch the character

    if( 3 == c ) {  // ^C
        // Ctrl-C: set a breakpoint at PC so GDB will display the
        // correct program context when stopping rather than the
        // interrupt handler.
        hal_pc_break_pc = regs[9] ;      
#if 1
        cyg_hal_gdb_interrupt (hal_pc_break_pc);
#else
      
        asm("movl %0, %%eax
             movl %%eax, %%dr0
             movl %%dr7, %%eax
             andl $0xFFF0FFFC, %%eax
             orl $0x00000002, %%eax
             movl %%eax, %%dr7"
            :	/* No outputs */
            :	"m" (hal_pc_break_pc)
            :	"eax");
#endif                
        HAL_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_COM1);
    }

    return 0;  // No need to run DSR
}

int 
hal_pc_interruptable(int state)
{
    if (state) {
        HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_COM1);
    } else {
        HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_COM1);
    }

    return 0;
}
#endif

//-----------------------------------------------------------------------------
// Initialize the current serial port.

void hal_pc_init_serial(void)
{
    cyg_uint8 lcr;

    // 8-1-no parity.
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_LCR, SIO_LCR_WLS0 | SIO_LCR_WLS1);

//  lcr = pc_inb(CYG_DEVICE_SERIAL_RS232_8250_LCR);
    lcr = 3 ;
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_LCR, lcr | SIO_LCR_DLAB);
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_DLL, CYG_DEVICE_SERIAL_RS232_BAUD_LSB);
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_DLM, CYG_DEVICE_SERIAL_RS232_BAUD_MSB);
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_LCR, lcr);

    // Maybe enable FIFOs... rumor has it that 16550 FIFOs don't work but that
    //	16550As do.
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_FCR, SIO_FCR_ENABLE) ;
    if ((pc_inb(CYG_DEVICE_SERIAL_RS232_8250_FCR) & 0xC0) != 0xC0)
        pc_outb(CYG_DEVICE_SERIAL_RS232_8250_FCR, SIO_FCR_DISABLE) ;
    
#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT) && !defined(CYGPKG_IO_SERIAL_I386_PC_SERIAL0)
#if 0        
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_COM1,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             (CYG_ADDRESS)cyg_hal_gdb_isr,
                             0,
                             &gdb_interrupt_handle,
                             &gdb_interrupt);
    cyg_drv_interrupt_attach(gdb_interrupt_handle);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_COM1);
#endif

    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_COM1,
                          cyg_hal_gdb_isr,
                          0,
                          0);
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_COM1 );
    
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_IER, SIO_IER_RCV);
    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_MCR, SIO_MCR_INT|SIO_MCR_DTR|SIO_MCR_RTS);
#endif
}


short * screenPointer = (short*) 0xB8000 ;
int screenPosition = 0 ;


// Write C to the current serial port.
void hal_pc_put_char(int c)
{
    cyg_uint8 lsr;

    screenPointer[screenPosition++] = 0x0700 | (c & 0xFF) ;
    screenPosition %= (80 * 25) ;

    do {
        lsr = pc_inb(CYG_DEVICE_SERIAL_RS232_8250_LSR);
    } while ((lsr & SIO_LSR_THRE) == 0);

    pc_outb(CYG_DEVICE_SERIAL_RS232_8250_THR, c);
}


// Read one character from the current serial port.
int hal_pc_get_char(void)
{
    char c;
    cyg_uint8 lsr;

    do {
        lsr = pc_inb(CYG_DEVICE_SERIAL_RS232_8250_LSR);
    } while ((lsr & SIO_LSR_DR) == 0);

    c = pc_inb(CYG_DEVICE_SERIAL_RS232_8250_RBR);

    screenPointer[screenPosition++] = 0x7000 | (c & 0xFF) ;
    screenPosition %= (80 * 25) ;

    return c;
}

//-----------------------------------------------------------------------------
// All of this is borrowed from gdb/i386-stub.c.

int hal_pc_exception_handler(int vector, void * routine)
{
    short * p = (short*) 0 ;
    int i = vector * 4 ;
    int r = (int) routine ;

    /* Construct an interrupt gate for the spec'd vector. */
    p[i++] = (r & 0xFFFF) ;
    p[i++] = 8 ;
    p[i++] = 0x8E00 ;
    p[i++] = (r >> 16) ;

    return 0 ;
}



int hal_pc_trap_registers[NUMREGS] ;
int hal_pc_trap_number ;
int gdb_i386errcode;

/* GDB stores segment registers in 32-bit words (that's just the way
   m-i386v.h is written).  So zero the appropriate areas in registers.  */
#define SAVE_REGISTERS1()                       \
  asm ("movl %eax, hal_pc_trap_registers");     \
  asm ("movl %ecx, hal_pc_trap_registers+4");   \
  asm ("movl %edx, hal_pc_trap_registers+8");   \
  asm ("movl %ebx, hal_pc_trap_registers+12");  \
  asm ("movl %ebp, hal_pc_trap_registers+20");  \
  asm ("movl %esi, hal_pc_trap_registers+24");  \
  asm ("movl %edi, hal_pc_trap_registers+28");  \
  asm ("movw $0, %ax");                         \
  asm ("movw %ds, hal_pc_trap_registers+48");   \
  asm ("movw %ax, hal_pc_trap_registers+50");   \
  asm ("movw %es, hal_pc_trap_registers+52");   \
  asm ("movw %ax, hal_pc_trap_registers+54");   \
  asm ("movw %fs, hal_pc_trap_registers+56");   \
  asm ("movw %ax, hal_pc_trap_registers+58");   \
  asm ("movw %gs, hal_pc_trap_registers+60");   \
  asm ("movw %ax, hal_pc_trap_registers+62");

#define SAVE_ERRCODE() \
  asm ("popl %ebx"); \
  asm ("movl %ebx, gdb_i386errcode");


#define SAVE_REGISTERS2() \
  asm ("popl %ebx"); /* old eip */			  		                 \
  asm ("movl %ebx, hal_pc_trap_registers+32");			  		     \
  asm ("popl %ebx");	 /* old cs */			  		             \
  asm ("movl %ebx, hal_pc_trap_registers+40");			  		     \
  asm ("movw %ax, hal_pc_trap_registers+42");                        \
  asm ("popl %ebx");	 /* old eflags */		  		             \
  asm ("movl %ebx, hal_pc_trap_registers+36");			 		     \
/* Now that we've done the pops, we can save the stack pointer. */   \
  asm ("movw %ss, hal_pc_trap_registers+44");					     \
  asm ("movw %ax, hal_pc_trap_registers+46");     	       	         \
  asm ("movl %esp, hal_pc_trap_registers+16");



#define CALL_HOOK() \
	asm("popl %eax; movl %eax, hal_pc_trap_number; call __handle_exception; jmp hal_pc_trap_exit")

asm(".text");
asm(".globl return_to_prog");
asm("hal_pc_trap_exit:");
asm("        movw registers+44, %ss");
asm("        movl registers+16, %esp");
asm("        movl registers+4, %ecx");
asm("        movl registers+8, %edx");
asm("        movl registers+12, %ebx");
asm("        movl registers+20, %ebp");
asm("        movl registers+24, %esi");
asm("        movl registers+28, %edi");
asm("        movw registers+48, %ds");
asm("        movw registers+52, %es");
asm("        movw registers+56, %fs");
asm("        movw registers+60, %gs");
asm("        movl registers+36, %eax");
asm("        pushl %eax");  /* saved eflags */
asm("        movl registers+40, %eax");
asm("        pushl %eax");  /* saved cs */
asm("        movl registers+32, %eax");
asm("        pushl %eax");  /* saved eip */
asm("        movl registers, %eax");
/* use iret to restore pc and flags together so
   that trace flag works right.  */
asm("        iret");



#if 0
extern void return_to_prog ();

/* Restore the program's registers (including the stack pointer, which
   means we get the right stack and don't have to worry about popping our
   return address and any stack frames and so on) and return.  */
asm(".text");
asm(".globl return_to_prog");
asm("return_to_prog:");
asm("        movw registers+44, %ss");
asm("        movl registers+16, %esp");
asm("        movl registers+4, %ecx");
asm("        movl registers+8, %edx");
asm("        movl registers+12, %ebx");
asm("        movl registers+20, %ebp");
asm("        movl registers+24, %esi");
asm("        movl registers+28, %edi");
asm("        movw registers+48, %ds");
asm("        movw registers+52, %es");
asm("        movw registers+56, %fs");
asm("        movw registers+60, %gs");
asm("        movl registers+36, %eax");
asm("        pushl %eax");  /* saved eflags */
asm("        movl registers+40, %eax");
asm("        pushl %eax");  /* saved cs */
asm("        movl registers+32, %eax");
asm("        pushl %eax");  /* saved eip */
asm("        movl registers, %eax");
/* use iret to restore pc and flags together so
   that trace flag works right.  */
asm("        iret");

#define BREAKPOINT() asm("   int $3");

/* Put the error code here just in case the user cares.  */
int gdb_i386errcode;
/* Likewise, the vector number here (since GDB only gets the signal
   number through the usual means, and that's not very specific).  */
int gdb_i386vector = -1;

/* GDB stores segment registers in 32-bit words (that's just the way
   m-i386v.h is written).  So zero the appropriate areas in registers.  */
#define SAVE_REGISTERS1() \
  asm ("movl %eax, registers");                                   	  \
  asm ("movl %ecx, registers+4");			  		     \
  asm ("movl %edx, registers+8");			  		     \
  asm ("movl %ebx, registers+12");			  		     \
  asm ("movl %ebp, registers+20");			  		     \
  asm ("movl %esi, registers+24");			  		     \
  asm ("movl %edi, registers+28");			  		     \
  asm ("movw $0, %ax");							     \
  asm ("movw %ds, registers+48");			  		     \
  asm ("movw %ax, registers+50");					     \
  asm ("movw %es, registers+52");			  		     \
  asm ("movw %ax, registers+54");					     \
  asm ("movw %fs, registers+56");			  		     \
  asm ("movw %ax, registers+58");					     \
  asm ("movw %gs, registers+60");			  		     \
  asm ("movw %ax, registers+62");
#define SAVE_ERRCODE() \
  asm ("popl %ebx");                                  \
  asm ("movl %ebx, gdb_i386errcode");
#define SAVE_REGISTERS2() \
  asm ("popl %ebx"); /* old eip */			  		     \
  asm ("movl %ebx, registers+32");			  		     \
  asm ("popl %ebx");	 /* old cs */			  		     \
  asm ("movl %ebx, registers+40");			  		     \
  asm ("movw %ax, registers+42");                                           \
  asm ("popl %ebx");	 /* old eflags */		  		     \
  asm ("movl %ebx, registers+36");			 		     \
  /* Now that we've done the pops, we can save the stack pointer.");  */   \
  asm ("movw %ss, registers+44");					     \
  asm ("movw %ax, registers+46");     	       	       	       	       	     \
  asm ("movl %esp, registers+16");

/* See if mem_fault_routine is set, if so just IRET to that address.  */
#define CHECK_FAULT() \
  asm ("cmpl $0, mem_fault_routine");					   \
  asm ("jne mem_fault");

asm (".text");
asm ("mem_fault:");
/* OK to clobber temp registers; we're just going to end up in set_mem_err.  */
/* Pop error code from the stack and save it.  */
asm ("     popl %eax");
asm ("     movl %eax, gdb_i386errcode");

asm ("     popl %eax"); /* eip */
/* We don't want to return there, we want to return to the function
   pointed to by mem_fault_routine instead.  */
asm ("     movl mem_fault_routine, %eax");
asm ("     popl %ecx"); /* cs (low 16 bits; junk in hi 16 bits).  */
asm ("     popl %edx"); /* eflags */

/* Remove this stack frame; when we do the iret, we will be going to
   the start of a function, so we want the stack to look just like it
   would after a "call" instruction.  */
asm ("     leave");

/* Push the stuff that iret wants.  */
asm ("     pushl %edx"); /* eflags */
asm ("     pushl %ecx"); /* cs */
asm ("     pushl %eax"); /* eip */

/* Zero mem_fault_routine.  */
asm ("     movl $0, %eax");
asm ("     movl %eax, mem_fault_routine");

asm ("iret");

/*
 * remcomHandler is a front end for handle_exception.  It moves the
 * stack pointer into an area reserved for debugger use.
 */
asm("remcomHandler:");
asm("           popl %eax");        /* pop off return address     */
asm("           popl %eax");      /* get the exception number   */
asm("		movl stackPtr, %esp"); /* move to remcom stack area  */
asm("		pushl %eax");	/* push exception onto stack  */
asm("		call  handle_exception");    /* this never returns */

void _returnFromException()
{
  return_to_prog ();
}


#define CALL_HOOK() asm("call remcomHandler");

#endif /* pjo */


#define CHECK_FAULT()


/* This function is called when a i386 exception occurs.  It saves
 * all the cpu regs in the registers array, munges the stack a bit,
 * and invokes an exception handler (remcom_handler).
 *
 * stack on entry:                       stack on exit:
 *   old eflags                          vector number
 *   old cs (zero-filled to 32 bits)
 *   old eip
 *
 */
externC void hal_pc_exception_3(void);
asm(".text");
asm(".globl hal_pc_exception_3");
asm("hal_pc_exception_3:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $3");
CALL_HOOK();

/* Same thing for exception 1.  */
externC void hal_pc_exception_1(void);
asm(".text");
asm(".globl hal_pc_exception_1");
asm("hal_pc_exception_1:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $1");
CALL_HOOK();

/* Same thing for exception 0.  */
externC void hal_pc_exception_0(void);
asm(".text");
asm(".globl hal_pc_exception_0");
asm("hal_pc_exception_0:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $0");
CALL_HOOK();

/* Same thing for exception 4.  */
externC void hal_pc_exception_4(void);
asm(".text");
asm(".globl hal_pc_exception_4");
asm("hal_pc_exception_4:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $4");
CALL_HOOK();

/* Same thing for exception 5.  */
externC void hal_pc_exception_5(void);
asm(".text");
asm(".globl hal_pc_exception_5");
asm("hal_pc_exception_5:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $5");
CALL_HOOK();

/* Same thing for exception 6.  */
externC void hal_pc_exception_6(void);
asm(".text");
asm(".globl hal_pc_exception_6");
asm("hal_pc_exception_6:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $6");
CALL_HOOK();

/* Same thing for exception 7.  */
externC void hal_pc_exception_7(void);
asm(".text");
asm(".globl hal_pc_exception_7");
asm("hal_pc_exception_7:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $7");
CALL_HOOK();

/* Same thing for exception 8.  */
externC void hal_pc_exception_8(void);
asm(".text");
asm(".globl hal_pc_exception_8");
asm("hal_pc_exception_8:");
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $8");
CALL_HOOK();

/* Same thing for exception 9.  */
externC void hal_pc_exception_9(void);
asm(".text");
asm(".globl hal_pc_exception_9");
asm("hal_pc_exception_9:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $9");
CALL_HOOK();

/* Same thing for exception 10.  */
externC void hal_pc_exception_10(void);
asm(".text");
asm(".globl hal_pc_exception_10");
asm("hal_pc_exception_10:");
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $10");
CALL_HOOK();

/* Same thing for exception 12.  */
externC void hal_pc_exception_12(void);
asm(".text");
asm(".globl hal_pc_exception_12");
asm("hal_pc_exception_12:");
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $12");
CALL_HOOK();

/* Same thing for exception 16.  */
externC void hal_pc_exception_16(void);
asm(".text");
asm(".globl hal_pc_exception_16");
asm("hal_pc_exception_16:");
SAVE_REGISTERS1();
SAVE_REGISTERS2();
asm ("pushl $16");
CALL_HOOK();

/* For 13, 11, and 14 we have to deal with the CHECK_FAULT stuff.  */

/* Same thing for exception 13.  */
externC void hal_pc_exception_13 (void);
asm (".text");
asm (".globl hal_pc_exception_13");
asm ("hal_pc_exception_13:");
CHECK_FAULT();
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $13");
CALL_HOOK();

/* Same thing for exception 11.  */
externC void hal_pc_exception_11 (void);
asm (".text");
asm (".globl hal_pc_exception_11");
asm ("hal_pc_exception_11:");
CHECK_FAULT();
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $11");
CALL_HOOK();

/* Same thing for exception 14.  */
externC void hal_pc_exception_14 (void);
asm (".text");
asm (".globl hal_pc_exception_14");
asm ("hal_pc_exception_14:");
CHECK_FAULT();
SAVE_REGISTERS1();
SAVE_ERRCODE();
SAVE_REGISTERS2();
asm ("pushl $14");
CALL_HOOK();


// Connect to the appropriate ISRs.
/* this function is used to set up exception handlers for tracing and
   breakpoints */
void hal_pc_stubs_init(void)
{
#if 0
extern void remcomHandler();
int exception;

  stackPtr  = &remcomStack[STACKSIZE/sizeof(int) - 1];
#endif

#if 0  
	int i ;
	int * p = (int*) 0 ;

	for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++)
	{	p[0] = 0 ;		p++ ;
		p[0] = 0 ;		p++ ;
	}
#endif
        
  hal_pc_exception_handler(0, hal_pc_exception_0);
  hal_pc_exception_handler(1, hal_pc_exception_1);
  hal_pc_exception_handler(3, hal_pc_exception_3);
  hal_pc_exception_handler(4, hal_pc_exception_4);
  hal_pc_exception_handler(5, hal_pc_exception_5);
  hal_pc_exception_handler(6, hal_pc_exception_6);
  hal_pc_exception_handler(7, hal_pc_exception_7);
  hal_pc_exception_handler(8, hal_pc_exception_8);
  hal_pc_exception_handler(9, hal_pc_exception_9);
  hal_pc_exception_handler(10, hal_pc_exception_10);
  hal_pc_exception_handler(11, hal_pc_exception_11);
  hal_pc_exception_handler(12, hal_pc_exception_12);
  hal_pc_exception_handler(13, hal_pc_exception_13);
  hal_pc_exception_handler(14, hal_pc_exception_14);
  hal_pc_exception_handler(16, hal_pc_exception_16);
}


void hal_get_gdb_registers(target_register_t * d, HAL_SavedRegisters * s)
{
	int i ;

	for (i = 0; i < NUMREGS; i++)
		d[i] = hal_pc_trap_registers[i] ;
}


void hal_set_gdb_registers(HAL_SavedRegisters * d, target_register_t * s)
{
	int i ;
	for (i = 0; i < NUMREGS; i++)
		hal_pc_trap_registers[i] = s[i] ;
}


#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
