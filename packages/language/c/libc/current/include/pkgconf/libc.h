#ifndef CYGONCE_PKGCONF_LIBC_H
#define CYGONCE_PKGCONF_LIBC_H
//========================================================================
//
//      libc.h
//
//      Configuration header for the ANSI standard C library
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-20
// Purpose:       Configuration of the ISO C library
// Description:   The preprocessor defines here can be used to configure
//                the ISO C library, preferably with a graphical tool,
//                but also manually if required.
// Usage:         #include <pkgconf/libc.h>  
//              
//
//####DESCRIPTIONEND####
//
//========================================================================

// INCLUDES

// Include system configuration file to work out whether the C library is
// to be included at all
#include <pkgconf/system.h>

/*
    {{CFG_DATA

        cdl_package CYGPKG_LIBC {
            display      "C library"
            description  "This package enables compatibility with the ISO
                          C standard - ISO/IEC 9899:1990. This allows the
                          user application to use well known standard
                          C library functions, and in eCos starts a
                          thread to invoke the user function main()"
            type         boolean
            requires     CYGPKG_ERROR
            doc          ref/ecos-ref/the-iso-standard-c-and-math-libraries.html
        }

    }}CFG_DATA
*/

//========================================================================

// CTYPE CONFIGURATION OPTIONS

/*
    {{CFG_DATA
        cdl_option CYGIMP_LIBC_CTYPE_INLINES {
            display      "Inline versions of <ctype.h> functions"
            description  "This option chooses whether the simple character
                          classification and conversion functions (e.g.
                          isupper(), isalpha(), toupper(), etc.)
                          from <ctype.h> are available as inline
                          functions. This may improve performance and as
                          the functions are small, may even improve code
                          size."
            type         boolean
            parent       CYGPKG_LIBC
        }
    }}CFG_DATA
*/

#define CYGIMP_LIBC_CTYPE_INLINES

//========================================================================

// STRING CONFIGURATION OPTIONS

/*
    {{CFG_DATA
        cdl_component CYGPKG_LIBC_STRING {
            display      "String functions"
            description  "Options associated with the standard string
                          functions"
            type         dummy
            parent       CYGPKG_LIBC
        }

        cdl_option CYGIMP_LIBC_STRING_INLINES {
            display      "Inline versions of <string.h> functions"
            description  "This option chooses whether some of the
                          particularly simple string functions from
                          <string.h> are available as inline
                          functions. This may improve performance, and as
                          the functions are small, may even improve code
                          size."
            type         boolean
            parent       CYGPKG_LIBC_STRING
        }

        cdl_component CYGPKG_LIBC_STRING_OPTIMIZATIONS {
            display      "Options for string function optimization"
            description  "This component lets you choose between different
                          implementations of the string functions with
                          different tradeoffs. Most of the string
                          functions can be implemented in two ways. One
                          approach is geared towards smaller code size,
                          while the other tries to improve execution speed
                          at the expense of code size. "
            type         dummy
            parent       CYGPKG_LIBC_STRING
        }

        cdl_option CYGIMP_LIBC_STRING_PREFER_FAST {
            display      "Fast"
            description  "This option improves string function execution
                          speed at the expense of code size. This
                          option is overriden if the code is
                          compiled with the -Os option to the compiler."
            type         radio
            parent       CYGPKG_LIBC_STRING_OPTIMIZATIONS
        }

        cdl_option CYGIMP_LIBC_STRING_PREFER_SMALL {
            display      "Small"
            description  "This option tries to reduce string function
                          code size at the expense of execution speed. The
                          same effect can be produced if the code is
                          compiled with the -Os option to the compiler."
            type         radio
            parent       CYGPKG_LIBC_STRING_OPTIMIZATIONS
        }

        cdl_component CYGPKG_LIBC_STRING_STRTOK {
            display      "strtok"
            description  "These options control the behaviour of the
                          strtok() and strtok_r() string tokenization
                          functions."
            type         dummy
            parent       CYGPKG_LIBC_STRING
        }

        cdl_option CYGSEM_LIBC_PER_THREAD_STRTOK {
            display      "Per-thread strtok()"
            description  "This option controls whether the string function
                          strtok() has its state recorded on a per-thread
                          basis rather than global. If this option is
                          disabled, some per-thread space can be saved.
                          Note there is also a POSIX-standard strtok_r()
                          function to achieve a similar effect with user
                          support. Enabling this option will use one slot
                          of kernel per-thread data. You should ensure you
                          have enough slots configured for all your
                          per-thread data."
            doc          ref/ecos-ref/c-and-math-library-overview-thread-safety.html
            type         boolean
            parent       CYGPKG_LIBC_STRING_STRTOK
            requires     CYGVAR_KERNEL_THREADS_DATA
        }

        cdl_option CYGNUM_LIBC_STRTOK_TRACE_LEVEL {
            display      "Tracing level"
            description  "Trace verbosity level for debugging the <string.h>
                          functions strtok() and strtok_r(). Increase this
                          value to get additional trace output."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_STRING_STRTOK
        }

    }}CFG_DATA
*/

#define CYGIMP_LIBC_STRING_INLINES

#undef CYGIMP_LIBC_STRING_PREFER_SMALL

#define CYGIMP_LIBC_STRING_PREFER_FAST

#ifdef CYGIMP_LIBC_STRING_PREFER_SMALL
# define CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST
#endif

#undef CYGSEM_LIBC_PER_THREAD_STRTOK

#define CYGNUM_LIBC_STRTOK_TRACE_LEVEL 0

//========================================================================

// SETJMP CONFIGURATION OPTIONS

/*
    {{CFG_DATA
        cdl_option CYGIMP_LIBC_SETJMP_INLINES {
            display     "Inline version of the longjmp() function"
            description "This option chooses whether the longjmp() function
                         is available as an inline function. This may
                         improve performance, and as the function is small
                         may even improve code size. "
            type        boolean
            parent      CYGPKG_LIBC
        }
    }}CFG_DATA
*/

#define CYGIMP_LIBC_SETJMP_INLINES

//========================================================================

// STDLIB CONFIGURATION OPTIONS


