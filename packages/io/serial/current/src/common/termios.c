/* ====================================================================
//
//      termios.c
//
//      POSIX termios API implementation
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-07-22
// Purpose:      POSIX termios API support
// Description:  Most of the real work happens in the POSIX termios tty
//               drivers
//
//####DESCRIPTIONEND####
//
// ==================================================================*/

// CONFIGURATION

#include <pkgconf/io_serial.h>

#ifdef CYGPKG_IO_SERIAL_TERMIOS

// INCLUDES

#include <termios.h>             // Header for this file
#include <cyg/infra/cyg_type.h>  // Common stuff
#include <cyg/infra/cyg_ass.h>   // Assertion support
#include <cyg/infra/cyg_trac.h>  // Tracing support
#include <cyg/io/serialio.h>     // eCos serial implementation
#include <cyg/fileio/fileio.h>   // file operations
#include <cyg/io/io.h>
#include <errno.h>               // errno
#include <unistd.h>              // isatty()

// TYPES

typedef struct {
    const struct termios *termios_p;
    int optact;
} setattr_struct;

// FUNCTIONS

extern speed_t
cfgetospeed( const struct termios *termios_p )
{
    CYG_REPORT_FUNCTYPE( "returning speed code %d" );
    CYG_CHECK_DATA_PTRC( termios_p );
    CYG_REPORT_FUNCARG1XV( termios_p );
    CYG_REPORT_RETVAL( termios_p->c_ospeed );
    return termios_p->c_ospeed;
} // cfgetospeed()


