//==========================================================================
//
//      flash.c
//
//      RedBoot - FLASH memory support
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
// Contributors: gthomas
// Date:         2000-07-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <cyg/io/flash.h>
#include <fis.h>

// Exported CLI functions
RedBoot_cmd("fis", 
            "Manage FLASH images", 
            "{cmds}",
            do_fis
    );
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
RedBoot_cmd("fconfig",
            "Manage configuration kept in FLASH memory",
            "[-l]",
            do_flash_config
    );
#endif

// Internal commands
local_cmd_entry("init",
                "Initialize FLASH Image System [FIS]",
                "[-f]",
                fis_init,
                FIS_cmds
    );
local_cmd_entry("list",
                "Display contents of FLASH Image System [FIS]",
                "[-c]",
                fis_list,
                FIS_cmds
    );
local_cmd_entry("free",
                "Display free [available] locations within FLASH Image System [FIS]",
                "",
                fis_free,
                FIS_cmds
    );
local_cmd_entry("erase",
                "Erase FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_erase,
                FIS_cmds
    );
#if 0 < CYGHWR_IO_FLASH_BLOCK_LOCKING // This is an *interface*
local_cmd_entry("lock",
                "LOCK FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_lock,
                FIS_cmds
    );
local_cmd_entry("unlock",
                "UNLOCK FLASH contents",
                "-f <flash_addr> -l <length>",
                fis_unlock,
                FIS_cmds
    );
#endif
local_cmd_entry("delete",
                "Display an image from FLASH Image System [FIS]",
                "name",
                fis_delete,
                FIS_cmds
    );
local_cmd_entry("load",
                "Load image from FLASH Image System [FIS] into RAM",
                "[-b <memory_load_address>] [-c] name",
                fis_load,
                FIS_cmds
    );
local_cmd_entry("create",
                "Create an image",
                "-b <mem_base> -l <image_length> [-s <data_length>] 
                [-f <flash_addr>] [-e <entry_point>] [-r <ram_addr>] [-n] <name>",
                fis_create,
                FIS_cmds
    );

// Define table boundaries
CYG_HAL_TABLE_BEGIN( __FIS_cmds_TAB__, FIS_cmds);
CYG_HAL_TABLE_END( __FIS_cmds_TAB_END__, FIS_cmds);

extern struct cmd __FIS_cmds_TAB__[], __FIS_cmds_TAB_END__;

// Local data used by these routines
static void *flash_start, *flash_end;
static int block_size, blocks;
static void *fis_work_block;

struct fis_image_desc *
fis_lookup(char *name)
{
    int i;
    void *fis_addr;
    struct fis_image_desc *img;

    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            return img;
        }
    }
    return (struct fis_image_desc *)0;
}

static void
fis_usage(char *why)
{
    printf("*** invalid 'fis' command: %s\n", why);
    cmd_usage(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, "fis ");
}