/*
    {{CFG_DATA
        cdl_component CYGPKG_LIBC_STDLIB {
            display     "Standard utility functions"
            description "Options associated with the standard utility
                         functions in <stdlib.h>"
            type        dummy
            parent      CYGPKG_LIBC
        }

        cdl_option CYGIMP_LIBC_STDLIB_INLINES {
            display     "Inline versions of <stdlib.h> functions"
            description "This option chooses whether some of the
                         particularly simple standard utility functions
                         from <stdlib.h> are available as inline
                         functions. This may improve performance, and as
                         the functions are small, may even improve code
                         size."
            type        boolean
            parent      CYGPKG_LIBC_STDLIB
        }

        cdl_component CYGPKG_LIBC_RAND {
            display      "Random number generation"
            description  "These options control the behaviour of the
                          functions rand(), srand() and rand_r()"
            type         dummy
            parent       CYGPKG_LIBC_STDLIB
        }

        cdl_option CYGSEM_LIBC_PER_THREAD_RAND {
            display      "Per-thread random seed"
            description  "This option controls whether the pseudo-random
                          number generation functions rand() and srand()
                          have their state recorded on a per-thread
                          basis rather than global. If this option is
                          disabled, some per-thread space can be saved.
                          Note there is also a POSIX-standard rand_r()
                          function to achieve a similar effect with user
                          support. Enabling this option will use one slot
                          of kernel per-thread data. You should ensure you
                          have enough slots configured for all your
                          per-thread data."
            doc          ref/ecos-ref/c-and-math-library-overview-thread-safety.html
            type         boolean
            requires     CYGVAR_KERNEL_THREADS_DATA
            parent       CYGPKG_LIBC_RAND
        }

        cdl_option CYGNUM_LIBC_RAND_SEED {
            display      "Random number seed"
            description  "This selects the initial random number seed for
                          rand()'s pseudo-random number generator. For
                          strict ISO standard compliance, this should be 1,
                          as per section 7.10.2.2 of the standard."
            type         count
            legal_values 0 to 0x7fffffff
            parent       CYGPKG_LIBC_RAND
        }

        cdl_option CYGNUM_LIBC_RAND_TRACE_LEVEL {
            display      "Tracing level"
            description  "Trace verbosity level for debugging the rand(),
                          srand() and rand_r() functions. Increase this
                          value to get additional trace output."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_RAND
        }

        cdl_option CYGIMP_LIBC_RAND_SIMPLEST {
            display      "Simplest implementation"
            description  "This provides a very simple implementation of rand()
                          that does not perform well with randomness in the
                          lower significant bits. However it is exceptionally
                          fast. It uses the sample algorithm from the ISO C
                          standard itself."
            type         radio
            parent       CYGPKG_LIBC_RAND
        }

        cdl_option CYGIMP_LIBC_RAND_SIMPLE1 {
            display      "Simple implementation #1"
            description  "This provides a very simple implementation of rand()
                          based on the simplest implementation above. However
                          it does try to work around the lack of randomness
                          in the lower significant bits, at the expense of a
                          little speed."
            type         radio
            parent       CYGPKG_LIBC_RAND
        }

        cdl_option CYGIMP_LIBC_RAND_KNUTH1 {
            display      "Knuth implementation #1"
            description  "This implements a slightly more complex algorithm
                          published in Donald E. Knuth's Art of Computer
                          Programming Vol.2 section 3.6 (p.185 in the 3rd ed.).
                          This produces better random numbers than the
                          simplest approach but is slower."
            type         radio
            parent       CYGPKG_LIBC_RAND
        }

        cdl_component CYGPKG_LIBC_MALLOC {
            display      "Support for dynamic memory allocation"
            description  "This enables support for dynamic memory
                          allocation as supplied by the functions malloc(),
                          free(), calloc() and realloc(). As these
                          functions are often used, but can have quite an
                          overhead, disabling them here can ensure they
                          cannot even be used accidentally when static
                          allocation is preferred."
            type         boolean
            parent       CYGPKG_LIBC_STDLIB
            requires     CYGPKG_KERNEL
        }

        cdl_option CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE {
            display      "Size of the dynamic memory pool in bytes"
            description  "At this stage, dynamic memory allocation by
                          malloc() and calloc() must be from a fixed-size,
                          contiguous memory pool (note here that it is the
                          pool that is of a fixed size, but malloc() is still
                          able to allocate variable sized chunks of memory
                          from it). This option is the size
                          of that pool, in bytes. Note that not all of
                          this is available for programs to
                          use - some is needed for internal information
                          about memory regions, and some may be lost to
                          ensure that memory allocation only returns
                          memory aligned on word (or double word)
                          boundaries - a very common architecture
                          constraint."
            type         count
            legal_values 32 to 0x7fffffff
            parent       CYGPKG_LIBC_MALLOC
        }

        cdl_option CYGIMP_LIBC_MALLOC_CXX_DELETE_CALLS_FREE {
            display      "Support for C++ delete operator"
            description  "C++ new and delete operators can call
                          the C library's malloc() and free() implicitly.
                          If this is what is required, enable this option.
                          However, if enabled, the dynamic memory allocation 
                          code is always linked in to the application,
                          even if it is not explicitly called and new/delete
                          are not used.
                          This increases code and data size needlessly."
            type         bool
            parent       CYGPKG_LIBC_MALLOC
        }

        cdl_option CYGFUN_LIBC_strtod {
            display      "Provides strtod() and atof()"
            description  "This option allows use of the utility function
                          strtod() (and consequently atof()) to convert
                          from string to double precision floating point
                          numbers. Disabling this option removes the
                          dependency on the math library package."
            type         boolean
            parent       CYGPKG_LIBC_STDLIB
            requires     CYGPKG_LIBM
        }

        cdl_option CYGNUM_LIBC_BSEARCH_TRACE_LEVEL {
            display      "bsearch() tracing level"
            description  "Trace verbosity level for debugging the <stdlib.h>
                          binary search function bsearch(). Increase this
                          value to get additional trace output."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_STDLIB
        }

        cdl_option CYGNUM_LIBC_QSORT_TRACE_LEVEL {
            display      "qsort() tracing level"
            description  "Trace verbosity level for debugging the <stdlib.h>
                          quicksort function qsort(). Increase this value
                          to get additional trace output."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_STDLIB
        }

    }}CFG_DATA
*/

#define CYGIMP_LIBC_STDLIB_INLINES

