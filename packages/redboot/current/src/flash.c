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

struct image_desc {
    unsigned char name[16];      // Null terminated name
    unsigned long flash_base;    // Address within FLASH of image
    unsigned long mem_base;      // Address in memory where it executes
    unsigned long size;          // Length of image
    unsigned long entry_point;   // Execution entry point
    unsigned char _pad[256-40];
    unsigned long desc_cksum;    // Checksum over image descriptor
    unsigned long file_cksum;    // Checksum over image data
};

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
                "",
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
local_cmd_entry("delete",
                "Display an image from FLASH Image System [FIS]",
                "name",
                fis_delete,
                FIS_cmds
    );
local_cmd_entry("load",
                "Load image from FLASH Image System [FIS] into RAM",
                "name",
                fis_load,
                FIS_cmds
    );
local_cmd_entry("create",
                "Create an image",
                "-b <mem_base> -l <length> [-f <flash_addr>] [-e <entry_point>] [-r <ram_addr>] <name>",
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
    struct image_desc *img;
    void *fis_base, *err_addr;
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    void *cfg_base;
#endif
    bool full_init = false;
    struct option_info opts[1];

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
    if (full_init) {
        if ((stat = flash_erase((void *)((unsigned long)flash_start+(2*block_size)), 
                                (blocks-4)*block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    } else {
        printf("    Warning: device contents not erased, some blocks may not be usable\n");
    }
    // Create a pseudo image for RedBoot
    img = (struct image_desc *)fis_work_block;
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot");
    img->flash_base = (unsigned long)flash_start;
    img->mem_base = (unsigned long)flash_start;
    img->size = 0;
    img++;  img_count++;
    // And a backup image
    memset(img, 0, sizeof(*img));
    strcpy(img->name, "RedBoot[backup]");
    img->flash_base = (unsigned long)flash_start+block_size;
    img->mem_base = (unsigned long)flash_start+block_size;
    img->size = 0;
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
    if ((stat = flash_erase(fis_base, block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
    } else {
        if ((stat = flash_program(fis_base, fis_work_block, 
                                  img_count*sizeof(*img), (void **)&err_addr)) != 0) {
            printf("Error writing image descriptors at %p: %x(%s)\n", 
                   err_addr, stat, flash_errmsg(stat));
        }
    }
}

static void
fis_list(int argc, char *argv[])
{
    struct image_desc *img;
    int i;

    img = (struct image_desc *)((unsigned long)flash_end - block_size);    
    printf("Name              FLASH addr   Mem addr    Length    Entry point\n");
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if (img->name[0] != (unsigned char)0xFF) {
            printf("%-16s  0x%08lX   0x%08lX  0x%06lX  0x%08lX\n", img->name, 
                   img->flash_base, img->mem_base, img->size, img->entry_point);
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
    unsigned long mem_addr, exec_addr, flash_addr, entry_addr, length;
    char *name;
    bool mem_addr_set = false;
    bool exec_addr_set = false;
    bool entry_addr_set = false;
    bool flash_addr_set = false;
    bool length_set = false;
    void *fis_addr, *err_addr;
    struct image_desc *img;
    bool slot_found;
    struct option_info opts[5];
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
              (void **)&length, (bool *)&length_set, "length");
    if (!scan_opts(argc, argv, 2, opts, 5, (void *)&name, OPTION_ARG_TYPE_STR, "file name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    if (!mem_addr_set || !length_set || !name) {
        fis_usage("required parameter missing");
        return;
    }
    if (flash_addr_set &&
        ((stat = flash_verify_addr((void *)flash_addr)) ||
         (stat = flash_verify_addr((void *)(flash_addr+length-1))))) {
        printf("Invalid FLASH address: %p (%s)\n", (void *)flash_addr, flash_errmsg(stat));
        printf("   valid range is %p-%p\n", (void *)flash_start, (void *)flash_end);
        return;
    }
    if ((mem_addr < (unsigned long)ram_start) ||
        ((mem_addr+length) >= (unsigned long)ram_end)) {
        printf("Invalid RAM address: %p\n", (void *)mem_addr);
        printf("   valid range is %p-%p\n", (void *)ram_start, (void *)ram_end);
        return;
    }
    if (strlen(name) >= sizeof(img->name)) {
        printf("Name is too long, must be less than %d chars\n", (int)sizeof(img->name));
        return;
    }
    if (!flash_addr_set && !fis_find_free(&flash_addr, length)) {
        printf("Can't locate %ld bytes free in FLASH\n", length);
        return;
    }
    // Find a slot in the directory for this entry
    // First, see if an image by this name is already present
    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            if (img->flash_base != flash_addr) {
                printf("Image found, but FLASH address incorrect\n");
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
        img = (struct image_desc *)fis_work_block;
        for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
            if (img->name[0] == (unsigned char)0xFF) {
                slot_found = true;
                break;
            }
        }
    }
    // Safety check - make sure the address range is not within the code we're running
    if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1))) {
        printf("Can't program this region - contains code in use!\n");
        return;
    }
    prog_ok = true;
    if (prog_ok) {
        // Erase area to be programmed
        if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
            printf("Can't erase region at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
            prog_ok = false;
        }
    }
    if (prog_ok) {
        // Now program it
        if ((stat = flash_program((void *)flash_addr, (void *)mem_addr, length, (void **)&err_addr)) != 0) {
            printf("Can't program region at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
            prog_ok = false;
        }
    }
    // Update directory
    memset(img, 0, sizeof(*img));
    strcpy(img->name, name);
    img->flash_base = flash_addr;
    img->mem_base = exec_addr_set ? exec_addr : (flash_addr_set ? flash_addr : mem_addr);
    img->entry_point = entry_addr_set ? entry_addr : (unsigned long)entry_address;  // Hope it's been set
    img->size = length;
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
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
        printf("Can't program this region - contains code in use!\n");
        return;
    }
    if ((stat = flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
}

static void
fis_delete(int argc, char *argv[])
{
    char *name;
    int i, stat;
    void *fis_addr, *err_addr;
    struct image_desc *img;
    bool slot_found;

    if (!scan_opts(argc, argv, 2, 0, 0, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct image_desc *)fis_work_block;
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
    // Erase data blocks (free space)
    if ((stat = flash_erase((void *)img->flash_base, img->size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
    }
    // Update directory
    memset(img, 0xFF, sizeof(*img));
    if ((stat = flash_erase((void *)fis_addr, block_size, (void **)&err_addr)) != 0) {
        printf("Error erasing at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        // Don't try to program if the erase failed
    } else {
        // Now program it
        if ((stat = flash_program((void *)fis_addr, (void *)fis_work_block, block_size, (void **)&err_addr)) != 0) {
            printf("Error programming at %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        }
    }
}

static void
fis_load(int argc, char *argv[])
{
    char *name;
    int i;
    void *fis_addr;
    struct image_desc *img;
    bool slot_found;

    if (!scan_opts(argc, argv, 2, 0, 0, (void **)&name, OPTION_ARG_TYPE_STR, "image name"))
    {
        fis_usage("invalid arguments");
        return;
    }

    slot_found = false;
    fis_addr = (void *)((unsigned long)flash_end - block_size);
    memcpy(fis_work_block, fis_addr, block_size);
    img = (struct image_desc *)fis_work_block;
    for (i = 0;  i < block_size/sizeof(*img);  i++, img++) {
        if ((img->name[0] != (unsigned char)0xFF) && (strcmp(name, img->name) == 0)) {
            slot_found = true;
            break;
        }
    }
    if (!slot_found) {
        printf("No image '%s' found\n", name);
        return;
    }
    // Load image from FLASH into RAM
    if ((img->mem_base < (unsigned long)ram_start) ||
        ((img->mem_base+img->size) >= (unsigned long)ram_end)) {
        printf("Not a loadable image\n");
        return;
    }
    memcpy((void *)img->mem_base, (void *)img->flash_base, img->size);
    entry_address = (unsigned long *)img->entry_point;
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
            printf("FLASH: driver init failed!, status: %x\n", stat);
            return false;
        }
        flash_get_limits((void *)0, (void **)&flash_start, (void **)&flash_end);
        flash_get_block_info(&block_size, &blocks);
        printf("FLASH: %p - %p, %d blocks of %p bytes ea.\n", 
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
    unsigned long key1;
    unsigned char config_data[1024-(3*4)];
    unsigned long key2;
    unsigned long cksum;
} config;

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
                      CONFIG_BOOL
    );
RedBoot_config_option("Boot script",
                      boot_script_data,
                      "boot_script", true,
                      CONFIG_SCRIPT
    );
RedBoot_config_option("Boot script timeout",
                      boot_script_timeout,
                      "boot_script", true,
                      CONFIG_INT
    );

CYG_HAL_TABLE_BEGIN( __CONFIG_options_TAB__, RedBoot_config_options);
CYG_HAL_TABLE_END( __CONFIG_options_TAB_END__, RedBoot_config_options);

extern struct config_option __CONFIG_options_TAB__[], __CONFIG_options_TAB_END__[];

static int
get_config(struct config_option *opt, int offset, bool list_only)
{
    char line[80], *sp, *lp;
    int ret;
    bool hold_bool_val, enable;
    unsigned long hold_int_val;
#ifdef CYGPKG_REDBOOT_NETWORKING
    in_addr_t hold_ip_val;
    enet_addr_t hold_esa_val;
    int esa_ptr;
    char *esp;
#endif
    void *val_ptr;

    if (opt->enable) {
        flash_get_config(opt->enable, &enable, CONFIG_BOOL);
        if ((opt->enable_sense && !enable) ||
            (!opt->enable_sense && enable)) {
            return CONFIG_OK;  // Disabled field
        }
    }
    val_ptr = (void *)((unsigned char *)&config + offset);
    printf("%s: ", opt->title);
    switch (opt->type) {
    case CONFIG_BOOL:
        printf("%s ", *(bool *)val_ptr ? "true" : "false");
        break;
    case CONFIG_INT:
        printf("%d ", *(int *)val_ptr);
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
    if (opt->type != CONFIG_SCRIPT) {
        ret = gets(line, sizeof(line), 0);    
        if (ret < 0) return CONFIG_ABORT;
        if (strlen(line) == 0) return CONFIG_OK;  // Just a CR - leave value untouched
        if (line[0] == '.') return CONFIG_DONE;
        if (line[0] == '^') return CONFIG_BACK;
    }
    switch (opt->type) {
    case CONFIG_BOOL:
        hold_bool_val = *(bool *)val_ptr;
        if (!parse_bool(line, (bool *)val_ptr)) {
            return CONFIG_BAD;
        }
        if (hold_bool_val != *(bool *)val_ptr) {
            return CONFIG_CHANGED;
        } else {
            return CONFIG_OK;
        }
        break;
    case CONFIG_INT:
        hold_int_val = *(unsigned long *)val_ptr;
        if (!parse_num(line, (unsigned long *)val_ptr, 0, 0)) {
            return CONFIG_BAD;
        }
        if (hold_int_val != *(unsigned long *)val_ptr) {
            return CONFIG_CHANGED;
        } else {
            return CONFIG_OK;
        }
        break;
#ifdef CYGPKG_REDBOOT_NETWORKING
    case CONFIG_IP:
        hold_ip_val.s_addr = ((in_addr_t *)val_ptr)->s_addr;
        if (!inet_aton(line, (in_addr_t *)val_ptr)) {
            return CONFIG_BAD;
        }
        if (hold_ip_val.s_addr != ((in_addr_t *)val_ptr)->s_addr) {
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

// Calculate a simple checksum.  This is used to validate configuration
// data.  Since it is used for sensitive information, it makes sense to
// be somewhat careful when accepting it.  Also, the [strange] addition
// of shifted data helps make this work even if the data layout changes
// over time.

static unsigned long
_cksum(unsigned long *buf, int len)
{
    unsigned long cksum = 0;
    int shift = 0;

    // Round 'len' up to multiple of longwords
    len = (len + (sizeof(unsigned long)-1)) / sizeof(unsigned long);   
    while (len-- > 0) {
        cksum ^= (*buf | (*buf << shift));
        buf++;
        if (++shift == 16) shift = 0;
    }
    return cksum;
}

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
    int stat, ret;
    void *cfg_base, *err_addr;
    bool need_update = false;
    struct config_option *optbeg = __CONFIG_options_TAB__;
    struct config_option *optend = __CONFIG_options_TAB_END__;
    struct config_option *opt = __CONFIG_options_TAB__;
    struct _config hold_config;
    int offset = sizeof(unsigned long);
    struct option_info opts[1];
    bool list_only;

    if (!do_flash_init()) return;
    memcpy(&hold_config, &config, sizeof(config));
    script = (unsigned char *)0;

    init_opts(&opts[0], 'l', false, OPTION_ARG_TYPE_FLG, 
              (void **)&list_only, (bool *)0, "list configuration only");
    if (!scan_opts(argc, argv, 1, opts, 1, 0, 0, ""))
    {
        return;
    }

    while (opt != optend) {
        ret = get_config(opt, offset, list_only);
        switch (ret) {
        case CONFIG_DONE:
            goto done;
        case CONFIG_ABORT:
            memcpy(&config, &hold_config, sizeof(config));
            return;
        case CONFIG_CHANGED:
            need_update = true;
        case CONFIG_OK:
            offset += config_length(opt->type);
            opt++;
            break;
        case CONFIG_BACK:
            if (opt != optbeg) opt--;
            continue;
        case CONFIG_BAD:
            // Nothing - make him do it again
            printf ("** invalid entry\n");
        }
    }

 done:
    if (!need_update) return;
    config.key1 = CONFIG_KEY1;  config.key2 = CONFIG_KEY2;
    config.cksum = _cksum((unsigned long *)&config, sizeof(config)-sizeof(config.cksum));
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    if (verify_action("Update RedBoot non-volatile configuration")) {
        if ((stat = flash_erase(cfg_base, block_size, (void **)&err_addr)) != 0) {
            printf("   initialization failed %p: %x(%s)\n", err_addr, stat, flash_errmsg(stat));
        } else {
            if ((stat = flash_program(cfg_base, (void *)&config, 
                                      sizeof(config), (void **)&err_addr)) != 0) {
                printf("Error writing config data at %p: %x(%s)\n", 
                       err_addr, stat, flash_errmsg(stat));
            }
        }
    }
}

void
flash_get_config(char *key, void *val, int type)
{
    struct config_option *optend = __CONFIG_options_TAB_END__;
    struct config_option *opt = __CONFIG_options_TAB__;
    int offset = sizeof(unsigned long);  // Offset past starting 'key'
    void *val_ptr;

    if (!do_flash_init()) return;

    while (opt != optend) {
        val_ptr = (void *)((unsigned char *)&config + offset);
        if (strcmp(opt->key, key) == 0) {
            if (opt->type == type) {
                switch (opt->type) {
                case CONFIG_BOOL:
                    *(bool *)val = *(bool *)val_ptr;
                    break;
                case CONFIG_INT:
                    *(unsigned long *)val = *(unsigned long *)val_ptr;
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
            return;
        }
        offset += config_length(opt->type);
        opt++;
    }

    printf("Can't find config value '%s'\n", key);
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
    if (!do_flash_init()) return;
    cfg_base = (void *)((unsigned long)flash_end - (2*block_size));
    memcpy(&config, cfg_base, sizeof(config));
    if ((_cksum((unsigned long *)&config, sizeof(config)-sizeof(config.cksum)) != config.cksum) ||
        (config.key1 != CONFIG_KEY1)|| (config.key2 != CONFIG_KEY2)) {
        printf("FLASH configuration checksum error or invalid key\n");
        script = (unsigned char *)0;
        memset(&config, 0, sizeof(config));
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
