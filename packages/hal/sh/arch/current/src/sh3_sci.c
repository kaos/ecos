//=============================================================================
//
//      sh3_sci.c
//
//      Simple driver for the SH Serial Communication Interface (SCI)
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
// Date:        1999-05-17
// Description: Simple driver for the SH Serial Communication Interface
//              Clients of this file can configure the behavior with:
//              CYGNUM_SCI_PORTS:  number of SCI ports
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGNUM_HAL_SH_SH3_SCI_PORTS

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP
#include <cyg/hal/hal_if.h>             // Calling-if API
#include <cyg/hal/sh_regs.h>            // serial register definitions

#include <cyg/hal/sh3_sci.h>            // our header

//--------------------------------------------------------------------------

void
cyg_hal_plf_sci_init_channel(const channel_data_t* chan)
{
    cyg_uint8 tmp;
    cyg_uint8* base = chan->base;

    // Disable Tx/Rx interrupts, but enable Tx/Rx
    HAL_WRITE_UINT8(base+_REG_SCSCR,
                    CYGARC_REG_SCSCR_TE|CYGARC_REG_SCSCR_RE);

    // 8-1-no parity.
    HAL_WRITE_UINT8(base+_REG_SCSMR, 0);

    // Set speed to 38400
    HAL_READ_UINT8(base+_REG_SCSMR, tmp);
    tmp &= ~CYGARC_REG_SCSMR_CKSx_MASK;
    tmp |= CYGARC_SCBRR_CKSx(38400);
    HAL_WRITE_UINT8(base+_REG_SCSMR, tmp);
    HAL_WRITE_UINT8(base+_REG_SCBRR, CYGARC_SCBRR_N(38400));
}

static cyg_bool
cyg_hal_plf_sci_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint8 sr;

    HAL_READ_UINT8(base+_REG_SCSSR, sr);
    if ((sr & CYGARC_REG_SCSSR_RDRF) == 0)
        return false;

    HAL_READ_UINT8(base+_REG_SCRDR, *ch);

    // Clear buffer full flag.
    HAL_WRITE_UINT8(base+_REG_SCSSR, sr & ~CYGARC_REG_SCSSR_RDRF);

    return true;
}

cyg_uint8
cyg_hal_plf_sci_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!cyg_hal_plf_sci_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

void
cyg_hal_plf_sci_putc(void* __ch_data, cyg_uint8 c)
{
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    cyg_uint8 sr;
    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT8(base+_REG_SCSSR, sr);
    } while ((sr & CYGARC_REG_SCSSR_TDRE) == 0);

    HAL_WRITE_UINT8(base+_REG_SCTDR, c);

    // Clear empty flag.
    HAL_WRITE_UINT8(base+_REG_SCSSR, sr & ~CYGARC_REG_SCSSR_TDRE);

    // Hang around until the character has been safely sent.
    do {
        HAL_READ_UINT8(base+_REG_SCSSR, sr);
    } while ((sr & CYGARC_REG_SCSSR_TDRE) == 0);

    CYGARC_HAL_RESTORE_GP();
}

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG) \
    || defined(CYGPRI_HAL_IMPLEMENTS_IF_SERVICES)

static channel_data_t channels[CYGNUM_HAL_SH_SH3_SCI_PORTS];

static void
cyg_hal_plf_sci_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_plf_sci_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_plf_sci_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_sci_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

cyg_bool
cyg_hal_plf_sci_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    channel_data_t* chan = (channel_data_t*)__ch_data;
    int delay_count;
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    delay_count = chan->msec_timeout * 10; // delay in .1 ms steps

    for(;;) {
        res = cyg_hal_plf_sci_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        
        CYGACC_CALL_IF_DELAY_US(100);
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static int
cyg_hal_plf_sci_control(void *__ch_data, __comm_control_cmd_t __func, ...)
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
        scr |= CYGARC_REG_SCSCR_RIE;
        HAL_WRITE_UINT8(chan->base+_REG_SCSCR, scr);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        HAL_READ_UINT8(chan->base+_REG_SCSCR, scr);
        scr &= ~CYGARC_REG_SCSCR_RIE;
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
cyg_hal_plf_sci_isr(void *__ch_data, int* __ctrlc, 
                    CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    cyg_uint8 c, sr;
    cyg_uint8* base = ((channel_data_t*)__ch_data)->base;
    int res = 0;
    CYGARC_HAL_SAVE_GP();

    *__ctrlc = 0;
    HAL_READ_UINT8(base+_REG_SCSSR, sr);
    if (sr & CYGARC_REG_SCSSR_RDRF) {
        HAL_READ_UINT8(base+_REG_SCRDR, c);

        // Clear buffer full flag.
        HAL_WRITE_UINT8(base+_REG_SCSSR, 
                        CYGARC_REG_SCSSR_CLEARMASK & ~CYGARC_REG_SCSSR_RDRF);

        if( cyg_hal_is_break( &c , 1 ) )
            *__ctrlc = 1;

        res = CYG_ISR_HANDLED;
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}

void
cyg_hal_plf_sci_init(int sci_index, int comm_index, 
                     int rcv_vect, cyg_uint8* base)
{
    channel_data_t* chan = &channels[sci_index];
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Initialize channel table
    chan->base = base;
    chan->isr_vector = rcv_vect;
    chan->msec_timeout = 1000;

    // Disable interrupts.
    HAL_INTERRUPT_MASK(chan->isr_vector);

    // Init channel
    
    cyg_hal_plf_sci_init_channel(chan);

    // Setup procs in the vector table

    // Initialize channel procs
    CYGACC_CALL_IF_SET_CONSOLE_COMM(comm_index);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, chan);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_sci_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_sci_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_sci_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_sci_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_sci_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_sci_isr);
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_sci_getc_timeout);

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG || CYGPRI_HAL_IMPLEMENTS_IF_SERVICES

#endif // CYGNUM_HAL_SH_SH3_SCI_PORTS

//-----------------------------------------------------------------------------
// end of sh_sci.c
