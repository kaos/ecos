//==========================================================================
//
//      gpio.c
//
//      MCF5272 GPIO support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2004, 2005, 2006, 2008 Free Software Foundation, Inc.
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
// Author(s):   bartv
// Date:        2006-09-05
//
//####DESCRIPTIONEND####
//=============================================================================

// This file is just #include'd by the main mcf5272.c file. It serves
// to keep all the GPIO definitions in a separate file. GPIO
// initialization is all handled by configury, thus minimizing the
// amount of work explicitly needed in application code.

#define A0_DAT      1
#define A1_DAT      1
#define A2_DAT      1
#define A3_DAT      1
#define A4_DAT      1
#define A5_DAT      1
#define A6_DAT      1
#define A7_DAT      1
#define A8_DAT      1
#define A9_DAT      1
#define A10_DAT     1
#define A11_DAT     1
#define A12_DAT     1
#define A13_DAT     1
#define A14_DAT     1
#define A15_DAT     1

#define A0_DDR      0
#define A1_DDR      0
#define A2_DDR      0
#define A3_DDR      0
#define A4_DDR      0
#define A5_DDR      0
#define A6_DDR      0
#define A7_DDR      0
#define A8_DDR      0
#define A9_DDR      0
#define A10_DDR     0
#define A11_DDR     0
#define A12_DDR     0
#define A13_DDR     0
#define A14_DDR     0
#define A15_DDR     0

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA0_usb_tp)
# define A0_PAR     1
#else
# define A0_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA0_in)
#  undef  A0_DDR
#  define A0_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA0_out0)
#   undef  A0_DAT
#   define A0_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA0_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA1_usb_rp)
# define A1_PAR     1
#else
# define A1_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA1_in)
#  undef  A1_DDR
#  define A1_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA1_out0)
#   undef  A1_DAT
#   define A1_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA1_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA2_usb_rn)
# define A2_PAR     1
#else
# define A2_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA2_in)
#  undef  A2_DDR
#  define A2_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA2_out0)
#   undef  A2_DAT
#   define A2_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA2_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA3_usb_tn)
# define A3_PAR     1
#else
# define A3_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA3_in)
#  undef  A3_DDR
#  define A3_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA3_out0)
#   undef  A3_DAT
#   define A3_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA3_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA4_usb_susp)
# define A4_PAR     1
#else
# define A4_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA4_in)
#  undef  A4_DDR
#  define A4_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA4_out0)
#   undef  A4_DAT
#   define A4_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA4_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA5_usb_txen)
# define A5_PAR     1
#else
# define A5_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA5_in)
#  undef  A5_DDR
#  define A5_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA5_out0)
#   undef  A5_DAT
#   define A5_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA5_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA6_usb_rxd)
# define A6_PAR     1
#else
# define A6_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA6_in)
#  undef  A6_DDR
#  define A6_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA6_out0)
#   undef  A6_DAT
#   define A6_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA6_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA7_qspi_cs3)
# define A7_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA7_dout3)
# define A7_PAR     2
#else
# define A7_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA7_in)
#  undef  A7_DDR
#  define A7_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA7_out0)
#   undef  A7_DAT
#   define A7_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA7_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA8_fsc0_fsr0)
# define A8_PAR     1
#else
# define A8_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA8_in)
#  undef  A8_DDR
#  define A8_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA8_out0)
#   undef  A8_DAT
#   define A8_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA8_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA9_dgnt0)
# define A9_PAR     1
#else
# define A9_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA9_in)
#  undef  A9_DDR
#  define A9_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA9_out0)
#   undef  A9_DAT
#   define A9_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA9_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA10_dreq0)
# define A10_PAR     1
#else
# define A10_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA10_in)
#  undef  A10_DDR
#  define A10_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA10_out0)
#   undef  A10_DAT
#   define A10_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA10_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA11_qspi_cs1)
# define A11_PAR     2
#else
# define A11_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA11_in)
#  undef  A11_DDR
#  define A11_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA11_out0)
#   undef  A11_DAT
#   define A11_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA11_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA12_dfsc2)
# define A12_PAR     1
#else
# define A12_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA12_in)
#  undef  A12_DDR
#  define A12_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA12_out0)
#   undef  A12_DAT
#   define A12_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA12_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA13_dfsc3)
# define A13_PAR     1
#else
# define A13_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA13_in)
#  undef  A13_DDR
#  define A13_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA13_out0)
#   undef  A13_DAT
#   define A13_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA13_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA14_dreq1)
# define A14_PAR     1
#else
# define A14_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA14_in)
#  undef  A14_DDR
#  define A14_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA14_out0)
#   undef  A14_DAT
#   define A14_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA14_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA15_dgnt1)
# define A15_PAR     1
#else
# define A15_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA15_in)
#  undef  A15_DDR
#  define A15_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA15_out0)
#   undef  A15_DAT
#   define A15_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTA_PA15_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#define A_DAT ((A0_DAT  <<  0) | (A1_DAT  <<  1) | (A2_DAT  <<  2) | (A3_DAT  <<  3) |  \
               (A4_DAT  <<  4) | (A5_DAT  <<  5) | (A6_DAT  <<  6) | (A7_DAT  <<  7) |  \
               (A8_DAT  <<  8) | (A9_DAT  <<  9) | (A10_DAT << 10) | (A11_DAT << 11) |  \
               (A12_DAT << 12) | (A13_DAT << 13) | (A14_DAT << 14) | (A15_DAT << 15))
