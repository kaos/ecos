//==========================================================================
//
//      stm32_flash.c
//
//      STM32 internal flash driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011, 2012 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-09-22
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal_cortexm_stm32.h>
#include <pkgconf/devs_flash_stm32.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <string.h>

#include <cyg/io/stm32_flash.h>

#include CYGHWR_MEMORY_LAYOUT_H

// Does this look like an F1 or F2/F4 device? It makes a difference to sizing and operation.
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
# define F1STYLE 1
#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
# define F2STYLE 1
#endif

// ----------------------------------------------------------------------------

#if defined(F1STYLE)
typedef cyg_uint16 STM32_TYPE;
#elif defined(F2STYLE)
// F2/F4's alignment requirements depend on parallelism config, so we treat as
// bytes for now.
typedef cyg_uint8 STM32_TYPE;
#endif

// ----------------------------------------------------------------------------

// How many loops before we consider this a timeout.
#define STM32_FLASH_TIMEOUT 1000000

// A quick helper macro to avoid repetition.
#define WAIT_FOR_FLASH_NOT_BUSY(_timeout_)                              \
  CYG_MACRO_START                                                       \
  (_timeout_) = STM32_FLASH_TIMEOUT;                                    \
  do {                                                                  \
      HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_SR, sr );            \
  } while( (sr & CYGHWR_HAL_STM32_FLASH_SR_BSY) && (_timeout_)-- > 0 ); \
  CYG_MACRO_END

// ----------------------------------------------------------------------------

// Note that although the F2/F4 parts need special treatment for the Flash's
// built-in icache and dcache, that doesn't matter for suspend/resume
// because no-one should expect any part of the Flash in the area being
// erased/programmed to be any particular value.

# define STM32_INTSCACHE_STATE     int _saved_ints_
# define STM32_INTSCACHE_BEGIN()   HAL_DISABLE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_SUSPEND() HAL_RESTORE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_RESUME()  HAL_DISABLE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_END()     HAL_RESTORE_INTERRUPTS(_saved_ints_)

#define STM32_UNCACHED_ADDRESS(__x) ((STM32_TYPE *)(__x))

// ----------------------------------------------------------------------------
// Forward declarations for functions that need to be placed in RAM:

static int stm32_enable_hsi(void);
static void stm32_disable_hsi(void);
static int stm32_flash_hw_erase(cyg_flashaddr_t addr, cyg_uint16 block_num) __attribute__((section (".2ram.stm32_flash_hw_erase")));
static int stm32_flash_hw_program( volatile STM32_TYPE* addr, const STM32_TYPE *buf, cyg_uint32 count) __attribute__((section (".2ram.stm32_flash_hw_program")));
    
// ----------------------------------------------------------------------------
// Diagnostic routines.

#if 0
#define stf_diag( __fmt, ... ) diag_printf("STF: %20s[%3d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define stf_dump_buf( __addr, __size ) diag_dump_buf( __addr, __size )
#else
#define stf_diag( __fmt, ... )
#define stf_dump_buf( __addr, __size )
#endif

// ----------------------------------------------------------------------------
// Select Flash geometry

#if defined(CYGHWR_HAL_CORTEXM_STM32_F103RC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F103VC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F103ZC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F105RC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F105RC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F105VC) || \
    defined(CYGHWR_HAL_CORTEXM_STM32_F107VC)
    
    // High-density device with 256K flash (2K blocks)
#define STM32_FLASH_SIZE        0x40000
    
#elif defined(CYGHWR_HAL_CORTEXM_STM32_F103RD) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F103VD) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F103ZD)
    
    // High-density device with 384K flash (2K blocks)
#define STM32_FLASH_SIZE        0x60000
    
#elif defined(CYGHWR_HAL_CORTEXM_STM32_F103RE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F103VE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F103ZE)
    
    // High-density device with 512K flash (2K blocks)
#define STM32_FLASH_SIZE        0x80000

#elif defined(CYGHWR_HAL_CORTEXM_STM32_F205RB) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205VB) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205ZB) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207VB) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207ZB) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207IB)
// 128K
#define STM32_FLASH_SIZE              (128*1024)
#define xxxSTM32_FLASH_BLOCK_INFO        { { { 16*1024, 4 } , { 64*1024, 1 } } }   // guesswork at present - documentation is elusive
#define STM32_FLASH_NUM_BLOCK_INFOS   2

