// eCos memory layout - Wed Oct 27 13:28:21 1999

// This is a generated file - changes will be lost if ConfigTool(MLT) is run

#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#define CYGMEM_REGION_ram (0)
#define CYGMEM_REGION_ram_SIZE (0x00FD7000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
extern char CYG_LABEL_NAME (_reserved) [];
#define CYGMEM_SECTION_reserved (CYG_LABEL_NAME (_reserved))
#define CYGMEM_SECTION_reserved_SIZE (0x8000 - (size_t) CYG_LABEL_NAME (_reserved))
