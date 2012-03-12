// eCos memory layout

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
#define CYGMEM_REGION_sram (0x20000000)
#define CYGMEM_REGION_sram_SIZE (0x00010000-CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE)
#define CYGMEM_REGION_sram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

// Only to get Redboot to compile..
#define CYGMEM_REGION_ram (CYGMEM_REGION_sram)
#define CYGMEM_REGION_ram_SIZE (CYGMEM_REGION_sram_SIZE)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_sram_ATTR)

#define CYGMEM_REGION_flash (0x60000000)
#define CYGMEM_REGION_flash_SIZE (0x00040000)
#define CYGMEM_REGION_flash_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (CYGMEM_REGION_sram_SIZE+CYGMEM_REGION_sram_SIZE - (size_t) CYG_LABEL_NAME (__heap1))