static void
fis_init(int argc, char *argv[])
{
    int stat, img_count = 0;
    struct fis_image_desc *img;
    void *fis_base, *err_addr;
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    void *cfg_base;
#endif
    bool full_init = false;
    struct option_info opts[1];
    unsigned long redboot_image_size, redboot_flash_start;

    init_opts(&opts[0], 'f', false, OPTION_ARG_TYPE_FLG, 
              (void **)&full_init, (bool *)0, "full initialization, erases all of flash");
    if (!scan_opts(argc, argv, 2, opts, 1, 0, 0, ""))
    {
        return;
    }

    if (!verify_action("About to initialize [format] FLASH image system")) {
        printf("** Aborted\n");
        return;
    }
    printf("*** Initialize FLASH Image System\n");

    redboot_flash_start = (unsigned long)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
#define MIN_REDBOOT_IMAGE_SIZE CYGBLD_REDBOOT_MIN_IMAGE_SIZE
    redboot_image_size = block_size > MIN_REDBOOT_IMAGE_SIZE ? block_size : MIN_REDBOOT_IMAGE_SIZE;

    if (full_init) {
	// Erase everything except default RedBoot images, fis block, and config block.
	// FIXME! This still assumes that fis and config blocks can use top of FLASH.
	if (CYGBLD_REDBOOT_FLASH_BOOT_OFFSET == 0) {
	    if ((stat = flash_erase((void *)((unsigned long)flash_start+(2*redboot_image_size)), 
				    ((blocks-2)*block_size) - (2*redboot_image_size),
				    (void **)&err_addr)) != 0) {
		printf("   initialization failed %p: 0x%x(%s)\n",
		       err_addr, stat, flash_errmsg(stat));
	    }
	} else {
	    if ((stat = flash_erase(flash_start, CYGBLD_REDBOOT_FLASH_BOOT_OFFSET, 
				    (void **)&err_addr)) != 0) {
		printf("   initialization failed %p: 0x%x(%s)\n",
		       err_addr, stat, flash_errmsg(stat));
	    } else {
		unsigned long erase_start, erase_size;

		erase_start = redboot_flash_start+(2*redboot_image_size);
		erase_size = (blocks-2)*block_size;
		erase_size -= erase_start - (unsigned long)flash_start;

		if (erase_size && (stat = flash_erase((void *)erase_start, erase_size,
						      (void **)&err_addr)) != 0) {
		    printf("   initialization failed %p: 0x%x(%s)\n",
			   err_addr, stat, flash_errmsg(stat));
		}
	    }
	}

    } else {
        printf("    Warning: device contents not erased, some blocks may not be usable\n");
    }
    // Create a pseudo image for RedBoot
    img = (struct fis_image_desc *)fis_work_block;
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot");
    img->flash_base = redboot_flash_start;
    img->mem_base = redboot_flash_start;
    img->size = redboot_image_size;
    img++;  img_count++;
    // And a backup image
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot[backup]");
    img->flash_base = redboot_flash_start+redboot_image_size;
    img->mem_base = redboot_flash_start+redboot_image_size;
    img->size = redboot_image_size;
    img++;  img_count++;
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    // And a descriptor for the configuration data
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot config");
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    img->flash_base = (unsigned long)cfg_base;
    img->mem_base = (unsigned long)cfg_base;
    img->size = block_size;
    img++;  img_count++;
#endif
    // And a descriptor for the descriptor table itself
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "FIS directory");
    fis_base = (void *)((unsigned long)flash_end - block_size);
    img->flash_base = (unsigned long)fis_base;
    img->mem_base = (unsigned long)fis_base;
    img->size = block_size;
    img++;  img_count++;
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_base, block_size, (void **)&err_addr);
#endif
    if ((stat = flash_erase(fis_base, block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    } else {
        if ((stat = flash_program(fis_base, fis_work_block, 
                                  img_count*sizeof(*img), (void **)&err_addr)) != 0) {
            printf("Error writing image descriptors at %p: 0x%x(%s)\n", 
                   err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_base, block_size, (void **)&err_addr);
#endif
}

static void
fis_list(int argc, char *argv[])
{
    struct fis_image_desc *img;
    int i;
    bool show_cksums = false;
    struct option_info opts[1];

    init_opts(&opts[0], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void **)&show_cksums, (bool *)0, "display checksums");
    if (!scan_opts(argc, argv, 2, opts, 1, 0, 0, ""))
    {
        return;
    }

    img = (struct fis_image_desc *)((unsigned long)flash_end - block_size);    
    printf("Name              FLASH addr   %s    Length      Entry point\n",
           show_cksums ? "Checksum" : "Mem addr");
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if (img->name[0] != (unsigned char)0xFF) {
            printf("%-16s  0x%08lX   0x%08lX  0x%08lX  0x%08lX\n", img->name, 
                   img->flash_base, 
                   show_cksums ? img->file_cksum : img->mem_base, 
                   img->size, img->entry_point);
        }
    }
}

static void
fis_free(int argc, char *argv[])
{
    unsigned long *fis_ptr, *fis_end;
    unsigned long *area_start;

    fis_ptr = (unsigned long *)((unsigned long)flash_start + 2*block_size);
    fis_end = (unsigned long *)((unsigned long)flash_end + block_size);
    fis_end = (unsigned long *)(unsigned long)flash_end;
    area_start = fis_ptr;
    while (fis_ptr < fis_end) {
        if (*fis_ptr != (unsigned long)0xFFFFFFFF) {
            if (area_start != fis_ptr) {
                // Assume that this is something
                printf("  0x%08lX .. 0x%08lX\n", 
                       (unsigned long)area_start, (unsigned long)fis_ptr);
            }
            // Find next blank block
            area_start = fis_ptr;
            while (area_start < fis_end) {
                if (*area_start == (unsigned long)0xFFFFFFFF) {
                    break;
                }
                area_start += block_size / sizeof(unsigned long);
            }
            fis_ptr = area_start;
        } else {
            fis_ptr += block_size / sizeof(unsigned long);
        }
    }
    if (area_start != fis_ptr) {
        printf("  0x%08lX .. 0x%08lX\n", 
               (unsigned long)area_start, (unsigned long)fis_ptr);
    }
}

// Find the first unused area of flash which is long enougn
static bool
fis_find_free(unsigned long *addr, unsigned long length)
{
    unsigned long *fis_ptr, *fis_end;
    unsigned long *area_start;

    fis_ptr = (unsigned long *)((unsigned long)flash_start + 2*block_size);
    fis_end = (unsigned long *)(unsigned long)flash_end;
    area_start = fis_ptr;
    while (fis_ptr < fis_end) {
        if (*fis_ptr != (unsigned long)0xFFFFFFFF) {
            if (area_start != fis_ptr) {
                // Assume that this is something
                if ((fis_ptr-area_start) >= length) {
                    *addr = (unsigned long)area_start;
                    return true;
                }
            }
            // Find next blank block
            area_start = fis_ptr;
            while (area_start < fis_end) {
                if (*area_start == (unsigned long)0xFFFFFFFF) {
                    break;
                }
                area_start += block_size / sizeof(unsigned long);
            }
            fis_ptr = area_start;
        } else {
            fis_ptr += block_size / sizeof(unsigned long);
        }
    }
    if (area_start != fis_ptr) {
        if ((fis_ptr-area_start) >= length) {
            *addr = (unsigned long)area_start;
            return true;
        }
    }
    return false;
}

static void
fis_create(int argc, char *argv[])
{
    int i, stat;
    unsigned long mem_addr, exec_addr, flash_addr, entry_addr, length, img_size;
    char *name;
    bool mem_addr_set = false;
    bool exec_addr_set = false;
    bool entry_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    bool img_size_set = false;
    bool no_copy = false;
    void *fis_addr, *err_addr;
    struct fis_image_desc *img;
    bool slot_found;
    struct option_info opts[7];
    bool prog_ok;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&mem_addr, (bool *)&mem_addr_set, "memory base address");
    init_opts(&opts[1], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&exec_addr, (bool *)&exec_addr_set, "ram base address");
    init_opts(&opts[2], 'e', true, OPTION_ARG_TYPE_NUM, 
              (void **)&entry_addr, (bool *)&entry_addr_set, "entry point address");
    init_opts(&opts[3], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[4], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "image length [in FLASH]");
    init_opts(&opts[5], 's', true, OPTION_ARG_TYPE_NUM, 
              (void **)&img_size, (bool *)&img_size_set, "image size [actual data]");
    init_opts(&opts[6], 'n', false, OPTION_ARG_TYPE_FLG, 
              (void **)&no_copy, (bool *)0, "don't copy from RAM to FLASH, just update directory");
    if (!scan_opts(argc, argv, 2, opts, 7, (void *)&name, OPTION_ARG_TYPE_STR, "file name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    if ((!no_copy && !mem_addr_set) || (no_copy && !flash_addr_set) ||
        !length_set || !name) {
        fis_usage("required parameter missing");
        return;
    }
    if (!img_size_set) {
        img_size = length;
    }
    // 'length' is size of FLASH image, 'img_size' is actual data size
    // Round up length to FLASH block size
#ifndef CYGPKG_HAL_MIPS // FIXME: compiler is b0rken
    length = ((length + block_size - 1) / block_size) * block_size;
    if (length < img_size) {
        printf("Invalid FLASH image size/length combination\n");
        return;
    }
#endif
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+img_size-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if (strlen(name) >= sizeof(img->name)) {
        printf("Name is too long, must be less than %d chars\n", (int)sizeof(img->name));
        return;
    }
    if (!no_copy) {
        if ((mem_addr < (unsigned long)ram_start) ||
            ((mem_addr+img_size) >= (unsigned long)ram_end)) {
            printf("** WARNING: RAM address: %p may be invalid\n", (void *)mem_addr);
            printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
        }
        if (!flash_addr_set && !fis_find_free(&flash_addr, length)) {
            printf("Can't locate %ld bytes free in FLASH\n", length);
            return;
        }
    }
    // Find a slot in the directory for this entry
    // First, see if an image by this name is already present
    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            if (flash_addr_set && (img->flash_base != flash_addr)) {
                printf("Image found, but FLASH address incorrect\n");
                return;
            }
            if (img->size != length) {
                printf("Image found, but LENGTH is incorrect (0x%lx != 0x%lx)\n", img->size, length);
                return;
            }
            if (!verify_action("An image named '%s' exists", name)) {
                return;
            } else {
                slot_found = true;
                break;
            }
        }
    }
    // If not found, try and find an empty slot
    if (!slot_found) {
        img = (struct fis_image_desc *)fis_work_block;
        for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
            if (img->name[0] == (unsigned char)0xFF) {
                slot_found = true;
                break;
            }
        }
    }
    if (!no_copy) {
        // Safety check - make sure the address range is not within the code we're running
        if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+img_size-1))) {
            printf("Can't program this region - contains code in use!\n");
            return;
        }
        prog_ok = true;
        if (prog_ok) {
            // Erase area to be programmed
            if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
                printf("Can't erase region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
                prog_ok = false;
            }
        }
        if (prog_ok) {
            // Now program it
            if ((stat = flash_program((void *)flash_addr, (void *)mem_addr, img_size, (void **)&err_addr)) != 0) {
                printf("Can't program region at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
                prog_ok = false;
            }
        }
    }
    // Update directory
    memset(img, 0, sizeof(*img));
    strcpy(img->name, name);
    img->flash_base = flash_addr;
    img->mem_base = exec_addr_set ? exec_addr : (flash_addr_set ? flash_addr : mem_addr);
    img->entry_point = entry_addr_set ? entry_addr : (unsigned long)entry_address;  // Hope it's been set
    img->size = length;
    img->data_length = img_size;
    img->file_cksum = crc32((unsigned char *)flash_addr, img_size);
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
}

