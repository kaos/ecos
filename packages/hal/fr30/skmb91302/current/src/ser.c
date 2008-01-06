//=============================================================================
//
//      ser.c
//
//      Simple (polling) driver for the Fujitsu MB91302 on-chip serial port
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
// Author(s):   larsi
// Contributors:
// Date:        2007-01-10
// Description: Simple driver for the MB91302 internal serial port
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

// We have no control over baud rate
// #if CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD==57600
// #define CYG_DEV_SERIAL_BAUD_DIVISOR    BAUD_57600
// #endif

// #ifndef CYG_DEV_SERIAL_BAUD_DIVISOR
// #error Missing/incorrect serial baud rate defined - CDL error?
// #endif

#define CYG_HAL_FR30_MB91301_SMR0     0x63
#define CYG_HAL_FR30_MB91301_SCR0     0x62
#define CYG_HAL_FR30_MB91301_SIDR0    0x61
#define CYG_HAL_FR30_MB91301_SODR0    0x61
#define CYG_HAL_FR30_MB91301_SSR0     0x60
#define CYG_HAL_FR30_MB91301_UTIM0    0x64
#define CYG_HAL_FR30_MB91301_UTIMR0   0x64
#define CYG_HAL_FR30_MB91301_DRCL     0x66
#define CYG_HAL_FR30_MB91301_UTIMC0   0x67


#define CYG_HAL_FR30_MB91301_SER0_BASE  0x60
#define CYG_HAL_FR30_MB91301_SER1_BASE  0x68
#define CYG_HAL_FR30_MB91301_SER2_BASE  0x70

#define CYG_HAL_FR30_MB91301_SMR_OFFSET     0x03
#define CYG_HAL_FR30_MB91301_SCR_OFFSET     0x02
#define CYG_HAL_FR30_MB91301_SIDR_OFFSET    0x01
#define CYG_HAL_FR30_MB91301_SODR_OFFSET    0x01
#define CYG_HAL_FR30_MB91301_SSR_OFFSET     0x00
#define CYG_HAL_FR30_MB91301_UTIM_OFFSET    0x04
#define CYG_HAL_FR30_MB91301_UTIMR_OFFSET   0x04
#define CYG_HAL_FR30_MB91301_DRCL_OFFSET    0x06
#define CYG_HAL_FR30_MB91301_UTIMC_OFFSET   0x07

#define CYG_HAL_FR30_MB91301_PDRG     0x10
#define CYG_HAL_FR30_MB91301_DDRG     0x400
#define CYG_HAL_FR30_MB91301_PFRG     0x410
#define CYG_HAL_FR30_MB91301_PDRJ     0x13
#define CYG_HAL_FR30_MB91301_DDRJ     0x403
#define CYG_HAL_FR30_MB91301_PFRJ     0x413


//-----------------------------------------------------------------------------
typedef struct {
    cyg_uint8 base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

static channel_data_t channels[2] = {
    {  CYG_HAL_FR30_MB91301_SER0_BASE, 1000, CYGNUM_HAL_INTERRUPT_UART0_RX},
    {  CYG_HAL_FR30_MB91301_SER1_BASE, 1000, CYGNUM_HAL_INTERRUPT_UART1_RX}/*,
    {  CYG_HAL_FR30_MB91301_SER2_BASE, 1000, CYGNUM_HAL_INTERRUPT_UART2_RX}*/
};

//-----------------------------------------------------------------------------
// function for calculating and setting the baudrate

static void cyg_hal_plf_serial_set_baudrate_internal(cyg_uint8 port, int baudrate){

    float n, nn;
    int t, tt;

    n = (CYGHWR_HAL_FR30_MB91301_SYSTEM_CLOCK_MHZ * 1000000 / CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER) / (float) (32 * baudrate) - 1;
    nn = (CYGHWR_HAL_FR30_MB91301_SYSTEM_CLOCK_MHZ * 1000000 / CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER) / (float) (32 * baudrate) - 1.5;

    /* rounding */
    t = n;
    tt = nn;
    if ( (n-t) > (1 - (n-t)) ) t++;
    if ( (nn-tt) > (1 - (nn-tt)) ) tt++;

    /* check which is better t or tt */

    /* back calculation of baudrate from t and tt */
    n = (CYGHWR_HAL_FR30_MB91301_SYSTEM_CLOCK_MHZ * 1000000 / CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER) / (float) ((2*t+2) * 16);
    nn = (CYGHWR_HAL_FR30_MB91301_SYSTEM_CLOCK_MHZ * 1000000 / CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER) / (float) ((2*tt+3) * 16);

    /* taking difference between wanted baudrate and back calculated br */
    if ((baudrate - n) < 0)
        n = n - baudrate;
    else
        n = baudrate - n;

    if ((baudrate - nn) < 0)
        nn = nn - baudrate;
    else
        nn = baudrate - nn;

    /* and finally take the best */
    if (n < nn){
        /* UCC1 = 0 */
        HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_UTIMC_OFFSET, 0x02);
        HAL_WRITE_UINT16(port + CYG_HAL_FR30_MB91301_UTIMR_OFFSET, t);
    } else {
        /* UCC1 = 1 */
        HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_UTIMC_OFFSET, 0x82);
        HAL_WRITE_UINT16(port + CYG_HAL_FR30_MB91301_UTIMR_OFFSET, tt);
    }
}

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

