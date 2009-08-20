#ifndef CYGONCE_CONFIG_H
#define CYGONCE_CONFIG_H
// ==========================================================================
//
//      config.h
//
//      Manually constructed uSTL configuration file for eCos RTOS
//
// ===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.
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
// ===========================================================================
// ===========================================================================
// #####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: 
// Date:         2009-07-28
// Purpose:      eCos specific uSTL configuration
// Description:
//
// ####DESCRIPTIONEND####
//
// ========================================================================*/
#include <stdint.h>
#include <pkgconf/ustl.h> // ustl package configuration


//
// If there is no RTTI support (normally this is the default eCos setting)
// C++ typeid keyword is not supported. We hide this by using a macro instead
// of using typeid keyword directly.
//
#ifdef __GXX_RTTI
    #define USTL_TYPENAME(_t_) typeid(_t_).name()
#else
    #define USTL_TYPENAME(_t_) "unknown type (RTTI disabled)"
#endif

//
// If there is no exception support (normally this is the default eCos setting)
// C++ keywords try, catch and throw are not supported. To compile uSTL without
// exception support we use macros USLT_TRY, USLT_CATCH_ALL and USLT_THROW
// instead of the real keywords. To handle exceptions in debug builds we define 
// a "lightwight" exception handler. This handler is simply a function that 
// prints exception information do diagnostic channel.
//
#ifdef __EXCEPTIONS
    #define USTL_TRY try
    #define USTL_CATCH_ALL catch (...) {}
    #define USTL_THROW(_exception_) throw (_exception_)
#else
	namespace ustl
	{
	class exception;
	extern void exception_handler(const exception& ex);
	}
    #define USTL_TRY
    #define USTL_CATCH_ALL
    #define USTL_THROW(_exception_) ustl::exception_handler(_exception_)
#endif

//
// There is no eCos header that exports this function so we do it here
//
#ifdef CYGCLS_USTL_FSTREAMS
__externC int ioctl( int fd, CYG_ADDRWORD com, ... );
#endif


/// Define to 1 if you want stream operations to throw exceptions on
/// insufficient data or insufficient space. All these errors should
/// be preventable in output code; the input code should verify the
/// data in a separate step. It slows down stream operations a lot,
/// but it's your call. By default only debug builds throw.
///
#ifdef CYGSEM_USTL_STREAM_BOUNDS_CHECK
#define WANT_STREAM_BOUNDS_CHECKING 1 
#else
#undef WANT_STREAM_BOUNDS_CHECKING
#endif

#if !defined(WANT_STREAM_BOUNDS_CHECKING) && !defined(NDEBUG)
    #define WANT_STREAM_BOUNDS_CHECKING 1
#endif

/// Define to 1 if you want backtrace symbols demangled.
/// This adds some 15k to the library size, and requires that you link it and
/// any executables you make with the -rdynamic flag (increasing library size
/// even more). By default this option is disable. Enabling this option also
/// pulls in __cxa_demangle from libsupc++
#undef WANT_NAME_DEMANGLING

/// Define to 1 if you want to build without libstdc++
#define WITHOUT_LIBSTDCPP 1

/// Define GNU extensions if unavailable.
#ifndef __GNUC__
    /// GCC (and some other compilers) define '__attribute__'; ustl is using this
    /// macro to alert the compiler to flag inconsistencies in printf/scanf-like
    /// function calls.  Just in case '__attribute__' is undefined, make a dummy.
    /// 
    #ifndef __attribute__
    #define __attribute__(p)
    #endif
#endif
#if defined(__GNUC__) && __GNUC__ >= 4
    #define INLINE      __attribute__((always_inline))
#else
    #define INLINE
#endif
#define DLL_EXPORT
#define DLL_LOCAL
#if defined(__GNUC__) && __GNUC__ >= 3 && (__i386__ || __x86_64__)
    /// GCC 3+ supports the prefetch directive, which some CPUs use to improve caching
    #define prefetch(p,rw,loc)  __builtin_prefetch(p,rw,loc)
#else
    #define prefetch(p,rw,loc)
