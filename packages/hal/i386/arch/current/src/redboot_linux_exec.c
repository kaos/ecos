//=============================================================================
//
//      redboot_linux_exec.c
//
//      Boot linux from RedBoot
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ian Campbell
// Contributors:
// Date:        29 Oct 2004
// Purpose:     Boot Linux from Redboot
// Description: 
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <redboot.h>

#ifdef CYGPKG_IO_PCI
#include <cyg/io/pci.h>
#endif

#ifdef CYGPKG_IO_ETH_DRIVERS
#include <cyg/io/eth/eth_drv.h>
#endif
 
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#include <cyg/hal/hal_io.h>

/* 
 * Code to launch a Linux image directly in protected mode.
 *
 * Jumps directly to the protected mode part of the kernel
 */

typedef void (*trampoline_func)
     (unsigned long base, unsigned long length, unsigned long entry);

// Defines for the linux loader
#define SETUP_SIZE_OFF  497
#define SECTSIZE        512
#define SETUP_VERSION   0x0201
#define SETUP_HIGH      0x01
#define BIG_SYSSEG      0x10000
#define DEF_BOOTLSEG    0x9020

// From etherboot, this is the header to the image startup code
// see Documentation/i386/boot.txt
/* Boot sector: bootsect.S */
/* VERSION: ALL */
struct bootsect_header {
     cyg_uint8          pad0[0x1f1];
     cyg_uint8          setup_sects;
     cyg_uint16         root_flags;     // If set, the root is mounted readonly
     cyg_uint16         syssize;        // DO NOT USE - for bootsect.S use only
     cyg_uint16         swap_dev;       // DO NOT USE - obsolete
     cyg_uint16         ram_size;       // DO NOT USE - for bootsect.S use only
     cyg_uint16         vid_mode;       // Video mode control
     cyg_uint16         root_dev;       // Default root device number
     cyg_uint16         boot_flag;      // 0xAA55 magic number
} __attribute__((packed));

/* setup.S */
/* VERSION: 2.00+ */
struct setup_header {
     cyg_uint8          jump[2];
     cyg_uint8          magic[4];       // "HdrS"
     cyg_uint16         version;        // >= 0x0201 for initrd
     cyg_uint8          realmode_swtch[4];
     cyg_uint16         start_sys_seg;
     cyg_uint16         kernel_version;
     /* note: above part of header is compatible with loadlin-1.5
      * (header v1.5), must not change it */
     cyg_uint8          type_of_loader;
     cyg_uint8          loadflags;
     cyg_uint16         setup_move_size;
     unsigned long      code32_start;
     unsigned long      ramdisk_image;
     unsigned long      ramdisk_size;
     unsigned long      bootsect_kludge;
     /* VERSION: 2.01+ */
     cyg_uint16         heap_end_ptr;
     cyg_uint16         pad1;
     /* VERSION: 2.02+ */
     unsigned long      cmd_line_ptr;
     /* VERSION: 2.03+ */
     unsigned long      initrd_addr_max;
} __attribute__((packed));

#define PARAM                   0x90000
#define PARAM_ORIG_X            *(cyg_uint8*) (PARAM+0x000)
#define PARAM_ORIG_Y            *(cyg_uint8*) (PARAM+0x001)
#define PARAM_EXT_MEM_K         *(cyg_uint16*)(PARAM+0x002)
#define PARAM_ORIG_VIDEO_PAGE   *(cyg_uint16*)(PARAM+0x004)
#define PARAM_ORIG_VIDEO_MODE   *(cyg_uint8*) (PARAM+0x006)
#define PARAM_ORIG_VIDEO_COLS   *(cyg_uint8*) (PARAM+0x007)
#define PARAM_ORIG_VIDEO_EGA_BX *(cyg_uint16*)(PARAM+0x00a)
#define PARAM_ORIG_VIDEO_LINES  *(cyg_uint8*) (PARAM+0x00E)
#define PARAM_ORIG_VIDEO_ISVGA  *(cyg_uint8*) (PARAM+0x00F)
#define PARAM_ORIG_VIDEO_POINTS *(cyg_uint16*)(PARAM+0x010)