void
cyg_hal_plf_serial_init_channel(void* __ch_data)
{
    cyg_uint8 port;
    cyg_uint8 value;

    port = ((channel_data_t*)__ch_data)->base;

    // set the port direction and function registers to serial
    switch (port){
        case CYG_HAL_FR30_MB91301_SER0_BASE:
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_DDRJ, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_DDRJ, value | 0x2);
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_PFRJ, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_PFRJ, value | 0x3);
            if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_DEFAULT == 0){
                cyg_hal_plf_serial_set_baudrate_internal(port, CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD);
            } else {
                cyg_hal_plf_serial_set_baudrate_internal(port, CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD);
            }
            break;

        case CYG_HAL_FR30_MB91301_SER1_BASE:
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_DDRJ, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_DDRJ, value | 0x10);
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_PFRJ, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_PFRJ, value | 0x18);
            if (CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_DEFAULT == 1){
                cyg_hal_plf_serial_set_baudrate_internal(port, CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL_BAUD);
            } else {
                cyg_hal_plf_serial_set_baudrate_internal(port, CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL_BAUD);
            }
            break;
/*
        case CYG_HAL_FR30_MB91301_SER2_BASE:
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_DDRG, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_DDRG, value | 0x40);
            HAL_READ_UINT8(CYG_HAL_FR30_MB91301_PFRG, value);
            HAL_WRITE_UINT8(CYG_HAL_FR30_MB91301_PFRG, value | 0x60);
            break;
*/
    }


    // set up U-Timer
/*    HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_UTIMC_OFFSET, 0x02);
    // 115200 bps
    HAL_WRITE_UINT16(port + CYG_HAL_FR30_MB91301_UTIMR_OFFSET, 0x7);

    cyg_hal_plf_serial_set_baudrate_internal(port, baudrate);
*/
    // setup UART
    HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_SCR_OFFSET, 0x13);
    HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_SMR_OFFSET, 0x30);
}

void
cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 __ch)
{
    cyg_uint8 ssr;
    cyg_uint8 port;

    // wait for tx rdy
    port = ((channel_data_t*)__ch_data)->base;
    do
    {
        HAL_READ_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, ssr);
    } while (!(ssr & BIT3));
    // Now, write it
    HAL_WRITE_UINT8( port + CYG_HAL_FR30_MB91301_SODR_OFFSET, __ch );
}

static cyg_bool
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8 ssr;
    cyg_uint8 port;

    port = ((channel_data_t*)__ch_data)->base;
    HAL_READ_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, ssr);
    if (!(ssr & BIT4))
        return false;

    HAL_READ_UINT8(port + CYG_HAL_FR30_MB91301_SIDR_OFFSET, *ch);
//     hal_diag_led(port);
    return true;
}

cyg_uint8
cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;

    while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch));
    return ch;
}

static void
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    while(__len-- > 0)
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);
}

static void
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);
}


cyg_bool
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    channel_data_t* chan;
    cyg_bool res;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    chan = (channel_data_t*)__ch_data;

    delay_count = chan->msec_timeout; // delay in 1000 us steps
    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        CYGACC_CALL_IF_DELAY_US(1000);
    }
    return res;
}

static int
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    static int irq_state = 0;
    channel_data_t* chan;
    int ret = 0;
    cyg_uint8 port;
    cyg_uint8 value;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    chan = (channel_data_t*)__ch_data;

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;
        port = ((channel_data_t*)__ch_data)->base;
        HAL_READ_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, value);
        HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, value | BIT1);
        break;
    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;
        port = ((channel_data_t*)__ch_data)->base;
        HAL_READ_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, value);
        HAL_WRITE_UINT8(port + CYG_HAL_FR30_MB91301_SSR_OFFSET, value & ~BIT1);
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
    break;
    case __COMMCTL_SETBAUD:
    {
        cyg_uint32 baud_rate;
        va_list ap;

        va_start(ap, __func);
        baud_rate = va_arg(ap, cyg_uint32);
        va_end(ap);
        port = ((channel_data_t*)__ch_data)->base;
        cyg_hal_plf_serial_set_baudrate_internal(port, baud_rate);
    }
    break;

    case __COMMCTL_GETBAUD:
        break;
    default:
        break;
    }
    return ret;
}

static int
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc, 
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    *__ctrlc = 0;
    return 0;
}

static void
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int i, cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

#define NUM_CHANNELS CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS
    for (i = 0; i < NUM_CHANNELS; i++) {

	// Disable interrupts.
	HAL_INTERRUPT_MASK(channels[i].isr_vector);

	// Init channels
	cyg_hal_plf_serial_init_channel((void*)&channels[i]);
    
	// Setup procs in the vector table

	// Set COMM callbacks for channel
	CYGACC_CALL_IF_SET_CONSOLE_COMM(i);
	comm = CYGACC_CALL_IF_CONSOLE_PROCS();
	CYGACC_COMM_IF_CH_DATA_SET(*comm, &channels[i]);
	CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
	CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
	CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
	CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
	CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
	CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
	CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
    }
    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);
}

void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;

    if (initialized)
        return;

    initialized = 1;

    cyg_hal_plf_serial_init();

}

//-----------------------------------------------------------------------------
// end of ser.c