#define A_DDR ((A0_DDR  <<  0) | (A1_DDR  <<  1) | (A2_DDR  <<  2) | (A3_DDR  <<  3) |  \
               (A4_DDR  <<  4) | (A5_DDR  <<  5) | (A6_DDR  <<  6) | (A7_DDR  <<  7) |  \
               (A8_DDR  <<  8) | (A9_DDR  <<  9) | (A10_DDR << 10) | (A11_DDR << 11) |  \
               (A12_DDR << 12) | (A13_DDR << 13) | (A14_DDR << 14) | (A15_DDR << 15))
#define A_PAR ((A0_PAR  <<  0) | (A1_PAR  <<  2) | (A2_PAR  <<  4) | (A3_PAR  <<  6) |  \
               (A4_PAR  <<  8) | (A5_PAR  << 10) | (A6_PAR  << 12) | (A7_PAR  << 14) |  \
               (A8_PAR  << 16) | (A9_PAR  << 18) | (A10_PAR << 20) | (A11_PAR << 22) |  \
               (A12_PAR << 24) | (A13_PAR << 26) | (A14_PAR << 28) | (A15_PAR << 30))

// ----------------------------------------------------------------------------
#define B0_DAT      1
#define B1_DAT      1
#define B2_DAT      1
#define B3_DAT      1
#define B4_DAT      1
#define B5_DAT      1
#define B6_DAT      1
#define B7_DAT      1
#define B8_DAT      1
#define B9_DAT      1
#define B10_DAT     1
#define B11_DAT     1
#define B12_DAT     1
#define B13_DAT     1
#define B14_DAT     1
#define B15_DAT     1

#define B0_DDR      0
#define B1_DDR      0
#define B2_DDR      0
#define B3_DDR      0
#define B4_DDR      0
#define B5_DDR      0
#define B6_DDR      0
#define B7_DDR      0
#define B8_DDR      0
#define B9_DDR      0
#define B10_DDR     0
#define B11_DDR     0
#define B12_DDR     0
#define B13_DDR     0
#define B14_DDR     0
#define B15_DDR     0

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB0_txd0)
# define B0_PAR     1
#else
# define B0_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB0_in)
#  undef  B0_DDR
#  define B0_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB0_out0)
#   undef  B0_DAT
#   define B0_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB0_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB1_rxd0)
# define B1_PAR     1
#else
# define B1_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB1_in)
#  undef  B1_DDR
#  define B1_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB1_out0)
#   undef  B1_DAT
#   define B1_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB1_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB2_cts0)
# define B2_PAR     1
#else
# define B2_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB2_in)
#  undef  B2_DDR
#  define B2_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB2_out0)
#   undef  B2_DAT
#   define B2_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB2_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB3_rts0)
# define B3_PAR     1
#else
# define B3_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB3_in)
#  undef  B3_DDR
#  define B3_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB3_out0)
#   undef  B3_DAT
#   define B3_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB3_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB4_clk0)
# define B4_PAR     1
#else
# define B4_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB4_in)
#  undef  B4_DDR
#  define B4_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB4_out0)
#   undef  B4_DAT
#   define B4_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB4_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB5_ta)
# define B5_PAR     1
#else
# define B5_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB5_in)
#  undef  B5_DDR
#  define B5_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB5_out0)
#   undef  B5_DAT
#   define B5_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB5_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

