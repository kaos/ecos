// eCos memory layout - Tue Feb 29 14:30:16 2000

// This is a generated file - do not edit

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0x28000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_rom (0x4018000)
#define CYGMEM_REGION_rom_SIZE (0x8000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
extern char CYG_LABEL_NAME (_reserved_vectors) [];
#define CYGMEM_SECTION_reserved_vectors (CYG_LABEL_NAME (_reserved_vectors))
#define CYGMEM_SECTION_reserved_vectors_SIZE (0x1000)
extern char CYG_LABEL_NAME (_reserved_not_mapped) [];
#define CYGMEM_SECTION_reserved_not_mapped (CYG_LABEL_NAME (_reserved_not_mapped))
#define CYGMEM_SECTION_reserved_not_mapped_SIZE (0x7000)
