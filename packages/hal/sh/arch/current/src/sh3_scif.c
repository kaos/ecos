//=============================================================================
//
//      sh3_scif.c
//
//      Simple driver for the SH3 Serial Communication Interface with FIFO
//
//=============================================================================
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
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-03-30
// Description: Simple driver for the SH Serial Communication Interface
//              The driver can be used for either the SCIF or the IRDA
//              modules (the latter can act as the former).
//              Clients of this file can configure the behavior with:
//              CYGNUM_SCIF_PORTS: number of SCI ports
//
// Note:        It should be possible to configure a channel to IRDA mode.
//              Worry about that when some board needs it.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGNUM_HAL_SH_SH3_SCIF_PORTS

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP
#include <cyg/hal/hal_if.h>             // Calling-if API
#include <cyg/hal/sh_regs.h>            // serial register definitions
#include <cyg/hal/sh_stub.h>            // target_register_t

#include <cyg/hal/sh3_scif.h>           // our header

//--------------------------------------------------------------------------

void
cyg_hal_plf_scif_init_channel(const channel_data_t* chan)
{
    cyg_uint8* base = chan->base;
    cyg_uint8 tmp;
    cyg_uint16 sr;

    // Disable everything.
    HAL_WRITE_UINT8(base+_REG_SCSCR, 0);

    // Reset FIFO.
    HAL_WRITE_UINT8(base+_REG_SCFCR, 
                    CYGARC_REG_SCFCR2_TFRST|CYGARC_REG_SCFCR2_RFRST);

    // 8-1-no parity.
    HAL_WRITE_UINT8(base+_REG_SCSMR, 0);

    // Set speed to 38400
    HAL_READ_UINT8(base+_REG_SCSMR, tmp);
    tmp &= ~CYGARC_REG_SCSMR2_CKSx_MASK;
    tmp |= CYGARC_SCBRR_CKSx(38400);
    HAL_WRITE_UINT8(base+_REG_SCSMR, tmp);
    HAL_WRITE_UINT8(base+_REG_SCBRR, CYGARC_SCBRR_N(38400));

    // Let things settle: Here we should should wait the equivalent of
    // one bit interval, i.e. 1/38400 second, but until we have
    // something like the Linux delay loop, it's hard to do reliably. So
    // just move on and hope for the best (this is unlikely to cause
    // problems since the CPU has just come out of reset anyway).

    // Clear status register (read back first).
    HAL_READ_UINT16(base+_REG_SCSSR, sr);
    HAL_WRITE_UINT16(base+_REG_SCSSR, 0);

    // Bring FIFO out of reset and set to trigger on every char in
    // FIFO (or C-c input would not be processed).
    HAL_WRITE_UINT8(base+_REG_SCFCR, 
                    CYGARC_REG_SCFCR2_RTRG_1|CYGARC_REG_SCFCR2_TTRG_1);

    // Leave Tx/Rx interrupts disabled, but enable Tx/Rx
    HAL_WRITE_UINT8(base+_REG_SCSCR, 
                    CYGARC_REG_SCSCR2_TE|CYGARC_REG_SCSCR2_RE);
}

//static 
cyg_bool
cyg_hal_plf_scif_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint16 fdr, sr;

    HAL_READ_UINT16(base+_REG_SCFDR, fdr);
    if (0 == (fdr & CYGARC_REG_SCFDR2_RCOUNT_MASK))
        return false;

    HAL_READ_UINT8(base+_REG_SCFRDR, *ch);

    // Clear DR/RDF flags
    HAL_READ_UINT16(base+_REG_SCSSR, sr);
    HAL_WRITE_UINT16(base+_REG_SCSSR,
                     CYGARC_REG_SCSSR2_CLEARMASK & ~(CYGARC_REG_SCSSR2_RDF | CYGARC_REG_SCSSR2_DR));

    return true;
}

cyg_uint8
cyg_hal_plf_scif_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!cyg_hal_plf_scif_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