#endif
#if !defined(__GNUC__) || __GNUC__ < 3
    /// __alignof__ returns the recommended alignment for the type
    #define __alignof__(v)  min(sizeof(v), sizeof(void*))
    /// This macro returns 1 if the value of x is known at compile time.
    #ifndef __builtin_constant_p
    #define __builtin_constant_p(x) 0
    #endif
#endif

// Define to 1 if you have the `atexit' function.
#define HAVE_ATEXIT 1

// Define to 1 if you have the <assert.h> header file.
#define HAVE_ASSERT_H 1

// Define to 1 if you have the <ctype.h> header file.
#define HAVE_CTYPE_H 1

// Define to 1 if you have the <errno.h> header file.
#define HAVE_ERRNO_H 1

// Define to 1 if you have the <fcntl.h> header file.
#define HAVE_FCNTL_H 1

// Define to 1 if you have the <float.h> header file.
#define HAVE_FLOAT_H 1

// Define to 1 if you have the <limits.h> header file.
#define HAVE_LIMITS_H 1

// Define to 1 if you have the <locale.h> header file.
#define HAVE_LOCALE_H 1

// Define to 1 if your system has a working `malloc' function.
#define HAVE_MALLOC 1

// Define to 1 if you have the `memchr' function.
#define HAVE_MEMCHR 1

// Define to 1 if you have the `memmove' function.
#define HAVE_MEMMOVE 1

// Define to 1 if you have the `memset' function.
#define HAVE_MEMSET 1

// Define to 1 if the system has the type `ptrdiff_t'.
#define HAVE_PTRDIFF_T 1

// Define to 1 if you have the <signal.h> header file.
#define HAVE_SIGNAL_H 1

// Define to 1 if you have the <stdarg.h> header file.
#define HAVE_STDARG_H 1

// Define to 1 if you have the <stddef.h> header file.
#define HAVE_STDDEF_H 1

// Define to 1 if you have the <stdint.h> header file.
#define HAVE_STDINT_H 1

// Define to 1 if you have the <stdio.h> header file.
#define HAVE_STDIO_H 1

// Define to 1 if you have the <stdlib.h> header file.
#define HAVE_STDLIB_H 1

// Define to 1 if you have the `strerror' function.
#define HAVE_STRERROR 1

// Define to 1 if you have the <string.h> header file.
#define HAVE_STRING_H 1

// Define to 1 if you have the `strrchr' function.
#define HAVE_STRRCHR 1

// Define to 1 if you have the `strtol' function.
#define HAVE_STRTOL 1

// Define to 1 if you have the <sys/stat.h> header file.
#define HAVE_SYS_STAT_H 1

// Define to 1 if you have the <sys/types.h> header file.
#define HAVE_SYS_TYPES_H 1

// Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible.
#define HAVE_SYS_WAIT_H 1

// Define to 1 if you have the <time.h> header file.
#define HAVE_TIME_H 1

// Define to 1 if you have the <unistd.h> header file.
#define HAVE_UNISTD_H 1

// Define to 1 if you have the <math.h> header file.
#define HAVE_MATH_H 1

// Define to 1 if you have the long long type
#define HAVE_LONG_LONG 1

// Define to 1 if you have 64 bit types available
#define HAVE_INT64_T 1

// Define to 1 if your compiler treats char as a separate type along with
// signed char and unsigned char. This will create overloads for char.
#define HAVE_THREE_CHAR_TYPES 1

// Define to 1 if you have the <cxxabi.h> header file.
#if __GNUC__ >= 3
    #define HAVE_CXXABI_H 1
#endif

// Define to 1 if you have the rintf function. Will use rint otherwise.
#undef HAVE_RINTF

// Define to 1 if you have the <strings.h> header file.
#undef HAVE_STRINGS_H

// Define to 1 if you have the `strsignal' function.
#undef HAVE_STRSIGNAL

// Define to 1 if you have the __va_copy function
#undef HAVE_VA_COPY

// Define to 1 if you have the <inttypes.h> header file.
#undef HAVE_INTTYPES_H

// Define to 1 if you have the <malloc.h> header file.
#undef HAVE_MALLOC_H

// Define to 1 if you have the <memory.h> header file.
#undef HAVE_MEMORY_H

// Define to 1 if you have the <alloca.h> header file.
#undef HAVE_ALLOCA_H

