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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, tsmith
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
#ifdef CYGPKG_REDBOOT_DISK
#include <fs/disk.h>
#endif


// Buffer used by redboot_getc
getc_info_t getc_info;

static char usage[] = "[-r] [-v] "
#ifdef CYGPKG_COMPRESS_ZLIB
                      "[-d] "
#endif
                      "[-h <host>] [-m {TFTP | xyzMODEM"
#ifdef CYGPKG_REDBOOT_DISK
                      " | disk"
#endif
                      "}]\n        [-b <base_address>] <file_name>";

// Exported CLI function
RedBoot_cmd("load", 
            "Load a file", 
            usage,
            do_load 
    );

static unsigned long
load_elf_image(int (*getc)(void))
{
    diag_printf("ELF images not supported\n");
    return 0;
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
static unsigned long
load_srec_image(int (*getc)(void), void (*terminate)(int method,int (*getc)(void)), unsigned long base)
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
	    if (terminate) (*terminate)(xyzModem_abort, getc);
            diag_printf("Invalid S-record at offset %p, input: %c\n", 
                   (void *)offset, c);
            return 0;
        }
        type = (*getc)();
        offset += 2;
        sum = 0;
        if ((count = _hex2(getc, 1, &sum)) < 0) {
	    if (terminate) (*terminate)(xyzModem_abort, getc);
            diag_printf("Bad S-record count at offset %p\n", (void *)offset);
            return 0;
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
                } else {
                    addr_offset = 0;                    
                }
                first_addr = false;
            }
            addr += addr_offset;
            if ((unsigned long)(addr-addr_offset) < lowest_address) {
                lowest_address = (unsigned long)(addr - addr_offset);
            }
#ifdef CYGSEM_REDBOOT_VALIDATE_USER_RAM_LOADS
            if ((addr < user_ram_start) || (addr > user_ram_end)) {
	      // Only if there is no need to stop the download before printing
	      // output can we ask confirmation questions.
	      if (terminate) {
		(*terminate)(xyzModem_abort, getc);
		diag_printf("*** Warning! Attempt to load S-record to address: %p\nRedBoot does not believe this is in RAM\nUse TFTP for a chance to override this.\n",(void*)addr);
	      } else {
                if (!verify_action("Attempt to load S-record data to address: %p\n"
                                   "RedBoot does not believe this is in RAM", (void*)addr))
                    return 0;
	      }
            }
