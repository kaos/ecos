// eCos memory layout - Thu Jun 15 15:46:43 2000

// This is a generated file - do not edit

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0x400000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_rom (0xfe000000)
#define CYGMEM_REGION_rom_SIZE (0x800000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
extern char CYG_LABEL_NAME (_reserved_vectors) [];
#define CYGMEM_SECTION_reserved_vectors (CYG_LABEL_NAME (_reserved_vectors))
#define CYGMEM_SECTION_reserved_vectors_SIZE (0x3000)
extern char CYG_LABEL_NAME (_reserved_vsr_table) [];
#define CYGMEM_SECTION_reserved_vsr_table (CYG_LABEL_NAME (_reserved_vsr_table))
#define CYGMEM_SECTION_reserved_vsr_table_SIZE (0x200)
extern char CYG_LABEL_NAME (_reserved_virtual_table) [];
#define CYGMEM_SECTION_reserved_virtual_table (CYG_LABEL_NAME (_reserved_virtual_table))
#define CYGMEM_SECTION_reserved_virtual_table_SIZE (0x100)
