//==========================================================================
//
//        ser_test_protocol.c
//
//        Serial device driver testing protocol
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
// by Cygnus are Copyright (C) 1998, 1999 Cygnus Solutions.  
// All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          1999-03-17
// Description:   Protocol implementation used to test eCos serial devices.
//                Relies on ser_filter to be present on the host side to
//                respond to test requests.
// 
// To Do:
//  o Clean up.
//  o Clean up config change magic.
//  o Figure out how to handle kernel dependency
//    : without kernel, no timeout. Without timeout, no filter auto detection.
//
//####DESCRIPTIONEND####

#include <pkgconf/system.h>
#include <pkgconf/io.h>
#include <pkgconf/io_serial.h>

#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/ttyio.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_intr.h>           // for reclaiming interrup vector

#ifdef CYG_HAL_SPARCLITE_SLEB
#include <pkgconf/hal_sparclite_sleb.h> // CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT
#endif

//----------------------------------------------------------------------------
// Definition of which device to run tests on on various platforms.

#define NA_MSG "No test device specified"


#if defined(CYGPKG_HAL_POWERPC_COGENT)                          \
    && defined(CYGPKG_IO_SERIAL_POWERPC_COGENT)                 \
    && defined(CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B)
# define TEST_SER_DEV CYGDAT_IO_SERIAL_POWERPC_COGENT_SERIAL_B_NAME
# if defined(CYGPKG_IO_SERIAL_TTY_TTY2)
#  define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY2_DEV
# endif
#endif
#if defined(CYGPKG_HAL_ARM_PID)                         \
    && defined(CYGPKG_IO_SERIAL_ARM_PID)                \
    && defined(CYGPKG_IO_SERIAL_ARM_PID_SERIAL0)
# define TEST_SER_DEV CYGDAT_IO_SERIAL_ARM_PID_SERIAL0_NAME
# if defined(CYGPKG_IO_SERIAL_TTY_TTY0)
#  define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY0_DEV
# endif
#endif
#if defined(CYGPKG_HAL_ARM_AEB)                         \
    && defined(CYGPKG_IO_SERIAL_ARM_AEB)                \
    && defined(CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1)
# define TEST_SER_DEV CYGDAT_IO_SERIAL_ARM_AEB_SERIAL1_NAME
# if defined(CYGPKG_IO_SERIAL_TTY_TTY1)
#  define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY1_DEV
# endif
#endif
#if defined(CYGPKG_HAL_MIPS_TX39_JMR3904)               \
    && defined(CYGPKG_IO_SERIAL_TX39_JMR3904)           \
    && defined(CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0)
# define TEST_SER_DEV CYGDAT_IO_SERIAL_TX39_JMR3904_SERIAL0_NAME
# if defined(CYGPKG_IO_SERIAL_TTY_TTY1)
#  define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY1_DEV
# endif
#endif
#if defined(CYGPKG_HAL_MN10300_AM31_STDEVAL1)           \
    && defined(CYGPKG_IO_SERIAL_MN10300)                \
    && defined(CYGPKG_IO_SERIAL_MN10300_SERIAL2)
# define TEST_SER_DEV CYGDAT_IO_SERIAL_MN10300_SERIAL2_NAME
# if defined(CYGPKG_IO_SERIAL_TTY_TTY2)
#  define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY1_DEV
# endif
#endif
#if defined(CYGPKG_HAL_SPARCLITE_SLEB)                    \
    && defined(CYGPKG_IO_SERIAL_SPARCLITE_SLEB)           \
    && defined(CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1)
# undef NA_MSG
# define NA_MSG "CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT is unset"
# if !defined(CYG_HAL_STARTUP_RAM) \
     || defined(CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT)
#  define TEST_SER_DEV CYGDAT_IO_SERIAL_SPARCLITE_SLEB_CON1_NAME
// The interrupt vectors are normally in CygMon's control. Steal them back,
// but beware that this results in GDB acknowledge characters showing up in
// the serial driver if CygMon is used for hal_diag output.
// Set CYG_KERNEL_DIAG_GDB_SERIAL_DIRECT in hal_sparclite_sleb as a workaround.
#  define SER_OVERRIDE_INT_1 CYGNUM_HAL_INTERRUPT_9
#  define SER_OVERRIDE_INT_2 CYGNUM_HAL_INTERRUPT_10
#  if defined(CYGPKG_IO_SERIAL_TTY_TTY0)
#   define TEST_TTY_DEV CYGDAT_IO_SERIAL_TTY_TTY0_DEV
#  endif
# endif
#endif