#elif defined(CYGHWR_HAL_CORTEXM_STM32_F205RC) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205VC) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205ZC) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207VC) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207ZC) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207IC)
// 256K
#define STM32_FLASH_SIZE              (256*1024)
#define xxxSTM32_FLASH_BLOCK_INFO        { { { 16*1024, 4 } , { 64*1024, 1 }, { 128*1024, 1 } } }   // guesswork at present - documentation is elusive
#define STM32_FLASH_NUM_BLOCK_INFOS   3

#elif defined(CYGHWR_HAL_CORTEXM_STM32_F205RE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205VE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205ZE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207VE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207ZE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207IE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407IE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407VE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407ZE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417IE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417VE) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417ZE)
// 512K
#define STM32_FLASH_SIZE              (512*1024)
#define xxxSTM32_FLASH_BLOCK_INFO        { { { 16*1024, 4 } , { 64*1024, 1 }, { 128*1024, 3 } } }   // guesswork at present - documentation is elusive
#define STM32_FLASH_NUM_BLOCK_INFOS   3

#elif defined(CYGHWR_HAL_CORTEXM_STM32_F205RF) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205VF) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205ZF) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207VF) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207ZF) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207IF)
// 768K
#define STM32_FLASH_SIZE              (768*1024)
#define xxxSTM32_FLASH_BLOCK_INFO        { { { 16*1024, 4 } , { 64*1024, 1 }, { 128*1024, 5 } } }   // guesswork at present - documentation is elusive
#define STM32_FLASH_NUM_BLOCK_INFOS   3

#elif defined(CYGHWR_HAL_CORTEXM_STM32_F205RG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205VG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F205ZG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207VG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207ZG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F207IG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F405RG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F405VG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F405ZG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407IG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407VG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F407ZG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417IG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417VG) || \
      defined(CYGHWR_HAL_CORTEXM_STM32_F417ZG)

// 1024K
#define STM32_FLASH_SIZE              (1024*1024)
#define STM32_FLASH_BLOCK_INFO        { { { 16*1024, 4 } , { 64*1024, 1 }, { 128*1024, 7 } } }
#define STM32_FLASH_NUM_BLOCK_INFOS   3

#else
#error Unknown STM32 microprocessor variant.
#endif
    
#ifdef F1STYLE
// Always 2K blocks
#define STM32_FLASH_BLOCK_SIZE  0x800
#endif
    
// If there's just one block size, it's straightforward.
#if defined(STM32_FLASH_BLOCK_SIZE)
const cyg_stm32_flash_dev hal_stm32_flash_priv = { { STM32_FLASH_BLOCK_SIZE, STM32_FLASH_SIZE / STM32_FLASH_BLOCK_SIZE } };
#elif defined(STM32_FLASH_BLOCK_INFO)
const cyg_stm32_flash_dev hal_stm32_flash_priv = STM32_FLASH_BLOCK_INFO;
#else
# error Incomplete STM32 variant details. It needs filling in.
#endif
    
// ----------------------------------------------------------------------------

#ifdef CYGNUM_DEVS_FLASH_STM32_PARALLELISM
# define CR_PSIZE_MAX        CYGHWR_HAL_STM32_FLASH_CR_PSIZE(CYGNUM_DEVS_FLASH_STM32_PARALLELISM)
# define PARALLEL_BYTES      (CYGNUM_DEVS_FLASH_STM32_PARALLELISM/8)
# define PARALLEL_ALIGN_MASK (PARALLEL_BYTES - 1)
#endif

// ----------------------------------------------------------------------------
// Translate our error values into eCos flash driver error values

// Some little helper macros for this function to make it shorter/simpler
#define _SRBIT(_x_) CYGHWR_HAL_STM32_FLASH_SR_##_x_
#define _FERR(_x_) CYG_FLASH_ERR_##_x_
#define DECODE_SR_ERROR(_srbit_, _code_) if ( sr & _SRBIT(_srbit_) ) result = _FERR(_code_)

