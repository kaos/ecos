//==========================================================================
//
//      load.c
//
//      RedBoot file/image loader
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

// Buffer used by redboot_getc
getc_info_t getc_info;


// Exported CLI function
RedBoot_cmd("load", 
            "Load a file", 
            "[-v] [-r] [-b <mem_addr>] <file_name>",
            do_load 
    );

static void
load_elf_image(int (*getc)(void))
{
    printf("ELF images not supported\n");
}

//
// Scan a string of hex bytes and update the checksum
//
static long
_hex2(int (*getc)(void), int len, long *sum)
{
    int val, byte;
    char c1, c2;

    val = 0;
    while (len-- > 0) {
        c1 = (*getc)();
        c2 = (*getc)();
        if (_is_hex(c1) && _is_hex(c2)) {
            val <<= 8;
            byte = (_from_hex(c1)<<4) | _from_hex(c2);
            val |= byte;
            if (sum) {
                *sum += byte;
            }
        } else {
            return (-1);
        }
    }
    return (val);
}

//
// Process a set of S-records, loading the contents into memory.  
// Note: if a "base" value is provided, the data will be relocated
// relative to that location.  Of course, this can only work for
// the first section of the data, so if there are non-contiguous
// pieces of data, they will end up relocated in the same fashion.
// Because of this, "base" probably only makes sense for a set of
// data which has only one section, e.g. a ROM image.
//
#define MAX_LINE 80
static void
load_srec_image(int (*getc)(void), unsigned long base)
{
    char line[MAX_LINE];
    char *lp;
    int  c, len;
    long offset = 0, count, sum, val, cksum;
    unsigned char *addr, *base_addr;
    char type;
    bool first_addr = true;
    unsigned long addr_offset = 0;
    unsigned long highest_address = 0;
    unsigned long lowest_address = 0xFFFFFFFF;

    while ((c = (*getc)()) > 0) {
        lp = line;  len = 0;
        // Start of line
        if (c != 'S') {
            printf("Invalid S-record at offset %p, input: %c\n", 
                   (void *)offset, c);
            return;
        }
        type = (*getc)();
        offset += 2;
        sum = 0;
        if ((count = _hex2(getc, 1, &sum)) < 0) {
            printf("Bad S-record count at offset %p\n", (void *)offset);
            return;
        }
        offset += 1;
        switch (type) {
        case '0':
            break;
        case '1':
        case '2':
        case '3':
            base_addr = addr = (unsigned char *)_hex2(getc, (type-'1'+2), &sum);
            offset += (type-'1'+2);
            if (first_addr) {
                if (base) {
                    addr_offset = (unsigned long)base - (unsigned long)addr;
                    printf("Address offset = %p\n", (void *)addr_offset);
                } else {
                    addr_offset = 0;                    
                }
                first_addr = false;
            }
            addr += addr_offset;
            if ((unsigned long)(addr-addr_offset) < lowest_address) {
                lowest_address = (unsigned long)(addr - addr_offset);
            }
            if ((addr < ram_start) || (addr > ram_end)) {
                printf("Attempt to load S-record data to address: %p [not in RAM]\n", addr);
                return;
            }
            count -= ((type-'1'+2)+1);
            offset += count;
            while (count-- > 0) {
                val = _hex2(getc, 1, &sum);
                *addr++ = val;
            }
            cksum = _hex2(getc, 1, 0);
            offset += 1;
            sum = sum & 0xFF;
            cksum = (~cksum & 0xFF);
            if (cksum != sum) {
                printf("*** Warning! Checksum failure - Addr: %lx, %02lX <> %02lX\n", 
                       (unsigned long)base_addr, sum, cksum);
            }
            if ((unsigned long)(addr-addr_offset) > highest_address) {
                highest_address = (unsigned long)(addr - addr_offset);
            }
            break;
        case '7':
        case '8':
        case '9':
            addr = (unsigned char *)_hex2(getc, ('9'-type+2), &sum);
            offset += ('9'-type+2);
            entry_address = (unsigned long *)addr;
            printf("Entry point: %p, address range: %p-%p\n", 
                   entry_address, (void *)lowest_address, (void *)highest_address);
            return;
        default:
            printf("Invalid S-record at offset 0x%lx, type: %x\n", 
                   (unsigned long)offset, type);
            return;
        }
        while ((c = (*getc)()) != '\n') offset++;
    }
}


int
redboot_getc(void)
{
    static char spin[] = "|/-\\|-";
    if (getc_info.avail < 0) {
      return -1;
    }
    if (getc_info.avail == 0) {
        if (getc_info.verbose) {
            printf("%c\b", spin[getc_info.tick++]);
            if (getc_info.tick >= sizeof(spin)) {
                getc_info.tick = 0;
            }
        }
        if (getc_info.len < BUF_SIZE) {
            // No more data available
            if (getc_info.verbose) printf("\n");
            return -1;
        }
        getc_info.bufp = getc_info.buf;
        getc_info.len = (*getc_info.fun)(getc_info.bufp, BUF_SIZE, &getc_info.err);
        if ((getc_info.avail = getc_info.len) <= 0) {
            if (getc_info.verbose) printf("\n");
            return -1;
        }
    }
    getc_info.avail--;
    return *getc_info.bufp++;
}