// We can't rely on haldiag for ser_filter detection - it may not define
// a working character reading function.
#ifndef   TEST_SER_DEV
# define  SER_NOP_TEST
# define  TTY_NOP_TEST
# define  TEST_SER_DEV "/dev/null"
# define  TEST_TTY_DEV "/dev/null"
#else
# ifndef  TEST_TTY_DEV
#  define TTY_NOP_TEST
# define  TEST_TTY_DEV "/dev/null"
# endif
#endif

//----------------------------------------------------------------------------
// The data in buffer and the cmd buffer
#define IN_BUFFER_SIZE 1024
cyg_uint8 in_buffer[IN_BUFFER_SIZE];

cyg_int8 cmd_buffer[128];

//----------------------------------------------------------------------------
// Some types specific to the testing protocol.
typedef enum {
    MODE_NO_ECHO = 0,
    MODE_EOP_ECHO,
    MODE_DUPLEX_ECHO
} cyg_mode_t;

typedef enum {
    TEST_RETURN_OK = ENOERR,
    TEST_RETURN_NA
} cyg_test_return_t;

typedef struct ser_cfg {
    cyg_serial_baud_rate_t   baud_rate;
    cyg_serial_word_length_t data_bits;
    cyg_serial_stop_bits_t   stop_bits;
    cyg_serial_parity_t      parity;
    // etc...
} cyg_ser_cfg_t;

typedef enum {
    OPT_SERIAL_DEBUG = 0,
    OPT_VERBOSE_LEVEL
} cyg_option_t;

typedef enum {
    NONE = 0,
    PROTOCOL_PROGRESS,
    PROTOCOL_DATA,
} cyg_verbosity_level_t;


// A few predifined option macros. Use after test_ping().
#define TEST_OPTIONS(__handle, __array) \
  test_options(__handle, sizeof(__array)/8, __array)

#define TEST_HOST_DEBUG(__handle)                               \
    CYG_MACRO_START                                             \
    cyg_uint32 __options[] = {OPT_SERIAL_DEBUG, 1};             \
    test_options((__handle), sizeof(__options)/8,               \
                 __options);                                    \
    CYG_MACRO_END

#define TEST_HOST_PROGRESS(__handle)                            \
    CYG_MACRO_START                                             \
    cyg_uint32 __options[] =                                    \
        {OPT_SERIAL_DEBUG, 1,                                   \
         OPT_VERBOSE_LEVEL, PROTOCOL_PROGRESS};                 \
    test_options((__handle), sizeof(__options)/8,               \
                 __options);                                    \
    CYG_MACRO_END

#define TEST_HOST_DATA(__handle)                                \
    CYG_MACRO_START                                             \
    cyg_uint32 __options[] =                                    \
        {OPT_SERIAL_DEBUG, 1,                                   \
         OPT_VERBOSE_LEVEL, PROTOCOL_DATA};                     \
    test_options((__handle), sizeof(__options)/8,               \
                 __options);                                    \
    CYG_MACRO_END