void
cyg_hal_plf_scif_putc(void* __ch_data, cyg_uint8 c)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint16 fdr, sr;
    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT16(base+_REG_SCFDR, fdr);
    } while (((fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK) >> CYGARC_REG_SCFDR2_TCOUNT_shift) == 16);

    HAL_WRITE_UINT8(base+_REG_SCFTDR, c);

    // Clear FIFO-empty/transmit end flags (read back SR first)
    HAL_READ_UINT16(base+_REG_SCSSR, sr);
    HAL_WRITE_UINT16(base+_REG_SCSSR, CYGARC_REG_SCSSR2_CLEARMASK   
                     & ~(CYGARC_REG_SCSSR2_TDFE | CYGARC_REG_SCSSR2_TEND ));

    // Hang around until the character has been safely sent.
    do {
        HAL_READ_UINT16(base+_REG_SCFDR, fdr);
    } while ((fdr & CYGARC_REG_SCFDR2_TCOUNT_MASK) != 0);

    CYGARC_HAL_RESTORE_GP();
}

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG) \
    || defined(CYGPRI_HAL_IMPLEMENTS_IF_SERVICES)

static channel_data_t channels[CYGNUM_HAL_SH_SH3_SCIF_PORTS];

static void
cyg_hal_plf_scif_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_plf_scif_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_plf_scif_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_scif_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

cyg_bool
cyg_hal_plf_scif_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    int delay_count;
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    delay_count = chan->msec_timeout * 10; // delay in .1 ms steps

    for(;;) {
        res = cyg_hal_plf_scif_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        
        CYGACC_CALL_IF_DELAY_US(100);
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static int
cyg_hal_plf_scif_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    static int irq_state = 0;
    channel_data_t* chan = (channel_data_t*)__ch_data;
    cyg_uint8 scr;
    int ret = 0;
    CYGARC_HAL_SAVE_GP();

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        HAL_READ_UINT8(chan->base+_REG_SCSCR, scr);
        scr |= CYGARC_REG_SCSCR2_RIE;
        HAL_WRITE_UINT8(chan->base+_REG_SCSCR, scr);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        HAL_READ_UINT8(chan->base+_REG_SCSCR, scr);
        scr &= ~CYGARC_REG_SCSCR2_RIE;
        HAL_WRITE_UINT8(chan->base+_REG_SCSCR, scr);
        break;
    case __COMMCTL_DBG_ISR_VECTOR:
        ret = chan->isr_vector;
        break;
    case __COMMCTL_SET_TIMEOUT:
    {
        va_list ap;

        va_start(ap, __func);

        ret = chan->msec_timeout;
        chan->msec_timeout = va_arg(ap, cyg_uint32);

        va_end(ap);
    }        
    default:
        break;
    }
    CYGARC_HAL_RESTORE_GP();
    return ret;
}

static int
cyg_hal_plf_scif_isr(void *__ch_data, int* __ctrlc, 
                     CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    cyg_uint8 c;
    cyg_uint16 fdr, sr;
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    int res = 0;
    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;
    HAL_READ_UINT16(base+_REG_SCFDR, fdr);
    if ((fdr & CYGARC_REG_SCFDR2_RCOUNT_MASK) != 0) {
        HAL_READ_UINT8(base+_REG_SCFRDR, c);

        // Clear buffer full flag (read back first).
        HAL_READ_UINT16(base+_REG_SCSSR, sr);
        HAL_WRITE_UINT16(base+_REG_SCSSR, 
                         CYGARC_REG_SCSSR2_CLEARMASK & ~CYGARC_REG_SCSSR2_RDF);

        if( cyg_hal_is_break( &c , 1 ) )
            *__ctrlc = 1;

        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

void
cyg_hal_plf_scif_init(int scif_index, int comm_index, 
                      int rcv_vect, cyg_uint8* base)
{
    channel_data_t* chan = &channels[scif_index];
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Initialize channel table
    chan->base = base;
    chan->isr_vector = rcv_vect;
    chan->msec_timeout = 1000;

    // Disable interrupts.
    HAL_INTERRUPT_MASK(chan->isr_vector);

    // Init channel
    cyg_hal_plf_scif_init_channel(chan);

    // Setup procs in the vector table

    // Initialize channel procs
    CYGACC_CALL_IF_SET_CONSOLE_COMM(comm_index);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, chan);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_scif_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_scif_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_scif_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_scif_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_scif_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_scif_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_scif_getc_timeout);

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG || CYGPRI_HAL_IMPLEMENTS_IF_SERVICES

#endif // CYGNUM_HAL_SH_SH3_SCIF_PORTS

//-----------------------------------------------------------------------------
// end of sh3_scif.c
