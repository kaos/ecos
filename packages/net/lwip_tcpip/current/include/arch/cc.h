//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2011 Free Software Foundation
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================

#ifndef __LWIP_ARCH_CC_H__
#define __LWIP_ARCH_CC_H__

#include <string.h>
#include <errno.h>
#include <cyg/infra/cyg_type.h>

typedef int sys_prot_t;

#define LWIP_ERR_T int

// Define error codes used by lwIP but not by eCos
#define EFAULT          14
#define ELOOP           40
#define ETIME           62
#define ENSRNOTFOUND    163

// If errno support, define ERRNO used in api/sockets.c
#if CYGINT_ISO_ERRNO > 0
#define ERRNO
#endif

// Platform byteorder
#if (CYG_BYTEORDER == CYG_LSBFIRST)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif

// Define generic types used in lwIP
#ifndef __U8_T__
typedef unsigned char u8_t;
# define __U8_T__
#endif

#ifndef __S8_T__
typedef signed char s8_t;
# define __S8_T__
#endif

#ifndef __U16_T__
typedef unsigned short u16_t;
# define __U16_T__
#endif

#ifndef __S16_T__
typedef signed short s16_t;
# define __S16_T__
#endif

#ifndef __U32_T__
typedef unsigned long u32_t;
# define __U32_T__
#endif

#ifndef __S32_T__
typedef signed long s32_t;
# define __S32_T__
#endif

typedef unsigned    long    mem_ptr_t;

// Define (sn)printf formatters for these lwIP types
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

// Compiler hints for packing structures
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

// Prototypes for printf() and abort()
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_ass.h>
// Plaform specific diagnostic output
#define LWIP_PLATFORM_DIAG(x) do {diag_printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x) do {CYG_FAIL(x);} while(0)

#endif // __LWIP_ARCH_CC_H__
