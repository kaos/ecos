//==========================================================================
//
//        io.c
//
//        Linux I/O support for Cirrus Logic EDB7xxx eval board tools
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
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1999-06-16
// Description:   Linux I/O support for Cirrus Logic EDB7xxx FLASH tools
//####DESCRIPTIONEND####

//
// io.c - I/O functions
//

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

char ReceiveChar(long port)
{
    char buf;
    int res;
    // Port is in non-blocking mode, this needs to have a loop
    do {
        res = read(port, &buf, 1);
    } while (res < 0);
//    printf("Read: %c\n", buf);
    return buf;
}

static void
uspin(int len)
{
    volatile int cnt;
    while (--len >= 0) {
        for (cnt = 1;  cnt < 2000;  cnt++) ;
    }
}

void SendChar(long port, char ch)
{
    char buf = ch;
    write(port, &buf, 1);
//    printf("Send: %x\n", ch);
//    usleep(100);
    uspin(100);
}

void SetBaud(long port, long reqRate)
{
    struct termios buf;
    int rate;

    // Get original configuration
    if (tcgetattr(port, &buf)) {
        fprintf(stderr, "Can't get port config\n");
        return;
    }

    // Reset to raw.
    buf.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                     |INLCR|IGNCR|ICRNL|IXON);
    buf.c_oflag &= ~OPOST;
    buf.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    buf.c_cflag &= ~(CSIZE|PARENB);
    buf.c_cflag |= CS8;

    // Set baud rate.
    switch (reqRate) {
    case 9600:
        rate = B9600;
        break;
    case 38400:
        rate = B38400;
        break;
    case 57600:
        rate = B57600;
        break;
    case 115200:
        rate = B115200;
        break;
    }
    cfsetispeed(&buf, rate);
    cfsetospeed(&buf, rate);

    // Set data bits.
    buf.c_cflag &= ~CSIZE;
    buf.c_cflag |= CS8;

    // Set stop bits.
    buf.c_cflag &= ~CSTOPB;  // One stop

    // Set parity.
    buf.c_cflag &= ~(PARENB | PARODD); // no parity.
    
    // Set the new settings
    if (tcsetattr(port, TCSADRAIN, &buf)) {
	fprintf(stderr, "Error: tcsetattr\n");
	return;
    }

    tcdrain(port);
    usleep(1000000/10*2);
}

int CharReady(long port)
{
#ifdef __CYGWIN__
    // Windows doesn't support the below ioctl
    return 1;
#else
    int n, res;
    res = ioctl(port, FIONREAD, &n);
    return n;  // Non-zero if characters ready to read
#endif
}

void WaitForOutputEmpty(long port)
{
    usleep(2000000);
}

long OpenPort(char *name)
{
    int fd;
    fd = open(name, O_RDWR|O_NONBLOCK);
    return fd;
}
