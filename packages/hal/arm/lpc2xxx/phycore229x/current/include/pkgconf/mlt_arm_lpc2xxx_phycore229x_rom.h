// eCos memory layout - Wed Apr 11 13:49:55 2001

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
#define CYGMEM_REGION_sram (0x40000000)
#define CYGMEM_REGION_sram_SIZE (0x00004000)
#define CYGMEM_REGION_sram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_ram (0x81000000)
#define CYGMEM_REGION_ram_SIZE (CYGHWR_HAL_ARM_PHYCORE229X_SRAM_SIZE)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_rom (0x00000000)
#define CYGMEM_REGION_rom_SIZE (0x00040000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)

#define CYGMEM_REGION_flash (0x80000000)
#define CYGMEM_REGION_flash_SIZE (CYGHWR_HAL_ARM_PHYCORE229X_FLASH_SIZE)
#define CYGMEM_REGION_flash_ATTR (CYGMEM_REGION_ATTR_R)

#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (0x81000000 + CYGHWR_HAL_ARM_PHYCORE229X_SRAM_SIZE - (size_t) CYG_LABEL_NAME (__heap1))

