#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-04-21
// Purpose:      Cirrus EDB7XXX platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>           // System-wide configuration info
#include CYGBLD_HAL_PLATFORM_H        // Platform specific configuration
#include <cyg/hal/hal_edb7xxx.h>       // Platform specific hardware definitions
#include <cyg/hal/hal_mmu.h>          // MMU definitions

#define CYGHWR_HAL_ARM_HAS_MMU        // This processor has an MMU

//
// Memory map - set up by ROM (GDB stubs)
//
// Region        Logical Address              Physical Address        
//   DRAM          0x00000000..0x00xFFFFF        0xC00x0000  (see below)
//   Expansion 2   0x20000000                    0x20000000
//   Expansion 3   0x30000000                    0x30000000
//   PCMCIA 0      0x40000000                    0x40000000
//   PCMCIA 1      0x50000000                    0x50000000
//   SRAM          0x60000000..0x600007FF        0x60000000
//   I/O           0x80000000                    0x80000000
//   MMU Tables                                  0xC00y0000
//   LCD buffer    0xC0000000..0xC001FFFF        0xC0000000
//   ROM           0xE0000000..0xEFFFFFFF        0x00000000
//   ROM           0xF0000000..0xFFFFFFFF        0x10000000

#ifdef CYGHWR_HAL_ARM_EDB7XXX_LCD_INSTALLED
#define LCD_BUFFER_SIZE  0x00020000
#else
#define LCD_BUFFER_SIZE  0x00000000
#endif
#define DRAM_PA_START    0xC0000000
#define MMU_BASE         DRAM_PA_START+LCD_BUFFER_SIZE
#define PTE_BASE         DRAM_PA_START+0x4000+LCD_BUFFER_SIZE
#if (CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE == 2)
#define MMU_TABLES_SIZE  (0x4000+0x1000+0x1000)   // RAM used for PTE entries
#define DRAM_LA_END      (0x00200000-MMU_TABLES_SIZE-LCD_BUFFER_SIZE)
#elif (CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE == 16)
#define MMU_TABLES_SIZE  (0x4000+0x4000+0x1000)  // RAM used for PTE entries
#define DRAM_LA_END      (0x01000000-MMU_TABLES_SIZE-LCD_BUFFER_SIZE)
#endif
#define DRAM_LA_START    0x00000000
#define DRAM_PA          MMU_BASE+MMU_TABLES_SIZE
#define LCD_LA_START     0xC0000000
#define LCD_LA_END       0xC0020000
#define LCD_PA           0xC0000000
#define ROM0_LA_START    0xE0000000
#define ROM0_PA          0x00000000
#define ROM0_LA_END      0xF0000000
#define ROM1_LA_START    0xF0000000
#define ROM1_LA_END      0x00000000
#define ROM1_PA          0x10000000
#define EXPANSION2_LA_START 0x20000000
#define EXPANSION2_PA       0x20000000
#define EXPANSION3_LA_START 0x30000000
#define EXPANSION3_PA       0x30000000
#define PCMCIA0_LA_START 0x40000000
#define PCMCIA0_PA       0x40000000
#define PCMCIA1_LA_START 0x50000000
#define PCMCIA1_PA       0x50000000
#define SRAM_LA_START    0x60000000
#define SRAM_LA_END      0x60001000
#define SRAM_PA          0x60000000
#define IO_LA_START      0x80000000
#define IO_LA_END        0x80003000
#define IO_PA            0x80000000

// Define startup code [macros]
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_STUBS)