static int
stm32_flash_decode_error( int sr )
{
    int result = CYG_FLASH_ERR_OK;

    // -1 can never be a valid sr value so we use it to indicate a timeout
    if ( -1 == sr )
        return CYG_FLASH_ERR_DRV_TIMEOUT;

#if defined(F1STYLE)
    DECODE_SR_ERROR( PGERR, PROGRAM );
    DECODE_SR_ERROR( WRPRTERR, PROTECT );
#elif defined(F2STYLE)
    // OPERR is probably unnecessary really, but just in case.
    // Do it before others though so they can override with a better value.
    DECODE_SR_ERROR( OPERR, HWR );
    DECODE_SR_ERROR( WRPERR, PROTECT );
    DECODE_SR_ERROR( PGAERR, INVALID );
    DECODE_SR_ERROR( PGPERR, PROTOCOL );
    DECODE_SR_ERROR( PGSERR, PROTOCOL );
#endif
    return result;
}

static void stm32_flash_clear_sr_err(void)
{
    cyg_uint32 sr;
#if defined(F1STYLE)
    sr = CYGHWR_HAL_STM32_FLASH_SR_PGERR | CYGHWR_HAL_STM32_FLASH_SR_WRPRTERR;
#elif defined(F2STYLE)
    sr = CYGHWR_HAL_STM32_FLASH_SR_OPERR |
      CYGHWR_HAL_STM32_FLASH_SR_WRPERR |
      CYGHWR_HAL_STM32_FLASH_SR_PGAERR |
      CYGHWR_HAL_STM32_FLASH_SR_PGPERR |
      CYGHWR_HAL_STM32_FLASH_SR_PGSERR;
#endif
    HAL_WRITE_UINT32( CYGHWR_HAL_STM32_FLASH + CYGHWR_HAL_STM32_FLASH_SR, sr );
}

// ----------------------------------------------------------------------------
// Initialize the flash.


static int
stm32_flash_init(struct cyg_flash_dev* dev)
{
    // Set up the block info entries.

    dev->block_info                             = &hal_stm32_flash_priv.block_info[0];
#if defined(STM32_FLASH_NUM_BLOCK_INFOS)
    dev->num_block_infos                        = STM32_FLASH_NUM_BLOCK_INFOS;
#else
    dev->num_block_infos                        = sizeof(hal_stm32_flash_priv.block_info) / sizeof(hal_stm32_flash_priv.block_info[0]);
#endif

    // As stated in the errata sheet, the debug register can only be read in
    // debug mode and is therefore not accessible by user software.

    // Set end address
    dev->end                                    = dev->start + STM32_FLASH_SIZE - 1;

    stf_diag("block_size %d size %08x end %08x\n", dev->block_info[0].block_size, STM32_FLASH_SIZE, dev->end );    

    // Ensure there's nothing hanging over from before us.
    stm32_flash_clear_sr_err();
    
    return CYG_FLASH_ERR_OK;
}

// ----------------------------------------------------------------------------

static size_t
stm32_flash_query(struct cyg_flash_dev* dev, void* data, size_t len)
{
    static char query[] = "STM32 Internal Flash";
    memcpy( data, query, sizeof(query));
    return sizeof(query);
}

// ----------------------------------------------------------------------------
// Get info about the current block, i.e. base and size.

static void
stm32_flash_get_block_info(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr, cyg_flashaddr_t* block_start,
                           size_t* block_size, cyg_uint16 *block_num)
{
    size_t          offset  = addr - dev->start;
    cyg_ucount8  i;
    cyg_uint32   bi_size_passed = 0;
    cyg_uint16   blocks_passed = 0;
    
    // This loop has the termination condition commented out to silence a
    // warning. It should never be reached anyway, so that's fine, although
    // that is checked with an assert.

    for ( i=0; /* i < dev->num_block_info */ ; i++ )
{
        const cyg_flash_block_info_t *bi = &dev->block_info[i];
        cyg_uint32 bi_size = bi->blocks * bi->block_size;

        CYG_ASSERTC( i < dev->num_block_infos );
    
        if ( offset < bi_size)
        {
            *block_start = dev->start + bi_size_passed + (offset & ~(bi->block_size-1));
            *block_size  = bi->block_size;
            while (offset >= bi->block_size)
            {
                offset -= bi->block_size;
                blocks_passed++;
}
            *block_num = blocks_passed;
            break;
        }
        bi_size_passed += bi_size;
        offset -= bi_size;
        blocks_passed += bi->blocks;
    }
}