//----------------------------------------------------------------------------
// A few predefined configurations. These must all be valid for any
// given target until change_config is behaving correctly.
cyg_ser_cfg_t test_configs[] = {
#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904) && !defined(CYGPKG_HAL_ARM_PID)
    { CYGNUM_SERIAL_BAUD_9600, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },
#endif

#if !defined(CYGPKG_HAL_MN10300_AM31) &&    \
    1
    { CYGNUM_SERIAL_BAUD_14400, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },
#endif

    { CYGNUM_SERIAL_BAUD_19200, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },

#if !defined(CYGPKG_HAL_SPARCLITE_SLEB) &&      \
    !defined(CYGPKG_HAL_ARM_AEB)
    { CYGNUM_SERIAL_BAUD_38400, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },
#endif

#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904) &&   \
    !defined(CYGPKG_HAL_ARM_AEB) &&             \
    !defined(CYGPKG_HAL_SPARCLITE_SLEB) &&      \
    1
    { CYGNUM_SERIAL_BAUD_57600, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },
#endif

#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904) &&   \
    !defined(CYGPKG_HAL_ARM_PID) &&             \
    !defined(CYGPKG_HAL_ARM_AEB) &&             \
    !defined(CYGPKG_HAL_MN10300_STDEVAL1) &&    \
    !defined(CYGPKG_HAL_SPARCLITE_SLEB)
    { CYGNUM_SERIAL_BAUD_115200, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE },
#endif

#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904) &&   \
    !defined(CYGPKG_HAL_ARM_AEB) &&             \
    !defined(CYGPKG_HAL_ARM_PID)
    // One stop bit, even parity
    { CYGNUM_SERIAL_BAUD_19200, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_EVEN },
#endif

#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904) &&   \
    !defined(CYGPKG_HAL_ARM_AEB) &&             \
    !defined(CYGPKG_HAL_ARM_PID)
    // Two stop bits, even parity
    { CYGNUM_SERIAL_BAUD_19200, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_2, CYGNUM_SERIAL_PARITY_EVEN },
#endif

#if !defined(CYGPKG_HAL_MIPS_TX39_JMR3904)
    // Two stop bits, no parity
    { CYGNUM_SERIAL_BAUD_19200, CYGNUM_SERIAL_WORD_LENGTH_8, 
      CYGNUM_SERIAL_STOP_2, CYGNUM_SERIAL_PARITY_NONE },
#endif
};

//----------------------------------------------------------------------------
// Macros to help extract values from the argument string.
// Note: This is probably not an ideal solution, but it was easy to make :)

#define INIT_VALUE(__args)                      \
    unsigned int v;                             \
    char *__ptr1, *__ptr2 = (__args)

#define SET_VALUE(__slot)                       \
do {                                            \
    __ptr1 = index(__ptr2, (int) ':');          \
    if (__ptr1)                                 \
        *__ptr1 = 0;                            \
    v = atoi(__ptr2);                           \
    __ptr2 = __ptr1+1;                          \
    (__slot) = v;                               \
} while (0)


//----------------------------------------------------------------------------
// CRC magic - it's a bit of a hack for now.
// FIXME: standard definition?
#define ADD_CRC_BYTE(__crc, __c)                \
    CYG_MACRO_START                             \
    (__crc) = ((__crc) << 1) ^ (__c);           \
    CYG_MACRO_END

// FIXME: Hack to allow easy ASCII transfer.
#define FIX_CRC(__crc, __icrc)                  \
    CYG_MACRO_START                             \
    __icrc = (int) (__crc);                     \
    if (__icrc < 0)                             \
        __icrc = -__icrc;                       \
    CYG_MACRO_END

//----------------------------------------------------------------------------
// Macros for read/write to serial with error cheking.
static volatile cyg_uint32 r_stamp;
static volatile int aborted;

// This routine will be called if the read "times out"
static void
do_abort(void *handle)
{
    cyg_io_handle_t io_handle = (cyg_io_handle_t)handle;
    cyg_int32 len = 1;  // Need something here
    cyg_io_get_config(io_handle, CYG_IO_GET_CONFIG_SERIAL_ABORT, 0, &len);
    aborted = 1;
}
#include "timeout.inl"

// Read with timeout (__t = timeout in ticks, int* __r = result)
#define Tcyg_io_read_timeout(__h, __d, __l, __t, __r)                         \
    CYG_MACRO_START                                                           \
    int __res;                                                                \
    r_stamp = timeout((__t), do_abort, (__h));                                \
    __res = cyg_io_read((__h), (__d), (__l));                                 \
    CYG_TEST_CHECK((ENOERR == __res || -EINTR == __res),"cyg_io_read failed");\
    *(__r) = __res;                                                           \
    untimeout(r_stamp);                                                       \
    CYG_MACRO_END

