//==========================================================================
//
//      devs/table.cxx
//
//      Virtual Device Table without Virtual Functions
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:  proven
// Date:        1998-05-20
// Purpose:     Virtual Device Table
// Description: Allow packages to use devices without having to know the 
//              device class.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devices.h>    // Include all the possible devices
#include <cyg/infra/cyg_type.h> // Common type definitions and support
#include <cyg/devs/common/table.h>
#include <cyg/hal/hal_diag.h>   // HAL diagnostic output

#ifdef CYG_DEVICE_TABLE

// ---------------------------------------------------------------------------
// CYG_DEVICE_SERIAL_RS232_MN10300_1

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_read_cancel(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.io_read_cancel(iorb);
    CYG_ASSERT(ret == 0, "Read cancel failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_read_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.io_read_blocking(iorb);
    CYG_ASSERT(ret == 0, "Read failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_write_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.io_write_blocking(iorb);
    CYG_ASSERT(ret == 0, "Write failed to the mn10300 serial port 1.");
    return ENOERR;
}

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_KMODE_INTERRUPT
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_read_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.io_read_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Read asynchronous failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_write_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.io_write_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Write asynchronous failed to the mn10300 serial port 1.");
    return ENOERR;
}
#endif

// Device specific routines;
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_set_kmode(CYG_ADDRWORD cookie, cyg_uint32 mode)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.set_kmode(mode);
    CYG_ASSERT(ret == 0, "Set kmode failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_get_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 * old_baud_rate)
{
    *old_baud_rate = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.get_baud_rate();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_set_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 baud_rate, cyg_int32 * old_baud_rate)
{
    *old_baud_rate = 
        CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.set_baud_rate(baud_rate);
    CYG_ASSERT(*old_baud_rate >= 0, 
               "Set baud rate failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_get_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 * line_mode)
{
    *line_mode = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.get_line_mode();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_set_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 line_mode, cyg_int32 * old_line_mode)
{
    *old_line_mode = 
        CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.set_line_mode(line_mode);
    CYG_ASSERT(*old_line_mode >= 0, 
               "Set line mode failed to the mn10300 serial port 1.");
    return ENOERR;
}

// externC Cyg_ErrNo
// Cyg_Device_Serial_RS232_mn10300_1_get_read_mode(CYG_ADDRWORD cookie, 
//   cyg_int32 * read_mode)
// {
//     *line_mode = serial.get_read_mode();
//     return ENOERR;
// }

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_set_read_mode(CYG_ADDRWORD cookie, 
  cyg_int32 read_mode, cyg_int32 * old_read_mode)
{
    *old_read_mode = 
        CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.set_read_mode(read_mode);
    CYG_ASSERT(*old_read_mode >= 0, 
               "Set read mode failed to the mn10300 serial port 1.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_1_set_read_data(CYG_ADDRWORD cookie, 
  cyg_int32 mode, const char * str, cyg_uint32 length)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME.set_read_data(mode, str, length);
    CYG_ASSERT(0 == ret, "Set read data failed to the mn10300 serial port 1.");
    return ENOERR;
}

#endif // CYG_DEVICE_SERIAL_RS232_MN10300_1

// ---------------------------------------------------------------------------
// CYG_DEVICE_SERIAL_RS232_MN10300_2

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_read_cancel(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.io_read_cancel(iorb);
    CYG_ASSERT(ret == 0, "Read cancel failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_read_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.io_read_blocking(iorb);
    CYG_ASSERT(ret == 0, "Read failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_write_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.io_write_blocking(iorb);
    CYG_ASSERT(ret == 0, "Write failed to the mn10300 serial port 2.");
    return ENOERR;
}

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2_KMODE_INTERRUPT
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_read_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.io_read_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Read asynchronous failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_write_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.io_write_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Write asynchronous failed to the mn10300 serial port 2.");
    return ENOERR;
}
#endif

// Device specific routines;
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_set_kmode(CYG_ADDRWORD cookie, cyg_uint32 mode)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.set_kmode(mode);
    CYG_ASSERT(ret == 0, "Set kmode failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_get_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 * old_baud_rate)
{
    *old_baud_rate = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.get_baud_rate();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_set_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 baud_rate, cyg_int32 * old_baud_rate)
{
    *old_baud_rate = 
        CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.set_baud_rate(baud_rate);
    CYG_ASSERT(*old_baud_rate >= 0, 
               "Set baud rate failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_get_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 * line_mode)
{
    *line_mode = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.get_line_mode();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_set_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 line_mode, cyg_int32 * old_line_mode)
{
    *old_line_mode = 
        CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.set_line_mode(line_mode);
    CYG_ASSERT(*old_line_mode >= 0, 
               "Set line mode failed to the mn10300 serial port 2.");
    return ENOERR;
}

// externC Cyg_ErrNo
// Cyg_Device_Serial_RS232_mn10300_2_get_read_mode(CYG_ADDRWORD cookie, 
//   cyg_int32 * read_mode)
// {
//     *line_mode = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.get_read_mode();
//     return ENOERR;
// }

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_set_read_mode(CYG_ADDRWORD cookie, 
  cyg_int32 read_mode, cyg_int32 * old_read_mode)
{
    *old_read_mode = 
        CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.set_read_mode(read_mode);
    CYG_ASSERT(*old_read_mode >= 0, 
               "Set read mode failed to the mn10300 serial port 2.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_mn10300_2_set_read_data(CYG_ADDRWORD cookie, 
  cyg_int32 mode, const char * str, cyg_uint32 length)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME.set_read_data(mode, str, length);
    CYG_ASSERT(0 == ret, "Set read data failed to the mn10300 serial port 2.");
    return ENOERR;
}

#endif // CYG_DEVICE_SERIAL_RS232_MN10300_2

// ---------------------------------------------------------------------------
// CYG_DEVICE_SERIAL_RS232_TX39

#ifdef CYG_DEVICE_SERIAL_RS232_TX39

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_read_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_TX39_NAME.io_read_blocking(iorb);
    CYG_ASSERT(ret == 0, "Read failed to the TX39 serial port 0.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_write_blocking(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_TX39_NAME.io_write_blocking(iorb);
    CYG_ASSERT(ret == 0, "Write failed to the TX39 serial port 0.");
    return ENOERR;
}

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_read_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_TX39_NAME.io_read_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Read asynchronous failed to the TX39 serial port 0.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_write_asynchronous(CYG_ADDRWORD cookie, Cyg_IORB * iorb)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_TX39_NAME.io_write_asynchronous(iorb);
    CYG_ASSERT(ret == 0, 
               "Write asynchronous failed to the TX39 serial port 0.");
    return ENOERR;
}
#endif

// Device specific routines;
externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_set_kmode(CYG_ADDRWORD cookie, cyg_uint32 mode)
{
    cyg_int32 ret;

    ret = CYG_DEVICE_SERIAL_RS232_TX39_NAME.set_kmode(mode);
    CYG_ASSERT(ret == 0, "Set kmode failed to the TX39 serial port 0.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_get_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 * old_baud_rate)
{
    *old_baud_rate = CYG_DEVICE_SERIAL_RS232_TX39_NAME.get_baud_rate();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_set_baud_rate(CYG_ADDRWORD cookie, 
  cyg_int32 baud_rate, cyg_int32 * old_baud_rate)
{
    *old_baud_rate = 
        CYG_DEVICE_SERIAL_RS232_TX39_NAME.set_baud_rate(baud_rate);
    CYG_ASSERT(*old_baud_rate >= 0, 
               "Set baud rate failed to the TX39 serial port 0.");
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_get_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 * line_mode)
{
    *line_mode = CYG_DEVICE_SERIAL_RS232_TX39_NAME.get_line_mode();
    return ENOERR;
}

externC Cyg_ErrNo
Cyg_Device_Serial_RS232_tx39_set_line_mode(CYG_ADDRWORD cookie, 
  cyg_int32 line_mode, cyg_int32 * old_line_mode)
{
    *old_line_mode = 
        CYG_DEVICE_SERIAL_RS232_TX39_NAME.set_line_mode(line_mode);
    CYG_ASSERT(*old_line_mode >= 0, 
               "Set line mode failed to the TX39 serial port 0.");
    return ENOERR;
}

#endif /* CYG_DEVICE_SERIAL_RS232_TX39 */

// ---------------------------------------------------------------------------
// HAL_DIAG

#ifdef HAL_DIAG_INIT
externC Cyg_ErrNo
Cyg_Device_Pseudo_Hal_Diag_open( CYG_ADDRWORD cookie, Cyg_DeviceOpenMode )
{
    HAL_DIAG_INIT();
    return ENOERR;
} 
#endif

#ifdef HAL_DIAG_READ_CHAR
externC Cyg_ErrNo
Cyg_Device_Pseudo_Hal_Diag_read_blocking( CYG_ADDRWORD cookie, Cyg_IORB *iorb )
{
    cyg_ucount32 i;
    cyg_bool exit_loop = false;

    for (i=0; (i < iorb->buffer_length) && !exit_loop; i++)
    {
        HAL_DIAG_READ_CHAR(* ((char*)iorb->buffer + i) );
        
        // exit loop on certain characters, newline, EOF, etc.
        switch (*((char*)iorb->buffer + i))
        {
        case 4:
        case 10:
        case 13:
        case 26:
            exit_loop = true;
            break;
        default:
            break;
        } // switch
    } // for

    iorb->xferred_length = i;
    return ENOERR;
}
#endif

#ifdef HAL_DIAG_WRITE_CHAR
externC Cyg_ErrNo
Cyg_Device_Pseudo_Hal_Diag_write_blocking( CYG_ADDRWORD cookie, Cyg_IORB *iorb )
{
    cyg_ucount32 i;

    for (i=0; i < iorb->buffer_length; i++)
    {
        HAL_DIAG_WRITE_CHAR(*((char *)iorb->buffer + i));
    } // for

    iorb->xferred_length = iorb->buffer_length;
    return ENOERR;
}
#endif

// ---------------------------------------------------------------------------
// Device specific ioctl table
//
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1
struct Cyg_Device_Serial_RS232_Table_t Cyg_Device_Serial_RS232_mn10300_1_Table =
{
    &Cyg_Device_Serial_RS232_mn10300_1_set_kmode,
    &Cyg_Device_Serial_RS232_mn10300_1_get_baud_rate,
    &Cyg_Device_Serial_RS232_mn10300_1_set_baud_rate,
    &Cyg_Device_Serial_RS232_mn10300_1_get_line_mode,
    &Cyg_Device_Serial_RS232_mn10300_1_set_line_mode,
    NULL,
    &Cyg_Device_Serial_RS232_mn10300_1_set_read_mode,
    &Cyg_Device_Serial_RS232_mn10300_1_set_read_data
};
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
struct Cyg_Device_Serial_RS232_Table_t Cyg_Device_Serial_RS232_mn10300_2_Table =
{
    &Cyg_Device_Serial_RS232_mn10300_2_set_kmode,
    &Cyg_Device_Serial_RS232_mn10300_2_get_baud_rate,
    &Cyg_Device_Serial_RS232_mn10300_2_set_baud_rate,
    &Cyg_Device_Serial_RS232_mn10300_2_get_line_mode,
    &Cyg_Device_Serial_RS232_mn10300_2_set_line_mode,
    NULL,
    &Cyg_Device_Serial_RS232_mn10300_2_set_read_mode,
    &Cyg_Device_Serial_RS232_mn10300_2_set_read_data
};
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39
struct Cyg_Device_Serial_RS232_Table_t Cyg_Device_Serial_RS232_tx39_Table =
{
    &Cyg_Device_Serial_RS232_tx39_set_kmode,
    &Cyg_Device_Serial_RS232_tx39_get_baud_rate,
    &Cyg_Device_Serial_RS232_tx39_set_baud_rate,
    &Cyg_Device_Serial_RS232_tx39_get_line_mode,
    &Cyg_Device_Serial_RS232_tx39_set_line_mode,
    NULL, NULL, NULL
};
#endif
// ---------------------------------------------------------------------------
// Device table
//
struct Cyg_Device_Table_t Cyg_Device_Table[] = {

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1
{	"serial1", (CYG_ADDRWORD) 0,
        NULL,       // no open
	&Cyg_Device_Serial_RS232_mn10300_1_read_cancel,
	NULL,
	&Cyg_Device_Serial_RS232_mn10300_1_read_blocking,
	&Cyg_Device_Serial_RS232_mn10300_1_write_blocking,
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_KMODE_INTERRUPT
	&Cyg_Device_Serial_RS232_mn10300_1_read_asynchronous,
	&Cyg_Device_Serial_RS232_mn10300_1_write_asynchronous,
#else
        NULL, NULL,
#endif
        NULL,       // no close
	(CYG_ADDRWORD)&Cyg_Device_Serial_RS232_mn10300_1_Table 
},
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
{	"serial2", (CYG_ADDRWORD) 0,	
        NULL,       // no open
	&Cyg_Device_Serial_RS232_mn10300_2_read_cancel,
	NULL,
	&Cyg_Device_Serial_RS232_mn10300_2_read_blocking,
	&Cyg_Device_Serial_RS232_mn10300_2_write_blocking,
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2_KMODE_INTERRUPT
	&Cyg_Device_Serial_RS232_mn10300_2_read_asynchronous,
        &Cyg_Device_Serial_RS232_mn10300_2_write_asynchronous,
#else
        NULL, NULL,
#endif
        NULL,       // no close
	(CYG_ADDRWORD)&Cyg_Device_Serial_RS232_mn10300_2_Table
},    
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39
{	"serial0", (CYG_ADDRWORD) 0,	
        NULL,       // no open
	NULL,
	NULL,
	&Cyg_Device_Serial_RS232_tx39_read_blocking,
	&Cyg_Device_Serial_RS232_tx39_write_blocking,
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT
	&Cyg_Device_Serial_RS232_tx39_read_asynchronous,
	&Cyg_Device_Serial_RS232_tx39_write_asynchronous,
#else
        NULL,NULL,
#endif
        NULL,       // no close
	(CYG_ADDRWORD)&Cyg_Device_Serial_RS232_tx39_Table 
},
#endif

{       "haldiag", (CYG_ADDRWORD) 0,
#ifdef HAL_DIAG_INIT
        &Cyg_Device_Pseudo_Hal_Diag_open,
#else
        NULL,
#endif

	NULL,
	NULL,

#ifdef HAL_DIAG_READ_CHAR
        &Cyg_Device_Pseudo_Hal_Diag_read_blocking,
#else
        NULL,
#endif

#ifdef HAL_DIAG_WRITE_CHAR
        &Cyg_Device_Pseudo_Hal_Diag_write_blocking,
#else
        NULL,
#endif
        NULL }, // no close

{	NULL, (CYG_ADDRWORD) 0, NULL, NULL, NULL, NULL }
};

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1
CYG_ADDRWORD stdeval1_serial1 = (CYG_ADDRWORD)&Cyg_Device_Table[0];
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
CYG_ADDRWORD stdeval1_serial2 = (CYG_ADDRWORD)&Cyg_Device_Table[1];
#endif
#else
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2
CYG_ADDRWORD stdeval1_serial2 = (CYG_ADDRWORD)&Cyg_Device_Table[0];
#endif
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39
CYG_ADDRWORD jmr3904_serial = (CYG_ADDRWORD)&Cyg_Device_Table[0];
#endif

#endif

/*---------------------------------------------------------------------------*/
/* End of devs/table.cxx */
