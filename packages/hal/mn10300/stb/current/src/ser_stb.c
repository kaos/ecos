//=============================================================================
//
//      ser_stb.c
//
//      Simple driver for the serial controllers on the AM33 STB board
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
// at http://sources.redhat.com/ecos/ecos-license
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   dhowells
// Contributors:dmoseley
// Date:        2000-08-11
// Description: Simple driver for the 16c550c serial controller
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // interface API
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS
#include <cyg/hal/hal_misc.h>           // Helper functions
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

//-----------------------------------------------------------------------------
// Base Registers
#define STB_SER0_BASE      0xD4002000
#define STB_SER1_BASE      0xD4002010

/*---------------------------------------------------------------------------*/
// MN10300 Serial line

#define _SERIAL_CR       0x00
#define _SERIAL_ICR      0x04
#define _SERIAL_TXR      0x08
#define _SERIAL_RXR      0x09
#define _SERIAL_SR       0x0c

#define SERIAL0_CR       ((volatile cyg_uint16 *)(STB_SER0_BASE + _SERIAL_CR))
#define SERIAL0_ICR      ((volatile cyg_uint8 *) (STB_SER0_BASE + _SERIAL_ICR))
#define SERIAL0_TXR      ((volatile cyg_uint8 *) (STB_SER0_BASE + _SERIAL_TXR))
#define SERIAL0_RXR      ((volatile cyg_uint8 *) (STB_SER0_BASE + _SERIAL_RXR))
#define SERIAL0_SR       ((volatile cyg_uint16 *)(STB_SER0_BASE + _SERIAL_SR))

#define SERIAL1_CR       ((volatile cyg_uint16 *)(STB_SER1_BASE + _SERIAL_CR))
#define SERIAL1_ICR      ((volatile cyg_uint8 *) (STB_SER1_BASE + _SERIAL_ICR))
#define SERIAL1_TXR      ((volatile cyg_uint8 *) (STB_SER1_BASE + _SERIAL_TXR))
#define SERIAL1_RXR      ((volatile cyg_uint8 *) (STB_SER1_BASE + _SERIAL_RXR))
#define SERIAL1_SR       ((volatile cyg_uint16 *)(STB_SER1_BASE + _SERIAL_SR))

// Timer 0 provides a prescaler for lower baud rates
#define TIMER0_MD       ((volatile cyg_uint8 *)0xd4003000)
#define TIMER0_BR       ((volatile cyg_uint8 *)0xd4003010)

// Timer 2 provides baud rate divisor
#define TIMER2_MD       ((volatile cyg_uint8 *)0xd4003002)
#define TIMER2_BR       ((volatile cyg_uint8 *)0xd4003012)

// Timer 1 provides a prescaler for lower baud rates
#define TIMER1_MD       ((volatile cyg_uint8 *)0xd4003001)
#define TIMER1_BR       ((volatile cyg_uint8 *)0xd4003011)

// Timer 3 provides baud rate divisor
#define TIMER3_MD       ((volatile cyg_uint8 *)0xd4003003)
#define TIMER3_BR       ((volatile cyg_uint8 *)0xd4003013)

#define SIO_LSTAT_TRDY  0x20
#define SIO_LSTAT_RRDY  0x10

#define SIO_INT_ENABLE  0x11

// These values are calculated based on the MN103E010 LSI Manual
#define PRESCALAR_BASE_1200          198
#define PRESCALAR_BASE_2400          198
#define PRESCALAR_BASE_4800          198
#define PRESCALAR_BASE_9600          198
#define PRESCALAR_BASE_96002         198
#define PRESCALAR_BASE_14400         66
#define PRESCALAR_BASE_19200         0
#define PRESCALAR_BASE_38400         0
#define PRESCALAR_BASE_56000         0
#define PRESCALAR_BASE_57600         0
#define PRESCALAR_BASE_115200        0
#define PRESCALAR_BASE_230400        0