#define Tcyg_io_read(__h, __d, __l)                             \
    CYG_MACRO_START                                             \
    int __res = cyg_io_read((__h), (__d), (__l));               \
    CYG_TEST_CHECK(ENOERR == __res, "cyg_io_read failed");      \
    CYG_MACRO_END

#define Tcyg_io_write(__h, __d, __l)                                    \
    CYG_MACRO_START                                                     \
    int __res;                                                          \
    cyg_uint32 __len = 1;                                               \
    __res = cyg_io_write((__h), (__d), (__l));                          \
    if (ENOERR != __res) diag_printf("[%d]\n", __res);                  \
    CYG_TEST_CHECK(ENOERR == __res, "cyg_io_write failed");             \
    __res = cyg_io_get_config((__h),                                    \
                              CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN,    \
                              0, &__len);                               \
    CYG_TEST_CHECK(ENOERR == __res, "DRAIN failed");                    \
    CYG_MACRO_END


//----------------------------------------------------------------------------
// Some libc like functions that are handy to have around.
static int
strlen(const char *c)
{
    int l = 0;
    while (*c++) l++;
    return l;
}

static char*
strcpy(char* dest, const char* src)
{
    char c;
    while ((c = *src++)) {
        *dest++ = c;
    }
    *dest = c;

    return dest;
}

static char*
itoa(char* dest, int v)
{
    char b[16];
    char* p = &b[16];

    *--p = 0;
    if (v) {
        while (v){
            *--p = (v % 10) + '0';
            v = v / 10;
        }
    } else
        *--p = '0';

    return strcpy(dest, p);
}

#define min(_a, _b) ((_a) < (_b)) ? (_a) : (_b)

void
hang(void)
{
    while (1);
}