#if (CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE == 2)
// Note: The DRAM on this board is very irregular in that every
// other 256K piece is missing.  E.g. only these [physical] 
// addresses are valid:
//   0xC0000000..0xC003FFFF
//   0xC0080000..0xC00BFFFF
//   0xC0200000..0xC023FFFF    Note the additional GAP!
//      etc.
//   0xC0800000..0xC083FFFF    Note the additional GAP!
//   0xC0880000..0xC08CFFFF
//   0xC0A00000..0xC0A3FFFF
//      etc.
// The MMU mapping code takes this into consideration and creates
// a continuous logical map for the DRAM.
#define MAP_DRAM                                                                         \
/* Map DRAM */                                                                          ;\
	ldr	r3,=DRAM_LA_START                                                       ;\
	ldr	r4,=DRAM_LA_END                                                         ;\
	ldr	r5,=DRAM_PA                                                             ;\
/* 0x00000000..0x000FFFFF */                                                            ;\
	mov	r6,r2		/* Set up page table descriptor */                      ;\
	ldr	r7,=MMU_L1_TYPE_Page                                                    ;\
	orr	r6,r6,r7                                                                ;\
	str	r6,[r1],#4	/* Store PTE, update pointer */                         ;\
10:	mov	r6,r5		/* Build page table entry */                            ;\
	ldr	r7,=MMU_L2_TYPE_Small|MMU_AP_Any|MMU_Bufferable|MMU_Cacheable           ;\
	orr	r6,r6,r7                                                                ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
	str	r6,[r2],#4	/* Next page */                                         ;\
	add	r3,r3,r7                                                                ;\
	add	r5,r5,r7                                                                ;\
	ldr	r8,=DRAM_LA_START+MMU_SECTION_SIZE                                      ;\
	cmp	r3,r8		/* Done with first 1M? */                               ;\
	beq	20f                                                                     ;\
	ldr	r7,=0x40000	/* Special check for 256K boundary */                   ;\
	and	r7,r7,r5                                                                ;\
	cmp	r7,#0                                                                   ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7	/* Skip 256K hole */                                    ;\
	ldr	r7,=0x100000                                                            ;\
	and	r7,r5,r7                                                                ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7	/* Nothing at 0xC0100000 */                             ;\
	ldr	r7,=0x400000	/* Also nothing at 0xC0400000 */                        ;\
	and	r7,r5,r7                                                                ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7                                                                ;\
	b	10b                                                                     ;\
20:                                                                                     ;\
/* 0x00100000..0x001FFFFF */                                                            ;\
	mov	r6,r2		/* Set up page table descriptor */                      ;\
	ldr	r7,=MMU_L1_TYPE_Page                                                    ;\
	orr	r6,r6,r7                                                                ;\
	str	r6,[r1],#4	/* Store PTE, update pointer */                         ;\
10:	mov	r6,r5		/* Build page table entry */                            ;\
	ldr	r7,=MMU_L2_TYPE_Small|MMU_AP_Any|MMU_Bufferable|MMU_Cacheable           ;\
	orr	r6,r6,r7                                                                ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
	str	r6,[r2],#4	/* Next page */                                         ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4		/* Done with first DRAM? */                             ;\
	beq	20f                                                                     ;\
	add	r5,r5,r7                                                                ;\
	ldr	r7,=0x40000	/* Special check for 256K boundary */                   ;\
	and	r7,r7,r5                                                                ;\
	cmp	r7,#0                                                                   ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7	/* Skip 256K hole */                                    ;\
	ldr	r7,=0x100000                                                            ;\
	and	r7,r5,r7                                                                ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7	/* Nothing at 0xC0300000 */                             ;\
	ldr	r7,=0x400000	/* Also nothing at 0xC0400000 */                        ;\
	and	r7,r5,r7                                                                ;\
	beq	10b                                                                     ;\
	add	r5,r5,r7                                                                ;\
	b	10b                                                                     ;\
20:                                                                                     ;
#elif (CYGHWR_HAL_ARM_EDB7XXX_DRAM_SIZE == 16)
// The 16M board is arranged as:
//   0xC0000000..0xC07FFFFF
//   0xC1000000..0xC17FFFFF
#define MAP_DRAM                                                                         \
/* Map DRAM */                                                                          ;\
	ldr	r3,=DRAM_LA_START                                                       ;\
	ldr	r4,=DRAM_LA_END                                                         ;\
	ldr	r5,=DRAM_PA                                                             ;\