// ----------------------------------------------------------------------------

static int
stm32_enable_hsi(void)
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;
    cyg_uint32 cr;
    
    HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    if( cr & CYGHWR_HAL_STM32_RCC_CR_HSIRDY )
        return 0;
    
    cr |= CYGHWR_HAL_STM32_RCC_CR_HSION;
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    while( cr & CYGHWR_HAL_STM32_RCC_CR_HSIRDY )
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    
    return 1;
}

static void
stm32_disable_hsi(void)
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;
    cyg_uint32 cr;
    
    HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
    cr &= ~(CYGHWR_HAL_STM32_RCC_CR_HSION | CYGHWR_HAL_STM32_RCC_CR_HSIRDY);
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CR, cr );
}

// ----------------------------------------------------------------------------
// Erase a single sector. There is no API support for chip-erase. The
// generic code operates one sector at a time, invoking the driver for
// each sector, so there is no opportunity inside the driver for
// erasing multiple sectors in a single call. The address argument
// points at the start of the sector.

static int
stm32_flash_hw_erase(cyg_flashaddr_t addr, cyg_uint16 block_num)
{
    cyg_uint32 base = CYGHWR_HAL_STM32_FLASH;
    cyg_uint32 sr, cr;
    cyg_uint32 timeout;

#ifdef CYGDBG_USE_ASSERTS
    HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_SR, sr );
    CYG_ASSERT( 0 == (sr &  CYGHWR_HAL_STM32_FLASH_SR_BSY),
                "Flash busy at start of erase, but it shouldn't be" );
#endif

    // Unlock the flash control registers
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY1 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY2 );

#if defined(F1STYLE)
    cr = CYGHWR_HAL_STM32_FLASH_CR_PER;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_AR, addr );
    cr |= CYGHWR_HAL_STM32_FLASH_CR_STRT;
#elif defined(F2STYLE)
    cr = CYGHWR_HAL_STM32_FLASH_CR_SER |
      CR_PSIZE_MAX |
      CYGHWR_HAL_STM32_FLASH_CR_SNB(block_num) |
      CYGHWR_HAL_STM32_FLASH_CR_STRT;
#endif

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );
    
    WAIT_FOR_FLASH_NOT_BUSY( timeout );

    // Lock CR again (and clear other bits)
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, CYGHWR_HAL_STM32_FLASH_CR_LOCK );
    
#ifdef F2STYLE
    // For F2 parts, we need to disable and reset the icache and dcache in the ACR.
    {
        cyg_uint32 acr;

        HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // disable
        acr &= ~(CYGHWR_HAL_STM32_FLASH_ACR_DCEN|CYGHWR_HAL_STM32_FLASH_ACR_ICEN);
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // reset
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_DCRST|CYGHWR_HAL_STM32_FLASH_ACR_ICRST;
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // re-enable
        acr &= ~(CYGHWR_HAL_STM32_FLASH_ACR_DCRST|CYGHWR_HAL_STM32_FLASH_ACR_ICRST);
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_DCEN|CYGHWR_HAL_STM32_FLASH_ACR_ICEN;
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
    }
#endif

    if (0 == timeout)
        return -1;
    
    return sr;
}

// ----------------------------------------------------------------------------
// Write data to flash, using individual word writes on F1, or something more
// complicated on F2. On F1, the destination address will be aligned in a way suitable
// for the bus. The source address need not be aligned. The count is in STM32_TYPE's on
// F1, bytes on F2.

static int
stm32_flash_hw_program( volatile STM32_TYPE* addr, const STM32_TYPE* buf, cyg_uint32 count)
{
    cyg_uint32 base = CYGHWR_HAL_STM32_FLASH;
    cyg_uint32 sr = 0, cr = 0;
    cyg_uint32 timeout = 1; // Have to set timeout to non-zero to avoid confusing tests later.
    
#ifdef CYGDBG_USE_ASSERTS
    HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_SR, sr );
    CYG_ASSERT( 0 == (sr &  CYGHWR_HAL_STM32_FLASH_SR_BSY),
                "Flash busy at start of program, but it shouldn't be" );