#define PARAM_ALT_MEM_K         *(cyg_uint32*)(PARAM+0x1e0)
#define PARAM_E820NR            *(cyg_uint8*) (PARAM+0x1e8)
#define PARAM_VID_MODE          *(cyg_uint16*)(PARAM+0x1fa)
#define PARAM_E820MAP           (struct e820entry*)(PARAM+0x2d0);
#define PARAM_CMDLINE           (char *)(PARAM+0x3400)

void
do_exec(int argc, char **argv)
{    
     unsigned long entry;
     unsigned long oldints;
     bool wait_time_set;
     int  wait_time, res;
     bool  base_addr_set, length_set, cmd_line_set;
     bool ramdisk_addr_set, ramdisk_size_set;
     unsigned long base_addr, length;
     unsigned long ramdisk_addr, ramdisk_size;
     struct option_info opts[6];
     char *cmd_line;
     char line[8];
     cyg_uint32 mem_size;
     cyg_uint32 int15_e801;
     extern char __tramp_start__[], __tramp_end__[];
     trampoline_func trampoline = 
          (trampoline_func)CYGHWR_REDBOOT_I386_TRAMPOLINE_ADDRESS;
     struct bootsect_header *bs_header;
     struct setup_header *s_header;
     int setup_sects;
     int xpos = 0, ypos = 0;
     
     base_addr = load_address;
     length = load_address_end - load_address;
     // Round length up to the next quad word
     length = (length + 3) & ~0x3;
     
     ramdisk_size = 4096*1024;
     init_opts(&opts[0], 'w', true, OPTION_ARG_TYPE_NUM,
               &wait_time, &wait_time_set, "wait timeout");
     init_opts(&opts[1], 'b', true, OPTION_ARG_TYPE_NUM,
               &base_addr, &base_addr_set, "base address");
     init_opts(&opts[2], 'l', true, OPTION_ARG_TYPE_NUM,
               &length, &length_set, "length");
     init_opts(&opts[3], 'c', true, OPTION_ARG_TYPE_STR,
               &cmd_line, &cmd_line_set, "kernel command line");
     init_opts(&opts[4], 'r', true, OPTION_ARG_TYPE_NUM,
               &ramdisk_addr, &ramdisk_addr_set, "ramdisk_addr");
     init_opts(&opts[5], 's', true, OPTION_ARG_TYPE_NUM,
               &ramdisk_size, &ramdisk_size_set, "ramdisk_size");
     if (!scan_opts(argc, argv, 1, opts, 6, 0, 0, "starting address"))
     {
          return;
     }
     
     if (wait_time_set) {
          int script_timeout_ms = wait_time * 1000;
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
          unsigned char *hold_script = script;
          script = (unsigned char *)0;
#endif
          diag_printf("About to boot linux kernel at %p - "
                      "abort with ^C within %d seconds\n",
                      (void *)base_addr, wait_time);
          while (script_timeout_ms >= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT) {
               res = _rb_gets(line, sizeof(line), 
                              CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT);
               if (res == _GETS_CTRLC) {
#ifdef CYGFUN_REDBOOT_BOOT_SCRIPT
                    script = hold_script;  // Re-enable script
#endif
                    return;
               }
               script_timeout_ms -= CYGNUM_REDBOOT_CLI_IDLE_TIMEOUT;
          }
     }
     
     if (base_addr_set && !length_set) {
          diag_printf("Length required for non-standard base address\n");
          return;
     }
     
     bs_header = (struct bootsect_header *)base_addr;
     s_header = (struct setup_header *)(base_addr + SECTSIZE);
     
     if (bs_header->boot_flag != 0xAA55) {
          diag_printf("Bootsector magic not found (0x%04x @ %4p)\n", 
                      bs_header->boot_flag, &bs_header->boot_flag);
          return;
     }
     if (memcmp(s_header->magic,"HdrS",4) != 0) {
          diag_printf("Linux header (HdrS) not found\n");
          return;
     }
     if (s_header->version < SETUP_VERSION) {
          diag_printf("Linux header version = 0x%04x. "
                      "Needs to be at least 0x%04x\n",
                      s_header->version, SETUP_VERSION);
          return;
     }
     
     setup_sects = bs_header->setup_sects ? bs_header->setup_sects : 4;
     
     entry = s_header->code32_start;
     // + 1 for boot sector
     base_addr += (setup_sects + 1 ) * SECTSIZE;
     length -= (setup_sects + 1 ) * SECTSIZE;
     
     mem_size = (cyg_uint32)HAL_MEM_REAL_REGION_TOP((cyg_uint8 *)0x1000000);
     mem_size >>= 10;   // convert from bytes to kilobytes.
     // Result of int15 ax=0xe801
     int15_e801 = mem_size - 1024 ; // 1M+ only
     
     // Stop all network devices
#ifdef CYGPKG_IO_ETH_DRIVERS
     eth_drv_stop();
#endif
     
#ifdef CYGPKG_IO_PCI
     cyg_pci_init();
#endif
     
#if CYGINT_HAL_I386_PCMB_SCREEN_SUPPORT > 0
     cyg_hal_plf_screen_position(&xpos, &ypos);
#endif
     
     HAL_DISABLE_INTERRUPTS(oldints);
     HAL_DCACHE_SYNC();
     HAL_ICACHE_DISABLE();
     HAL_DCACHE_DISABLE();
     HAL_DCACHE_SYNC();
     HAL_ICACHE_INVALIDATE_ALL();
     HAL_DCACHE_INVALIDATE_ALL();
     
     // Clear the data area
     memset ( (void*)PARAM, 0, 512 );
     
     if ( cmd_line_set )
          strcpy( PARAM_CMDLINE, cmd_line );
     else
          strcpy( PARAM_CMDLINE, "auto");
     
     memcpy((void*)(PARAM+SECTSIZE), s_header, sizeof(struct setup_header));
     s_header = (struct setup_header*)(0x90000+SECTSIZE);
     
     s_header->version = SETUP_VERSION;
     
     // Command Line
     s_header->cmd_line_ptr = 0x93400;
     
     // Loader type
     s_header->type_of_loader = 0xFF;
     
     // Fill in the interesting bits of data area...
     // ... Memory sizes
     PARAM_EXT_MEM_K = int15_e801;
     PARAM_ALT_MEM_K = int15_e801;
     
     // ... No e820 map!
     PARAM_E820NR = 0;   // Length of map
     
     // ... Video stuff
     PARAM_ORIG_X = xpos;
     PARAM_ORIG_Y = ypos;
     PARAM_ORIG_VIDEO_MODE = 2;
     PARAM_ORIG_VIDEO_COLS = 80;
     PARAM_ORIG_VIDEO_LINES = 25;
     PARAM_ORIG_VIDEO_ISVGA = 0;
     
     // Copy trampoline to trampoline address
     memcpy((char *)CYGHWR_REDBOOT_I386_TRAMPOLINE_ADDRESS,
            __tramp_start__,
            __tramp_end__ - __tramp_start__);
     
     trampoline(base_addr, length, entry);
     
#define _QUOTE_STRING(__x__)    #__x__
#define QUOTE_STRING(__x__)     _QUOTE_STRING(__x__)
     
     asm volatile (
          "__tramp_start__:\n"
          "       push   %%ebp;\n"
          "       mov    %%esp,%%ebp;\n"
          
          /* STACK IS:
           * OLD BP               0x4(%ebp)
           * ENTRY                0x8(%ebp)
           * LENGTH               0xC(%ebp)
           * BASE ADDRESS         0x10(%ebp) */
          
          "       movl    0x10(%%ebp), %%ebx;\n"  /* Save entry point
                                                     in EBX, because
                                                     we overwrite the
                                                     stack */
          
          "       cli;\n"                         /* no interrupts allowed ! */
          
          "       movb    $0x80, %%al;\n"         /* disable NMI for bootup */
          "       outb    %%al, $0x70;\n"         /* sequence */
          
          /* Copy GDT to RAM at 0x90400 */
          "       movl    $(linux_gdt_end - linux_gdt), %%ecx;\n" /* Length */
          "       shrl    $2, %%ecx;\n"                   /* Bytes -> Longs */
          "       leal    linux_gdt, %%eax;\n"            /* Source */
          "       movl    %%eax, %%esi;\n"
          "       movl    $(0x90400), %%edi;\n"           /* Dest */
          "1:\n"
          "       lodsl;\n"
          "       stosl;\n"
          "       loop    1b;\n"
          
          /* If necessary, copy linux image to correct location */
          "       movl    0x8(%%ebp), %%esi;\n"           /* Source */
          "       movl    %%ebx, %%edi;\n"                /* Destination
                                                           * (saved in
                                                           * EBX
                                                           * above) */
          "       cmpl    %%edi, %%esi;\n"
          "       je      2f;\n"
          "       movl    0xC(%%ebp), %%ecx;\n"           /* Length */
          "       shrl    $2, %%ecx;\n"                   /* Bytes to Longs */
          "1:\n"
          "       lodsl;\n"
          "       stosl;\n"
          "       loop    1b;\n"
          "2:\n"
          
          /* Create a GDT descriptor at 0 and load it */
          "       movl    $0x90000, %%esi;\n"
          "       movw    $(linux_gdt_end - linux_gdt), %%ax;\n"
          "       dec     %%ax;\n"
          "       movw    %%ax,0;\n"
          "       movl    $0x90400,%%eax;\n"
          "       movl    %%eax,2;\n"
          "       lgdt    0;\n"
          
          /* Reload segment registers */
          "       mov     $(0x18), %%eax;\n"
          "       movl    %%eax, %%ds;\n"
          "       movl    %%eax, %%es;\n"
          "       movl    %%eax, %%fs;\n"
          "       movl    %%eax, %%gs;\n"
          
          /* Reload CS */
          "       ljmp    $(0x10), $(1f - __tramp_start__ + " 
          QUOTE_STRING(CYGHWR_REDBOOT_I386_TRAMPOLINE_ADDRESS) ");\n"
          "1:\n"
          
          /* Start kernel */
          "       jmp     *%%ebx;\n"
          
          ".ALIGN 4, 0xCC;\n"
          
          "__tramp_end__:\n"
          
          /* Descriptor tables */
          "linux_gdt:\n"
          "       .word   0, 0, 0, 0;\n"    /* dummy */
          "       .word   0, 0, 0, 0;\n"    /* unused */
          "       .word   0xFFFF;\n"        /* 4Gb - (0x100000*0x1000
                                               * = * 4Gb) */
          "       .word   0;\n"             /* base address = 0 */
          "       .word   0x9A00;\n"        /* code read/exec */
          "       .word   0x00CF;\n"        /* granularity = 4096, 386 */
          /*  (+5th nibble of limit) */
          "       .word   0xFFFF;\n"        /* 4Gb - (0x100000*0x1000 = 4Gb) */
          "       .word   0;\n"             /* base address = 0 */
          "       .word   0x9200;\n"        /* data read/write */
          "       .word   0x00CF;\n"        /* granularity = 4096, 386 */
                                            /*  (+5th nibble of limit) */
          "linux_gdt_end:\n"
          : : : "eax", "ebx", "ecx");
}

RedBoot_cmd("exec",
            "Execute a Linux image",
            "[-w timeout] [-b <base address> [-l <image length>]]\n"
            "        [-r <ramdisk addr> [-s <ramdisk length>]]\n"
            "        [-c \"kernel command line\"]",
            do_exec
     );
