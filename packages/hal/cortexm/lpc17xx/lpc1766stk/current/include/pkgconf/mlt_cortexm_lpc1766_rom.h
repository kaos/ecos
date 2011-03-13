// eCos memory layout

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
#define CYGMEM_REGION_ram (0x10000000)
#define CYGMEM_REGION_ram_SIZE (0x00008000-CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_ahb_sram_bank0 (0x2007C000)
#define CYGMEM_REGION_ahb_sram_bank0_SIZE (0x00004000)
#define CYGMEM_REGION_ahb_sram_bank0_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_ahb_sram_bank1 (0x20080000)
#define CYGMEM_REGION_ahb_sram_bank1_SIZE (0x00004000)
#define CYGMEM_REGION_ahb_sram_bank1_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#define CYGMEM_REGION_flash (0x00000000)
#define CYGMEM_REGION_flash_SIZE (0x00040000)
#define CYGMEM_REGION_flash_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE - (size_t) CYG_LABEL_NAME (__heap1))