/* 0xXXX00000..0xXXXFFFFF */                                                            ;\
10:	mov	r6,r2		/* Set up page table descriptor */                      ;\
	ldr	r7,=MMU_L1_TYPE_Page                                                    ;\
	orr	r6,r6,r7                                                                ;\
	str	r6,[r1],#4	/* Store PTE, update pointer */                         ;\
        ldr     r8,=MMU_SECTION_SIZE/MMU_PAGE_SIZE                                      ;\
	ldr	r9,=DRAM_PA_START+0x00800000   /* Need to skip at 8M boundary */        ;\
12:     cmp     r5,r9                                                                   ;\
        bne     15f                                                                     ;\
	ldr	r5,=DRAM_PA_START+0x01000000   /* Next chunk of DRAM */                 ;\
15:	mov	r6,r5		/* Build page table entry */                            ;\
	ldr	r7,=MMU_L2_TYPE_Small|MMU_AP_Any|MMU_Bufferable|MMU_Cacheable           ;\
	orr	r6,r6,r7                                                                ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
	str	r6,[r2],#4	/* Next page */                                         ;\
	add	r3,r3,r7                                                                ;\
	add	r5,r5,r7                                                                ;\
        cmp     r3,r4           /* End of DRAM? */                                      ;\
        beq     20f                                                                     ;\
        sub     r8,r8,#1        /* End of 1M section? */                                ;\
        cmp     r8,#0                                                                   ;\
        bne     12b             /* Next page */                                         ;\
        b       10b             /* Next section */                                      ;\
20:                                                                                     
#else
#err Invalid DRAM size selected
#endif

#ifdef CYG_HAL_ARM_CL7111 // CL7111, 710 processor
#define MMU_INITIALIZE                                                                   \
	ldr	r1,=MMU_Control_Init                                                    ;\
	mcr	MMU_CP,0,r1,MMU_Control,c0	/* MMU off */                           ;\
	ldr	r1,=MMU_BASE                                                            ;\
	mcr	MMU_CP,0,r1,MMU_Base,c0                                                 ;\
	mcr	MMU_CP,0,r1,MMU_FlushTLB,c0,0	/* Invalidate TLB */                    ;\
	mcr	MMU_CP,0,r1,MMU_FlushIDC,c0,0	/* Invalidate Caches */                 ;\
	ldr	r1,=0xFFFFFFFF                                                          ;\
	mcr	MMU_CP,0,r1,MMU_DomainAccess,c0	                                        ;\
	ldr	r2,=10f                 	                                        ;\
	ldr	r1,=MMU_Control_Init|MMU_Control_M                                      ;\
	mcr	MMU_CP,0,r1,MMU_Control,c0	                                        ;\
	mov	pc,r2    /* Change address spaces */                                    ;\
	nop	                                                                        ;\
	nop	                                                                        ;\
	nop	                                                                        ;\
10:
#else // EP7211, 720T processor
#define MMU_INITIALIZE                                                                   ;\
	ldr	r1,=MMU_Control_Init                                                     ;\
        mcr	MMU_CP,0,r1,MMU_Control,c0	/* MMU off */                            ;\
	ldr	r1,=MMU_BASE                                                             ;\
	mcr	MMU_CP,0,r1,MMU_Base,c0                                                  ;\
	mcr	MMU_CP,0,r1,MMU_TLB,c7,0	/*  Invalidate TLB - 720 style */        ;\
	mcr	MMU_CP,0,r1,MMU_FlushIDC,c0,0	/* Invalidate Caches */                  ;\
	ldr	r1,=0xFFFFFFFF                                                           ;\
	mcr	MMU_CP,0,r1,MMU_DomainAccess,c0	                                         ;\
	ldr	r2,=10f                 	                                         ;\
        ldr     r3,=__exception_handlers                                                 ;\
        sub     r2,r2,r3                                                                 ;\
        ldr     r3,=ROM0_LA_START                                                        ;\
        add     r2,r2,r3                                                                 ;\
	ldr	r1,=MMU_Control_Init|MMU_Control_M                                       ;\
	mcr	MMU_CP,0,r1,MMU_Control,c0	                                         ;\
	mov	pc,r2    /* Change address spaces */                                     ;\
	nop	                                                                         ;\
	nop	                                                                         ;\
	nop	                                                                         ;\