#define TIMER_BASE_1200              15
#define TIMER_BASE_2400              7
#define TIMER_BASE_4800              3
#define TIMER_BASE_9600              1
#define TIMER_BASE_96002             2
#define TIMER_BASE_14400             3
#define TIMER_BASE_19200             198
#define TIMER_BASE_38400             99
#define TIMER_BASE_56000             68
#define TIMER_BASE_57600             66
#define TIMER_BASE_115200            33
#define TIMER_BASE_230400            16

#define TMR_ENABLE                   0x80
#define TMR_SRC_IOCLOCK              0x00
#define TMR_SRC_TMR0_UNDERFLOW       0x04

#define PRESCALAR_MODE_1200          (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_2400          (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_4800          (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_9600          (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_96002         (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_14400         (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define PRESCALAR_MODE_19200         0
#define PRESCALAR_MODE_38400         0
#define PRESCALAR_MODE_56000         0
#define PRESCALAR_MODE_57600         0
#define PRESCALAR_MODE_115200        0
#define PRESCALAR_MODE_230400        0

#define TIMER_MODE_1200              (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_2400              (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_4800              (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_9600              (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_96002             (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_14400             (TMR_ENABLE | TMR_SRC_TMR0_UNDERFLOW)
#define TIMER_MODE_19200             (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define TIMER_MODE_38400             (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define TIMER_MODE_56000             (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define TIMER_MODE_57600             (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define TIMER_MODE_115200            (TMR_ENABLE | TMR_SRC_IOCLOCK)
#define TIMER_MODE_230400            (TMR_ENABLE | TMR_SRC_IOCLOCK)

#define BAUD_CASE(num)                                   \
    case num:                                            \
        prescalar_base_value = PRESCALAR_BASE_ ## num;   \
        prescalar_mode_value = PRESCALAR_MODE_ ## num;   \
        timer_base_value = TIMER_BASE_ ## num;           \
        timer_mode_value = TIMER_MODE_ ## num;           \
        break;

//-----------------------------------------------------------------------------
typedef struct {
    cyg_uint8* base;
    cyg_int32 msec_timeout;
    int isr_vector;
} channel_data_t;

static const channel_data_t channels[2] = {
    { (cyg_uint8*)STB_SER0_BASE, 1000, CYGNUM_HAL_INTERRUPT_SERIAL_0_RX },
    { (cyg_uint8*)STB_SER1_BASE, 1000, CYGNUM_HAL_INTERRUPT_SERIAL_1_RX },
};

//-----------------------------------------------------------------------------
// Set the baud rate

static int
cyg_hal_plf_serial_set_baud(cyg_uint8* port, cyg_uint32 baud_rate)
{
    volatile cyg_uint8 *prescalar_base_reg = 0, prescalar_base_value = 0;
    volatile cyg_uint8 *prescalar_mode_reg = 0, prescalar_mode_value = 0;
    volatile cyg_uint8 *timer_base_reg = 0,     timer_base_value = 0;
    volatile cyg_uint8 *timer_mode_reg = 0,     timer_mode_value = 0;

    // Always use TMR0 as the prescalar
    prescalar_base_reg = TIMER0_BR;
    prescalar_mode_reg = TIMER0_MD;

    if (port == (cyg_uint8*)STB_SER0_BASE)
    {
        // SER0 uses TMR2
        timer_base_reg = TIMER2_BR;
        timer_mode_reg = TIMER2_MD;
    } else if (port == (cyg_uint8*)STB_SER1_BASE) {
        // SER1 uses TMR3
        timer_base_reg = TIMER3_BR;
        timer_mode_reg = TIMER3_MD;
    } else {
        // Unknown port.
        return -1;
    }

    switch (baud_rate)
    {
        BAUD_CASE(1200);
        BAUD_CASE(2400);
        BAUD_CASE(4800);
        BAUD_CASE(9600);
        BAUD_CASE(96002);
        BAUD_CASE(14400);
        BAUD_CASE(19200);
        BAUD_CASE(38400);
        BAUD_CASE(56000);
        BAUD_CASE(57600);
        BAUD_CASE(115200);
        BAUD_CASE(230400);

    default:
        // Unknown baud.  Don't change anything
        return -1;
    }

    HAL_WRITE_UINT8(prescalar_base_reg, prescalar_base_value);
    HAL_WRITE_UINT8(prescalar_mode_reg, prescalar_mode_value);
    HAL_WRITE_UINT8(timer_base_reg,     timer_base_value);
    HAL_WRITE_UINT8(timer_mode_reg,     timer_mode_value);

    return 0;
}

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