//-----------------------------------------------------------------------------
// Configuration changing function.
//
// First change to the new config and back again to determine if the driver
// can handle the config.
// If not, return error.
//
// Then query the host for its capability to use the config:
// Format out:
//  "@CONFIG:<baud rate code>:<#data bits>:<#stop bits>:<parity on/off>!"
// Format in:
//  OK/ER
//
// On ER, return error.
//
// On OK, change to the new configuration. Resynchronize with the host:
//  Target waits for host to send S(ync) 
//     [host will delay at least .1 secs after changing baud rate so the 
//      line has time to settle.]
//
//  When receiving S(ync), target replies OK to the host which then
//  acknowledges with D(one).
//
//  Host can also send R(esync) which means it didn't receieve the OK. If
//  so the target resends its S(ync) message.
//
// If the synchronization has not succeeded within 1 second
// (configurable in the protocol), both host and target will revert to
// the previous configuration and attempt to synchronize again. If
// this fails, this call will hang and the host will consider the test
// a failure.
//
// To Do:
//  Host&protocol currently only supports:
//   - no/even parity
int
change_config(cyg_io_handle_t handle, cyg_ser_cfg_t* cfg)
{
    cyg_serial_info_t old_cfg, new_cfg;
    const char cmd[] = "@CONFIG:";
    char reply[2];
    int msglen;
    int res, len;
    cyg_uint8 *p1;

    // Prepare the command.
    p1 = &cmd_buffer[0];
    p1 = strcpy(p1, &cmd[0]);
    p1 = itoa(p1, cfg->baud_rate);
    *p1++ = ':';
    p1 = itoa(p1, cfg->data_bits);
    *p1++ = ':';
    p1 = itoa(p1, cfg->stop_bits);
    *p1++ = ':';
    p1 = itoa(p1, cfg->parity);
    *p1++ = '!';
    *p1 = 0;                            // note: we may append to this later

    // Tell user what we're up to.
    CYG_TEST_INFO(&cmd_buffer[1]);

    // Change to new config and then back to determine if the driver likes it.
    len = sizeof(old_cfg);
    res = cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_INFO, 
                            &old_cfg, &len);
    res = cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_INFO, 
                            &new_cfg, &len);

     if (res != ENOERR) {
        diag_printf("Can't get serial config - DEVIO error: %d\n", res);
        hang();
    }

    new_cfg.baud = cfg->baud_rate;
    new_cfg.word_length = cfg->data_bits;
    new_cfg.stop = cfg->stop_bits;
    new_cfg.parity = cfg->parity;

    res = cyg_io_set_config(handle, CYG_IO_SET_CONFIG_SERIAL_INFO, 
                            &new_cfg, &len);
    cyg_thread_delay(10);  // Some chips don't like changes to happen to fast...

    // Driver didn't like it. It will not have changed anything, so it's
    // safe to return now.
    if (ENOERR != res) {
        // Let user know that the config was skipped due to the target.
        const char txt_tskipped[] = "- skipped by target!";
        p1 = strcpy(p1, txt_tskipped);
        *p1 = 0;
        CYG_TEST_INFO(&cmd_buffer[1]);
        return res;
    }

    // Succeeded. Change back to the original config so we can communicate
    // with the host.
    res = cyg_io_set_config(handle, CYG_IO_SET_CONFIG_SERIAL_INFO, 
                            &old_cfg, &len);
    cyg_thread_delay(10); // Some chips don't like changes to happen to fast...
    if (ENOERR != res) {
        diag_printf("change_config: set_config failed/1 (%d)\n", res);
        hang();
    }

    // Send command to host and read host's reply.
    msglen = strlen(&cmd_buffer[0]);
    Tcyg_io_write(handle, &cmd_buffer[0], &msglen);
    msglen = 2;
    Tcyg_io_read(handle, &reply[0], &msglen);

    // Did host accept configuration?
    if ('O' != reply[0] || 'K' != reply[1]) {
        // Let user know that the config was skipped due to the host.
        const char txt_hskipped[] = "- skipped by host!";
        p1 = strcpy(p1, txt_hskipped);
        *p1 = 0;
        CYG_TEST_INFO(&cmd_buffer[1]);
        diag_printf("Host didn't accept config (%02x, %02x).\n",
                    reply[0], reply[1]);

        res = ENOSUPP;
        return res;
    }

    // Now change config and wait for host to send us a S(ync)
    // character.  
    // Loop until protocol exchange completed. This may hang (as seen
    // from the host), but only when we get totally lost, in which
    // case there's not much else to do really. In this case the host
    // will consider the test a FAIL.
    len = sizeof(new_cfg);
    res = cyg_io_set_config(handle, CYG_IO_SET_CONFIG_SERIAL_INFO, 
                            &new_cfg, &len);
    cyg_thread_delay(10);  // Some chips don't like changes to happen to fast...
    if (ENOERR != res) {
        diag_printf("change_config: set_config failed/2 (%d)\n", res);
        hang();
    }

    {
        int change_succeeded = 0;
        int using_old_config = 0;
        char in_buf[1];
        int len;
        int saw_host_sync;

        for (;;) {
            aborted = 0;                    // global abort flag

            // FIXME: Timeout time needs to be configurable, and needs to
            // be sent to the host before getting here. That would allow
            // changing the timeout by just rebuilding the test - without
            // changing the host software.
            saw_host_sync = 0;
            r_stamp = timeout(100, do_abort, handle);
            while(!aborted) {
                len = 1;
                in_buf[0] = 0;
                res = cyg_io_read(handle, in_buf, &len);
                if (ENOERR != res && -EINTR != res) {
                    // We may have to reset the driver here if the fail
                    // was due to a framing or parity error.
                    break;
                }
                if ('R' == in_buf[0]) {
                    // Resync - host didn't see our message. Try again.
                    saw_host_sync = 0;
                } else if ('S' == in_buf[0] && !saw_host_sync) {
                    // In sync - reply to host if we haven't already
                    char ok_msg[2] = "OK";
                    int ok_len = 2;
                    Tcyg_io_write(handle, ok_msg, &ok_len);
                    saw_host_sync = 1;
                } else if ('D' == in_buf[0] && saw_host_sync) {
                    // Done - exchange completed.
                    change_succeeded = 1;
                    break;
                }
            }
            untimeout(r_stamp);

            if (change_succeeded) {
                // If we had to revert to the old configuration, return error.
                if (using_old_config)
                    return -EIO;
                else
                    return ENOERR;
            }

            // We didn't synchronize with the host. Due to an IO error?
            if (ENOERR != res && -EINTR != res) {
                // We may have to reset the driver if the fail was due to
                // a framing or parity error.
            }

            // Revert to the old configuration and try again.
            len = sizeof(old_cfg);
            res = cyg_io_set_config(handle, CYG_IO_SET_CONFIG_SERIAL_INFO, 
                                    &old_cfg, &len);
            cyg_thread_delay(10);  // Some chips don't like changes to happen to fast...
            if (res != ENOERR) {
                diag_printf("change_config: set_config failed/3 (%d)\n", res);
                hang();
            }
            using_old_config = 1;
        }

    }
}


