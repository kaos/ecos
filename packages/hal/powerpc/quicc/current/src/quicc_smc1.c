//==========================================================================
//
//      quicc_smc1.c
//
//      PowerPC QUICC basic Serial IO using port SMC1/SCC1
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2002 Gary Thomas
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Red Hat
// Contributors: hmt, gthomas
// Date:         1999-06-08
// Purpose:      Provide basic Serial IO for MPC8xx boards (like Motorola MBX)
// Description:  Serial IO for MPC8xx boards which connect their debug channel
//               to SMC1 or SCC1; or any QUICC user who wants to use SMC1/SCC1
// Usage:
// Notes:        The driver hooks itself up on procs channel 0. This should
//               probably be made configurable, allowing the platform
//               to specify location.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_powerpc_quicc.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_cache.h>

#include <cyg/hal/hal_arch.h>

#ifdef CYGPKG_HAL_POWERPC_MPC860

// eCos headers decribing PowerQUICC:
#include <cyg/hal/quicc/ppc8xx.h>

#include <cyg/hal/quicc/quicc_smc1.h>

#include <cyg/hal/hal_stub.h>           // target_register_t
#include <cyg/hal/hal_intr.h>           // HAL_INTERRUPT_UNMASK(...)
#include <cyg/hal/hal_if.h>             // Calling interface definitions
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

#define UART_BIT_RATE(n) (((int)(CYGHWR_HAL_POWERPC_BOARD_SPEED*1000000)/16)/n)
#define UART_BAUD_RATE CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD

// Note: buffers will be placed just after descriptors
// Sufficient space should be provided between descrptors
// for the buffers (single characters)
struct port_info {
    int                         Txbd;    // Offset to Tx descriptors
    int                         Txnum;   // Number of Tx buffers
    int                         Rxbd;    // Offset to Rx descriptors
    int                         Rxnum;   // Number of Rx buffers
    int                         intnum;  // Interrupt bit
    int                         timeout; // Timeout in msec
    int                         pram;    // [Pointer] to PRAM data
    int                         regs;    // [Pointer] to control registers
    volatile struct cp_bufdesc *next_rxbd;
    int                         irq;     // Interrupt state
};

static struct port_info ports[] = {
    { 0x2800, 1, 0x2810, 4, CYGNUM_HAL_INTERRUPT_CPM_SMC1, 1000,
      (int)&((EPPC *)0)->pram[2].scc.pothers.smc_modem.psmc.u, 
      (int)&((EPPC *)0)->smc_regs[0]
    }, 
#if CYGNUM_HAL_QUICC_SCC1 > 0
    { 0x2700, 1, 0x2710, 4, CYGNUM_HAL_INTERRUPT_CPM_SCC1, 1000,
      (int)&((EPPC *)0)->pram[0].scc.pscc.u, 
      (int)&((EPPC *)0)->scc_regs[0]
    },
#endif
};

// SMC Events (interrupts)
#define QUICC_SMCE_BRK 0x10  // Break received
#define QUICC_SMCE_BSY 0x04  // Busy - receive buffer overrun
#define QUICC_SMCE_TX  0x02  // Tx interrupt
#define QUICC_SMCE_RX  0x01  // Rx interrupt

/*
 * Reset the communications processor
 */
static void
reset_cpm(void)
{
    EPPC *eppc = eppc_base();
    int i;
    static int init_done = 0;

    if (init_done) return;
    init_done++;

    eppc->cp_cr = QUICC_CPM_CR_RESET | QUICC_CPM_CR_BUSY;
    for (i = 0; i < 100000; i++);

}

/*
 *  Initialize SMC1 as a uart.
 *
 *  Comments below reference Motorola's "MPC860 User Manual".
 *  The basic initialization steps are from Section 16.15.8
 *  of that manual.
 */	
