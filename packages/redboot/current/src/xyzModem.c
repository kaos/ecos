//==========================================================================
//
//      xyzModem.c
//
//      RedBoot stream handler for xyzModem protocol
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

#include <redboot.h>
#include <xyzModem.h>

// Assumption - run xyzModem protocol over the console port

// Values magic to the protocol
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

// Data & state local to the protocol
static struct {
    hal_virtual_comm_table_t* __chan;
    unsigned char pkt[1024], *bufp;
    unsigned char blk,cblk,crc1,crc2;
    unsigned char next_blk;  // Expected block
    int len, mode, total_retries;
    int total_SOH, total_STX, total_CAN;
    bool crc_mode, at_eof;
} xyz;

#define xyzModem_CHAR_TIMEOUT            2000  // 2 seconds
#define xyzModem_MAX_RETRIES             20
#define xyzModem_MAX_RETRIES_WITH_CRC    10

// Table of CRC constants - implements x^16+x^12+x^5+1
static unsigned short crc16[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};

#ifdef DEBUG

//
// Note: this debug setup only works if the target platform has two serial ports
// available so that the other one (currently only port 1) can be used for debug
// messages.
//
void
zm_dprintf(char *fmt, ...)
{
    int cur_console;
    va_list args;

    va_start(args, fmt);
    cur_console = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);
    vprintf(fmt, args);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur_console);
}

void
zm_dump_buf(void *buf, int len)
{
    int cur_console;

    cur_console = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(1);
    dump_buf(buf, len);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(cur_console);
}

static unsigned char zm_buf[2048];
static unsigned char *zm_bp;

void
zm_new(void)
{
    zm_bp = zm_buf;
}

void
zm_save(unsigned char c)
{
    *zm_bp++ = c;
}

void
zm_dump(int line)
{
    zm_dprintf("Packet at line: %d\n", line);
    zm_dump_buf(zm_buf, zm_bp-zm_buf);
}

#define ZM_DEBUG(x) x
#else
#define ZM_DEBUG(x)
#endif

// Wait for the line to go idle
static void
xyzModem_flush(void)
{
    int res;
    char c;
    while (true) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        if (!res) return;
    }
}

static int
xyzModem_get_hdr(void)
{
    char c;
    int res;
    bool hdr_found = false;
    int i;
    unsigned short cksum;

    ZM_DEBUG(zm_new());
    // Find the start of a header
    while (!hdr_found) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        ZM_DEBUG(zm_save(c));
        if (res) {
            switch (c) {
            case SOH:
                xyz.total_SOH++;
            case STX:
                if (c == STX) xyz.total_STX++;
                hdr_found = true;
                break;
            case CAN:
                xyz.total_CAN++;
                ZM_DEBUG(zm_dump(__LINE__));
                return xyzModem_cancel;
            case EOT:
                ZM_DEBUG(zm_dump(__LINE__));
                return xyzModem_eof;
            default:
                // Ignore, waiting for start of header
            }
        } else {
            // Data stream timed out
            return xyzModem_timeout;
        }
    }

    // Header found, now read the data
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.blk);
    ZM_DEBUG(zm_save(xyz.blk));
    if (!res) {
        return xyzModem_timeout;
    }
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.cblk);
    ZM_DEBUG(zm_save(xyz.cblk));
    if (!res) {
        return xyzModem_timeout;
    }
    xyz.len = (c == SOH) ? 128 : 1024;
    xyz.bufp = xyz.pkt;
    for (i = 0;  i < xyz.len;  i++) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &c);
        ZM_DEBUG(zm_save(c));
        if (res) {
            xyz.pkt[i] = c;
        } else {
            return xyzModem_timeout;
        }
    }
    res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.crc1);
    ZM_DEBUG(zm_save(xyz.crc1));
    if (!res) {
        return xyzModem_timeout;
    }
    if (xyz.crc_mode) {
        res = CYGACC_COMM_IF_GETC_TIMEOUT(*xyz.__chan, &xyz.crc2);
        ZM_DEBUG(zm_save(xyz.crc2));
        if (!res) {
            return xyzModem_timeout;
        }
    }
    ZM_DEBUG(zm_dump(__LINE__));
    // Validate the message
    if ((xyz.blk ^ xyz.cblk) != (unsigned char)0xFF) {
        ZM_DEBUG(zm_dprintf("Framing error - blk: %x/%x/%x\n", xyz.blk, xyz.cblk, (xyz.blk ^ xyz.cblk)));
        ZM_DEBUG(zm_dump_buf(xyz.pkt, xyz.len));
        xyzModem_flush();
        return xyzModem_frame;
    }
    // Verify checksum/CRC
    if (xyz.crc_mode) {
        cksum = 0;
        for (i = 0;  i < xyz.len;  i++) {
            cksum = crc16[((cksum>>8) ^ xyz.pkt[i]) & 0xFF] ^ (cksum << 8);
        }
        if (cksum != ((xyz.crc1 << 8) | xyz.crc2)) {
            ZM_DEBUG(zm_dprintf("CRC error - recvd: %02x%02x, computed: %x\n", 
                                xyz.crc1, xyz.crc2, cksum & 0xFFFF));
            return xyzModem_cksum;
        }
    } else {
        cksum = 0;
        for (i = 0;  i < xyz.len;  i++) {
            cksum += xyz.pkt[i];
        }
        if (xyz.crc1 != (cksum & 0xFF)) {
            ZM_DEBUG(zm_dprintf("Checksum error - recvd: %x, computed: %x\n", xyz.crc1, cksum & 0xFF));
            return xyzModem_cksum;
        }
    }
    // If we get here, the message passes [structural] muster
    return 0;
}