void
redboot_getc_init(int (*fun)(char *, int, int *), int verbose)
{
    getc_info.avail = 0;
    getc_info.len = BUF_SIZE;
    getc_info.fun = fun;
    getc_info.verbose = verbose;
    getc_info.tick = 0;
}

void
redboot_getc_rewind(void)
{
    getc_info.bufp = getc_info.buf;
    getc_info.avail = getc_info.len;
}

#define MODE_TFTP   0
#define MODE_ZMODEM 1

void 
do_load(int argc, char *argv[])
{
    int i, res, err, num_options;
    int mode;
    bool verbose, raw;
    bool base_addr_set, mode_str_set;
    char *mode_str;
#ifdef CYGPKG_REDBOOT_NETWORKING
    struct sockaddr_in host;
    bool hostname_set;
    char *hostname;
#endif
    unsigned long base = 0;
    char type[4];
    char *filename = 0;
    char *usage = "usage: load <file_name> [-r] [-v] [-h <host>] [-m {TFTP | ZMODEM}] [-b <base_address>]\n";
    struct option_info opts[5];

#ifdef CYGPKG_REDBOOT_NETWORKING
    memset((char *)&host, 0, sizeof(host));
    host.sin_len = sizeof(host);
    host.sin_family = AF_INET;
    host.sin_addr = my_bootp_info.bp_siaddr;
    host.sin_port = 0;
#endif
#ifdef CYGPKG_REDBOOT_NETWORKING
    mode = MODE_TFTP;
#else
    mode = MODE_ZMODEM;
#endif

    init_opts(&opts[0], 'v', false, OPTION_ARG_TYPE_FLG, 
              (void **)&verbose, 0, "verbose");
    init_opts(&opts[1], 'r', false, OPTION_ARG_TYPE_FLG, 
              (void **)&raw, 0, "load raw data");
    init_opts(&opts[2], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_addr_set, "load address");
    init_opts(&opts[3], 'm', true, OPTION_ARG_TYPE_STR, 
              (void **)&mode_str, (bool *)&mode_str_set, "download mode (TFTP or ZMODEM)");
    num_options = 4;
#ifdef CYGPKG_REDBOOT_NETWORKING
    init_opts(&opts[4], 'h', true, OPTION_ARG_TYPE_STR, 
              (void **)&hostname, (bool *)&hostname_set, "host name (IP address)");
    num_options = 5;
#endif
    if (!scan_opts(argc, argv, 1, opts, num_options, (void *)&filename, OPTION_ARG_TYPE_STR, "file name"))
    {
        return;
    }
#ifdef CYGPKG_REDBOOT_NETWORKING
    if (hostname_set) {
        if (!inet_aton(hostname, (in_addr_t *)&host)) {
            printf("Invalid IP address: %s\n", hostname);
            return;
        }
    }
#endif
    if (mode_str_set) {
        if (strcmpci(mode_str, "zmodem") == 0) {
            mode = MODE_ZMODEM;
#ifdef CYGPKG_REDBOOT_NETWORKING
        } else if (strcmpci(mode_str, "tftp") == 0) {
            mode = MODE_TFTP;
            if (!have_net) {
                printf("TFTP mode requires a working network\n");
                return;
            }
#endif
        } else {
            printf("Invalid 'mode': %s\n", mode_str);
            return;
        }
    }
    if ((mode == MODE_TFTP) && !filename) {
        printf("File name missing\n");
        printf(usage);
        return;
    }
#ifdef CYGPKG_REDBOOT_NETWORKING
    if (mode == MODE_TFTP) {
        res = tftp_stream_open(filename, &host, TFTP_OCTET, &err);    
        if (res < 0) {
            printf("Can't load '%s': %s\n", filename, tftp_error(err));
            return;
        }
        redboot_getc_init(tftp_stream_read, verbose);
        for (i = 0;  i < sizeof(type);  i++) {
            if ((res = redboot_getc()) < 0) {
                err = getc_info.err;
                break;
            } 
            type[i] = res;
        }
        if (res < 0) {
            printf("Error reading header via TFTP: %s\n", tftp_error(err));
            tftp_stream_close(&err);
            return;
        }
    }
#endif
    if (mode == MODE_ZMODEM) {
        printf("Sorry, zmodem download not yet available\n");
        return;
    }
    redboot_getc_rewind();  // Restore header to stream
    if (raw) {
        if (!base_addr_set) {
            printf("Raw load requires a memory address\n");
        } else {
            unsigned char *mp = (unsigned char *)base;
            while ((res = redboot_getc()) >= 0) {
                *mp++ = res;
            }
            printf("Raw file loaded %p-%p\n", (void *)base, (void *)mp);
        }
    } else {
        // Treat data as some sort of executable image
        if (strncmp(&type[1], "ELF", 3) == 0) {
            load_elf_image(redboot_getc);
        } else if ((type[0] == 'S') &&
                   ((type[1] >= '0') && (type[1] <= '9'))) {
            load_srec_image(redboot_getc, base);
        } else {
            printf("Unrecognized image type: %lx\n", *(unsigned long *)type);
        }
    }
#ifdef CYGPKG_REDBOOT_NETWORKING
    if (mode == MODE_TFTP) {
        tftp_stream_close(&err);
    }
#endif
    return;
}