static void
cyg_hal_smc1_init_channel(struct port_info *info)
{
    EPPC *eppc = eppc_base();
    int i;
    volatile struct smc_uart_pram *uart_pram = (volatile struct smc_uart_pram *)((char *)eppc + info->pram);
    volatile struct smc_regs *regs = (volatile struct smc_regs *)((char *)eppc + info->regs);
    struct cp_bufdesc *txbd, *rxbd;

    static int init_done = 0;
    if (init_done) return;
    init_done++;

    reset_cpm();

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
    uart_pram->rbase = info->Rxbd;
    uart_pram->tbase = info->Txbd;

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

    /* setup RX buffer descriptors */
    rxbd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
    info->next_rxbd = rxbd;
    for (i = 0;  i < info->Rxnum;  i++) {
        rxbd->length = 0;
        rxbd->buffer = ((char *)eppc + (info->Rxbd+(info->Rxnum*sizeof(struct cp_bufdesc))))+i;
        rxbd->ctrl   = QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
        if (i == ((info->Rxnum)-1)) {
            rxbd->ctrl   |= QUICC_BD_CTL_Wrap;
        }
        rxbd++;
    }
    // Compiler bug: for whatever reason, the Wrap code above fails!
    rxbd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
    rxbd[(info->Rxnum)-1].ctrl   |= QUICC_BD_CTL_Wrap;

    /* setup TX buffer descriptor */
    txbd = (struct cp_bufdesc *)((char *)eppc + info->Txbd);
    txbd->length = 1;
    txbd->buffer = ((char *)eppc + (info->Txbd+(info->Txnum*sizeof(struct cp_bufdesc))));
    txbd->ctrl   = 0x2000;

    /*
     *  Clear any previous events. Mask interrupts.
     *  (Section 16.15.7.14 and 16.15.7.15)
     */
    regs->smc_smce = 0xff;
    regs->smc_smcm = 5;

    /*
     *  Set 8,n,1 characters, then also enable rx and tx.
     *  (Section 16.15.7.11)
     */
    regs->smc_smcmr = 0x4820;
    regs->smc_smcmr = 0x4823;

    /*
     *  Init Rx & Tx params for SMC1
     */
    eppc->cp_cr = 0x91;

    info->irq = 0;  // Interrupts not enabled
#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // remove below
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_CPM_SMC1 );
#endif
#endif
}


//#define UART_BUFSIZE 32

//static bsp_queue_t uart_queue;
//static char uart_buffer[UART_BUFSIZE];

#define QUICC_SMCE_TX     0x02    // Tx interrupt
#define QUICC_SMCE_RX     0x01    // Rx interrupt
#define QUICC_SMCMR_TEN       (1<<1)        // Enable transmitter
#define QUICC_SMCMR_REN       (1<<0)        // Enable receiver

#ifdef CYGDBG_DIAG_BUF
extern int enable_diag_uart;
#endif // CYGDBG_DIAG_BUF

static void 
cyg_hal_smc1_putc(void* __ch_data, cyg_uint8 ch)
{
    volatile struct cp_bufdesc *bd, *first;
    EPPC *eppc = eppc_base();
    struct port_info *info = (struct port_info *)__ch_data;
    volatile struct smc_uart_pram *uart_pram = (volatile struct smc_uart_pram *)((char *)eppc + info->pram);
    volatile struct smc_regs *regs = (volatile struct smc_regs *)((char *)eppc + info->regs);
    int timeout;
    CYGARC_HAL_SAVE_GP();

    /* tx buffer descriptor */
    bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbptr);

    // Scan for a free buffer
    first = bd;
    while (bd->ctrl & QUICC_BD_CTL_Ready) {
        if (bd->ctrl & QUICC_BD_CTL_Wrap) {
            bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbase);
        } else {
            bd++;
        }
        if (bd == first) break;
    }

    while (bd->ctrl & QUICC_BD_CTL_Ready) ;  // Wait for buffer free
    if (bd->ctrl & QUICC_BD_CTL_Int) {
        // This buffer has just completed interrupt output.  Reset bits
        bd->ctrl &= ~QUICC_BD_CTL_Int;
        bd->length = 0;
    }

    bd->buffer[bd->length++] = ch;
    bd->ctrl      |= QUICC_BD_CTL_Ready;

#ifdef CYGDBG_DIAG_BUF
        enable_diag_uart = 0;