#undef CYGSEM_LIBC_PER_THREAD_RAND

#define CYGNUM_LIBC_RAND_SEED 1

#define CYGNUM_LIBC_RAND_TRACE_LEVEL 0

#undef CYGIMP_LIBC_RAND_SIMPLEST

#define CYGIMP_LIBC_RAND_SIMPLE1

#undef CYGIMP_LIBC_RAND_KNUTH1

#define CYGPKG_LIBC_MALLOC

#ifdef CYGPKG_LIBC_MALLOC

#undef CYGIMP_LIBC_MALLOC_CXX_DELETE_CALLS_FREE

#define CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE 16384

#endif // ifdef CYGPKG_LIBC_MALLOC

#define CYGFUN_LIBC_strtod

#define CYGNUM_LIBC_BSEARCH_TRACE_LEVEL 0

#define CYGNUM_LIBC_QSORT_TRACE_LEVEL 0

#ifdef CYGFUN_LIBC_strtod
# ifndef CYGPKG_LIBM
#  error You must enable the Math library package to use strtod() or atof()
# endif
#endif

//========================================================================

// STDIO CONFIGURATION OPTIONS

/*
    {{CFG_DATA
        cdl_component CYGPKG_LIBC_STDIO {
            display      "Standard Input/Output functions"
            description  "This enables support for standard I/O functions
                          from <stdio.h>."
            type         boolean
            parent       CYGPKG_LIBC
            requires     CYGPKG_IO
        }

        cdl_option CYGIMP_LIBC_STDIO_INLINES {
            display      "Inline versions of <stdio.h> functions"
            description  "This option chooses whether some of the
                          particularly simple functions from
                          <stdio.h> are available as inline
                          functions. This may improve performance, and as
                          the functions are small, may even improve code
                          size."
            type         boolean
            parent       CYGPKG_LIBC_STDIO
        }

        cdl_option FOPEN_MAX {
            display      "Permitted number of open files"
            description  "This option controls the guaranteed minimum
                          number of simultaneously open files. The ISO
                          C standard requires it to be defined (para
                          7.9.1), and if strictly compliant, it must be
                          at least 8 (para 7.9.3). In practice it can be
                          as low as 3 - for stdin, stdout and stderr."
            type         count
            legal_values 3 to 0x7fffffff
            parent       CYGPKG_LIBC_STDIO
        }

        cdl_option FILENAME_MAX {
            display      "Maximum length of filename"
            description  "This option defines the maximum allowed size of
                          a filename in characters. The ISO C standard
                          requires it to be defined (para 7.9.1)."
            type         count
            legal_values 1 to 0x7fffffff
            parent       CYGPKG_LIBC_STDIO
        }

        cdl_option L_tmpnam {
            display      "Maximum length of filenames for temporary files"
            description  "This option defines the maximum allowed size of
                          filenames for temporary files as generated by
                          tmpnam(). It is measured in characters, and the
                          ISO C standard requires it to be defined (para 
                          7.9.1)."
            type         count
            legal_values 1 to 0x7fffffff
            parent       CYGPKG_LIBC_STDIO
        }

        cdl_option TMP_MAX {
            display       "Unique file names generated by tmpnam()"
            description   "This option defines the minimum number of
                           unique file names generated by tmpnam(). The
                           ISO C standard requires it to be defined (para
                           7.9.1)."
            type          count
            legal_values  0 to 0x7fffffff
            parent        CYGPKG_LIBC_STDIO
        }

        cdl_component CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO {
            display       "Buffered I/O"
            description   "This option controls whether input/output
                           through the <stdio.h> functions is buffered.
                           This may save some memory per file. It
                           is equivalent to putting every file into
                           non-buffered mode (_IONBF) through setvbuf(),
                           except now it can never be altered back to
                           buffered mode. Disabling buffering is strictly 
                           non-compliant with the ISO C standard."
            type          boolean
            parent        CYGPKG_LIBC_STDIO
        }

        cdl_option CYGNUM_LIBC_STDIO_BUFSIZE {
            display        "Default buffer size"
            description    "This option defines the default size of buffer
                            used with calls to setbuf(), and is the default
                            size for buffered streams that have not had
                            either setbuf() or setvbuf() invoked on them.
                            It is exactly equivalent to the standard
                            constant BUFSIZ, except that it is 0 if
                            CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO is disabled.
                            The ISO C standard requires this to be defined
                            (para 7.9.1), and says it must be at least 256
                            (para 7.9.2)."
            type           count
            legal_values   1 to 0x7fffffff
            parent         CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
        }

        cdl_option CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF {
            display        "setbuf()/setvbuf() uses malloc()"
            description    "This option controls whether I/O buffers are
                            implemented dynamically within the stdio
                            implementation. Otherwise they will be static,
                            and cannot be changed by setbuf()/setvbuf(). If
                            they are static (i.e. this option is disabled),
                            any attempt to use an arbitrary buffer size, or
                            to pass a user-supplied buffer to setbuf() or
                            setvbuf() will fail - the buffer is implemented
                            internally as a static array of taking the size
                            of the configuration option BUFSIZ. The only
                            exception is if a user buffer is not supplied,
                            and the buffer size requested is less than
                            BUFSIZ. In this case, the buffer will
                            be configured to only use that amount of
                            buffering. However the unused memory left over
                            will NOT be freed. If this option is enabled,
                            then CYGPKG_LIBC_MALLOC must also
                            be enabled. Either setting can be considered to
                            be compliant with the ISO C standard."
            type           boolean
            parent         CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
            requires       CYGPKG_LIBC_MALLOC
        }

        cdl_option CYGFUN_LIBC_STDIO_ungetc {
            display         "Support for ungetc()"
            description     "This option controls whether ungetc() is
                             supported. If not, then some space can be
                             saved, and speed is improved. Note that you
                             MUST have this defined if you want to use the
                             scanf() family of functions."
            type            boolean
            parent          CYGPKG_LIBC_STDIO
        }

        cdl_option CYGPKG_LIBC_STDIO_OPEN {
            display         "Dynamic opening/closing of files"
            description     "fopen() and fclose() use dynamic memory
                             allocation routines to allocate memory for
                             new FILE structure pointers. This option,
                             along with CYGPKG_LIBC_MALLOC, should be
                             enabled to use
                             fopen() and fclose(). If disabled, only the
                             default console streams - stdin, stdout and
                             stderr - will be available."
            type            boolean
            parent          CYGPKG_LIBC_STDIO
            requires        CYGPKG_LIBC_MALLOC
        }

        cdl_option CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE {
            display         "Default console device"
            description     "This option allows you to choose the
                             default console device. In the current
                             implementation, all these devices begin
                             with the prefix /dev/ and are followed by
                             the name of a device. The name of the device
                             depends on the device drivers you have
                             configured in your system. For example,
                             /dev/ttydiag could be the HAL diagnostic output
                             pseudo-device, or /dev/tty0 could be your
                             first serial driver, depending on your
                             configuration."
            type            string
            parent          CYGPKG_LIBC_STDIO
        }

        cdl_component CYGPKG_LIBC_STDIO_FLOATING_POINT {
            display         "Floating point support"
            description     "This component allows floating point support
                             to be enabled in certain standard I/O
                             functions."
            type            dummy
            parent          CYGPKG_LIBC_STDIO
        }

        cdl_option CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT {
            display         "printf() family"
            description     "This option enables the use of floating point
                             number output in the printf() family (i.e.
                             printf(), sprintf(), vprintf(), etc.) of
                             functions. A substantial amount of code space
                             can be saved if this option is disabled. If it
                             is disabled then floating point specifiers
                             (%e, %f, %g) are ignored, and nothing is
                             output."
            type            boolean
            parent          CYGPKG_LIBC_STDIO_FLOATING_POINT
            requires        CYGPKG_LIBM
        }
        
        cdl_option CYGSEM_LIBC_STDIO_SCANF_FLOATING_POINT {
            display         "scanf() family"
            description     "This option enables the use of floating point
                             number conversion in the scanf() family (i.e.
                             scanf(), sscanf(), vscanf(), etc.) of
                             functions. A substantial amount of code space
                             can be saved if this option is disabled. If it
                             is disabled then floating point specifiers
                             (%e, %f, %g) are ignored, and nothing is
                             converted."
            type            boolean
            parent          CYGPKG_LIBC_STDIO_FLOATING_POINT
            requires        CYGPKG_LIBM
            requires        CYGFUN_LIBC_strtod
        }
        
        cdl_option CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS {
            display      "Thread safe I/O streams"
            description  "This option controls whether standard I/O streams
                          are thread-safe. Having this option set allows
                          the streams to be locked when accessed by
                          multiple threads simultaneously."
            doc          ref/ecos-ref/c-and-math-library-overview-thread-safety.html
            type         boolean
            parent       CYGPKG_LIBC_STDIO
            requires     CYGPKG_KERNEL
        }

    }}CFG_DATA
*/

