// eCos memory layout - Tue Feb 29 14:28:13 2000

// This is a generated file - do not edit

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0x28000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
extern char CYG_LABEL_NAME (_reserved_vectors) [];
#define CYGMEM_SECTION_reserved_vectors (CYG_LABEL_NAME (_reserved_vectors))
#define CYGMEM_SECTION_reserved_vectors_SIZE (0x1000)
extern char CYG_LABEL_NAME (_reserved_not_mapped) [];
#define CYGMEM_SECTION_reserved_not_mapped (CYG_LABEL_NAME (_reserved_not_mapped))
#define CYGMEM_SECTION_reserved_not_mapped_SIZE (0x7000)
extern char CYG_LABEL_NAME (_reserved_for_rom) [];
#define CYGMEM_SECTION_reserved_for_rom (CYG_LABEL_NAME (_reserved_for_rom))
#define CYGMEM_SECTION_reserved_for_rom_SIZE (0x4000)
