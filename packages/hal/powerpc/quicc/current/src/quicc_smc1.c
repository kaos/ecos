//==========================================================================
//
//      quicc_smc1.c
//
//      PowerPC QUICC basic Serial IO using port SMC1
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
// Author(s):    Red Hat
// Contributors: hmt
// Date:         1999-06-08
// Purpose:      Provide basic Serial IO for MBX board
// Description:  Serial IO for MBX boards which connect their debug channel
//               to SMC1; or any QUICC user who wants to use SMC1.
// Usage:
// 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_cache.h>

#ifdef CYG_HAL_POWERPC_MPC860

// eCos headers decribing PowerQUICC:
#include <cyg/hal/quicc/ppc8xx.h>

#include <cyg/hal/quicc/quicc_smc1.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/hal_stub.h>           // target_register_t
#include <cyg/hal/hal_intr.h>           // HAL_INTERRUPT_UNMASK(...)
#endif

#define UART_BIT_RATE(n) (((CYGHWR_HAL_POWERPC_MBX_BOARD_SPEED*1000000)/16)/n)
#define UART_BAUD_RATE 38400

#define Rxbd     0x2800       /* Rx Buffer Descriptor Offset */
#define Txbd     0x2808       /* Tx Buffer Descriptor Offset */

#define Rxbuf    ((volatile char *)eppc + 0x2810)
#define Txbuf    ((volatile char *)eppc + 0x2820)


/*
 *  Initialize SMC1 as a uart.
 *
 *  Comments below reference Motorola's "MPC860 User Manual".
 *  The basic initialization steps are from Section 16.15.8
 *  of that manual.
 */	
static void
init_smc1_uart(void)
{
    EPPC *eppc;
    volatile struct smc_uart_pram *uart_pram;
    struct cp_bufdesc *txbd, *rxbd;

    static int init_done = 0;
    if (init_done) return;
    init_done++;

    eppc = eppc_base();

    /* SMC1 Uart parameter ram */
    uart_pram = &eppc->pram[2].scc.pothers.smc_modem.psmc.u;

    /* tx and rx buffer descriptors */
    txbd = (struct cp_bufdesc *)((char *)eppc + Txbd);
    rxbd = (struct cp_bufdesc *)((char *)eppc + Rxbd);

    /*
     *  Set up the PortB pins for UART operation.
     *  Set PAR and DIR to allow SMCTXD1 and SMRXD1
     *  (Table 16-39)
     */
    eppc->pip_pbpar |= 0xc0;
    eppc->pip_pbdir &= ~0xc0;


    /* Configure baud rate generator (Section 16.13.2) */
    eppc->brgc1 = 0x10000 | (UART_BIT_RATE(UART_BAUD_RATE)<<1);


    /*
     *  NMSI mode, BRG1 to SMC1
     *  (Section 16.12.5.2)
     */
    eppc->si_simode = 0;

    /*
     *  Set pointers to buffer descriptors.
     *  (Sections 16.15.4.1, 16.15.7.12, and 16.15.7.13)
     */
    uart_pram->rbase = Rxbd;
    uart_pram->tbase = Txbd;

    /*
     *  Init Rx & Tx params for SMC1
     */
    eppc->cp_cr = 0x91;

    /*
     *  SDMA & LCD bus request level 5
     *  (Section 16.10.2.1)
     */
    eppc->dma_sdcr = 1;

    /*
     *  Set Rx and Tx function code
     *  (Section 16.15.4.2)
     */
    uart_pram->rfcr = 0x18;
    uart_pram->tfcr = 0x18;

    /* max receive buffer length */
    uart_pram->mrblr = 1;

    /* disable max_idle feature */
    uart_pram->max_idl = 0;

    /* no last brk char received */
    uart_pram->brkln = 0;

    /* no break condition occurred */
    uart_pram->brkec = 0;

    /* 1 break char sent on top XMIT */
    uart_pram->brkcr = 1;

    /* setup RX buffer descriptor */
    rxbd->length = 0;
    rxbd->buffer = Rxbuf;
    rxbd->ctrl   = 0xb000;

    /* setup TX buffer descriptor */
    txbd->length = 1;
    txbd->buffer = Txbuf;
    txbd->ctrl   = 0x2000;

    /*
     *  Clear any previous events. Mask interrupts.
     *  (Section 16.15.7.14 and 16.15.7.15)
     */
    eppc->smc_regs[0].smc_smce = 0xff;
    eppc->smc_regs[0].smc_smcm = 5;

    /*
     *  Set 8,n,1 characters, then also enable rx and tx.
     *  (Section 16.15.7.11)
     */
    eppc->smc_regs[0].smc_smcmr = 0x4820;
    eppc->smc_regs[0].smc_smcmr = 0x4823;

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_CPM_SMC1 );
#endif
}


//#define UART_BUFSIZE 32

//static bsp_queue_t uart_queue;
//static char uart_buffer[UART_BUFSIZE];

#define QUICC_BD_CTL_Busy 0x8000  // Descriptor/buffer busy
#define QUICC_BD_CTL_Wrap 0x2000  // Last buffer in ring
#define QUICC_BD_CTL_Int  0x1000  // Interrupt enable
#define QUICC_BD_CTL_MASK 0xB000  // User settable bits
#define QUICC_SMCE_TX     0x02    // Tx interrupt
#define QUICC_SMCE_RX     0x01    // Rx interrupt
#define QUICC_SMCMR_TEN       (1<<1)        // Enable transmitter
#define QUICC_SMCMR_REN       (1<<0)        // Enable receiver