void
cyg_hal_plf_serial_init_channel(void* __ch_data)
{
    cyg_uint8* port;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    port = ((channel_data_t*)__ch_data)->base;

    // No interrupts for now.
    HAL_WRITE_UINT8(port + _SERIAL_ICR, 0x00);

    // Source from timer 2 or 3, 8bit chars, enable tx and rx
    HAL_WRITE_UINT16(port + _SERIAL_CR, 0xc085);
}

void
cyg_hal_plf_serial_putc(void* __ch_data, cyg_uint8 __ch)
{
    cyg_uint8* port;
    cyg_uint16 _status;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    port = ((channel_data_t*)__ch_data)->base;

    do {
        HAL_READ_UINT16(port + _SERIAL_SR, _status);
    } while ((_status & SIO_LSTAT_TRDY) != 0);

    HAL_WRITE_UINT8(port + _SERIAL_TXR, __ch);
}

static cyg_bool
cyg_hal_plf_serial_getc_nonblock(void* __ch_data, cyg_uint8* ch)
{
    cyg_uint8* port;
    cyg_uint8 _status;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    port = ((channel_data_t*)__ch_data)->base;

    HAL_READ_UINT8(port + _SERIAL_SR, _status);
    if ((_status & SIO_LSTAT_RRDY) == 0)
        return false;

    HAL_READ_UINT8(port + _SERIAL_RXR, *ch);

    // We must ack the interrupt caused by that read to avoid
    // confusing the GDB stub ROM.
    HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_SERIAL_0_RX );    

    return true;
}

cyg_uint8
cyg_hal_plf_serial_getc(void* __ch_data)
{
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    while(!cyg_hal_plf_serial_getc_nonblock(__ch_data, &ch));

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

void
cyg_hal_plf_serial_write(void* __ch_data, const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    while(__len-- > 0)
        cyg_hal_plf_serial_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_plf_serial_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    while(__len-- > 0)
        *__buf++ = cyg_hal_plf_serial_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

#if 1
cyg_bool
cyg_hal_plf_serial_getc_timeout(void* __ch_data, cyg_uint8* ch)
{
    int delay_count;
    channel_data_t* chan;
    cyg_bool res;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    chan = (channel_data_t*)__ch_data;

    delay_count = chan->msec_timeout * 10; // delay in .1 ms steps

    for(;;) {
        res = cyg_hal_plf_serial_getc_nonblock(__ch_data, ch);
        if (res || 0 == delay_count--)
            break;
        CYGACC_CALL_IF_DELAY_US(100);
    }

    CYGARC_HAL_RESTORE_GP();
    return res;
}
#endif

static int
cyg_hal_plf_serial_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    static int irq_state = 0;
    channel_data_t* chan;
    cyg_uint8 icr;
    int ret = 0;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    chan = (channel_data_t*)__ch_data;

    switch (__func) {
    case __COMMCTL_IRQ_ENABLE:
        irq_state = 1;

        HAL_READ_UINT8(chan->base + _SERIAL_ICR, icr);
        icr |= SIO_INT_ENABLE;
        HAL_WRITE_UINT8(chan->base + _SERIAL_ICR, icr);

        HAL_INTERRUPT_SET_LEVEL(chan->isr_vector, 1);
        HAL_INTERRUPT_UNMASK(chan->isr_vector);
        break;

    case __COMMCTL_IRQ_DISABLE:
        ret = irq_state;
        irq_state = 0;

        HAL_READ_UINT8(chan->base + _SERIAL_ICR, icr);
        icr &= ~SIO_INT_ENABLE;
        HAL_WRITE_UINT8(chan->base + _SERIAL_ICR, icr);

        HAL_INTERRUPT_MASK(chan->isr_vector);
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
        cyg_uint8* port = chan->base;
        va_list ap;

        va_start(ap, __func);
        baud_rate = va_arg(ap, cyg_uint32);
        va_end(ap);

        // Disable port interrupts while changing hardware
        HAL_READ_UINT8(port + _SERIAL_ICR, icr);
        HAL_WRITE_UINT8(port + _SERIAL_ICR, 0);

        // Set baud rate.
        ret = cyg_hal_plf_serial_set_baud(port, baud_rate);

        // Reenable interrupts if necessary
        HAL_WRITE_UINT8(port + _SERIAL_ICR, icr);
    }
    break;

    case __COMMCTL_GETBAUD:
        break;

    default:
        break;
    }

    CYGARC_HAL_RESTORE_GP();
    return ret;
}

