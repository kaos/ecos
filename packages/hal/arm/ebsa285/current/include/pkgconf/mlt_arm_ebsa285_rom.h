// eCos memory layout - Tue Jul 25 19:45:02 2000

// This is a generated file - do not edit - ho ho ho.

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#define CYGMEM_REGION_ram (0)

#ifndef CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE
// Then no PCI window is to be reserved.
#define CYGMEM_REGION_ram_SIZE (0x1000000)
#else
// Do not trample the PCI window:
#define CYGMEM_REGION_ram_SIZE CYGHWR_HAL_ARM_EBSA285_PCI_MEM_MAP_BASE
#endif

#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_rom (0x41000000)
#define CYGMEM_REGION_rom_SIZE (0x400000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)

extern char CYG_LABEL_NAME (__heap1) [];
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE \
     (CYGMEM_REGION_ram_SIZE - (size_t) CYG_LABEL_NAME (__heap1))