#endif
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
		if (terminate) (*terminate)(xyzModem_abort, getc);
                diag_printf("*** Warning! Checksum failure - Addr: %lx, %02lX <> %02lX\n", 
                       (unsigned long)base_addr, sum, cksum);
                return 0;
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
	    if (terminate) (*terminate)(xyzModem_close, getc);
            if (addr_offset) diag_printf("Address offset = %p\n", (void *)addr_offset);
            diag_printf("Entry point: %p, address range: %p-%p\n", 
                   (void*)entry_address, (void *)lowest_address, (void *)highest_address);
            return highest_address;
        default:
	    if (terminate) (*terminate)(xyzModem_abort, getc);
            diag_printf("Invalid S-record at offset 0x%lx, type: %x\n", 
                   (unsigned long)offset, type);
            return 0;
        }
        while ((c = (*getc)()) != '\n') offset++;
    }
    return 0;
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
            diag_printf("%c\b", spin[getc_info.tick++]);
            if (getc_info.tick >= sizeof(spin)) {
                getc_info.tick = 0;
            }
        }
        if (getc_info.len < BUF_SIZE) {
            // No more data available
            if (getc_info.verbose) diag_printf("\n");
            return -1;
        }
        getc_info.bufp = getc_info.buf;
        getc_info.len = (*getc_info.fun)(getc_info.bufp, BUF_SIZE, &getc_info.err);
        if ((getc_info.avail = getc_info.len) <= 0) {
            if (getc_info.verbose) diag_printf("\n");
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
#define MODE_XMODEM xyzModem_xmodem  // 1
#define MODE_YMODEM xyzModem_ymodem  // 2
#define MODE_ZMODEM xyzModem_zmodem  // 3
#define MODE_DISK   4

void 
do_load(int argc, char *argv[])
{
    int res, num_options;
    int i, err, mode;
    bool verbose, raw;
    bool base_addr_set, mode_str_set;
    char *mode_str;
#ifdef CYGPKG_REDBOOT_NETWORKING
    struct sockaddr_in host;
    bool hostname_set;
    char *hostname;
#endif
#ifdef CYGPKG_COMPRESS_ZLIB
    bool decompress;
#endif
    unsigned long base = 0;
    unsigned long end = 0;
    char type[4];
    char *filename = 0;
    struct option_info opts[6];

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
    mode = MODE_YMODEM;
#endif

    init_opts(&opts[0], 'v', false, OPTION_ARG_TYPE_FLG, 
              (void **)&verbose, 0, "verbose");
    init_opts(&opts[1], 'r', false, OPTION_ARG_TYPE_FLG, 
              (void **)&raw, 0, "load raw data");
    init_opts(&opts[2], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&base, (bool *)&base_addr_set, "load address");
    init_opts(&opts[3], 'm', true, OPTION_ARG_TYPE_STR, 
              (void **)&mode_str, (bool *)&mode_str_set, "download mode (TFTP, xyzMODEM, or disk)");
    num_options = 4;
#ifdef CYGPKG_REDBOOT_NETWORKING
    init_opts(&opts[num_options], 'h', true, OPTION_ARG_TYPE_STR, 
              (void **)&hostname, (bool *)&hostname_set, "host name (IP address)");
    num_options++;
#endif
#ifdef CYGPKG_COMPRESS_ZLIB
    init_opts(&opts[num_options], 'd', false, OPTION_ARG_TYPE_FLG, 
              (void **)&decompress, 0, "decompress");
    num_options++;
#endif

    if (!scan_opts(argc, argv, 1, opts, num_options, (void *)&filename, OPTION_ARG_TYPE_STR, "file name"))
    {
        return;
    }
#ifdef CYGPKG_REDBOOT_NETWORKING
    if (hostname_set) {
        if (!inet_aton(hostname, (in_addr_t *)&host)) {
            diag_printf("Invalid IP address: %s\n", hostname);
            return;
        }
    }
#endif
    if (mode_str_set) {
        if (strncasecmp(&mode_str[1], "modem", strlen(&mode_str[1])) == 0) {
            switch (_tolower(mode_str[0])) {
            case 'x':
                mode = MODE_XMODEM;
                break;
            case 'y':
                mode = MODE_YMODEM;
                break;
            case 'z':
                mode = MODE_ZMODEM;
                break;
            default:
                diag_printf("Invalid 'mode': %s\n", mode_str);
                return;
            }
            // When using a serial download type, override verbose
            // setting: spinner interferes with the protocol.
            verbose = false;
#ifdef CYGPKG_REDBOOT_DISK
	} else if (strcasecmp(mode_str, "disk") == 0) {
            mode = MODE_DISK;
#endif
#ifdef CYGPKG_REDBOOT_NETWORKING
        } else if (strcasecmp(mode_str, "tftp") == 0) {
            mode = MODE_TFTP;
            if (!have_net) {
                diag_printf("TFTP mode requires a working network\n");
                return;
            }
#endif
        } else {
            diag_printf("Invalid 'mode': %s\n", mode_str);
            return;
        }
    }
#if defined(CYGPKG_REDBOOT_NETWORKING) || defined(CYGPKG_REDBOOT_DISK)
    if ((mode == MODE_TFTP || mode == MODE_DISK) && !filename) {
        diag_printf("File name missing\n");
        diag_printf("usage: load %s\n", usage);
        return;
    }
#endif
#ifdef CYGSEM_REDBOOT_VALIDATE_USER_RAM_LOADS
    if (base_addr_set &&
        ((base < (unsigned long)user_ram_start) ||
         (base > (unsigned long)user_ram_end))) {
        if (!verify_action("Specified address (%p) is not believed to be in RAM", (void*)base))
            return;
    }
#endif
    if (raw && !base_addr_set) {
        diag_printf("Raw load requires a memory address\n");
        return;
    }
#ifdef CYGPKG_REDBOOT_NETWORKING
    if (mode == MODE_TFTP) {
        res = tftp_stream_open(filename, &host, TFTP_OCTET, &err);    
        if (res < 0) {
            diag_printf("Can't load '%s': %s\n", filename, tftp_error(err));
            return;
        }
        redboot_getc_init(tftp_stream_read, verbose);
    }
#endif
#ifdef CYGPKG_REDBOOT_DISK
    else if (mode == MODE_DISK) {
        res = disk_stream_open(filename, &err);
        if (res < 0) {
            diag_printf("Can't load '%s': %s\n", filename, disk_error(err));
            return;
        }
        redboot_getc_init(disk_stream_read, verbose);
    }
#endif
    else {
        res = xyzModem_stream_open(filename, mode, &err);
        if (res < 0) {
            diag_printf("Can't load '%s': %s\n", filename, xyzModem_error(err));
            return;
        }
        // Suppress verbosity when using xyz modem download
        redboot_getc_init(xyzModem_stream_read, 0 && verbose);
    }
    if (raw) {
#ifdef CYGPKG_COMPRESS_ZLIB
        if (decompress) {
            _pipe_t load_pipe;
            _pipe_t* p = &load_pipe;
            unsigned char _buffer[CYGNUM_REDBOOT_LOAD_ZLIB_BUFFER];

            p->out_buf = (unsigned char*) base;
            p->out_size = 0;
            p->in_buf = _buffer;
                
            err = (*_dc_init)(p);

            while (0 == err) {
                p->in_avail = 0;
                for (i = 0; i < CYGNUM_REDBOOT_LOAD_ZLIB_BUFFER; i++) {
                    res = redboot_getc();
                    if (res < 0) break;
                    p->in_buf[p->in_avail++] = res;
                }
                if (0 == p->in_avail) break;

                err = (*_dc_inflate)(p);
            }

            // Free used resources, do final translation of
            // error value.
            err = (*_dc_close)(p, err);

            if (0 != err && p->msg) {
                diag_printf("decompression error: %s\n", p->msg);
            }

            end = (unsigned long) base + p->out_size;
        } else // dangling block
#endif
        {
            unsigned char *mp = (unsigned char *)base;
            while ((res = redboot_getc()) >= 0) {
                *mp++ = res;
            }
            err = 0;
            end = (unsigned long) mp;
        }
        if (0 == err)
            diag_printf("Raw file loaded %p-%p\n", (void *)base, (void *)end);
    } else {
        // Read initial header - to determine file [image] type
        for (i = 0;  i < sizeof(type);  i++) {
            if ((res = redboot_getc()) < 0) {
                err = getc_info.err;
                break;
            } 
            type[i] = res;
        }
        if (res >= 0) {
            redboot_getc_rewind();  // Restore header to stream
            // Treat data as some sort of executable image
            if (strncmp(&type[1], "ELF", 3) == 0) {
                end = load_elf_image(redboot_getc);
            } else if ((type[0] == 'S') &&
                       ((type[1] >= '0') && (type[1] <= '9'))) {
		switch (mode) {
		  case MODE_XMODEM:
		  case MODE_YMODEM:
		  case MODE_ZMODEM:
                    end = load_srec_image(redboot_getc, xyzModem_stream_terminate, base);
		    break;
		  default:
                    end = load_srec_image(redboot_getc, NULL, base);
		    break;
		}
            } else {
                diag_printf("Unrecognized image type: 0x%lx\n", *(unsigned long *)type);
            }
        }
    }

    switch (mode) {
#ifdef CYGPKG_REDBOOT_DISK
      case MODE_DISK:
        disk_stream_close(&err);
	break;
#endif
#ifdef CYGPKG_REDBOOT_NETWORKING
      case MODE_TFTP:
        tftp_stream_close(&err);
	break;
#endif
      default:
        xyzModem_stream_close(&err);
	break;
    }
    return;
}
