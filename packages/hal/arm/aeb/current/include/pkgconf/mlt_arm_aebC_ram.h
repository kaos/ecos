// eCos memory layout - Tue Nov 09 07:03:07 1999

// This is a generated file - changes will be lost if ConfigTool(MLT) is run

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0x48000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
extern char CYG_LABEL_NAME (_reserved_vectors) [];
#define CYGMEM_SECTION_reserved_vectors (CYG_LABEL_NAME (_reserved_vectors))
#define CYGMEM_SECTION_reserved_vectors_SIZE (0x1000)
extern char CYG_LABEL_NAME (_reserved_not_mapped) [];
#define CYGMEM_SECTION_reserved_not_mapped (CYG_LABEL_NAME (_reserved_not_mapped))
#define CYGMEM_SECTION_reserved_not_mapped_SIZE (0x7000)
extern char CYG_LABEL_NAME (_reserved_rom_usage) [];
#define CYGMEM_SECTION_reserved_rom_usage (CYG_LABEL_NAME (_reserved_rom_usage))
#define CYGMEM_SECTION_reserved_rom_usage_SIZE (0x4000)
