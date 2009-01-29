//==========================================================================
//
//      stm32_wallclock.cxx
//
//      Wallclock implementation for STM32
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):     Simon Kallweit
// Contributors:  
// Date:          2008-10-27
// Purpose:       Wallclock driver for STM32
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_stm32.h>
#include <pkgconf/wallclock.h>
#include <pkgconf/devs_wallclock_stm32.h>

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/io/wallclock.hxx>

//==========================================================================
// Clock Initialization values

#if defined(CYGHWR_DEVS_WALLCLOCK_STM32_RTC_SOURCE_LSE)
# define CYGHWR_DEVS_WALLCLOCK_STM32_RTCSEL CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_LSE
# define CYGHWR_DEVS_WALLCLOCK_STM32_CLOCK 32768
#elif defined(CYGHWR_DEVS_WALLCLOCK_STM32_RTC_SOURCE_LSI)
# define CYGHWR_DEVS_WALLCLOCK_STM32_RTCSEL CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_LSI
# define CYGHWR_DEVS_WALLCLOCK_STM32_CLOCK 40000
#elif defined(CYGHWR_DEVS_WALLCLOCK_STM32_RTC_SOURCE_HSE_128)
# define CYGHWR_DEVS_WALLCLOCK_STM32_RTCSEL CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_HSE
# define CYGHWR_DEVS_WALLCLOCK_STM32_CLOCK (CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK / 128)
#endif

// Wait for registers to be synchronized
static void
wait_sync(void)
{
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;
    cyg_uint16 crl;
    
    do
    {
        HAL_READ_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CRL, crl );
    } while ( !(crl & CYGHWR_HAL_STM32_RTC_CRL_RSF) );
}

// Wait for last write operation to finish
static void
wait_rtoff(void)
{
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;
    cyg_uint16 crl;
    
    do
    {
        HAL_READ_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CRL, crl );
    } while ( !(crl & CYGHWR_HAL_STM32_RTC_CRL_RTOFF) );
}

// Enter/leave configuration mode
static void
config_mode(int val)
{
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;
    cyg_uint16 crl;
    
    if (val)
        wait_rtoff();

    HAL_READ_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CRL, crl );
    if (val)
        crl |= CYGHWR_HAL_STM32_RTC_CRL_CNF;
    else
        crl &= ~CYGHWR_HAL_STM32_RTC_CRL_CNF;
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CRL, crl );
    
    if (!val)
        wait_rtoff();
}

void
Cyg_WallClock::init_hw_seconds(void)
{
    CYG_ADDRESS rcc = CYGHWR_HAL_STM32_RCC;
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;
    cyg_uint32 bdcr, csr;
    cyg_uint32 prescaler = CYGHWR_DEVS_WALLCLOCK_STM32_CLOCK - 1;
    
    // Reset the backup domain if clock source does not match as
    // RTCSEL can only be written to backup domain once
    HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_BDCR, bdcr );
    if ((bdcr & CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_XXX) !=
        CYGHWR_DEVS_WALLCLOCK_STM32_RTCSEL)
    {
        // Reset backup domain
        CYGHWR_HAL_STM32_BD_RESET();
    }

    // Disable backup domain protection
    CYGHWR_HAL_STM32_BD_PROTECT(0);
    
#if defined(CYGHWR_DEVS_WALLCLOCK_STM32_RTC_SOURCE_LSI)
    // Start up LSI clock
    HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CSR, csr );
    csr |= CYGHWR_HAL_STM32_RCC_CSR_LSION;
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CSR, csr );
    
    // Wait for LSI clock to startup
    do
    {
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_CSR, csr );
    } while( !(csr & CYGHWR_HAL_STM32_RCC_CSR_LSIRDY) );
#endif
    
#if defined(CYGHWR_DEVS_WALLCLOCK_STM32_RTC_SOURCE_LSE)
    // Start up LSE clock
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_BDCR, CYGHWR_HAL_STM32_RCC_BDCR_LSEON );

    // Wait for LSE clock to startup
    do
    {
        HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_BDCR, bdcr );
    } while( !(bdcr & CYGHWR_HAL_STM32_RCC_BDCR_LSERDY) );
#endif
    
    // Enable RTC
    HAL_READ_UINT32( rcc+CYGHWR_HAL_STM32_RCC_BDCR, bdcr );
    bdcr |= CYGHWR_DEVS_WALLCLOCK_STM32_RTCSEL;
    bdcr |= CYGHWR_HAL_STM32_RCC_BDCR_RTCEN;
    HAL_WRITE_UINT32( rcc+CYGHWR_HAL_STM32_RCC_BDCR, bdcr );
    
    // Synchronize registers
    wait_sync();
    
    // Enter configuration mode
    config_mode(1);
    
    // Disable all interrupts
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CRH, 0 );
    
    // Set prescaler load value
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_PRLL, prescaler & 0xffff);
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_PRLH, prescaler >> 16);
    
    // Reset alarm value
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_ALRL, 0 );
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_ALRH, 0 );
    
#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
    // Reset counter value
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTL, 0 );
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTH, 0 );
#endif
    
    // Leave configuration mode
    config_mode(0);
    
    // Restore backup domain protection
    CYGHWR_HAL_STM32_BD_PROTECT(1);
}

cyg_uint32
Cyg_WallClock::get_hw_seconds(void)
{
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;
    cyg_uint16 cnt;
    cyg_uint32 secs;
    
    // Synchronize registers
    wait_sync();
    
    // Read counter value
    HAL_READ_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTL, cnt );
    secs = cnt;
    HAL_READ_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTH, cnt );
    secs |= (cnt << 16);
    
    return secs;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
void
Cyg_WallClock::set_hw_seconds(cyg_uint32 secs)
{
    CYG_ADDRESS rtc = CYGHWR_HAL_STM32_RTC;

    // Disable backup domain protection
    CYGHWR_HAL_STM32_BD_PROTECT(0);
    
    // Enter configuration mode
    config_mode(1);
    
    // Set counter value
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTL, secs & 0xffff );
    HAL_WRITE_UINT16( rtc+CYGHWR_HAL_STM32_RTC_CNTH, secs >> 16 );
    
    // Leave configuration mode
    config_mode(0);
    
    // Restore backup domain protection
    CYGHWR_HAL_STM32_BD_PROTECT(1);
}
#endif