static int
cyg_hal_plf_serial_isr(void *__ch_data, int* __ctrlc, 
                       CYG_ADDRWORD __vector, CYG_ADDRWORD __data)
{
    int res = 0;
    channel_data_t* chan;
    CYGARC_HAL_SAVE_GP();

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    chan = (channel_data_t*)__ch_data;

    HAL_INTERRUPT_ACKNOWLEDGE(chan->isr_vector);

#if 0
    HAL_READ_UINT8(chan->base + SER_16550_IIR, _iir);
    _iir &= SIO_IIR_ID_MASK;

    *__ctrlc = 0;
    if ((_iir == ISR_Rx_Avail) || (_iir == ISR_Rx_Char_Timeout)) {

        HAL_READ_UINT8(chan->base + SER_16550_RBR, c);
    
        if( cyg_hal_is_break( &c , 1 ) )
            *__ctrlc = 1;

        res = CYG_ISR_HANDLED;
    }
#endif

    CYGARC_HAL_RESTORE_GP();
    return res;
}

static void
cyg_hal_plf_serial_init(void)
{
    hal_virtual_comm_table_t* comm;
    int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Disable interrupts.
    HAL_INTERRUPT_MASK(channels[0].isr_vector);
    HAL_INTERRUPT_MASK(channels[1].isr_vector);

    // Init channels
    cyg_hal_plf_serial_init_channel((void*)&channels[0]);
    cyg_hal_plf_serial_set_baud(channels[0].base, CYGHWR_HAL_MN10300_AM33_STB_DIAG_BAUD);

    cyg_hal_plf_serial_init_channel((void*)&channels[1]);
    cyg_hal_plf_serial_set_baud(channels[1].base, CYGHWR_HAL_MN10300_AM33_STB_GDB_BAUD);
    
    // Setup procs in the vector table

    // Set channel 0
    CYGACC_CALL_IF_SET_CONSOLE_COMM(0);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &channels[0]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
#if 1
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
#endif

    // Set channel 1
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_COMM_IF_CH_DATA_SET(*comm, &channels[1]);
    CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_plf_serial_write);
    CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_plf_serial_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_plf_serial_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_plf_serial_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_plf_serial_control);
    CYGACC_COMM_IF_DBG_ISR_SET(*comm, cyg_hal_plf_serial_isr);
#if 1
    CYGACC_COMM_IF_GETC_TIMEOUT_SET(*comm, cyg_hal_plf_serial_getc_timeout);
#endif

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

void
cyg_hal_plf_serial_setbaud(void *__ch_data, cyg_uint32 baud_rate)
{
    cyg_uint8* port;

    // Some of the diagnostic print code calls through here with no idea what the ch_data is.
    // Go ahead and assume it is channels[0].
    if (__ch_data == 0)
      __ch_data = (void*)&channels[0];

    port = ((channel_data_t*)__ch_data)->base;

    cyg_hal_plf_serial_set_baud(port, baud_rate);
}

/*---------------------------------------------------------------------------*/
/* End of ser_stb.c */