int 
xyzModem_stream_open(char *filename, int mode, int *err)
{
    int console_chan, stat;
    int retries = xyzModem_MAX_RETRIES;
    int crc_retries = xyzModem_MAX_RETRIES_WITH_CRC;

    if (mode == xyzModem_zmodem) {
        *err = xyzModem_noZmodem;
        return -1;
    }

    // Set up the I/O channel.  Note: this allows for using a different port in the future
    console_chan = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    CYGACC_CALL_IF_SET_CONSOLE_COMM(console_chan);
    xyz.__chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    CYGACC_CALL_IF_SET_CONSOLE_COMM(console_chan);
    CYGACC_COMM_IF_CONTROL(*xyz.__chan, __COMMCTL_SET_TIMEOUT, xyzModem_CHAR_TIMEOUT);
    xyz.len = 0;
    xyz.crc_mode = true;
    xyz.at_eof = false;
    xyz.mode = mode;
    xyz.total_retries = 0;
    xyz.total_SOH = 0;
    xyz.total_STX = 0;
    xyz.total_CAN = 0;

    while (retries-- > 0) {
        stat = xyzModem_get_hdr();
        if (stat == 0) {
            if (xyz.blk == 0) {
                // Note: yModem file name data blocks quietly discarded
                CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
            }
            xyz.next_blk = 1;
            xyz.len = 0;
            return 0;
        } else 
        if (stat == xyzModem_timeout) {
            if (--crc_retries <= 0) xyz.crc_mode = false;
            CYGACC_CALL_IF_DELAY_US(5*100000);   // Extra delay for startup
            CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
            xyz.total_retries++;
            ZM_DEBUG(zm_dprintf("NAK (%d)\n", __LINE__));
        }
        if (stat == xyzModem_cancel) {
            break;
        }
    }
    *err = stat;
    return -1;
}

int 
xyzModem_stream_read(char *buf, int size, int *err)
{
    int stat, total, len;
    int retries;

    total = 0;
    // Try and get 'size' bytes into the buffer
    while (!xyz.at_eof && (size > 0)) {
        if (xyz.len == 0) {
            retries = xyzModem_MAX_RETRIES;
            while (retries-- > 0) {
                stat = xyzModem_get_hdr();
                if (stat == 0) {
                    if (xyz.blk == xyz.next_blk) {
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                        xyz.next_blk = (xyz.next_blk + 1) & 0xFF;
                        break;
                    } else if (xyz.blk == ((xyz.next_blk - 1) & 0xFF)) {
                        // Just re-ACK this so sender will get on with it
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        continue;  // Need new header
                    } else {
                        stat = xyzModem_sequence;
                    }
                }
                if (stat == xyzModem_cancel) {
                    break;
                }
                if (stat == xyzModem_eof) {
                    CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                    ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                    if (xyz.mode == xyzModem_ymodem) {
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
                        xyz.total_retries++;
                        stat = xyzModem_get_hdr();                        
                        CYGACC_COMM_IF_PUTC(*xyz.__chan, ACK);
                        ZM_DEBUG(zm_dprintf("ACK (%d)\n", __LINE__));
                    }
                    xyz.at_eof = true;
                    break;
                }
                CYGACC_COMM_IF_PUTC(*xyz.__chan, (xyz.crc_mode ? 'C' : NAK));
                xyz.total_retries++;
                ZM_DEBUG(zm_dprintf("NAK (%d)\n", __LINE__));
            }
            if (stat < 0) {
                *err = stat;
                xyz.len = -1;
                return total;
            }            
        }
        len = xyz.len;
        if (size < len) len = size;
        memcpy(buf, xyz.bufp, len);
        size -= len;
        buf += len;
        total += len;
        xyz.len -= len;
        xyz.bufp += len;
    }
    return total;
}

void
xyzModem_stream_close(int *err)
{
    printf("xyzModem - %s mode, %d(SOH)/%d(STX)/%d(CAN) packets, %d retries\n", 
           xyz.crc_mode ? "CRC" : "Cksum",
           xyz.total_SOH, xyz.total_STX, xyz.total_CAN,
           xyz.total_retries);
}

char *
xyzModem_error(int err)
{
    switch (err) {
    case xyzModem_access:
        return "Can't access file";
        break;
    case xyzModem_noZmodem:
        return "Sorry, zModem not available yet";
        break;
    case xyzModem_timeout:
        return "Timed out";
        break;
    case xyzModem_eof:
        return "End of file";
        break;
    case xyzModem_cancel:
        return "Cancelled";
        break;
    case xyzModem_frame:
        return "Invalid framing";
        break;
    case xyzModem_cksum:
        return "CRC/checksum error";
        break;
    case xyzModem_sequence:
        return "Block sequence error";
        break;
    default:
        return "Unknown error";
        break;
    }
}