# define B6_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB6_in)
#  undef  B6_DDR
#  define B6_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB6_out0)
#   undef  B6_DAT
#   define B6_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB6_out1)
#   error GPIO data mismatch
#  endif
# endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB7_tout0)
# define B7_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB7_dout3)
# define B7_PAR     2
#else
# define B7_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB7_in)
#  undef  B7_DDR
#  define B7_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB7_out0)
#   undef  B7_DAT
#   define B7_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB7_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB8_etxd3)
# define B8_PAR     1
#else
# define B8_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB8_in)
#  undef  B8_DDR
#  define B8_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB8_out0)
#   undef  B8_DAT
#   define B8_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB8_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB9_etxd2)
# define B9_PAR     1
#else
# define B9_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB9_in)
#  undef  B9_DDR
#  define B9_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB9_out0)
#   undef  B9_DAT
#   define B9_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB9_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB10_etxd1)
# define B10_PAR     1
#else
# define B10_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB10_in)
#  undef  B10_DDR
#  define B10_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB10_out0)
#   undef  B10_DAT
#   define B10_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB10_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB11_erxd3)
# define B11_PAR     1
#else
# define B11_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB11_in)
#  undef  B11_DDR
#  define B11_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB11_out0)
#   undef  B11_DAT
#   define B11_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB11_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB12_erxd2)
# define B12_PAR     1
#else
# define B12_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB12_in)
#  undef  B12_DDR
#  define B12_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB12_out0)
#   undef  B12_DAT
#   define B12_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB12_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB13_erxd1)
# define B13_PAR     1
#else
# define B13_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB13_in)
#  undef  B13_DDR
#  define B13_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB13_out0)
#   undef  B13_DAT
#   define B13_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB13_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB14_erxer)
# define B14_PAR     1
#else
# define B14_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB14_in)
#  undef  B14_DDR
#  define B14_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB14_out0)
#   undef  B14_DAT
#   define B14_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB14_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB15_e_mdc)
# define B15_PAR     1
#else
# define B15_PAR     0
# if !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB15_in)
#  undef  B15_DDR
#  define B15_DDR    1
#  if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB15_out0)
#   undef  B15_DAT
#   define B15_DAT   0
#  elif !defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTB_PB15_out1)
#   error GPIO data mismatch
#  endif
# endif
#endif

#define B_DAT ((B0_DAT  <<  0) | (B1_DAT  <<  1) | (B2_DAT  <<  2) | (B3_DAT  <<  3) |  \
               (B4_DAT  <<  4) | (B5_DAT  <<  5) | (B6_DAT  <<  6) | (B7_DAT  <<  7) |  \
               (B8_DAT  <<  8) | (B9_DAT  <<  9) | (B10_DAT << 10) | (B11_DAT << 11) |  \
               (B12_DAT << 12) | (B13_DAT << 13) | (B14_DAT << 14) | (B15_DAT << 15))
#define B_DDR ((B0_DDR  <<  0) | (B1_DDR  <<  1) | (B2_DDR  <<  2) | (B3_DDR  <<  3) |  \
               (B4_DDR  <<  4) | (B5_DDR  <<  5) | (B6_DDR  <<  6) | (B7_DDR  <<  7) |  \
               (B8_DDR  <<  8) | (B9_DDR  <<  9) | (B10_DDR << 10) | (B11_DDR << 11) |  \
               (B12_DDR << 12) | (B13_DDR << 13) | (B14_DDR << 14) | (B15_DDR << 15))
#define B_PAR ((B0_PAR  <<  0) | (B1_PAR  <<  2) | (B2_PAR  <<  4) | (B3_PAR  <<  6) |  \
               (B4_PAR  <<  8) | (B5_PAR  << 10) | (B6_PAR  << 12) | (B7_PAR  << 14) |  \
               (B8_PAR  << 16) | (B9_PAR  << 18) | (B10_PAR << 20) | (B11_PAR << 22) |  \
               (B12_PAR << 24) | (B13_PAR << 26) | (B14_PAR << 28) | (B15_PAR << 30))
               