extern int
cfsetospeed( struct termios *termios_p, speed_t speed )
{
    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_CHECK_DATA_PTRC( termios_p );
    CYG_REPORT_FUNCARG2( "termios_p=%08x, speed=%d", termios_p, speed );
    CYG_REPORT_RETVAL( termios_p->c_ospeed );
    if ( speed > B4000000 ) {
        errno = EINVAL;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    termios_p->c_ospeed = speed;
    CYG_REPORT_RETVAL( 0 );
    return 0;
} // cfsetospeed()


extern speed_t
cfgetispeed( const struct termios *termios_p )
{
    CYG_REPORT_FUNCTYPE( "returning speed code %d" );
    CYG_CHECK_DATA_PTRC( termios_p );
    CYG_REPORT_FUNCARG1XV( termios_p );
    CYG_REPORT_RETVAL( termios_p->c_ispeed );
    return termios_p->c_ispeed;
} // cfgetispeed()


extern int
cfsetispeed( struct termios *termios_p, speed_t speed )
{
    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_CHECK_DATA_PTRC( termios_p );
    CYG_REPORT_FUNCARG2( "termios_p=%08x, speed=%d", termios_p, speed );
    if ( speed > B115200 ) {
        errno = EINVAL;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    termios_p->c_ispeed = speed;
    CYG_REPORT_RETVAL( 0 );
    return 0;
} // cfsetispeed()


__externC cyg_file *
cyg_fp_get( int fd );

__externC void
cyg_fp_free( cyg_file *fp );

extern int
tcgetattr( int fildes, struct termios *termios_p )
{
    cyg_file *fp;
    int ret;
    int len = sizeof( *termios_p );

    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_REPORT_FUNCARG2( "fildes=%d, termios_p=%08x", fildes, termios_p );
    CYG_CHECK_DATA_PTRC( termios_p );

    if ( !isatty(fildes) ) {
        errno = ENOTTY;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
        
    fp = cyg_fp_get( fildes );

    if ( NULL == fp ) {
        errno = EBADF;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    ret = fp->f_ops->fo_getinfo( fp, CYG_IO_GET_CONFIG_TERMIOS, termios_p,
                                 len);
    cyg_fp_free( fp );

    if ( ret > 0 ) {
        errno = ret;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    CYG_REPORT_RETVAL( 0 );
    return 0;        
} // tcgetattr()


extern int
tcsetattr( int fildes, int optact, const struct termios *termios_p )
{
    cyg_file *fp;
    int ret;
    setattr_struct attr;
    int len = sizeof( attr );

    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_REPORT_FUNCARG3( "fildes=%d, optact=%d, termios_p=%08x",
                         fildes, optact, termios_p );
    CYG_CHECK_DATA_PTRC( termios_p );

    if ( !isatty(fildes) ) {
        errno = ENOTTY;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    if ( (optact != TCSANOW) && (optact != TCSADRAIN) &&
         (optact != TCSAFLUSH) ) {
        errno = EINVAL;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
        
    fp = cyg_fp_get( fildes );

    if ( NULL == fp ) {
        errno = EBADF;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    attr.termios_p = termios_p;
    attr.optact = optact;

    ret = fp->f_ops->fo_setinfo( fp, CYG_IO_SET_CONFIG_TERMIOS, &attr,
                                 len);

    cyg_fp_free( fp );

    if ( ret > 0 ) {
        errno = ret;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    CYG_REPORT_RETVAL( 0 );
    return 0;        
} // tcsetattr()


extern int
tcsendbreak( int fildes, int duration )
{
    // FIXME
    return -EINVAL;
} // tcsendbreak()

extern int
tcdrain( int fildes )
{
    cyg_file *fp;
    int ret;

    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_REPORT_FUNCARG1DV( fildes );

    if ( !isatty(fildes) ) {
        errno = ENOTTY;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    fp = cyg_fp_get( fildes );

    if ( NULL == fp ) {
        errno = EBADF;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    ret = fp->f_ops->fo_getinfo( fp,
                                 CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN,
                                 NULL, 0 );
    cyg_fp_free( fp );

    if ( ret > 0 ) {
        errno = ret;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    CYG_REPORT_RETVAL( 0 );
    return 0;        
} // tcdrain()

extern int
tcflush( int fildes, int queue_sel )
{
    cyg_file *fp;
    int ret;

    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_REPORT_FUNCARG2DV( fildes, queue_sel );

    if ( !isatty(fildes) ) {
        errno = ENOTTY;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    fp = cyg_fp_get( fildes );

    if ( NULL == fp ) {
        errno = EBADF;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    switch( queue_sel ) {
    case TCIOFLUSH:
        ret = fp->f_ops->fo_getinfo( fp,
                                     CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH,
                                     NULL, 0 );
        // fallthrough
    case TCIFLUSH:
        ret = fp->f_ops->fo_getinfo( fp,
                                     CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH,
                                     NULL, 0 );
        break;
    case TCOFLUSH:
        ret = fp->f_ops->fo_getinfo( fp,
                                     CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH,
                                     NULL, 0 );
        break;
    default:
        ret = EINVAL;
        break;
    }

    cyg_fp_free( fp );

    if ( ret > 0 ) {
        errno = ret;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    CYG_REPORT_RETVAL( 0 );
    return 0;        
} // tcflush()

extern int
tcflow( int fildes, int action )
{
    cyg_file *fp;
    int ret;
    cyg_uint32 forcearg;
    int len = sizeof(forcearg);

    CYG_REPORT_FUNCTYPE( "returning %d" );
    CYG_REPORT_FUNCARG2DV( fildes, action );

    if ( !isatty(fildes) ) {
        errno = ENOTTY;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    fp = cyg_fp_get( fildes );

    if ( NULL == fp ) {
        errno = EBADF;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }

    switch( action ) {
    case TCOOFF:
        forcearg = CYGNUM_SERIAL_FLOW_THROTTLE_TX;
        ret = fp->f_ops->fo_setinfo( fp,
                                    CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE,
                                     &forcearg, len );
        break;
    case TCOON:
        forcearg = CYGNUM_SERIAL_FLOW_RESTART_TX;
        ret = fp->f_ops->fo_setinfo( fp,
                                    CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE,
                                     &forcearg, len );
        break;
    case TCIOFF:
        forcearg = CYGNUM_SERIAL_FLOW_THROTTLE_RX;
        ret = fp->f_ops->fo_setinfo( fp,
                                    CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE,
                                     &forcearg, len );
        break;
    case TCION:
        forcearg = CYGNUM_SERIAL_FLOW_RESTART_RX;
        ret = fp->f_ops->fo_setinfo( fp,
                                    CYG_IO_SET_CONFIG_SERIAL_FLOW_CONTROL_FORCE,
                                     &forcearg, len );
        break;
    default:
        ret = EINVAL;
        break;
    }

    cyg_fp_free( fp );

    if ( ret > 0 ) {
        errno = ret;
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    CYG_REPORT_RETVAL( 0 );
    return 0;        
} // tcflow()

#endif // ifdef CYGPKG_IO_SERIAL_TERMIOS

// EOF termios.c