//-----------------------------------------------------------------------------
// Host sends CRC in decimal ASCII, terminated with !
int
read_host_crc(cyg_io_handle_t handle)
{
    int crc, len;
    cyg_uint8 ch;

    crc = 0;
    while (1) {
        len = 1;
        Tcyg_io_read(handle, &ch, &len);
        if ('!' == ch)
            break;

#ifdef __DEVELOPER__
        if (!((ch >= '0' && ch <= '9'))){
            diag_printf("CHAR: %02x\n", ch);
        }
#endif

        CYG_TEST_CHECK((ch >= '0' && ch <= '9'),
                       "Illegal CRC format from host");

        crc = crc*10 + (ch - '0');
    }

    return crc;
}

//---------------------------------------------------------------------------
// Test binary data transmission.
// Format out:
//  "@BINARY:<byte size>:<mode>!"
// Format in:
//  <checksum>!<#size bytes data>
// For echo modes, also:
//     Format out:
//      <#size bytes data>
//     Format in:
//      OK/ER
// Format out:
//  DONE
//
//  The last DONE allows the host to eat bytes if target is sending too many.
//
// Mode:
//   MODE_NO_ECHO:  
//       Just receive data and verify CRC.
//   MODE_EOP_ECHO:
//       Receive data, verify CRC, resend data.
//       Expect OK/ER reply from host when done.
//   MODE_DUPLEX_ECHO:
//       Receive data, echo data, verify CRC.
//       Expect OK/ER reply from host when done.
//
// Note:
//   Using diag_printf while talking with the host may cause some funky
//   errors (bytes from the host side being lost!?!?)
//
// To Do:
//   MODE_DUPLEX_ECHO:
//     The current implementation is simple and may not stress the
//     driver enough. Also, it's command packet format doesn't match
//     that of the other modes.
cyg_test_return_t
test_binary(cyg_io_handle_t handle, int size, cyg_mode_t mode)
{
    const char cmd[] = "@BINARY:";
    int msglen;
    cyg_uint32 xcrc;
    int icrc, host_crc;
    cyg_uint8 *p1;
    cyg_int8 host_status = 'O';         // host is happy by default

    // Verify that the test can be run with available ressources.
    if (MODE_EOP_ECHO == mode && size > IN_BUFFER_SIZE)
        return TEST_RETURN_NA;

    // Prepare and send the command.
    p1 = &cmd_buffer[0];
    p1 = strcpy(p1, &cmd[0]);
    p1 = itoa(p1, size);
    *p1++ = ':';
    p1 = itoa(p1, mode);
    *p1++ = '!';
    *p1++ = 0;

    CYG_TEST_INFO(&cmd_buffer[1]);
  
    msglen = strlen(&cmd_buffer[0]);
    Tcyg_io_write(handle, &cmd_buffer[0], &msglen);

    // Get CRC back.
    host_crc = read_host_crc(handle);

    // Depending on mode, start reading data.
    xcrc = 0;
    switch (mode) {
    case MODE_NO_ECHO:
    {
        // Break transfers into chunks no larger than the buffer size.
        int tx_len, chunk_len, i;
        while (size > 0) {
            chunk_len = min(IN_BUFFER_SIZE, size);
            tx_len = chunk_len;
            size -= chunk_len;

            Tcyg_io_read(handle, &in_buffer[0], &chunk_len);

            for (i = 0; i < tx_len; i++) {
                ADD_CRC_BYTE(xcrc, in_buffer[i]);
            }
        }

        // Reply that we have completed the test.
        {
            const char msg_done[] = "DONE";

            chunk_len = strlen(&msg_done[0]);
            Tcyg_io_write(handle, &msg_done[0], &chunk_len);
        }

    }
    break;
    case MODE_EOP_ECHO:
    {
        // We have already checked that the in buffer is large enough.
        int i, tx_len, chunk_len;
        chunk_len = tx_len = size;
        Tcyg_io_read(handle, &in_buffer[0], &chunk_len);

        for (i = 0; i < tx_len; i++) {
            ADD_CRC_BYTE(xcrc, in_buffer[i]);
        }
        
        // Echo data back.
        chunk_len = size;
        Tcyg_io_write(handle, &in_buffer[0], &chunk_len);
        
        // Now read host side's status
        chunk_len = 2;
        Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
        host_status = in_buffer[0];

        // Reply that we have completed the test.
        {
            const char msg_done[] = "DONE";

            chunk_len = strlen(&msg_done[0]);
            Tcyg_io_write(handle, &msg_done[0], &chunk_len);
        }
    }
    break;
    case MODE_DUPLEX_ECHO:
    {
        int chunk_len;
        int block_size = 64;

        // This is a simple implementation (maybe too simple).
        // Host sends 4 packets with the same size (64 bytes atm).
        // Target echoes in this way:
        //  packet1 -> packet1
        //  packet2 -> packet2, packet2
        //  packet3 -> packet3
        //  packet4 -> /dev/null
        //
        // The reads/writes are interleaved in a way that should ensure
        // the target out buffer to be full before the target starts to read
        // packet3. That is, the target should be both receiving (packet3)
        // and sending (packet2) at the same time.

        while (size--) {
            // block_size -> block_size
            chunk_len = block_size;
            Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
            chunk_len = block_size;
            Tcyg_io_write(handle, &in_buffer[0], &chunk_len);

            // block_size -> 2 x block_size
            chunk_len = block_size;
            Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
            chunk_len = block_size;
            Tcyg_io_write(handle, &in_buffer[0], &chunk_len);
            chunk_len = block_size;
            Tcyg_io_write(handle, &in_buffer[0], &chunk_len);

            // block_size -> block_size
            chunk_len = block_size;
            Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
            chunk_len = block_size;
            Tcyg_io_write(handle, &in_buffer[0], &chunk_len);
        
            // block_size -> 0
            chunk_len = block_size;
            Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
        }

        // Kill the CRC. Leave packet verification to the host for now.
        xcrc = host_crc = 0;

        // Now read host side's status
        chunk_len = 2;
        Tcyg_io_read(handle, &in_buffer[0], &chunk_len);
        host_status = in_buffer[0];

        // Reply that we have completed the test.
        {
            const char msg_done[] = "DONE";

            chunk_len = strlen(&msg_done[0]);
            Tcyg_io_write(handle, &msg_done[0], &chunk_len);
        }
    }
    break;
    default:
        CYG_TEST_CHECK(0, "unknown mode");
    }


    // Verify that the CRC matches the one from the host.
    FIX_CRC(xcrc, icrc);
#ifdef __DEVELOPER__
    if (host_crc != icrc)
        diag_printf("%d != %d\n", icrc, host_crc);
#endif
    CYG_TEST_CHECK(host_crc == icrc, "CRC failed!");

    // Verify that the host is happy with the data we echoed.
    CYG_TEST_CHECK('O' == host_status, "Host failed checksum on echoed data");

    CYG_TEST_PASS("Binary test completed");
    return TEST_RETURN_OK;
}