#endif // CYGDBG_DIAG_BUF
    timeout = 0;
    while (bd->ctrl & QUICC_BD_CTL_Ready) {
// Wait until buffer free
        if (++timeout == 0x7FFFF) {
            // A really long time!
#ifdef CYGDBG_DIAG_BUF
            diag_printf("bd fail? bd: %x, ctrl: %x, tx state: %x\n", bd, bd->ctrl, uart_pram->tstate);
#endif // CYGDBG_DIAG_BUF
            regs->smc_smcmr &= ~QUICC_SMCMR_TEN;  // Disable transmitter
            bd->ctrl &= ~QUICC_BD_CTL_Ready;
            regs->smc_smcmr |= QUICC_SMCMR_TEN;   // Enable transmitter
            bd->ctrl |= QUICC_BD_CTL_Ready;
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
    while (bd->ctrl & QUICC_BD_CTL_Ready) ;  // Wait until buffer free
    bd->length = 0;
#ifdef CYGDBG_DIAG_BUF
    enable_diag_uart = 1;
#endif // CYGDBG_DIAG_BUF

    CYGARC_HAL_RESTORE_GP();
}


/*
 * Get a character from a port, non-blocking
 * This function can be called on either an SMC or SCC port
 */
static cyg_bool
cyg_hal_sxx_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    volatile struct cp_bufdesc *bd;
    EPPC *eppc = eppc_base();
    struct port_info *info = (struct port_info *)__ch_data;
    volatile struct smc_uart_pram *uart_pram = (volatile struct smc_uart_pram *)((char *)eppc + info->pram);
    int cache_state;

    /* rx buffer descriptor */
    bd = info->next_rxbd;

    if (bd->ctrl & QUICC_BD_CTL_Ready)
        return false;

    *ch = bd->buffer[0];

    bd->length = 0;
    bd->buffer[0] = '\0';
    bd->ctrl |= QUICC_BD_CTL_Ready;
    if (bd->ctrl & QUICC_BD_CTL_Wrap) {
        bd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
    } else {
        bd++;
    }
    info->next_rxbd = bd;

    // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_INVALIDATE(bd->buffer, uart_pram->mrblr);  // Make sure no stale data
    }

    return true;
}

/*
 * Get a character from a port, blocking
 * This function can be called on either an SMC or SCC port
 */
static cyg_uint8
cyg_hal_sxx_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!cyg_hal_sxx_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();
    return ch;
}


static void
cyg_hal_smc1_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_smc1_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

/*
 * Read a sequence of characters from a port
 * This function can be called on either an SMC or SCC port
 */
static void
cyg_hal_sxx_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = cyg_hal_sxx_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

/*
 * Read a character from a port, with a timeout
 * This function can be called on either an SMC or SCC port
 */
static cyg_bool
cyg_hal_sxx_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    struct port_info *info = (struct port_info *)__ch_data;
    int delay_count = info->timeout * 10; // delay in .1 ms steps
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    for(;;) {
        res = cyg_hal_sxx_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        
        CYGACC_CALL_IF_DELAY_US(100);
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

/*
 * Control/query the state of a port
 * This function can be called on either an SMC or SCC port
 */
static int
cyg_hal_sxx_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    struct port_info *info = (struct port_info *)__ch_data;
    int ret = 0;
    CYGARC_HAL_SAVE_GP();

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        HAL_INTERRUPT_UNMASK(info->intnum);
        info->irq = 1;
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = info->irq;
        info->irq = 0;
        HAL_INTERRUPT_MASK(info->intnum);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = info->intnum;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = info->timeout;
        info->timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
    }        
    default:
        break;
    }
    CYGARC_HAL_RESTORE_GP();
    return ret;
}

/*
 * Low-level interrupt (ISR) handler
 * This function can be called on only an SMC port
 */
