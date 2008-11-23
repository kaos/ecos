#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif

#define CYGMEM_REGION_rom (0x00000000)
#define CYGMEM_REGION_rom_SIZE (0x00040000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
#define CYGMEM_REGION_sram (0x40000000)
#define CYGMEM_REGION_sram_SIZE (0x00004000)
#define CYGMEM_REGION_sram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_ram (0x81000000)
#define CYGMEM_REGION_ram_SIZE (0x00100000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];

#endif

#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))

#define CYGMEM_SECTION_heap1_SIZE (0x81100000 - (size_t) CYG_LABEL_NAME (__heap1))