static void
fis_erase(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    // Safety check - make sure the address range is not within the code we're running
    if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1))) {
        printf("Can't erase this region - contains code in use!\n");
        return;
    }
    if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}

#if 0 < CYGHWR_IO_FLASH_BLOCK_LOCKING // This is an *interface*

static void
fis_lock(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((stat = flash_lock((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error locking at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}

static void
fis_unlock(int argc, char *argv[])
{
    int stat;
    unsigned long flash_addr, length;
    bool flash_addr_set = false;
    bool length_set = false;
    void *err_addr;
    struct option_info opts[2];

    init_opts(&opts[0], 'f', true, OPTION_ARG_TYPE_NUM, 
              (void **)&flash_addr, (bool *)&flash_addr_set, "FLASH memory base address");
    init_opts(&opts[1], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)0, 0, ""))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!flash_addr_set || !length_set) {
        fis_usage("missing argument");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((stat = flash_unlock((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error unlocking at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}
#endif

static void
fis_delete(int argc, char *argv[])
{
    char *name;
    int i, stat;
    void *fis_addr, *err_addr;
    struct fis_image_desc *img;
    bool slot_found;

    if (!scan_opts(argc, argv, 2, 0, 0, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct fis_image_desc *)fis_work_block;
    img += 2;  // Skip reserved files
    for (i = 2;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            if (!verify_action("Delete image '%s'", name)) {
                return;
            } else {
                slot_found = true;
                break;
            }
        }
    }
    if (!slot_found) {
        printf("No image '%s' found\n", name);
        return;
    }
    // Erase Data blocks (free space)
    if ((stat = flash_erase((void *)img->flash_base, img->size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
    // Update directory
    memset(img, 0xFF, sizeof(*img));
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Insure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_addr, block_size, (void **)&err_addr);
#endif
}

static void
fis_load(int argc, char *argv[])
{
    char *name;
    struct fis_image_desc *img;
    unsigned long mem_addr;
    bool mem_addr_set = false;
    bool show_cksum = false;
    struct option_info opts[2];
    unsigned long cksum;

    init_opts(&opts[0], 'b', true, OPTION_ARG_TYPE_NUM, 
              (void **)&mem_addr, (bool *)&mem_addr_set, "memory [load] base address");
    init_opts(&opts[1], 'c', false, OPTION_ARG_TYPE_FLG, 
              (void **)&show_cksum, (bool *)0, "display checksum");
    if (!scan_opts(argc, argv, 2, opts, 2, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }
    if ((img = fis_lookup(name)) == (struct fis_image_desc *)0) {
        printf("No image '%s' found\n", name);
        return;
    }
    if (!mem_addr_set) {
        mem_addr = img->mem_base;
    }
    // Load image from FLASH into RAM
    if ((mem_addr < (unsigned long)ram_start) ||
        ((mem_addr+img->size) >= (unsigned long)ram_end)) {
        printf("Not a loadable image\n");
        return;
    }
    memcpy((void *)mem_addr, (void *)img->flash_base, img->size);
    entry_address = (unsigned long *)img->entry_point;
    cksum = crc32((unsigned char *)mem_addr, img->data_length);
    if (show_cksum) {
        printf("Checksum: 0x%08lx\n", cksum);
    }
    if (img->file_cksum) {
        if (cksum != img->file_cksum) {
            printf("** Warning - checksum failure.  stored: 0x%08lx, computed: 0x%08lx\n",
                   img->file_cksum, cksum);
        }
    }
}

static bool
do_flash_init(void)
{
    int stat;
    static int init = 0;

    if (!init) {
        init = 1;
        if ((stat = flash_init((void *)(ram_end-FLASH_MIN_WORKSPACE), 
                               FLASH_MIN_WORKSPACE)) != 0) {
            printf("FLASH: driver init failed!, status: 0x%x\n", stat);
            return false;
        }
        flash_get_limits((void *)0, (void **)&flash_start, (void **)&flash_end);
        flash_get_block_info(&block_size, &blocks);
        printf("FLASH: %p - %p, %d blocks of %p bytes each.\n", 
               flash_start, flash_end, blocks, (void *)block_size);
        fis_work_block = (unsigned char *)(ram_end-FLASH_MIN_WORKSPACE-block_size);
    }
    return true;
}

void
do_fis(int argc, char *argv[])
{
    struct cmd *cmd;

    if (argc < 2) {
        fis_usage("too few arguments");
        return;
    }
    if (!do_flash_init()) return;
    if ((cmd = cmd_search(__FIS_cmds_TAB__, &__FIS_cmds_TAB_END__, 
                          argv[1])) != (struct cmd *)0) {
        (cmd->fun)(argc, argv);
        return;
    }
    fis_usage("unrecognized command");
}

#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <flash_config.h>

// Configuration data, saved in FLASH, used to set/update RedBoot
// normal "configuration" data items.
static struct _config {
    unsigned long len;
    unsigned long key1;
    unsigned char config_data[MAX_CONFIG_DATA-(4*4)];
    unsigned long key2;
    unsigned long cksum;
} config;
static bool config_ok;

#define CONFIG_KEY1    0x0BADFACE
#define CONFIG_KEY2    0xDEADDEAD

#define CONFIG_DONE    0
#define CONFIG_ABORT  -1
#define CONFIG_CHANGED 1
#define CONFIG_OK      2
#define CONFIG_BACK    3
#define CONFIG_BAD     4

// Note: the following options are related.  If 'bootp' is false, then
// the other values are used in the configuration.  Because of the way
// that configuration tables are generated, they should have names which
// are related.  The configuration options will show up lexicographically
// ordered, thus the peculiar naming.
RedBoot_config_option("Run script at boot",
                      boot_script,
                      ALWAYS_ENABLED, true,
                      CONFIG_BOOL,
                      false
    );
RedBoot_config_option("Boot script",
                      boot_script_data,
                      "boot_script", true,
                      CONFIG_SCRIPT,
                      ""
    );
// Some preprocessor magic for building the [constant] prompt string
#define __cat(s1,c2,s3) s1 ## #c2 ## s3
#define _cat(s1,c2,s3) __cat(s1,c2,s3)
RedBoot_config_option(_cat("Boot script timeout (",
                           CYGNUM_REDBOOT_FLASH_SCRIPT_TIMEOUT_RESOLUTION,
                           "ms resolution)"),
                      boot_script_timeout,
                      "boot_script", true,
                      CONFIG_INT,
                      0
    );
#undef __cat
#undef _cat

CYG_HAL_TABLE_BEGIN( __CONFIG_options_TAB__, RedBoot_config_options);
CYG_HAL_TABLE_END( __CONFIG_options_TAB_END__, RedBoot_config_options);

extern struct config_option __CONFIG_options_TAB__[], __CONFIG_options_TAB_END__[];

// 
// Layout of config data
// Each data item is variable length, with the name, type and dependencies
// encoded into the object.
//  offset   contents
//       0   data type
//       1   length of name (N)
//       2   enable sense
//       3   length of enable key (M)
//       4   key name
//     N+4   enable key
//   M+N+4   data value
//

#define CONFIG_OBJECT_TYPE(dp)          (dp)[0]
#define CONFIG_OBJECT_KEYLEN(dp)        (dp)[1]
#define CONFIG_OBJECT_ENABLE_SENSE(dp)  (dp)[2]
#define CONFIG_OBJECT_ENABLE_KEYLEN(dp) (dp)[3]
#define CONFIG_OBJECT_KEY(dp)           ((dp)+4)
#define CONFIG_OBJECT_ENABLE_KEY(dp)    ((dp)+4+CONFIG_OBJECT_KEYLEN(dp))
#define CONFIG_OBJECT_VALUE(dp)         ((dp)+4+CONFIG_OBJECT_KEYLEN(dp)+CONFIG_OBJECT_ENABLE_KEYLEN(dp))

static int
get_config(unsigned char *dp, char *title, bool list_only)
{
    char line[256], *sp, *lp;
    int ret;
    bool hold_bool_val, new_bool_val, enable;
    unsigned long hold_int_val, new_int_val;
#ifdef CYGPKG_REDBOOT_NETWORKING
    in_addr_t hold_ip_val, new_ip_val;
    enet_addr_t hold_esa_val;
    int esa_ptr;
    char *esp;
#endif
    void *val_ptr;
    int type;

    if (CONFIG_OBJECT_ENABLE_KEYLEN(dp)) {
        flash_get_config(CONFIG_OBJECT_ENABLE_KEY(dp), &enable, CONFIG_BOOL);
        if (((bool)CONFIG_OBJECT_ENABLE_SENSE(dp) && !enable) ||
            (!(bool)CONFIG_OBJECT_ENABLE_SENSE(dp) && enable)) {
            return CONFIG_OK;  // Disabled field
        }
    }
    val_ptr = (void *)CONFIG_OBJECT_VALUE(dp);
    if (title != (char *)NULL) {
        printf("%s: ", title);
    } else {
        printf("%s: ", CONFIG_OBJECT_KEY(dp));
    }
    switch (type = CONFIG_OBJECT_TYPE(dp)) {
    case CONFIG_BOOL:
        memcpy(&hold_bool_val, val_ptr, sizeof(bool));
        printf("%s ", hold_bool_val ? "true" : "false");
        break;
    case CONFIG_INT:
        memcpy(&hold_int_val, val_ptr, sizeof(unsigned long));
        printf("%ld ", hold_int_val);
        break;
#ifdef CYGPKG_REDBOOT_NETWORKING
    case CONFIG_IP:
        printf("%s ", inet_ntoa((in_addr_t *)val_ptr));
        break;
    case CONFIG_ESA:
        for (esa_ptr = 0;  esa_ptr < sizeof(enet_addr_t);  esa_ptr++) {
            printf("0x%02X", ((unsigned char *)val_ptr)[esa_ptr]);
            if (esa_ptr < (sizeof(enet_addr_t)-1)) printf(":");
        }
        printf(" ");
        break;
#endif
    case CONFIG_STRING:
        printf("??");
        return CONFIG_OK;  // FIXME - skip for now
    case CONFIG_SCRIPT:
        printf("\n");
        sp = lp = (unsigned char *)val_ptr;
        while (*sp) {
            while (*lp != '\n') lp++;
            *lp = '\0';
            printf(".. %s\n", sp);
            *lp++ = '\n';
            sp = lp;
        }
        break;
    }
    if (list_only) {
        printf("\n");
        return CONFIG_OK;
    }
    if (type != CONFIG_SCRIPT) {
        ret = gets(line, sizeof(line), 0);    
        if (ret < 0) return CONFIG_ABORT;
        if (strlen(line) == 0) return CONFIG_OK;  // Just a CR - leave value untouched
        if (line[0] == '.') return CONFIG_DONE;
        if (line[0] == '^') return CONFIG_BACK;
    }
    switch (type) {
    case CONFIG_BOOL:
        memcpy(&hold_bool_val, val_ptr, sizeof(bool));
        if (!parse_bool(line, &new_bool_val)) {
            return CONFIG_BAD;
        }
        if (hold_bool_val != new_bool_val) {
            memcpy(val_ptr, &new_bool_val, sizeof(bool));
            return CONFIG_CHANGED;
        } else {
            return CONFIG_OK;
        }
        break;
    case CONFIG_INT:
        memcpy(&hold_int_val, val_ptr, sizeof(unsigned long));
        if (!parse_num(line, &new_int_val, 0, 0)) {
            return CONFIG_BAD;
        }
        if (hold_int_val != new_int_val) {
            memcpy(val_ptr, &new_int_val, sizeof(unsigned long));
            return CONFIG_CHANGED;
        } else {
            return CONFIG_OK;
        }
        break;
#ifdef CYGPKG_REDBOOT_NETWORKING
    case CONFIG_IP:
        memcpy(&hold_ip_val.s_addr, &((in_addr_t *)val_ptr)->s_addr, sizeof(in_addr_t));
        if (!inet_aton(line, &new_ip_val)) {
            return CONFIG_BAD;
        }
        if (hold_ip_val.s_addr != new_ip_val.s_addr) {
            memcpy(val_ptr, &new_ip_val, sizeof(in_addr_t));
            return CONFIG_CHANGED;
        } else {
            return CONFIG_OK;
        }
        break;
    case CONFIG_ESA:
        memcpy(&hold_esa_val, val_ptr, sizeof(enet_addr_t));
        esp = line;
        for (esa_ptr = 0;  esa_ptr < sizeof(enet_addr_t);  esa_ptr++) {
            unsigned long esa_byte;
            if (!parse_num(esp, &esa_byte, &esp, ":")) {
                memcpy(val_ptr, &hold_esa_val, sizeof(enet_addr_t));
                return CONFIG_BAD;
            }
            ((unsigned char *)val_ptr)[esa_ptr] = esa_byte;
        }
        return CONFIG_CHANGED;
        break;
#endif
    case CONFIG_SCRIPT:
        // Assume it always changes
        sp = (unsigned char *)val_ptr;
        printf("Enter script, terminate with empty line\n");
        while (true) {
            *sp = '\0';
            printf(">> ");
            ret = gets(line, sizeof(line), 0);
            if (ret < 0) return CONFIG_ABORT;
            if (strlen(line) == 0) break;
            lp = line;
            while (*lp) {
                *sp++ = *lp++;
            }
            *sp++ = '\n';
        }
        break;
    case CONFIG_STRING:
        printf("??");
    }
    return CONFIG_CHANGED;
}

//
// Manage configuration information with the FLASH
//

static int
config_length(int type)
{
    switch (type) {
    case CONFIG_BOOL:
        return sizeof(bool);
    case CONFIG_INT:
        return sizeof(unsigned long);
#ifdef CYGPKG_REDBOOT_NETWORKING
    case CONFIG_IP:
        return sizeof(in_addr_t);
    case CONFIG_ESA:
        // Would like this to be sizeof(enet_addr_t), but that causes much
        // pain since it fouls the alignment of data which follows.
        return 8;
#endif
    case CONFIG_STRING:
        return 0;
    case CONFIG_SCRIPT:
        return MAX_SCRIPT_LENGTH;
    }
}

void
do_flash_config(int argc, char *argv[])
{
    bool need_update = false;
    struct config_option *optend = __CONFIG_options_TAB_END__;
    struct config_option *opt = __CONFIG_options_TAB__;
    struct _config hold_config;
    struct option_info opts[1];
    bool list_only;
    unsigned char *dp;
    int len, ret;
    char *title;

    if (!do_flash_init()) return;
    memcpy(&hold_config, &config, sizeof(config));
    script = (unsigned char *)0;

    init_opts(&opts[0], 'l', false, OPTION_ARG_TYPE_FLG, 
              (void **)&list_only, (bool *)0, "list configuration only");
    if (!scan_opts(argc, argv, 1, opts, 1, 0, 0, ""))
    {
        return;
    }

    dp = &config.config_data[0];
    while (dp < &config.config_data[sizeof(config.config_data)]) {
        if (CONFIG_OBJECT_TYPE(dp) == CONFIG_EMPTY) {
            break;
        }
        len = 4 + CONFIG_OBJECT_KEYLEN(dp) + CONFIG_OBJECT_ENABLE_KEYLEN(dp) + 
            config_length(CONFIG_OBJECT_TYPE(dp));
        // Provide a title for well known [i.e. builtin] objects
        title = (char *)NULL;
        opt = __CONFIG_options_TAB__;
        while (opt != optend) {
            if (strcmp(opt->key, CONFIG_OBJECT_KEY(dp)) == 0) {
                title = opt->title;
                break;
            }
            opt++;
        }
        ret = get_config(dp, title, list_only);
        switch (ret) {
        case CONFIG_DONE:
            goto done;
        case CONFIG_ABORT:
            memcpy(&config, &hold_config, sizeof(config));
            return;
        case CONFIG_CHANGED:
            need_update = true;
        case CONFIG_OK:
            dp += len;
            break;
        case CONFIG_BACK:
            dp = &config.config_data[0];
            continue;
        case CONFIG_BAD:
            // Nothing - make him do it again
            printf ("** invalid entry\n");
        }
    }

 done:
    if (!need_update) return;
    flash_write_config();
}

//
// Write the in-memory copy of the configuration data to the flash device.
//
void
flash_write_config(void)
{
    int stat;
    void *cfg_base, *err_addr;

    config.len = sizeof(config);
    config.key1 = CONFIG_KEY1;  
    config.key2 = CONFIG_KEY2;
    config.cksum = crc32((unsigned char *)&config, sizeof(config)-sizeof(config.cksum));
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    if (verify_action("Update RedBoot non-volatile configuration")) {
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
        // Insure [quietly] that the config page is unlocked before trying to update
        flash_unlock((void *)cfg_base, block_size, (void **)&err_addr);
#endif
        if ((stat = flash_erase(cfg_base, block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: 0x%x(%s)\n", err_addr, stat, flash_errmsg(stat));
        } else {
            if ((stat = flash_program(cfg_base, (void *)&config, 
                                      sizeof(config), (void **)&err_addr)) != 0) {
                printf("Error writing config data at %p: 0x%x(%s)\n", 
                       err_addr, stat, flash_errmsg(stat));
            }
        }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
        // Insure [quietly] that the config data is locked after the update
        flash_lock((void *)cfg_base, block_size, (void **)&err_addr);
#endif
    }
}

//
// Retrieve a data object from the data base (in memory copy)
//
bool
flash_get_config(char *key, void *val, int type)
{
    unsigned char *dp;
    void *val_ptr;
    int len;

    if (!config_ok) return false;

    dp = &config.config_data[0];
    while (dp < &config.config_data[sizeof(config.config_data)]) {
        len = 4 + CONFIG_OBJECT_KEYLEN(dp) + CONFIG_OBJECT_ENABLE_KEYLEN(dp) +
            config_length(CONFIG_OBJECT_TYPE(dp));
        val_ptr = (void *)CONFIG_OBJECT_VALUE(dp);
        if (strcmp(key, CONFIG_OBJECT_KEY(dp)) == 0) {
            if (CONFIG_OBJECT_TYPE(dp) == type) {
                switch (type) {
                    // Note: the data may be unaligned in the configuration data
                case CONFIG_BOOL:
                    memcpy(val, val_ptr, sizeof(bool));
                    break;
                case CONFIG_INT:
                    memcpy(val, val_ptr, sizeof(unsigned long));
                    break;
#ifdef CYGPKG_REDBOOT_NETWORKING
                case CONFIG_IP:
                    memcpy(val, val_ptr, sizeof(in_addr_t));
                    break;
                case CONFIG_ESA:
                    memcpy(val, val_ptr, sizeof(enet_addr_t));
                    break;
#endif
                case CONFIG_STRING:
                    break;
                case CONFIG_SCRIPT:
                    // Just return a pointer to the script
                    *(unsigned char **)val = (unsigned char *)val_ptr;
                    break;
                }
            } else {
                printf("Request for config value '%s' - wrong type\n", key);
            }
            return true;
        }
        dp += len;
    }
    printf("Can't find config data for '%s'\n", key);
    return false;
}

//
// Add a new option to the database
//
bool
flash_add_config(struct config_option *opt)
{
    unsigned char *dp, *kp;
    int len, elen, size;

    dp = &config.config_data[0];
    size = 0;
    while (size < sizeof(config.config_data)) {
        if (CONFIG_OBJECT_TYPE(dp) == CONFIG_EMPTY) {
            kp = opt->key;
            len = strlen(kp) + 1;
            size += len + 2 + 2 + config_length(opt->type);
            if (opt->enable) {
                elen = strlen(opt->enable) + 1;
                size += elen;
            } else {
                elen = 0;
            }
            if (size > sizeof(config.config_data)) {
                break;
            }
            CONFIG_OBJECT_TYPE(dp) = opt->type; 
            CONFIG_OBJECT_KEYLEN(dp) = len;
            CONFIG_OBJECT_ENABLE_SENSE(dp) = opt->enable_sense;
            CONFIG_OBJECT_ENABLE_KEYLEN(dp) = elen;
            dp = CONFIG_OBJECT_KEY(dp);
            while (*kp) *dp++ += *kp++;
            *dp++ = '\0';    
            if (elen) {
                kp = opt->enable;
                while (*kp) *dp++ += *kp++;
                *dp++ = '\0';    
            }
            switch (opt->type) {
                // Note: the data may be unaligned in the configuration data
            case CONFIG_BOOL:
                memcpy(dp, (void *)&opt->dflt, sizeof(bool));
                break;
            case CONFIG_INT:
                memcpy(dp, (void *)&opt->dflt, sizeof(unsigned long));
                break;
#ifdef CYGPKG_REDBOOT_NETWORKING
            case CONFIG_IP:
                memcpy(dp, (void *)&opt->dflt, sizeof(in_addr_t));
                break;
            case CONFIG_ESA:
                memcpy(dp, (void *)&opt->dflt, sizeof(enet_addr_t));
                break;
#endif
            case CONFIG_STRING:
            case CONFIG_SCRIPT:
                break;
            }
            dp += config_length(opt->type);
            return true;
        } else {
            len = 4 + CONFIG_OBJECT_KEYLEN(dp) + CONFIG_OBJECT_ENABLE_KEYLEN(dp) +
                config_length(CONFIG_OBJECT_TYPE(dp));
            dp += len;
            size += len;
        }
    }
    printf("No space to add '%s'\n", opt->key);
    return false;
}

//
// Reset/initialize configuration data - used only when starting from scratch
//
static void
config_init(void)
{
    // Well known option strings
    struct config_option *optend = __CONFIG_options_TAB_END__;
    struct config_option *opt = __CONFIG_options_TAB__;

    memset(&config, 0, sizeof(config));
    while (opt != optend) {
        if (!flash_add_config(opt)) {
            return;
        }
        opt++;
    }
    config_ok = true;
}

//
// Attempt to get configuration information from the FLASH.
// If available (i.e. good checksum, etc), initialize "known"
// values for later use.
//
static void
load_flash_config(void)
{
    void *cfg_base;
    bool use_boot_script;

    config_ok = false;
    script = (unsigned char *)0;
    if (!do_flash_init()) return;
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    memcpy(&config, cfg_base, sizeof(config));
    if ((crc32((unsigned char *)&config, sizeof(config)-sizeof(config.cksum)) != config.cksum) ||
        (config.key1 != CONFIG_KEY1)|| (config.key2 != CONFIG_KEY2)) {
        printf("FLASH configuration checksum error or invalid key\n");
        config_init();
        return;
    }
    flash_get_config("boot_script", &use_boot_script, CONFIG_BOOL);
    if (use_boot_script) {
        flash_get_config("boot_script_data", &script, CONFIG_SCRIPT);
        flash_get_config("boot_script_timeout", &script_timeout, CONFIG_INT);
    }
    config_ok = true;
}

RedBoot_init(load_flash_config, RedBoot_INIT_FIRST);

#endif