10:
#endif

#define PLATFORM_SETUP1                                                                 \
/* Initialize memory configuration */                                                   ;\
	ldr	r1,=MEMCFG1                                                             ;\
	ldr	r2,=0x8200A080	/* FIXME - what is this? */                             ;\
	str	r2,[r1]                                                                 ;\
	ldr	r1,=MEMCFG2                                                             ;\
	ldr	r2,=0xFEFC0000                                                          ;\
	str	r2,[r1]                                                                 ;\
	ldr	r1,=DRFPR                                                               ;\
	ldr	r2,=0x81	/* DRAM refresh = 64KHz */                              ;\
	strb	r2,[r1]                                                                 ;\
/* Initialize MMU to create new memory map */                                           ;\
	ldr	r1,=MMU_BASE                                                            ;\
	ldr	r2,=PTE_BASE                                                            ;\
	MAP_DRAM                                                                        ;\
/* Nothing until PCMCIA0 */                                                             ;\
	ldr	r3,=0x3FF	/* Page tables need 2K boundary */                      ;\
	add	r2,r2,r3                                                                ;\
	ldr	r3,=~0x3FF                                                              ;\
	and	r2,r2,r3                                                                ;\
	ldr	r3,=(DRAM_LA_END+0x000FFFFF)&0xFFF00000                                 ;\
	ldr	r4,=EXPANSION2_LA_START                                                 ;\
	ldr	r5,=MMU_L1_TYPE_Fault                                                   ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
10:	str	r5,[r1],#4                                                              ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
/* EXPANSION2, EXPANSION3, PCMCIA0, PCMCIA1 */                                          ;\
	ldr	r3,=EXPANSION2_LA_START                                                 ;\
	ldr	r4,=SRAM_LA_START                                                       ;\
	ldr	r5,=EXPANSION2_PA                                                       ;\
	ldr	r6,=MMU_L1_TYPE_Section|MMU_AP_Any                                      ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
10:	orr	r0,r5,r6                                                                ;\
	str	r0,[r1],#4                                                              ;\
	add	r5,r5,r7                                                                ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
/* SRAM */                                                                              ;\
	ldr	r3,=SRAM_LA_START                                                       ;\
	ldr	r4,=MMU_L1_TYPE_Page|MMU_DOMAIN(0)                                      ;\
	orr	r4,r4,r2                                                                ;\
	str	r4,[r1],#4                                                              ;\
	ldr	r4,=MMU_L2_TYPE_Small|MMU_AP_Any|MMU_Bufferable|MMU_Cacheable           ;\
	orr	r4,r3,r4                                                                ;\
	str	r4,[r2],#4                                                              ;\
	ldr	r3,=SRAM_LA_START+MMU_PAGE_SIZE                                         ;\
	ldr	r4,=SRAM_LA_START+MMU_SECTION_SIZE                                      ;\
	ldr	r5,=MMU_L2_TYPE_Fault                                                   ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
10:	str	r5,[r2],#4                                                              ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
	ldr	r4,=IO_LA_START                                                         ;\
	ldr	r5,=MMU_L1_TYPE_Fault                                                   ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