// ----------------------------------------------------------------------------

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC0_in)
# define C0_DDR     0
# define C0_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC0_out0)
# define C0_DDR     1
# define C0_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC0_out1)
# define C0_DDR     1
# define C0_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC1_in)
# define C1_DDR     0
# define C1_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC1_out0)
# define C1_DDR     1
# define C1_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC1_out1)
# define C1_DDR     1
# define C1_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC2_in)
# define C2_DDR     0
# define C2_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC2_out0)
# define C2_DDR     1
# define C2_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC2_out1)
# define C2_DDR     1
# define C2_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC3_in)
# define C3_DDR     0
# define C3_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC3_out0)
# define C3_DDR     1
# define C3_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC3_out1)
# define C3_DDR     1
# define C3_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC4_in)
# define C4_DDR     0
# define C4_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC4_out0)
# define C4_DDR     1
# define C4_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC4_out1)
# define C4_DDR     1
# define C4_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC5_in)
# define C5_DDR     0
# define C5_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC5_out0)
# define C5_DDR     1
# define C5_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC5_out1)
# define C5_DDR     1
# define C5_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC6_in)
# define C6_DDR     0
# define C6_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC6_out0)
# define C6_DDR     1
# define C6_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC6_out1)
# define C6_DDR     1
# define C6_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC7_in)
# define C7_DDR     0
# define C7_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC7_out0)
# define C7_DDR     1
# define C7_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC7_out1)
# define C7_DDR     1
# define C7_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC8_in)
# define C8_DDR     0
# define C8_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC8_out0)
# define C8_DDR     1
# define C8_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC8_out1)
# define C8_DDR     1
# define C8_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC9_in)
# define C9_DDR     0
# define C9_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC9_out0)
# define C9_DDR     1
# define C9_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC9_out1)
# define C9_DDR     1
# define C9_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC10_in)
# define C10_DDR     0
# define C10_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC10_out0)
# define C10_DDR     1
# define C10_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC10_out1)
# define C10_DDR     1
# define C10_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC11_in)
# define C11_DDR     0
# define C11_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC11_out0)
# define C11_DDR     1
# define C11_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC11_out1)
# define C11_DDR     1
# define C11_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC12_in)
# define C12_DDR     0
# define C12_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC12_out0)
# define C12_DDR     1
# define C12_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC12_out1)
# define C12_DDR     1
# define C12_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC13_in)
# define C13_DDR     0
# define C13_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC13_out0)
# define C13_DDR     1
# define C13_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC13_out1)
# define C13_DDR     1
# define C13_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC14_in)
# define C14_DDR     0
# define C14_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC14_out0)
# define C14_DDR     1
# define C14_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC14_out1)
# define C14_DDR     1
# define C14_DAT     1
#else
# error GPIO data mismatch
#endif

#if defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC15_in)
# define C15_DDR     0
# define C15_DAT     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC15_out0)
# define C15_DDR     1
# define C15_DAT     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTC_PC0_out1)
# define C15_DDR     1
# define C15_DAT     1
#else
# error GPIO data mismatch
#endif

#define C_DAT ((C0_DAT  <<  0) | (C1_DAT  <<  1) | (C2_DAT  <<  2) | (C3_DAT  <<  3) |  \
               (C4_DAT  <<  4) | (C5_DAT  <<  5) | (C6_DAT  <<  6) | (C7_DAT  <<  7) |  \
               (C8_DAT  <<  8) | (C9_DAT  <<  9) | (C10_DAT << 10) | (C11_DAT << 11) |  \
               (C12_DAT << 12) | (C13_DAT << 13) | (C14_DAT << 14) | (C15_DAT << 15))
#define C_DDR ((C0_DDR  <<  0) | (C1_DDR  <<  1) | (C2_DDR  <<  2) | (C3_DDR  <<  3) |  \
               (C4_DDR  <<  4) | (C5_DDR  <<  5) | (C6_DDR  <<  6) | (C7_DDR  <<  7) |  \
               (C8_DDR  <<  8) | (C9_DDR  <<  9) | (C10_DDR << 10) | (C11_DDR << 11) |  \
               (C12_DDR << 12) | (C13_DDR << 13) | (C14_DDR << 14) | (C15_DDR << 15))

// ----------------------------------------------------------------------------

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD0_none)
# define D0_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD0_dcl0)
# define D0_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD0_clk1)
# define D0_PAR     2
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD1_none)
# define D1_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD1_din0)
# define D1_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD1_rxd1)
# define D1_PAR     2
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD2_none)
# define D2_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD2_cts1)
# define D2_PAR     2
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD2_qspi_cs2)
# define D2_PAR     3
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD3_none)
# define D3_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD3_rts1)
# define D3_PAR     2
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD3_int5)
# define D3_PAR     3
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD4_none)
# define D4_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD4_dout0)
# define D4_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD4_txd1)
# define D4_PAR     2
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD5_none)
# define D5_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD5_din3)
# define D5_PAR     2
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD5_int4)
# define D5_PAR     3
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD6_none)
# define D6_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD6_pwm_out1)
# define D6_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD6_tout1)
# define D6_PAR     2
#else
# error GPIO data mismatch
#endif

#if   defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD7_none)
# define D7_PAR     0
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD7_pwm_out2)
# define D7_PAR     1
#elif defined(CYGHWR_HAL_M68K_MCF5272_GPIO_PORTD_PD7_tin1)
# define D7_PAR     2
#else
# error GPIO data mismatch
#endif

#define D_PAR ((D0_PAR  <<  0) | (D1_PAR  <<  2) | (D2_PAR  <<  4) | (D3_PAR  <<  6) |  \
               (D4_PAR  <<  8) | (D5_PAR  << 10) | (D6_PAR  << 12) | (D7_PAR  << 14))