#define CYGPKG_LIBC_STDIO

#ifdef CYGPKG_LIBC_STDIO

#ifndef CYGPKG_IO
# error You must have generic device driver support enabled for C library \
 standard I/O
#endif

#define CYGIMP_LIBC_STDIO_INLINES

#define FOPEN_MAX 8

#define FILENAME_MAX 16

#define L_tmpnam 16

#define TMP_MAX 0

#define CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

#define CYGNUM_LIBC_STDIO_BUFSIZE 256

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
# define BUFSIZ CYGNUM_LIBC_STDIO_BUFSIZE
#else
# define BUFSIZ 0
#endif

#undef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF

#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
# ifndef CYGPKG_LIBC_MALLOC
#  error You must have malloc() support if you want dynamic setvbuf \
 support in the C library
# endif
#endif

#define CYGFUN_LIBC_STDIO_ungetc

#define CYGPKG_LIBC_STDIO_OPEN

#ifdef CYGPKG_LIBC_STDIO_OPEN
# ifndef CYGPKG_LIBC_MALLOC
#  error You must have malloc() support if you want to use fopen()
# endif
#endif

// We define a default console so that the console is defined
// statically, to prevent the requirement of dynamic memory allocation, and
// since there is generally only one relevant console which exists for the
// lifetime of the system. See the device driver documentation and examine
// your configuration to find out which device names are available. The
// device names should be prefixed with "/dev/".
//
// So what is the name of the device that is the default console device

#define CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE        "/dev/ttydiag"

#define CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT
# ifndef CYGPKG_LIBM
#  error You must enable the Math library to use floating point in the \
 *printf() functions
# endif
#endif

#define CYGSEM_LIBC_STDIO_SCANF_FLOATING_POINT

#ifdef CYGSEM_LIBC_STDIO_SCANF_FLOATING_POINT
# ifndef CYGFUN_LIBC_strtod // libm is implicit with this
#  error You must enable the use of strtod and the math library to use \
 floating point in the *scanf() functions
# endif
#endif

#endif // ifdef CYGPKG_LIBC_STDIO

#define CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS

//========================================================================

// INTERNATIONALIZATION AND LOCALIZATION OPTIONS

/*

    {{CFG_DATA

        cdl_option CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE {
            display       "Size of locale name strings"
            description   "This option controls the maximum size of
                           locale names and is used, among other things
                           to instantiate a static string used
                           as a return value from the
                           setlocale() function. When requesting the
                           current locale settings with LC_ALL, a string
                           must be constructed to contain this data, rather
                           than just returning a constant string. This
                           string data is stored in the static string.
                           This depends on the length of locale names,
                           hence this option. If just the C locale is
                           present, this option can be set as low as 2."
            type          count
            legal_values  2 to 0x7fffffff
            parent CYGPKG_LIBC
        }

    }}CFG_DATA

*/

#define CYGNUM_LIBC_MAX_LOCALE_NAME_SIZE 16

//========================================================================

// SIGNAL CONFIGURATION OPTIONS

