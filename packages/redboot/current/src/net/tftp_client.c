//==========================================================================
//
//      net/tftp_client.c
//
//      Stand-alone TFTP support for RedBoot
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

// TFTP client support

#include <redboot.h>     // have_net
#include <net/net.h>
#include <net/tftp.h>
#include <net/tftp_support.h>

//
// Read a file from a host into a local buffer.  Returns the
// number of bytes actually read, or (-1) if an error occurs.
// On error, *err will hold the reason.
//
int
tftp_get(char *filename,
         struct sockaddr_in *server,
         char *buf,
         int len,
         int mode,
         int *err)
{
    int res = 0;
    int actual_len, data_len, recv_len;
    static int get_port = 7700;
    struct sockaddr_in local_addr, from_addr;
    char data[SEGSIZE+sizeof(struct tftphdr)];
    struct tftphdr *hdr = (struct tftphdr *)data;
    char *cp, *fp;
    struct timeval timeout;
    int last_good_block = 0;
    int total_timeouts = 0;

    *err = 0;  // Just in case

    // Create initial request
    hdr->th_opcode = htons(RRQ);  // Read file
    cp = (char *)&hdr->th_stuff;
    fp = filename;
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';
    if (mode == TFTP_NETASCII) {
        fp = "NETASCII";
    } else if (mode == TFTP_OCTET) {
        fp = "OCTET";
    } else {
        *err = TFTP_INVALID;
        return -1;
    }
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';

    memset((char *)&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(get_port++);

    if (server->sin_port == 0) {
        server->sin_port = htons(TFTP_PORT);
    }

    // Send request
    if (__udp_sendto(data, sizeof(data), server, &local_addr) < 0) {
        // Problem sending request
        *err = TFTP_NETERR;
        return -1;
    }

    // Read data
    fp = buf;
    while (true) {
        timeout.tv_sec = TFTP_TIMEOUT_PERIOD;
        timeout.tv_usec = 0;
        recv_len = sizeof(data);
        if ((data_len = __udp_recvfrom(&data[0], recv_len, &from_addr, &local_addr,  &timeout)) < 0) {
            // No data, try again
            if ((++total_timeouts > TFTP_TIMEOUT_MAX) || (last_good_block == 0)) {
                // Timeout - no data received
                *err = TFTP_TIMEOUT;
                return -1;
            }
            // Try resending last ACK
            hdr->th_opcode = htons(ACK);
            hdr->th_block = htons(last_good_block);
            if (__udp_sendto(data, 4 /* FIXME */, &from_addr, &local_addr) < 0) {
                // Problem sending request
                *err = TFTP_NETERR;
                return -1;
            }
        } else {
            if (ntohs(hdr->th_opcode) == DATA) {
                actual_len = 0;
                if (ntohs(hdr->th_block) == (last_good_block+1)) {
                    // Consume this data
                    cp = hdr->th_data;
                    data_len -= 4;  /* Sizeof TFTP header */
                    actual_len = data_len;
                    res += actual_len;
                    while (data_len-- > 0) {
                        if (len-- > 0) {
                            *fp++ = *cp++;
                        } else {
                            // Buffer overflow
                            *err = TFTP_TOOLARGE;
                            return -1;
                        }
                    }
                    last_good_block++;
                }
                // Send out the ACK
                hdr->th_opcode = htons(ACK);
                hdr->th_block = htons(last_good_block);
                if (__udp_sendto(data, 4 /* FIXME */, &from_addr, &local_addr) < 0) {
                    // Problem sending ACK
                    *err = TFTP_NETERR;
                    return -1;
                }
                if ((actual_len >= 0) && (actual_len < SEGSIZE)) {
                    // End of data
                    return res;
                }
            } else 
            if (ntohs(hdr->th_opcode) == ERROR) {
                *err = ntohs(hdr->th_code);
                return -1;
            } else {
                // What kind of packet is this?
                *err = TFTP_PROTOCOL;
                return -1;
            }
        }
    }
}

static struct {
    bool open;
    int  total_timeouts;
    int  last_good_block;
    int  avail, actual_len;
    struct sockaddr_in local_addr, from_addr;
    char data[SEGSIZE+sizeof(struct tftphdr)];
    char *bufp;
} tftp_stream;

int
tftp_stream_open(char *filename,
                 struct sockaddr_in *server,
                 int mode,
                 int *err)
{
    struct tftphdr *hdr = (struct tftphdr *)tftp_stream.data;
    char *cp, *fp;
    static int get_port = 7700;
    char test_buf;

    if (!have_net || tftp_stream.open) {
        *err = TFTP_INVALID;  // Already open
        return -1;
    }

    // Create initial request
    hdr->th_opcode = htons(RRQ);  // Read file
    cp = (char *)&hdr->th_stuff;
    fp = filename;
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';
    if (mode == TFTP_NETASCII) {
        fp = "NETASCII";
    } else if (mode == TFTP_OCTET) {
        fp = "OCTET";
    } else {
        *err = TFTP_INVALID;
        return -1;
    }
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';

    memset((char *)&tftp_stream.local_addr, 0, sizeof(tftp_stream.local_addr));
    tftp_stream.local_addr.sin_family = AF_INET;
    tftp_stream.local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    tftp_stream.local_addr.sin_port = htons(get_port++);

    if (server->sin_port == 0) {
        server->sin_port = htons(TFTP_PORT);
    }

    // Send request
    if (__udp_sendto(tftp_stream.data, sizeof(tftp_stream.data), 
                     server, &tftp_stream.local_addr) < 0) {
        // Problem sending request
        *err = TFTP_NETERR;
        return -1;
    }

    tftp_stream.open = true;
    tftp_stream.avail = 0;
    tftp_stream.actual_len = -1;
    tftp_stream.last_good_block = 0;
    tftp_stream.total_timeouts = 0;

    // Try and read the first byte [block] since no errors are
    // reported until then.
    if (tftp_stream_read(&test_buf, 1, err) == 1) {
        // Back up [rewind] over this datum
        tftp_stream.bufp--;
        tftp_stream.avail++;
        return 0;  // Open and first read successful
    } else {
        tftp_stream.open = false;
        return -1; // Couldn't read
    }
}

int
tftp_stream_close(int *err)
{
    tftp_stream.open = false;
    return 0;
}

int
tftp_stream_read(char *buf,
                 int len,
                 int *err)
{
    int total_bytes = 0;
    int size, recv_len, data_len;
    struct timeval timeout;
    struct tftphdr *hdr = (struct tftphdr *)tftp_stream.data;

    while (total_bytes < len) {
        // Move any bytes which we've already read/buffered
        if (tftp_stream.avail > 0) {
            size = tftp_stream.avail;
            if (size > (len - total_bytes)) size = len - total_bytes;
            memcpy(buf, tftp_stream.bufp, size);
            buf += size;
            tftp_stream.bufp += size;
            tftp_stream.avail -= size;
            total_bytes += size;
        } else {
            if ((tftp_stream.actual_len >= 0) && (tftp_stream.actual_len < SEGSIZE)) {
                // Out of data
                break;
            }
            timeout.tv_sec = TFTP_TIMEOUT_PERIOD;
            timeout.tv_usec = 0;
            recv_len = sizeof(tftp_stream.data);
            if ((data_len = __udp_recvfrom(&tftp_stream.data[0], recv_len, &tftp_stream.from_addr, 
                                           &tftp_stream.local_addr,  &timeout)) < 0) {
                // No data, try again
                if ((++tftp_stream.total_timeouts > TFTP_TIMEOUT_MAX) || 
                    (tftp_stream.last_good_block == 0)) {
                    // Timeout - no data received
                    *err = TFTP_TIMEOUT;
                    return -1;
                }
                // Try resending last ACK
                hdr->th_opcode = htons(ACK);
                hdr->th_block = htons(tftp_stream.last_good_block);
                if (__udp_sendto(tftp_stream.data, 4 /* FIXME */,
                                 &tftp_stream.from_addr, &tftp_stream.local_addr) < 0) {
                    // Problem sending request
                    *err = TFTP_NETERR;
                    return -1;
                }
            } else {
                if (ntohs(hdr->th_opcode) == DATA) {
                    if (ntohs(hdr->th_block) == (tftp_stream.last_good_block+1)) {
                        // Consume this data
                        data_len -= 4;  /* Sizeof TFTP header */
                        tftp_stream.avail = tftp_stream.actual_len = data_len;
                        tftp_stream.bufp = hdr->th_data;
                        tftp_stream.last_good_block++;
                    }
                    // Send out the ACK
                    hdr->th_opcode = htons(ACK);
                    hdr->th_block = htons(tftp_stream.last_good_block);
                    if (__udp_sendto(tftp_stream.data, 4 /* FIXME */, 
                                     &tftp_stream.from_addr, &tftp_stream.local_addr) < 0) {
                        // Problem sending ACK
                        *err = TFTP_NETERR;
                        return -1;
                    }
                } else {
                    if (ntohs(hdr->th_opcode) == ERROR) {
                        *err = ntohs(hdr->th_code);
                        return -1;
                    } else {
                        // What kind of packet is this?
                        *err = TFTP_PROTOCOL;
                        return -1;
                    }
                }
            }
        }
    }
    return total_bytes;
}

char *
tftp_error(int err)
{
    char *errmsg = "Unknown error";

    switch (err) {
    case TFTP_ENOTFOUND:
        return "file not found";
    case TFTP_EACCESS:
        return "access violation";
    case TFTP_ENOSPACE:
        return "disk full or allocation exceeded";
    case TFTP_EBADOP:
        return "illegal TFTP operation";
    case TFTP_EBADID:
        return "unknown transfer ID";
    case TFTP_EEXISTS:
        return "file already exists";
    case TFTP_ENOUSER:
        return "no such user";
    case TFTP_TIMEOUT:
        return "operation timed out";
    case TFTP_NETERR:
        return "some sort of network error";
    case TFTP_INVALID:
        return "invalid parameter";
    case TFTP_PROTOCOL:
        return "protocol violation";
    case TFTP_TOOLARGE:
        return "file is larger than buffer";
    }
    return errmsg;
}