// Define to 1 if `stat' has the bug that it succeeds when given the
// zero-length file name argument.
#undef HAVE_STAT_EMPTY_STRING_BUG

// STDC_HEADERS is defined to 1 on sane systems.
#if defined(HAVE_ASSERT_H) && defined(HAVE_CTYPE_H) &&\
    defined(HAVE_ERRNO_H) && defined(HAVE_FLOAT_H) &&\
    defined(HAVE_LIMITS_H) && defined(HAVE_LOCALE_H) &&\
    defined(HAVE_MATH_H) && defined(HAVE_SIGNAL_H) &&\
    defined(HAVE_STDARG_H) && defined(HAVE_STDDEF_H) &&\
    defined(HAVE_STDIO_H) && defined(HAVE_STDLIB_H) &&\
    defined(HAVE_STRING_H) && defined(HAVE_TIME_H)
#define STDC_HEADERS 1
#endif

// STDC_HEADERS is defined to 1 on unix systems.
#if defined(HAVE_FCNTL_H) && defined(HAVE_SYS_STAT_H) && defined(HAVE_UNISTD_H)
#define STDUNIX_HEADERS 1
#endif

// Define to 1 if you have the <byteswap.h> header file.
#if (__GNUC__ >= 3) // gcc 2.95 somehow doesn't recognize 'asm volatile' in libc byteswap.h
#undef HAVE_BYTESWAP_H
#endif

// Define to 1 if `lstat' dereferences a symlink specified with a trailing slash.
#undef LSTAT_FOLLOWS_SLASHED_SYMLINK

// Define as the return type of signal handlers (`int' or `void').
#undef RETSIGTYPE

// Define to 1 if you want unrolled specializations for fill and copy
#undef WANT_UNROLLED_COPY

// Define to 1 if you want to use MMX/SSE/3dNow! processor instructions
#undef WANT_MMX

// Define to byte sizes of types
#define SIZE_OF_CHAR      1
#define SIZE_OF_SHORT     2
#define SIZE_OF_INT       4
#define SIZE_OF_LONG      4
#define SIZE_OF_LONG_LONG 8
#define SIZE_OF_POINTER   4
#define SIZE_OF_SIZE_T    4
#define SIZE_OF_BOOL      1


// Byte order macros, converted in utypes.h
#define USTL_LITTLE_ENDIAN	4321
#define USTL_BIG_ENDIAN		1234
#if (CYG_BYTEORDER == CYG_LSBFIRST)
#define USTL_BYTE_ORDER		USTL_BIG_ENDIAN
#else
#define USTL_BYTE_ORDER     USTL_LITTLE_ENDIAN
#endif

// Extended CPU capabilities
#undef CPU_HAS_FPU
#undef CPU_HAS_EXT_DEBUG
#undef CPU_HAS_TIMESTAMPC
#undef CPU_HAS_MSR
#undef CPU_HAS_CMPXCHG8
#undef CPU_HAS_APIC
#undef CPU_HAS_SYSCALL
#undef CPU_HAS_MTRR
#undef CPU_HAS_CMOV
#undef CPU_HAS_FCMOV
#if defined WANT_MMX
#undef CPU_HAS_MMX
#undef CPU_HAS_FXSAVE
#undef CPU_HAS_SSE 
#undef CPU_HAS_SSE2
#undef CPU_HAS_SSE3
#undef CPU_HAS_EXT_3DNOW
#undef CPU_HAS_3DNOW
#endif

// GCC vector extensions
#if defined(CPU_HAS_MMX) || defined(CPU_HAS_SSE)
    #undef HAVE_VECTOR_EXTENSIONS
#endif

#if defined(CPU_HAS_SSE) && defined(__GNUC__)
    #define __sse_align	__attribute__((aligned(16)))
#else
    #define __sse_align	
#endif


// Define to empty if `const' does not conform to ANSI C.
//#undef const

// Define as `__inline' if that's what the C compiler calls it, or to nothing
// if it is not supported.
//#undef inline

// Define to `long' if <sys/types.h> does not define.
//#undef off_t

// Define to `unsigned' if <sys/types.h> does not define.
//#undef size_t

// ---------------------------------------------------------------------------
#endif	// CYGONCE_CONFIG_H