//---------------------------------------------------------------------------
// Test transformations on text transmissions
// Format out:
//  "@TEXT:<mode>!<4 bytes binary checksum><C string>"
// Format in:
//  "<C string>"
//  OK/ER
//
// Mode:
//   MODE_EOP_ECHO:
//       Receive data, verify CRC, resend data.
//       Expect OK/ER reply from host when done.
//   MODE_DUPLEX_ECHO:
//       Receive data, echo data, verify CRC.
//       Expect OK/ER reply from host when done.
//
cyg_test_return_t
test_text(cyg_io_handle_t handle, cyg_mode_t mode, const char* s_base,
           const char* s_out, const char* s_in)
{
    return TEST_RETURN_NA;
}

//---------------------------------------------------------------------------
// Send PING to host, verifying the filter's presence.
// Format out:
//  "@PING!"
// Format in:
//  "OK"
// or
//  No response if directly connected to GDB.
//
// This call only returns if the ser_filter is listening. Otherwise it
// sends N/A and hangs.
void
test_ping(cyg_io_handle_t handle)
{
    char msg[] = "@PING:!";
    char msg2[] = "\n";
    int msglen = strlen(msg);
    int res;

    msglen = strlen(msg);
    Tcyg_io_write(handle, msg, &msglen);

    // Now read host side's status
    msglen = 2;
    Tcyg_io_read_timeout(handle, &in_buffer[0], &msglen, 100, &res);
    if (ENOERR == res && 'O' == in_buffer[0] && 'K' == in_buffer[1])
        return;

    msglen = strlen(msg2);
    Tcyg_io_write(handle, msg2, &msglen);

    CYG_TEST_NA("No host side testing harness detected.");
}