/*
    {{CFG_DATA
        cdl_component CYGPKG_LIBC_SIGNALS {
            display       "Signals"
            description   "This component controls signal functionality,
                          as implemented in ISO C chapter 7.7 with the
                          signal() and raise() functions. As well as
                          allowing a program to send itself signals, it is
                          also possible to cause hardware exceptions to
                          be signalled to the program in a similar way."
            type          boolean
            parent        CYGPKG_LIBC
        }

        cdl_option CYGSEM_LIBC_SIGNALS_THREAD_SAFE {
            display       "Thread safe signals"
            description   "Because signal handlers are global, and not
                           thread-specific (as mandated by POSIX 1003.1)
                           it is possible for multiple threads to clash
                           and corrupt data if setting the same signal
                           handler at the same time. With this option
                           enabled, a kernel mutex will be used to protect
                           the shared data."
            type          boolean
            requires      CYGPKG_KERNEL
            parent        CYGPKG_LIBC_SIGNALS
        }

        cdl_component CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS {
            display       "Hardware exceptions cause signals"
            description   "This causes hardware exceptions like
                           arithmetic exceptions (e.g. divide by zero)
                           or bad memory accesses to be passed through as
                           signals. It is thus possible for a program to
                           handle these exceptions in a generic way using
                           the appropriate signal handlers. The details of
                           exactly which exceptions are passed through
                           under what circumstances depend on the
                           underlying HAL implementation. Consequently it
                           must be remembered that signal handlers cannot
                           be guaranteed to be invoked in the expected
                           circumstances if you do not know about the
                           underlying HAL implementation - it is possible
                           that the architecture is unable to detect that
                           exception."
            type          boolean
            requires      CYGSEM_KERNEL_EXCEPTIONS_GLOBAL
            requires      CYGSEM_KERNEL_EXCEPTIONS_DECODE
            parent        CYGPKG_LIBC_SIGNALS
        }

        cdl_option CYGSEM_LIBC_SIGNALS_CHAIN_HWEXCEPTIONS {
            display       "Chain old exception handlers"
            description   "When hardware exceptions are generated, it
                           is possible to chain into the old kernel exception
                           handler before calling any registered signal
                           handler. If this is not required, disable this
                           option and only the signal handler will be
                           called when this exception occurs. Disabling
                           this option will also save some space and code
                           usage."
            type          boolean
            requires      !CYGPKG_HAL_I386_LINUX
            parent        CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS
        }

        cdl_component CYGPKG_LIBC_SIGNALS_INLINE {
            display       "Inline versions of functions"
            description   "Some functions can be made inline instead of
                           being function calls. These can be set here."
            type          dummy
            parent        CYGPKG_LIBC_SIGNALS
        }

        cdl_option CYGIMP_LIBC_SIGNALS_RAISE_INLINE {
            display       "raise()"
            description   "Enabling this option makes raise() an inline
                           function."
            type          boolean
            parent        CYGPKG_LIBC_SIGNALS_INLINE
        }

        cdl_option CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE {
            display       "signal()"
            description   "Enabling this option makes signal() an inline
                           function."
            type          boolean
            parent        CYGPKG_LIBC_SIGNALS_INLINE
        }

        cdl_component CYGPKG_LIBC_SIGNALS_ERROR_HANDLING {
            display       "Error handling"
            description   "You may or may not want errno to be set by
                           certain functions. Not using errno could save
                           both space, code, and avoid worrying about
                           interactions between multiple threads when
                           setting it."
            type          dummy
            parent        CYGPKG_LIBC_SIGNALS
        }

        cdl_option CYGSEM_LIBC_SIGNALS_BAD_SIGNAL_FATAL {
            display       "Bad signal fatal"
            description   "Enabling this option will cause an assertion
                           failure if the signal handling code is
                           passed a bad signal. Otherwise an error will
                           be returned, or in the case of a
                           hardware-generated exception it will be silently
                           ignored."
            type          boolean
            requires      CYGDBG_USE_ASSERTS
            parent        CYGPKG_LIBC_SIGNALS_ERROR_HANDLING
        }

        cdl_option CYGSEM_LIBC_SIGNALS_RAISE_SETS_ERRNO {
            display       "raise() sets errno"
            description   "You may or may not want errno to be set by
                           raise() on error conditions. Not using errno
                           could save both space, code, and avoid worrying
                           about interactions between multiple threads when
                           setting it. Strictly ISO C does not mandate
                           that errno be set by raise()."
            type          boolean
            parent        CYGPKG_LIBC_SIGNALS_ERROR_HANDLING
        }

        cdl_option CYGSEM_LIBC_SIGNALS_SIGNAL_SETS_ERRNO {
            display       "signal() sets errno"
            description   "You may or may not want errno to be set by
                           signal() on error conditions. Not using errno
                           could save both space, code, and avoid worrying
                           about interactions between multiple threads when
                           setting it. Strictly ISO C mandates
                           that errno be set by signal()."
            type          boolean
            parent        CYGPKG_LIBC_SIGNALS_ERROR_HANDLING
        }

        cdl_component CYGPKG_LIBC_SIGNALS_TRACING {
            display       "Tracing"
            description   "These options control the trace output for the C
                           library internal signal implementation."
            type          dummy
            parent        CYGPKG_LIBC_SIGNALS
        }

        cdl_option CYGNUM_LIBC_SIGNALS_RAISE_TRACE_LEVEL {
            display       "Default trace level for raise()"
            description   "This gives the default tracing level for the
                           raise() function. Higher numbers are more
                           verbose."
            type          count
            legal_values  0 to 1
            parent        CYGPKG_LIBC_SIGNALS_TRACING
        }

        cdl_option CYGNUM_LIBC_SIGNALS_HWHANDLER_TRACE_LEVEL {
            display       "Default hardware exception handler trace level"
            description   "This gives the default tracing level for the
                           hardware exception handler function (if enabled).
                           Higher numbers are more verbose."
            type          count
            legal_values  0 to 1
            parent        CYGPKG_LIBC_SIGNALS_TRACING
        }

    }}CFG_DATA
*/

#define CYGPKG_LIBC_SIGNALS
#define CYGSEM_LIBC_SIGNALS_THREAD_SAFE
#undef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS
#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS
# define CYGSEM_LIBC_SIGNALS_CHAIN_HWEXCEPTIONS
#endif
#define CYGIMP_LIBC_SIGNALS_RAISE_INLINE
#define CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE
#undef CYGSEM_LIBC_SIGNALS_BAD_SIGNAL_FATAL
#define CYGSEM_LIBC_SIGNALS_RAISE_SETS_ERRNO
#define CYGSEM_LIBC_SIGNALS_SIGNAL_SETS_ERRNO
#define CYGNUM_LIBC_SIGNALS_RAISE_TRACE_LEVEL 0
#define CYGNUM_LIBC_SIGNALS_HWHANDLER_TRACE_LEVEL 0

