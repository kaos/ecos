/* hal-stub.c - helper functions for stub, specific to ECC HAL
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

//#include <signal.h>
//#include "board.h"

#include <cyg/hal/hal_stub.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_cache.h>

/*---------------------------------------------------------------------------*/

#define DIAG_BASE       0xfffff300
#define DIAG_SLCR       (DIAG_BASE+0x00)
#define DIAG_SLSR       (DIAG_BASE+0x04)
#define DIAG_SLDICR     (DIAG_BASE+0x08)
#define DIAG_SLDISR     (DIAG_BASE+0x0C)
#define DIAG_SFCR       (DIAG_BASE+0x10)
#define DIAG_SBRG       (DIAG_BASE+0x14)
#define DIAG_TFIFO      (DIAG_BASE+0x20)
#define DIAG_RFIFO      (DIAG_BASE+0x30)

#define BRG_T0          0x0000
#define BRG_T2          0x0100
#define BRG_T4          0x0200
#define BRG_T5          0x0300

/*---------------------------------------------------------------------------*/

/* External data */

extern int output_debug;			/* dvemon.c */
extern int (*read_vector) (int, char *, int); 	/* read.c */
extern int (*write_vector) (int, char *, int); 	/* write.c */

/* External functions */

//int outbyte (unsigned char byte);	/* dvemon.c */
//unsigned char inbyte ();		/* dvemon.c */
//void general_exception (void);		/* tx39-exc.S */


/* Local data */


/* If ENABLED is non-zero, supply a memory fault handler that traps
   subsequent memory errors and sets _mem_err to 1. Otherwise, memory
   faults are processed as exceptions. */

void __set_mem_fault_trap (int enable) {}

static int serial_initialized = 0;

static void serial_init()
{
//hal_diag_led(0x10);    
    HAL_WRITE_UINT16( DIAG_SLCR , 0x0020 );

    HAL_WRITE_UINT16( DIAG_SLDICR , 0x0000 );
    
    HAL_WRITE_UINT16( DIAG_SFCR , 0x0000 );
    
//    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 20 );
//    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 10 );
    HAL_WRITE_UINT16( DIAG_SBRG , BRG_T2 | 5 );    

    serial_initialized = 1;
    
//hal_diag_led(0x10);
}

/* Write C to the current serial port. */

void putDebugChar (int c)
{
#ifdef CYG_HAL_MIPS_SIM

#else    
    CYG_WORD16 disr;
    
//hal_diag_led(0x20);

    if( !serial_initialized ) serial_init();
    
    for(;;)
    {
        HAL_READ_UINT16( DIAG_SLDISR , disr );

        if( disr & 0x0002 ) break;
    }

    disr = disr & ~0x0002;
    
    HAL_WRITE_UINT8( DIAG_TFIFO, c );

    HAL_WRITE_UINT16( DIAG_SLDISR , disr );    

//hal_diag_led(0x20);
//    HAL_DIAG_WRITE_CHAR( c );
#endif    
}

/* Read one character from the current serial port. */

int getDebugChar (void)
{
#ifdef CYG_HAL_MIPS_SIM
    static char input[] = "+$s#73";
    static int i = 0;
    return input[i++];
#else
    char c;

    CYG_WORD16 disr;

    if( !serial_initialized ) serial_init();
    
//hal_diag_led(0x40);        
    for(;;)
    {
        
        HAL_READ_UINT16( DIAG_SLDISR , disr );

        if( disr & 0x0001 ) break;
    }

    disr = disr & ~0x0001;
    
    HAL_READ_UINT8( DIAG_RFIFO, c );
    
    HAL_WRITE_UINT16( DIAG_SLDISR , disr );    

//hal_diag_led(0x40);
    
//    HAL_DIAG_READ_CHAR(c);
//    diag_printf("<%02x:%c>",c);

    return c;
    
#endif    
}


/* Install the standard set of trap handlers for the stub. */

void __install_traps (void)
{
    // Nothing to do, the HAL will call handle_exception when one occurs.
}

/* Flush the instruction cache. */
void flush_i_cache (void) { HAL_ICACHE_INVALIDATE_ALL(); }


/* Reset the board. */
void __reset (void) {}


/* Set the baud rate for the current serial port. */
void __set_baud_rate (int baud) {}


/* Initialize the hardware. */
void initHardware (void) {}

#if 0
/* Alternate read function for use when stub is running.  It
   reads the data for standard input from the remote GDB.  */

int dve_read (int fd, char *buf, int nbytes)
{
  __get_gdb_input (buf, nbytes, 1);
}

/* Alternate write function for use when stub is running.  It
   writes the data for standard output to the remote GDB.  */

int dve_write (int fd, char *buf, int nbytes)
{
  __output_gdb_string (buf, nbytes);
}

#endif

/* Install traps and otherwise initialize stub. */

void set_debug_traps (void)
{
//  output_debug = 0;	/* disable control-O prefixing on output */
  initialize_stub ();	/* will call __install_traps and initHardware */
//  read_vector = dve_read;	/* alternate read() function for stub */
//  write_vector = dve_write;	/* alternate write() function for stub */
}


/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */

void
breakpoint()
{
#ifdef CYG_HAL_MIPS_SIM

        // This code causes a fake breakpoint.
        asm volatile (
            "lui    $24,0x0000;"
            "ori    $24,0x0024;"
            "mtc0   $24,$13;"
            "lui    $25,%%hi(1f);"
            "ori    $25,$25,%%lo(1f);"
            "j      other_vector;"
            "nop;"
            ".global _breakinst;"
            "_breakinst:"
            "1: nop;"
            :
            :
            : "t8", "t9"
            );    
    
#else
        
    HAL_BREAKPOINT(_breakinst);

#endif
  
}


/* This function returns the opcode for a 'break 5' instruction.  */

unsigned long __break_opcode ()
{
  return HAL_BREAKINST;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