#endif
    
    // Unlock the flash control registers
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY1 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY2 );

#if defined(F1STYLE)
    cr |= CYGHWR_HAL_STM32_FLASH_CR_PG;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );

    while( count-- )
    {
        HAL_WRITE_UINT16( addr, *buf );
        addr++;
        buf++;

        WAIT_FOR_FLASH_NOT_BUSY(timeout);
        if ( 0 == timeout )
            break;
    }


#elif defined(F2STYLE)
    // F2 is more complicated because the alignment depends on the parallelism.
    // So we "simplify" by writing bytes until we've reached the desired alignment.

    {
        CYG_ADDRESS addr_max = (CYG_ADDRESS)addr + PARALLEL_ALIGN_MASK;
        addr_max &= ~PARALLEL_ALIGN_MASK;

    byte_write:
        cr = CYGHWR_HAL_STM32_FLASH_CR_PG | CYGHWR_HAL_STM32_FLASH_CR_PSIZE_8;
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );

        while (count && ((CYG_ADDRESS)addr < addr_max))
        {
            HAL_WRITE_UINT8( addr, *buf );
        addr++;
        buf++;
            count--;
            WAIT_FOR_FLASH_NOT_BUSY(timeout);
            if (0 == timeout)
                break;
        }

        if ( count && timeout )
        {
            // Now we should be aligned to the required parallelism boundary
            // But we have to make sure we stop at an aligned addr too.
            addr_max += count;
            addr_max &= ~PARALLEL_ALIGN_MASK;

            cr = CYGHWR_HAL_STM32_FLASH_CR_PG | CR_PSIZE_MAX;
            HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );
            while ((CYG_ADDRESS)addr < addr_max)
            {
#if (CYGNUM_DEVS_FLASH_STM32_PARALLELISM == 8)
                HAL_WRITE_UINT8( addr, *buf );
#elif (CYGNUM_DEVS_FLASH_STM32_PARALLELISM == 16)
                cyg_uint16 wbuf = *(cyg_uint16*)buf;
                HAL_WRITE_UINT16( addr, wbuf );
#elif (CYGNUM_DEVS_FLASH_STM32_PARALLELISM == 32)
                cyg_uint32 wbuf = *(cyg_uint32*)buf;
                HAL_WRITE_UINT32( addr, wbuf );
#elif (CYGNUM_DEVS_FLASH_STM32_PARALLELISM == 64)
                cyg_uint64 wbuf = *(cyg_uint64*)buf;
                HAL_WRITE_UINT64( addr, wbuf );
#endif
                addr += PARALLEL_BYTES;
                buf += PARALLEL_BYTES;
                count -= PARALLEL_BYTES;
                WAIT_FOR_FLASH_NOT_BUSY(timeout);
                if (0 == timeout)
                    break;
            } // while
        } // if

        if ( count && timeout )
        {
            // Still have some bytes left to write. Take a shortcut with goto, to save code.
            addr_max = (CYG_ADDRESS)addr+count;
            goto byte_write;
        } // if        
    }

    // For F2 parts, we need to disable and reset the icache and dcache in the ACR.
    {
        cyg_uint32 acr;

        HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // disable
        acr &= ~(CYGHWR_HAL_STM32_FLASH_ACR_DCEN|CYGHWR_HAL_STM32_FLASH_ACR_ICEN);
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // reset
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_DCRST|CYGHWR_HAL_STM32_FLASH_ACR_ICRST;
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
        // re-enable
        acr &= ~(CYGHWR_HAL_STM32_FLASH_ACR_DCRST|CYGHWR_HAL_STM32_FLASH_ACR_ICRST);
        acr |= CYGHWR_HAL_STM32_FLASH_ACR_DCEN|CYGHWR_HAL_STM32_FLASH_ACR_ICEN;
        HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_ACR, acr );
    }
#endif // elif defined(F2STYLE)

    // Lock CR again (and clear other bits)
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, CYGHWR_HAL_STM32_FLASH_CR_LOCK );

    if (0 == timeout)
        return -1;

    return sr;    
}

// ----------------------------------------------------------------------------
// Erase a single block. The calling code will have supplied a pointer
// aligned to a block boundary.