//========================================================================

// STARTUP OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBC_STARTUP {
            display       "ISO C startup/termination"
            description   "This component manages the control of the
                           environment (in the general sense) that the
                           C library provides for use for full ISO C
                           compatibility, including a main() entry point
                           supplied with arguments and an environment
                           (as retrievable by the getenv() function).
                           It also includes at the other end of things,
                           what happens when main() returns or exit() is
                           called."
            doc           ref/ecos-ref/c-library-startup.html.html
            type          dummy
            parent        CYGPKG_LIBC
        }

        cdl_option CYGDAT_LIBC_ARGUMENTS {
            display       "Arguments to main()"
            description   "This option allows the setting of the arguments
                           to the main function. This only has any effect
                           when cyg_iso_c_start() is used to start the
                           ISO C compatibility. This will automatically
                           invoke main() in an ISO C compatible fashion.
                           This option is in fact used as argv, and so is
                           an n-tuple of string constants. The final
                           entry must be NULL. Conventionally if argv[0]
                           is supplied, it is used to inform the program
                           of its name."
            type          string
            parent        CYGPKG_LIBC_STARTUP
        }

        cdl_component CYGSEM_LIBC_STARTUP_MAIN_THREAD {
            display       "main() invoked as thread"
            description   "It is normally expected (e.g. in a POSIX 1003.1
                           environment) that main() is run in the context
                           of a thread just like any other thread. If this
                           option is disabled then main() will instead be
                           called in the context of the initialization
                           routines, without the scheduler running and
                           using the interrupt stack. This means that 
                           any functions that may require interactions
                           with a running kernel will not work (including
                           libc functions, if configured to be thread safe
                           for example), so you should restrict yourself
                           to only calling fully re-entrant functions from
                           within main() in this case."
            type          boolean
            requires      CYGPKG_KERNEL
            parent        CYGPKG_LIBC_STARTUP
        }

        cdl_component CYGSEM_LIBC_MAIN_STACK_FROM_SYSTEM {
            display        "System provides stack for main()'s thread"
            description    "This option controls whether the stack of
                            main()'s thread is provided by the application or
                            provided by the system. When disabled, the
                            application must declare a pointer variable
                            cyg_libc_main_stack which is a pointer to an
                            appropriately aligned region of memory. The
                            application must also declare a variable of
                            type `int' called cyg_libc_main_stack_size
                            which contains the size of the stack in bytes.
                            This must be a multiple of 8."
            type           boolean
            parent         CYGSEM_LIBC_STARTUP_MAIN_THREAD
        }

        cdl_option CYGNUM_LIBC_MAIN_DEFAULT_STACK_SIZE {
            display        "main()'s default thread stack size"
            description    "With an eCos kernel, when the cyg_iso_c_start()
                            function is used to invoke the user-supplied
                            main() function in an ISO C compatible fashion,
                            a separate thread is created to call main().
                            This option controls the size of that thread's
                            stack, which is allocated in the BSS. It
                            must be a multiple of 8. Note, a low number
                            here may be overriden by the HAL if there is
                            a minimum requirement to process interrupts
                            etc."
            type           count
            legal_values   16 to 0x7fffffff
            parent         CYGSEM_LIBC_MAIN_STACK_FROM_SYSTEM
        }
        
        cdl_option CYGNUM_LIBC_MAIN_THREAD_PRIORITY {
            display         "Priority of main()'s thread"
            description     "This option is used to provide the thread
                             priority which main()'s thread runs at. Be
                             sure to check that this number is appropriate
                             for the kernel scheduler chosen. Different
                             kernel schedulers impose different restrictions
                             on the usable priorities."
            type            count
            legal_values    0 to 0x7fffffff
            parent          CYGSEM_LIBC_STARTUP_MAIN_THREAD
        }

        cdl_component CYGFUN_LIBC_ATEXIT {
            display        "Include atexit() function"
            description    "This option enables the use of the atexit()
                            function, and ensure that the atexit handlers
                            are invoked from within exit() as defined in
                            the ISO C standard."
            type           boolean
            parent         CYGPKG_LIBC_STARTUP
        }

        cdl_option CYGNUM_LIBC_ATEXIT_HANDLERS {
            display        "Number of functions atexit() supports"
            description    "This option sets the number of functions
                            that atexit() has room for. In other words,
                            the number of separate atexit() calls that
                            can be made before calling exit() or returning
                            from main() ( in an environment controlled by
                            cyg_iso_c_start() ). For strict compliance
                            with the ISO C standard this should be at
                            least 32."
            type           count
            parent         CYGFUN_LIBC_ATEXIT
            legal_values   1 to 0x7fffffff
        }

        cdl_option CYGSEM_LIBC_EXIT_CALLS_FFLUSH {
            display        "Make exit() call fflush()"
            description    "This option will ensure that all output
                            streams are flushed when exitting using
                            exit() as prescribed by the ISO C standard."
            type           boolean
            parent         CYGPKG_LIBC_STARTUP
            requires       CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
        }

        cdl_option CYGSEM_LIBC_EXIT_STOPS_SYSTEM {
            display        "_exit() stops all threads"
            description    "If this option is enabled then _exit(),
                            which is called from exit() which itself is
                            called on return from main(), stops the eCos
                            scheduler. For strict POSIX 1003.1
                            compatibility this would be the normal
                            behaviour. To allow the thread to simply
                            exit, without affecting other threads, simply
                            disable this option. If there is no eCos
                            scheduler, then _exit() simply loops
                            irrespective of the setting of this option."
            type           boolean
            parent         CYGPKG_LIBC_STARTUP
            requires       CYGPKG_KERNEL
        }

        cdl_component CYGPKG_LIBC_ENVIRONMENT {
            display       "Provide a system environment"
            description   "This option allows use of the getenv() function
                           and the global environment pointer
                           char **environ defined by POSIX 1003.1.
                           Disabling this saves a small amount of space."
            type          boolean
            parent        CYGPKG_LIBC_STARTUP
        }

        cdl_option CYGDAT_LIBC_DEFAULT_ENVIRONMENT {
            display       "Default environment"
            description   "This option allows the setting of the default
                           contents of the environment. This is in fact
                           the contents of the global environment pointer
                           char **environ defined by POSIX 1003.1. Strings
                           must be of the form NAME=VALUE, and the final
                           entry must be NULL."
            type          string
            parent        CYGPKG_LIBC_ENVIRONMENT
        }

        cdl_option CYGSEM_LIBC_INVOKE_DEFAULT_STATIC_CONSTRUCTORS {
            display        "Invoke default static constructors"
            description    "This option causes the C library to call
                            static constructors with default priority
                            from within the context of the main() thread,
                            working with the co-operation of the HAL.
                            This may be needed for environments that
                            expect static constructors to be run in a
                            thread context. Note that this uses an object
                            with priority 65534. Therefore do not use
                            priority 65534 for any other static objects
                            with this option enabled."
            type           boolean
            requires       CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
            requires       CYGSEM_LIBC_STARTUP_MAIN_THREAD
            parent         CYGPKG_LIBC_STARTUP
        }

    }}CFG_DATA
*/

#define CYGDAT_LIBC_ARGUMENTS { "", NULL }

#define CYGSEM_LIBC_STARTUP_MAIN_THREAD

#ifdef CYGSEM_LIBC_STARTUP_MAIN_THREAD
# define CYGSEM_LIBC_MAIN_STACK_FROM_SYSTEM
# define CYGNUM_LIBC_MAIN_DEFAULT_STACK_SIZE 8192
# define CYGNUM_LIBC_MAIN_THREAD_PRIORITY 10
#endif

#define CYGFUN_LIBC_ATEXIT

#ifdef CYGFUN_LIBC_ATEXIT
#define CYGNUM_LIBC_ATEXIT_HANDLERS 32
#endif

#define CYGSEM_LIBC_EXIT_CALLS_FFLUSH

#undef CYGSEM_LIBC_EXIT_STOPS_SYSTEM

#define CYGPKG_LIBC_ENVIRONMENT

#define CYGDAT_LIBC_DEFAULT_ENVIRONMENT { NULL }

#undef CYGSEM_LIBC_INVOKE_DEFAULT_STATIC_CONSTRUCTORS

//========================================================================

// ERRNO OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBC_ERRNO {
            display      "errno"
            description  "This package controls the behaviour of the
                          errno variable (or more strictly, expression)
                          from <errno.h>."
            type         dummy
            parent       CYGPKG_LIBC
        }
  
        cdl_option CYGSEM_LIBC_PER_THREAD_ERRNO {
            display      "Per-thread errno"
            description  "This option controls whether the standard error
                          code reporting variable errno is a per-thread
                          variable, rather than global. Enabling this
                          option will use one slot of kernel per-thread data.
                          You should ensure you have enough slots configured
                          for all your per-thread data."
            doc          ref/ecos-ref/c-and-math-library-overview-thread-safety.html
            type         boolean
            requires     CYGVAR_KERNEL_THREADS_DATA
            parent       CYGPKG_LIBC_ERRNO
        }

        cdl_option CYGNUM_LIBC_ERRNO_TRACE_LEVEL {
            display      "Tracing level"
            description  "Trace verbosity level for debugging the errno
                          retrieval mechanism in errno.cxx. Increase this
                          value to get additional trace output."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_ERRNO
        }

    }}CFG_DATA
*/

#define CYGSEM_LIBC_PER_THREAD_ERRNO

#define CYGNUM_LIBC_ERRNO_TRACE_LEVEL 0


//========================================================================

// TIME OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBC_TIME {
            display      "Date/Time"
            description  "Options for date and time related functions
                          from <time.h>"
            type         dummy
            parent       CYGPKG_LIBC
        }

        cdl_option CYGSEM_LIBC_TIME_CLOCK_WORKING {
            display      "Working clock() function"
            description  "This option controls whether clock() will
                          actually try and determine the process time
                          usage. With this option disabled, clock() does
                          not disappear, but will permanently return
                          (clock_t)-1 as mandated by the ISO C standard."
            type         boolean
            requires     CYGFUN_KERNEL_THREADS_TIMER
            requires     CYGVAR_KERNEL_COUNTERS_CLOCK
            parent       CYGPKG_LIBC_TIME
        }

        cdl_option CYGSEM_LIBC_TIME_TIME_WORKING {
            display      "Working time() function"
            description  "This option controls whether time() will
                          actually try and determine the current calendar
                          time. With this option disabled, time() does
                          not disappear, but will permanently return
                          (time_t)-1 as mandated by the ISO C standard."
            type         boolean
            requires     CYGPKG_DEVICES_WALLCLOCK
            parent       CYGPKG_LIBC_TIME
        }

        cdl_option CYGSEM_LIBC_TIME_SETTIME_WORKING {
            display      "Working cyg_libc_time_settime() function"
            description  "This option controls whether cyg_libc_time_settime()
                          will actually try and set the current calendar
                          time. With this option disabled,
                          cyg_libc_time_settime() does not disappear, but
                          will permanently return an error."
            type         boolean
            requires     CYGPKG_DEVICES_WALLCLOCK
            parent       CYGPKG_LIBC_TIME
        }

        cdl_option CYGFUN_LIBC_TIME_POSIX {
            display      "POSIX time functions"
            description  "Enabling this option allows the use of the
                          following functions defined in POSIX 1003.1:
                          asctime_r(), ctime_r(), gmtime_r(), and
                          localtime_r()."
            type         boolean
            parent       CYGPKG_LIBC_TIME
        }

        cdl_component CYGPKG_LIBC_TIME_ZONES {
            display      "Time zone offsets"
            description  "These options control the default STandarD (STD)
                          and Daylight Savings Time (DST)
                          time offsets so that dates can be set correctly
                          for the local environment."
            type         dummy
            parent       CYGPKG_LIBC_TIME
        }
        
        cdl_option CYGNUM_LIBC_TIME_DST_DEFAULT_STATE {
            display      "Default Daylight Savings Time state"
            description  "This option controls whether the initial
                          time environment is set up as STD, DST or
                          unknown. Use the value 1 for DST, 0 for STD, 
                          and (-1) for unknown. This can also be set at
                          runtime using the cyg_libc_time_setdst()
                          function."
            type         count
            legal_values -1 to 1
            parent       CYGPKG_LIBC_TIME_ZONES
        }

        cdl_option CYGNUM_LIBC_TIME_STD_DEFAULT_OFFSET {
            display      "Default Standard Time offset"
            description  "This option controls the offset from UTC in
                          seconds when in local Standard Time. This
                          value can be positive or negative. It
                          can also be set at run time using the
                          cyg_libc_time_setzoneoffsets() function."
            type         count
            legal_values -90000 to 90000
            parent       CYGPKG_LIBC_TIME_ZONES
        }
        
        cdl_option CYGNUM_LIBC_TIME_DST_DEFAULT_OFFSET {
            display      "Default Daylight Savings Time offset"
            description  "This option controls the offset from UTC in
                          seconds when in local Daylight Savings Time. This
                          value can be positive or negative. It
                          can also be set at run time using the
                          cyg_libc_time_setzoneoffsets() function."
            type         count
            legal_values -90000 to 90000
            parent       CYGPKG_LIBC_TIME_ZONES
        }
        

        cdl_component CYGPKG_LIBC_TIME_INLINES {
            display      "Inline functions"
            description  "These options control whether certain functions
                          are available in inline form. This may lead to
                          faster code at the expense of code space. But for
                          some functions, or some functions with constant
                          arguments, it may in fact lead to smaller code."
            type         dummy
            parent       CYGPKG_LIBC_TIME
        }

        cdl_option CYGIMP_LIBC_TIME_ASCTIME_INLINE {
            display      "asctime()"
            description  "Allow the asctime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_CTIME_INLINE {
            display      "ctime()"
            description  "Allow the ctime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_DIFFTIME_INLINE {
            display      "difftime()"
            description  "Allow the difftime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_GMTIME_INLINE {
            display      "gmtime()"
            description  "Allow the gmtime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_LOCALTIME_INLINE {
            display      "localtime()"
            description  "Allow the localtime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_MKTIME_INLINE {
            display      "mktime()"
            description  "Allow the mktime() function to be inlined"
            type         boolean
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_ASCTIME_R_INLINE {
            display      "asctime_r()"
            description  "Allow the asctime_r() function to be inlined"
            type         boolean
            requires     CYGFUN_LIBC_TIME_POSIX
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_CTIME_R_INLINE {
            display      "ctime_r()"
            description  "Allow the ctime_r() function to be inlined"
            type         boolean
            requires     CYGFUN_LIBC_TIME_POSIX
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_GMTIME_R_INLINE {
            display      "gmtime_r()"
            description  "Allow the gmtime_r() function to be inlined"
            type         boolean
            requires     CYGFUN_LIBC_TIME_POSIX
            parent       CYGPKG_LIBC_TIME_INLINES
        }

        cdl_option CYGIMP_LIBC_TIME_LOCALTIME_R_INLINE {
            display      "localtime_r()"
            description  "Allow the localtime_r() function to be inlined"
            type         boolean
            requires     CYGFUN_LIBC_TIME_POSIX
            parent       CYGPKG_LIBC_TIME_INLINES
        }

# FIXME: Also want "inline all" and "don't inline any" options which requires
# (or not) all the above, but
# if we do that then we will require the ones that also depend on
# CYGFUN_LIBC_TIME_POSIX :-( Wait for full CDL to fix this

        cdl_option CYGNUM_LIBC_TIME_CLOCK_TRACE_LEVEL {
            display      "clock() tracing level"
            description  "Trace verbosity level for debugging the clock()
                          function. Increase this value to get
                          additional trace output when tracing is enabled."
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TIME
        }

    }}CFG_DATA
*/

#define CYGSEM_LIBC_TIME_CLOCK_WORKING
#define CYGSEM_LIBC_TIME_TIME_WORKING
#define CYGSEM_LIBC_TIME_SETTIME_WORKING
#define CYGFUN_LIBC_TIME_POSIX
#define CYGNUM_LIBC_TIME_DST_DEFAULT_STATE (-1)
#define CYGNUM_LIBC_TIME_STD_DEFAULT_OFFSET 0
#define CYGNUM_LIBC_TIME_DST_DEFAULT_OFFSET 3600
#define CYGNUM_LIBC_TIME_CLOCK_TRACE_LEVEL 0
#define CYGIMP_LIBC_TIME_ASCTIME_INLINE
#define CYGIMP_LIBC_TIME_CTIME_INLINE
#define CYGIMP_LIBC_TIME_DIFFTIME_INLINE
#define CYGIMP_LIBC_TIME_GMTIME_INLINE
#define CYGIMP_LIBC_TIME_LOCALTIME_INLINE
#define CYGIMP_LIBC_TIME_MKTIME_INLINE
#define CYGIMP_LIBC_TIME_ASCTIME_R_INLINE
#define CYGIMP_LIBC_TIME_CTIME_R_INLINE
#define CYGIMP_LIBC_TIME_GMTIME_R_INLINE
#define CYGIMP_LIBC_TIME_LOCALTIME_R_INLINE

//========================================================================

// MISCELLANEOUS DEFINES


// These are not really adjustable by the user. Do not change these unless
// you know what you are doing!

// How to define inline functions
#define CYGPRI_LIBC_INLINE extern __inline__

// The following isn't supported yet - I'm leaving it for now
#ifdef __GNUC__xxxxxx
# define CYGPRI_LIBC_ATTRIB_FORMAT_STRFTIME \
   __attribute__ ((format (strftime, 3)))
#else
# define CYGPRI_LIBC_ATTRIB_FORMAT_STRFTIME
#endif

// The following are only for compatibility, and will eventually be removed
// once they are eliminated entirely from the code base

#define CYGPRI_LIBC_WEAK CYGBLD_ATTRIB_WEAK

// How to define aliases
#define CYGPRI_LIBC_ALIAS(__symbol__) __attribute__ ((alias (__symbol__)))

# define CYGPRI_LIBC_WEAK_ALIAS(__symbol__) \
        CYGBLD_ATTRIB_WEAK CYGPRI_LIBC_ALIAS(__symbol__)

// How to define functions that don't return
#define CYGPRI_LIBC_NORETURN CYGBLD_ATTRIB_NORET

#endif // CYGONCE_PKGCONF_LIBC_H multiple inclusion protection

// EOF libc.h
