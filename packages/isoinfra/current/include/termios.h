#ifndef CYGONCE_ISO_TERMIOS_H
#define CYGONCE_ISO_TERMIOS_H
/* ====================================================================
//
//      termios.h
//
//      POSIX termios
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
// Purpose:      POSIX termios support
// Description:
//
//####DESCRIPTIONEND####
//
// ==================================================================*/

#include <pkgconf/isoinfra.h>

#if CYGINT_ISO_TERMIOS
# ifdef CYGBLD_ISO_TERMIOS_HEADER
#  include CYGBLD_ISO_TERMIOS_HEADER
# else

/* TYPES */

typedef unsigned int tcflag_t;  /* terminal flags type */
typedef unsigned char cc_t;     /* control chars type */
typedef unsigned int speed_t;   /* baud rate type */

#define NCCS 16    /* May as well hard-code - ASCII isn't that configurable! */

struct termios {
    tcflag_t c_iflag;    /* Input mode flags */
    tcflag_t c_oflag;    /* Output mode flags */
    tcflag_t c_cflag;    /* Control mode flags */
    tcflag_t c_lflag;    /* Local mode flags */
    cc_t c_cc[NCCS];     /* Control characters */
    speed_t c_ispeed;    /* input speed */
    speed_t c_ospeed;    /* output speed */
};

/* CONSTANTS */

/* Input mode flags */

#define BRKINT          (1<<0)
#define ICRNL           (1<<1)
#define IGNBRK          (1<<2)
#define IGNCR           (1<<3)
#define IGNPAR          (1<<4)
#define INLCR           (1<<5)
#define INPCK           (1<<6)
#define ISTRIP          (1<<7)
#define IXOFF           (1<<8)
#define IXON            (1<<9)
#define PARMRK          (1<<10)

/* Output mode flags */

#define OPOST           (1<<0)
#define ONLCR           (1<<1) /* Note: This isn't POSIX */

/* Control mode flags */

#define CLOCAL          (1<<0)
#define CREAD           (1<<1)
#define   CS5              (0)
#define   CS6           (1<<2)
#define   CS7           (1<<3)
#define   CS8           (CS6|CS7)
#define CSIZE           (CS8)
#define CSTOPB          (1<<4)
#define HUPCL           (1<<5)
#define PARENB          (1<<6)
#define PARODD          (1<<7)
#ifndef _POSIX_SOURCE_
# define CRTSCTS        (1<<8)
#endif

/* Local mode flags */

#define ECHO            (1<<0)
#define ECHOE           (1<<1)
#define ECHOK           (1<<2)
#define ECHONL          (1<<3)
#define ICANON          (1<<4)
#define IEXTEN          (1<<5)
#define ISIG            (1<<6)
#define NOFLSH          (1<<7)
#define TOSTOP          (1<<8)

/* Special control characters */

#define VEOF            0
#define VEOL            1
#define VERASE          2
#define VINTR           3
#define VKILL           4
#define VMIN            5
#define VQUIT           6
#define VSUSP           7
#define VTIME           8
#define VSTART          9
#define VSTOP           10

/* Baud rates */
/* There may be tables in the implementation that rely on the
 * values here, so only append to this table - do not insert values!
 */
#define B0              0
#define B50             1
#define B75             2
#define B110            3
#define B134            4
#define B150            5
#define B200            6
#define B300            7
#define B600            8
#define B1200           9
#define B1800           10
#define B2400           11
#define B3600           12
#define B4800           13
#define B7200           14
#define B9600           15
#define B14400          16
#define B19200          17
#define B38400          18
#define B57600          19
#define B115200         20
#define B230400         21
#define B460800         22
#define B500000         23
#define B576000         24
#define B921600         25
#define B1000000        26
#define B1152000        27
#define B1500000        28
#define B2000000        29
#define B2500000        30
#define B3000000        31
#define B3500000        32
#define B4000000        33


/* Optional actions to tcsetattr() */

#define TCSANOW         0
#define TCSADRAIN       1
#define TCSAFLUSH       2

/* Queue selectors for tcflush() */

#define TCIFLUSH        0
#define TCOFLUSH        1
#define TCIOFLUSH       2

/* Actions for tcflow() */

#define TCOOFF          0
#define TCOON           1
#define TCIOFF          2
#define TCION           3


/* FUNCTIONS */

#ifdef __cplusplus
extern "C" {
#endif

extern speed_t
cfgetospeed( const struct termios *__termios_p );

extern int
cfsetospeed( struct termios *__termios_p, speed_t __speed );

extern speed_t
cfgetispeed( const struct termios *__termios_p );

extern int
cfsetispeed( struct termios *__termios_p, speed_t __speed );

extern int
tcgetattr( int __fildes, struct termios *__termios_p );

extern int
tcsetattr( int __fildes, int __optact, const struct termios *__termios_p );

extern int
tcsendbreak( int __fildes, int __duration );

extern int
tcdrain( int __fildes );

extern int
tcflush( int __fildes, int __queue_sel );

extern int
tcflow( int __fildes, int __action );

/* tcgetpgrp() and tcsetpgrp() not included in the absence of job control */

#ifdef __cplusplus
} /* extern "C" */
#endif

# endif /* ifndef CYGBLD_ISO_TERMIOS_HEADER */
#endif /* if CYGINT_ISO_TERMIOS */


#endif /* ifndef CYGONCE_ISO_TERMIOS_H */

/* EOF termios.h */