static int
cyg_hal_smc1_isr(void *__ch_data, int* __ctrlc, 
                 CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    EPPC *eppc = eppc_base();
    volatile struct cp_bufdesc *bd;
    struct port_info *info = (struct port_info *)__ch_data;
    volatile struct smc_regs *regs = (volatile struct smc_regs *)((char *)eppc + info->regs);
    char ch;
    int res = 0;
    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;
    if (regs->smc_smce & QUICC_SMCE_RX) {

        regs->smc_smce = QUICC_SMCE_RX;

        /* rx buffer descriptors */
        bd = info->next_rxbd;

        if ((bd->ctrl & QUICC_BD_CTL_Ready) == 0) {
            
            // then there be a character waiting
            ch = bd->buffer[0];
            bd->length = 1;
            bd->ctrl   |= QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
            if (bd->ctrl & QUICC_BD_CTL_Wrap) {
                bd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
            } else {
                bd++;
            }
            info->next_rxbd = bd;
        
            if( cyg_hal_is_break( &ch , 1 ) )
                *__ctrlc = 1;
        }

        // Interrupt handled. Acknowledge it.
        HAL_INTERRUPT_ACKNOWLEDGE(info->intnum);
        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

#if CYGNUM_HAL_QUICC_SCC1 > 0
/*
 *  Initialize SCC1 as a uart.
 *
 *  Comments below reference Motorola's "MPC860 User Manual".
 *  The basic initialization steps are from Section 16.15.8
 *  of that manual.
 */	
static void
cyg_hal_scc1_init_channel(struct port_info *info)
{
    EPPC *eppc = eppc_base();
    int i;
    volatile struct uart_pram *uart_pram = (volatile struct uart_pram *)((char *)eppc + info->pram);
    volatile struct scc_regs *regs = (volatile struct scc_regs *)((char *)eppc + info->regs);
    struct cp_bufdesc *txbd, *rxbd;

    static int init_done = 0;
    if (init_done) return;
    init_done++;

    reset_cpm();

    /*
     *  Set up the PortA pins for UART operation.
     */
    eppc->pio_papar |= 0x03;
    eppc->pio_padir &= ~0x03;
    eppc->pio_paodr &= ~0x03;

    /* CTS on PortC.11 */
    eppc->pio_pcdir &= 0x800;
    eppc->pio_pcpar &= 0x800;
    eppc->pio_pcso  |= 0x800;

    /* RTS on PortB.19 */
    eppc->pip_pbpar |= 0x1000;
    eppc->pip_pbdir |= 0x1000;

    /* Configure baud rate generator (Section 16.13.2) */
    eppc->brgc2 = 0x10000 | (UART_BIT_RATE(UART_BAUD_RATE)<<1);

    /*
     *  NMSI mode, BRG2 to SCC1
     */
    eppc->si_simode = 0;
    eppc->si_sicr = (1<<3)|(1<<0);

    /*
     *  Set pointers to buffer descriptors.
     */
    uart_pram->rbase = info->Rxbd;
    uart_pram->tbase = info->Txbd;

    /*
     *  SDMA & LCD bus request level 5
     */
    eppc->dma_sdcr = 1;

    /*
     *  Set Rx and Tx function code
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

    /* character mask */
    uart_pram->rccm  = 0xC0FF;

    /* setup RX buffer descriptors */
    rxbd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
    info->next_rxbd = rxbd;
    for (i = 0;  i < info->Rxnum;  i++) {
        rxbd->length = 0;
        rxbd->buffer = ((char *)eppc + (info->Rxbd+(info->Rxnum*sizeof(struct cp_bufdesc))))+i;
        rxbd->ctrl   = QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
        if (i == ((info->Rxnum)-1)) {
            rxbd->ctrl   |= QUICC_BD_CTL_Wrap;
        }
        rxbd++;
    }
    // Compiler bug: for whatever reason, the Wrap code above fails!
    rxbd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
    rxbd[(info->Rxnum)-1].ctrl   |= QUICC_BD_CTL_Wrap;

    /* setup TX buffer descriptor */
    txbd = (struct cp_bufdesc *)((char *)eppc + info->Txbd);
    txbd->length = 1;
    txbd->buffer = ((char *)eppc + (info->Txbd+(info->Txnum*sizeof(struct cp_bufdesc))));
    txbd->ctrl   = 0x2000;

    /*
     *  Init Rx & Tx params for SCC1
     */
    eppc->cp_cr = 0x41;

    /*
     *  Clear any previous events. Mask interrupts.
     *  (Section 16.15.7.14 and 16.15.7.15)
     */
    regs->scc_scce = 0xff;
    regs->scc_sccm = 5;

    /*
     *  Set 8,n,1 characters
     */
    regs->scc_psmr = (3<<12);
    regs->scc_gsmr_h = 0x20;          // 8bit FIFO
    regs->scc_gsmr_l = 0x00028004;    // 16x TxCLK, 16x RxCLK, UART
    regs->scc_gsmr_l |= 0x30;         // Enable Rx, Tx

    info->irq = 0;
#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // remove below
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_CPM_SCC1 );
#endif
#endif
}

static void 
cyg_hal_scc1_putc(void* __ch_data, cyg_uint8 ch)
{
    volatile struct cp_bufdesc *bd, *first;
    EPPC *eppc = eppc_base();
    struct port_info *info = (struct port_info *)__ch_data;
    volatile struct uart_pram *uart_pram = (volatile struct uart_pram *)((char *)eppc + info->pram);
    CYGARC_HAL_SAVE_GP();

    /* tx buffer descriptor */
    bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbptr);

    // Scan for a free buffer
    first = bd;
    while (bd->ctrl & QUICC_BD_CTL_Ready) {
        if (bd->ctrl & QUICC_BD_CTL_Wrap) {
            bd = (struct cp_bufdesc *)((char *)eppc + uart_pram->tbase);
        } else {
            bd++;
        }
        if (bd == first) break;
    }

    while (bd->ctrl & QUICC_BD_CTL_Ready) ;  // Wait for buffer free
    if (bd->ctrl & QUICC_BD_CTL_Int) {
        // This buffer has just completed interrupt output.  Reset bits
        bd->ctrl &= ~QUICC_BD_CTL_Int;
        bd->length = 0;
    }

    bd->buffer[bd->length++] = ch;
    bd->ctrl      |= QUICC_BD_CTL_Ready;

    while (bd->ctrl & QUICC_BD_CTL_Ready) ;  // Wait until buffer free
    bd->length = 0;

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_scc1_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_scc1_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static int
cyg_hal_scc1_isr(void *__ch_data, int* __ctrlc, 
                 CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    EPPC *eppc = eppc_base();
    volatile struct cp_bufdesc *bd;
    struct port_info *info = (struct port_info *)__ch_data;
    volatile struct scc_regs *regs = (volatile struct scc_regs *)((char *)eppc + info->regs);
    char ch;
    int res = 0;
    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;
    if (regs->scc_scce & QUICC_SMCE_RX) {

        regs->scc_scce = QUICC_SMCE_RX;

        /* rx buffer descriptors */
        bd = info->next_rxbd;

        if ((bd->ctrl & QUICC_BD_CTL_Ready) == 0) {
            
            // then there be a character waiting
            ch = bd->buffer[0];
            bd->length = 1;
            bd->ctrl   |= QUICC_BD_CTL_Ready | QUICC_BD_CTL_Int;
            if (bd->ctrl & QUICC_BD_CTL_Wrap) {
                bd = (struct cp_bufdesc *)((char *)eppc + info->Rxbd);
            } else {
                bd++;
            }
            info->next_rxbd = bd;
        
            if( cyg_hal_is_break( &ch , 1 ) )
                *__ctrlc = 1;
        }

        // Interrupt handled. Acknowledge it.
        HAL_INTERRUPT_ACKNOWLEDGE(info->intnum);
        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}
#endif // CYGNUM_HAL_QUICC_SCC1

/*
 * Early initialization of comm channels. Must not rely
 * on interrupts, yet. Interrupt operation can be enabled
 * in _bsp_board_init().
 */
void
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    static int init = 0;  // It's wrong to do this more than once
    if (init) return;
    init++;

    // Setup procs in the vector table

    // Set channel 0 - SMC1
    cyg_hal_smc1_init_channel(&ports[0]);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);// Should be configurable!
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &ports[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_smc1_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_sxx_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_smc1_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_sxx_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_sxx_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_smc1_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_sxx_getc_timeout);

#if CYGNUM_HAL_QUICC_SCC1 > 0

    // Set channel 1 - SCC1
    cyg_hal_scc1_init_channel(&ports[1]);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);// Should be configurable!
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &ports[1]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_scc1_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_sxx_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_scc1_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_sxx_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_sxx_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_scc1_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_sxx_getc_timeout);
#endif

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

#endif // CYGPKG_HAL_POWERPC_MPC860
// EOF quicc_smc1.c