20:	str	r5,[r1],#4                                                              ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	20b                                                                     ;\
/* I/O */                                                                               ;\
	ldr	r3,=0x3FF	/* Page tables need 2K boundary */                      ;\
	add	r2,r2,r3                                                                ;\
	ldr	r3,=~0x3FF                                                              ;\
	and	r2,r2,r3                                                                ;\
	ldr	r4,=MMU_L1_TYPE_Page|MMU_DOMAIN(0)                                      ;\
	orr	r4,r4,r2                                                                ;\
	str	r4,[r1],#4                                                              ;\
	ldr	r3,=IO_LA_START                                                         ;\
	ldr	r4,=IO_LA_END                                                           ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
	ldr	r5,=IO_PA|MMU_L2_TYPE_Small|MMU_AP_All                                  ;\
10:	str	r5,[r2],#4                                                              ;\
	add	r5,r5,r7                                                                ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
	ldr	r4,=IO_LA_START+MMU_SECTION_SIZE                                        ;\
	ldr	r5,=MMU_L2_TYPE_Fault                                                   ;\
	ldr	r7,=MMU_PAGE_SIZE                                                       ;\
10:	str	r5,[r2],#4                                                              ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
	ldr	r4,=LCD_LA_START                                                        ;\
	ldr	r5,=MMU_L1_TYPE_Fault                                                   ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
20:	str	r5,[r1],#4                                                              ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	20b                                                                     ;\
/* LCD Buffer & Unmapped DRAM (holes and all) */                                        ;\
	ldr	r3,=LCD_LA_START                                                        ;\
	ldr	r4,=ROM0_LA_START                                                       ;\
	ldr	r5,=LCD_PA                                                              ;\
	ldr	r6,=MMU_L1_TYPE_Section|MMU_AP_Any|MMU_Bufferable|MMU_Cacheable         ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
10:	orr	r0,r5,r6                                                                ;\
	str	r0,[r1],#4                                                              ;\
	add	r5,r5,r7                                                                ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
/* ROM0 */                                                                              ;\
	ldr	r3,=ROM0_LA_START                                                       ;\
	ldr	r4,=ROM0_LA_END                                                         ;\
	ldr	r5,=ROM0_PA                                                             ;\
	ldr	r6,=MMU_L1_TYPE_Section|MMU_AP_Any|MMU_Cacheable                        ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
10:	orr	r0,r5,r6                                                                ;\
	str	r0,[r1],#4                                                              ;\
	add	r5,r5,r7                                                                ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
/* ROM1 */                                                                              ;\
	ldr	r3,=ROM1_LA_START                                                       ;\
	ldr	r4,=ROM1_LA_END                                                         ;\
	ldr	r5,=ROM1_PA                                                             ;\
	ldr	r6,=MMU_L1_TYPE_Section|MMU_AP_Any                                      ;\
	ldr	r7,=MMU_SECTION_SIZE                                                    ;\
10:	orr	r0,r5,r6                                                                ;\
	str	r0,[r1],#4                                                              ;\
	add	r5,r5,r7                                                                ;\
	add	r3,r3,r7                                                                ;\
	cmp	r3,r4                                                                   ;\
	bne	10b                                                                     ;\
/* Now initialize the MMU to use this new page table */                                 ;\
        MMU_INITIALIZE                                                                  ;\
        ldr     r2,=__exception_handlers                                                ;\
        ldr     r3,=ROM0_LA_START                                                       ;\
        cmp     r2,r3                                                                   ;\
        beq     20f                                                                     ;\
        ldr     r4,=__rom_data_end                                                      ;\
15:                                                                                     ;\
        ldr     r0,[r3],#4                                                              ;\
        str     r0,[r2],#4                                                              ;\
        cmp     r2,r4                                                                   ;\
        bne     15b                                                                     ;\
20:

#else

#define PLATFORM_SETUP1                                                 
#endif

										
/*---------------------------------------------------------------------------*/
/* end of hal_platform_setup.h                                               */
#endif /* CYGONCE_HAL_PLATFORM_SETUP_H */