static int
stm32_flash_erase(struct cyg_flash_dev* dev, cyg_flashaddr_t dest)
{
    int                     (*erase_fn)(cyg_uint32, cyg_uint16);
    cyg_flashaddr_t         block_start;
    size_t                  block_size;
    cyg_uint16              block_num;
    int                     result;
    int                     hsi;
    STM32_INTSCACHE_STATE;

    stf_diag("dest %p\n", (void *) dest);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && (dest <= dev->end), "flash address out of device range");
    
    stm32_flash_get_block_info(dev, dest, &block_start, &block_size, &block_num);
    stf_diag("block_start %p block_size %d\n", (void *) block_start, block_size);
    CYG_ASSERT(dest == block_start, "erase address should be the start of a flash block");

    erase_fn    = (int (*)(cyg_uint32, cyg_uint16)) cyg_flash_anonymizer( & stm32_flash_hw_erase );
    
    hsi = stm32_enable_hsi();
    
    STM32_INTSCACHE_BEGIN();    

    result = (*erase_fn)(block_start, block_num);
    result = stm32_flash_decode_error( result );
    stm32_flash_clear_sr_err();
    
    STM32_INTSCACHE_END();
    
    if (hsi)
        stm32_disable_hsi();
    
    return result;
}

// ----------------------------------------------------------------------------
// Write some data to the flash. The destination must be aligned to a
// 16 bit boundary. Higher level code guarantees that the data will
// not straddle a block boundary.

int
stm32_flash_program(struct cyg_flash_dev* dev, cyg_flashaddr_t dest, const void* src, size_t len)
{
    int                     (*program_fn)(volatile STM32_TYPE*, const STM32_TYPE*, cyg_uint32);
    volatile STM32_TYPE*    uncached; 
    const STM32_TYPE*       data;
    size_t                  to_write;
    int                     result  = CYG_FLASH_ERR_OK;
    int                     hsi;

    STM32_INTSCACHE_STATE;

    stf_diag("dest %p src %p len %p(%d)\n", (void *) dest, src, (void *) len, len);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

#ifdef F1STYLE
    // Source and destination must be 16-bit aligned.
    if( (0 != ((CYG_ADDRESS)dest & 1)) ||
        (0 != ((CYG_ADDRESS)src  & 1)) )
        return CYG_FLASH_ERR_INVALID;
#endif
    
    uncached    = STM32_UNCACHED_ADDRESS(dest);
    data        = (const STM32_TYPE*) src;
    to_write    = len / sizeof(STM32_TYPE);      // For F1: Number of words, not bytes. For F2: STM32_TYPE is a byte.
    program_fn  = (int (*)(volatile STM32_TYPE*, const STM32_TYPE*, cyg_uint32)) cyg_flash_anonymizer( & stm32_flash_hw_program );

    hsi = stm32_enable_hsi();

    STM32_INTSCACHE_BEGIN();
    while (to_write > 0)
    {
        size_t this_write = (to_write < CYGNUM_DEVS_FLASH_STM32_V2_PROGRAM_BURST_SIZE) ?
                             to_write : CYGNUM_DEVS_FLASH_STM32_V2_PROGRAM_BURST_SIZE;

        
        result = (*program_fn)(uncached, data, this_write);
        result = stm32_flash_decode_error( result );
        if (result != CYG_FLASH_ERR_OK)
        {
            break;
        }
        to_write -= this_write;
        if (to_write > 0)
        {
            // There is still more to be written. The last write must have been a burst size
            uncached    += this_write;
            data        += this_write;
            STM32_INTSCACHE_SUSPEND();
            STM32_INTSCACHE_RESUME();
        }
    }
    stm32_flash_clear_sr_err();
    STM32_INTSCACHE_END();
    
    if (hsi)
        stm32_disable_hsi();
    
    return result;
}

// ----------------------------------------------------------------------------
// Function table

const CYG_FLASH_FUNS(cyg_stm32_flash_funs,
                     &stm32_flash_init,
                     &stm32_flash_query,
                     &stm32_flash_erase,
                     &stm32_flash_program,
                     (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                     cyg_flash_devfn_lock_nop,
                     cyg_flash_devfn_unlock_nop);

// ----------------------------------------------------------------------------
// End of stm32_flash.c