#ifdef CYGDBG_DIAG_BUF
extern int enable_diag_uart;
#endif // CYGDBG_DIAG_BUF

void
cyg_quicc_smc1_uart_putchar(char ch)
{
    volatile struct cp_bufdesc *bd, *first;
    EPPC *eppc = eppc_base();
    volatile struct smc_uart_pram *uart_pram = &eppc->pram[2].scc.pothers.smc_modem.psmc.u;
    int timeout;

    /* tx buffer descriptor */
    bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbptr);

    // Scan for a free buffer
    first = bd;
    while (bd->ctrl & QUICC_BD_CTL_Busy) {
        if (bd->ctrl & QUICC_BD_CTL_Wrap) {
            bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbase);
        } else {
            bd++;
        }
        if (bd == first) break;
    }

    while (bd->ctrl & QUICC_BD_CTL_Busy) ;  // Wait for buffer free
    if (bd->ctrl & QUICC_BD_CTL_Int) {
        // This buffer has just completed interrupt output.  Reset bits
        bd->ctrl &= ~QUICC_BD_CTL_Int;
        bd->length = 0;
    }

    bd->buffer[bd->length++] = ch;
    bd->ctrl      |= QUICC_BD_CTL_Busy;

#ifdef CYGDBG_DIAG_BUF
        enable_diag_uart = 0;
#endif // CYGDBG_DIAG_BUF
    timeout = 0;
    while (bd->ctrl & QUICC_BD_CTL_Busy) {
// Wait until buffer free
        if (++timeout == 0x7FFFF) {
            // A really long time!
#ifdef CYGDBG_DIAG_BUF
            diag_printf("bd fail? bd: %x, ctrl: %x, tx state: %x\n", bd, bd->ctrl, uart_pram->tstate);
#endif // CYGDBG_DIAG_BUF
            eppc->smc_regs[0].smc_smcmr &= ~QUICC_SMCMR_TEN;  // Disable transmitter
            bd->ctrl &= ~QUICC_BD_CTL_Busy;
            eppc->smc_regs[0].smc_smcmr |= QUICC_SMCMR_TEN;   // Enable transmitter
            bd->ctrl |= QUICC_BD_CTL_Busy;
            timeout = 0;
#ifdef CYGDBG_DIAG_BUF
            diag_printf("bd retry? bd: %x, ctrl: %x, tx state: %x\n", bd, bd->ctrl, uart_pram->tstate);
            first = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbase);
            while (true) {
                diag_printf("bd: %x, ctrl: %x, length: %x\n", first, first->ctrl, first->length);
                if (first->ctrl & QUICC_BD_CTL_Wrap) break;
                first++;
            }
#endif // CYGDBG_DIAG_BUF
        }
    }
    while (bd->ctrl & QUICC_BD_CTL_Busy) ;  // Wait until buffer free
    bd->length = 0;
#ifdef CYGDBG_DIAG_BUF
        enable_diag_uart = 1;
#endif // CYGDBG_DIAG_BUF
}


int
cyg_quicc_smc1_uart_rcvchar(void)
{
    volatile struct cp_bufdesc *bd;
    char ch;
    EPPC *eppc = eppc_base();
    volatile struct smc_uart_pram *uart_pram = &eppc->pram[2].scc.pothers.smc_modem.psmc.u;
    int cache_state;

    /* rx buffer descriptor */
    bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->rbptr);

    while (bd->ctrl & QUICC_BD_CTL_Busy) ;

    ch = bd->buffer[0];

    bd->length = 0;
    bd->buffer[0] = '\0';
    bd->ctrl |= QUICC_BD_CTL_Busy;
    // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_INVALIDATE(bd->buffer, uart_pram->mrblr);  // Make sure no stale data
    }

    return ch;
}

/*
 * Early initialization of comm channels. Must not rely
 * on interrupts, yet. Interrupt operation can be enabled
 * in _bsp_board_init().
 */
void
cyg_quicc_init_smc1(void)
{
    EPPC *eppc = eppc_base();
    int i;

    static int init = 0;  // It's wrong to do this more than once
    if (init) return;
    init++;

    /*
     *  Reset communications processor
     */
    eppc->cp_cr = 0x8001;
    for (i = 0; i < 100000; i++);

    init_smc1_uart();
}

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// This ISR is called from the interrupt handler. This should only
// happen when there is no real serial driver, so the code shouldn't
// mess anything up.
int cyg_hal_gdb_isr( target_register_t pc )
{
    EPPC *eppc = eppc_base();
    struct cp_bufdesc *bd;
    char ch;

    eppc->smc_regs[0].smc_smce = 0xff;

    /* rx buffer descriptors */
    bd = (struct cp_bufdesc *)((char *)eppc_base() + Rxbd);

    if ((bd->ctrl & 0x8000) == 0) {
        // then there be a character waiting
        ch = bd->buffer[0];
        bd->length = 1;
        bd->ctrl   = 0xb000;

        if ( 3 == ch ) {
            // Ctrl-C: set a breakpoint at PC so GDB will display the
            // correct program context when stopping rather than the
            // interrupt handler.
            cyg_hal_gdb_interrupt( pc );

            // Interrupt handled. Don't call ISR proper. At return
            // from the VSR, execution will stop at the breakpoint
            // just set.

            eppc->cpmi_cisr = 0x10;
            return 0;
        }
    }
    eppc->cpmi_cisr = 0x10; // acknowledge the Rx event anyway
                            // in case it was left over from polled reception
    // Not caused by GDB. Call ISR proper.
    return 1;
}
#endif // CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

#endif // CYG_HAL_POWERPC_MPC860
// EOF quicc_smc1.c
