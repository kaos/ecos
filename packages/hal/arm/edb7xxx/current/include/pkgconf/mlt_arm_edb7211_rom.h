// eCos memory layout - Mon Nov 22 15:15:11 1999

// This is a generated file - changes will be lost if ConfigTool(MLT) is run

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0xfd7000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_sram (0x60000000)
#define CYGMEM_REGION_sram_SIZE (0x9c00)
#define CYGMEM_REGION_sram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_rom (0xe0000000)
#define CYGMEM_REGION_rom_SIZE (0x800000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
extern char CYG_LABEL_NAME (_reserved) [];
#define CYGMEM_SECTION_reserved (CYG_LABEL_NAME (_reserved))
#define CYGMEM_SECTION_reserved_SIZE (0x1000 - (size_t) CYG_LABEL_NAME (_reserved))
extern char CYG_LABEL_NAME (_sram) [];
#define CYGMEM_SECTION_sram (CYG_LABEL_NAME (_sram))
#define CYGMEM_SECTION_sram_SIZE (0x60009c00 - (size_t) CYG_LABEL_NAME (_sram))