//---------------------------------------------------------------------------
// Send OPT to host, setting options in the filter.
// Format out:
//  "@OPT:option1,value1:...:optionN,valueN!"
// Format in:
//  "OK"
//
// Only integer values can be used. Any option not recognized by the
// filter will be silently ignored.
void
test_options(cyg_io_handle_t handle, int count, cyg_uint32* options)
{
    const char cmd[] = "@OPT:";
    int msglen;
    cyg_uint8 *p1;

    // Prepare and send the command.
    p1 = &cmd_buffer[0];
    p1 = strcpy(p1, &cmd[0]);
    while(count--) {
        p1 = itoa(p1, *options++);      // option
        *p1++ = ':';
        p1 = itoa(p1, *options++);      // value
        *p1++ = ':';
    }
    *(p1-1) = '!';
    *p1++ = 0;

    CYG_TEST_INFO(&cmd_buffer[1]);
  
    msglen = strlen(&cmd_buffer[0]);
    Tcyg_io_write(handle, &cmd_buffer[0], &msglen);

    // Now read host side's status
    msglen = 2;
    Tcyg_io_read(handle, &in_buffer[0], &msglen);
}


//---------------------------------------------------------------------------
// Some helper functions to get a test started.
void
test_open_ser( cyg_io_handle_t* handle )
{
#ifndef SER_NOP_TEST
    Cyg_ErrNo res;

    if (cyg_test_is_simulator)
        CYG_TEST_NA("Cannot run from simulator");

#if defined(HAL_VSR_SET_TO_ECOS_HANDLER)
# if defined(SER_OVERRIDE_INT_1)
    HAL_VSR_SET_TO_ECOS_HANDLER(SER_OVERRIDE_INT_1, NULL);
# endif
# if defined(SER_OVERRIDE_INT_2)
    HAL_VSR_SET_TO_ECOS_HANDLER(SER_OVERRIDE_INT_2, NULL);
# endif
#endif

    res = cyg_io_lookup(TEST_SER_DEV, handle);
    if (res != ENOERR) {
        CYG_TEST_FAIL_FINISH("Can't lookup " TEST_SER_DEV);
    }
#else
    CYG_TEST_NA(NA_MSG);
#endif
}

void
test_open_tty( cyg_io_handle_t* handle )
{
#ifndef TTY_NOP_TEST
    Cyg_ErrNo res;

    if (cyg_test_is_simulator)
        CYG_TEST_NA("Cannot run from simulator");

#if defined(HAL_VSR_SET_TO_ECOS_HANDLER)
# if defined(SER_OVERRIDE_INT_1)
    HAL_VSR_SET_TO_ECOS_HANDLER(SER_OVERRIDE_INT_1, NULL);
# endif
# if defined(SER_OVERRIDE_INT_2)
    HAL_VSR_SET_TO_ECOS_HANDLER(SER_OVERRIDE_INT_2, NULL);
# endif
#endif

    res = cyg_io_lookup(TEST_TTY_DEV, handle);
    if (res != ENOERR) {
        CYG_TEST_FAIL_FINISH("Can't lookup " TEST_TTY_DEV);
    }
#else
    CYG_TEST_NA(NA_MSG);
#endif
}

//---------------------------------------------------------------------------
// end of ser_test_protocol.inl
