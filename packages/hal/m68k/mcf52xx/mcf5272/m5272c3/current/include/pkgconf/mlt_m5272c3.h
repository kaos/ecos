#if !defined(__ASSEMBLER__) && !defined(__LDI__)
#include <cyg/infra/cyg_type.h>
#include <stddef.h>
#endif

#define CYGMEM_REGION_ram           (0x0)
#define CYGMEM_REGION_ram_SIZE      (0x00400000)
#define CYGMEM_REGION_ram_ATTR      (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_flash         (0xFFE00000)
#define CYGMEM_REGION_flash_SIZE    (0x00200000)
#define CYGMEM_REGION_flash_ATTR    (CYGMEM_REGION_ATTR_R)
#define CYGMEM_REGION_iram          (0x20000000)
#define CYGMEM_REGION_iram_SIZE     (0x00001000)
#define CYGMEM_REGION_iram_ATTR     (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)

#if !defined(__ASSEMBLER__) && !defined(__LDI__)
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1        (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE   ((CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE) - (size_t) CYG_LABEL_NAME (__heap1))
