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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:          1998-02-13
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

// Make sure kernel config is already there for
// CYGNUM_KERNEL_THREADS_DATA_LIBC and CYGVAR_KERNEL_THREADS_DATA
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

/*
    {{CFG_DATA

        cdl_package CYGPKG_LIBC {
            display      "C Library"
            description  "This package enables compatibility with the ISO
                          C standard - ISO/IEC 9899:1990. This allows the
                          user application to use well known standard
                          C library functions, and in eCos starts a
                          thread to invoke the user function main()"
            type         boolean
            requires      CYGPKG_ERROR
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

        cdl_option CYGFUN_LIBC_strtok {
            display      "Provides strtok()"
            description  "This option chooses whether the strtok() function
                          is to be available. Some space may be saved if
                          not."
            
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
            parent CYGPKG_LIBC_STRING
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

    }}CFG_DATA
*/

#define CYGIMP_LIBC_STRING_INLINES

#define CYGFUN_LIBC_strtok

#undef CYGIMP_LIBC_STRING_PREFER_SMALL

#define CYGIMP_LIBC_STRING_PREFER_FAST

#ifdef CYGIMP_LIBC_STRING_PREFER_SMALL
# define CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST
#endif

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
            display     "Provide rand() and srand()"
            description "This option chooses whether the pseudo-random
                         number generator functions rand() and
                         srand() are available. Some space may
                         saved if not."
            
            type        boolean
            parent      CYGPKG_LIBC_STDLIB
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
                          contiguous memory pool. This option is the size
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

    }}CFG_DATA
*/

#define CYGIMP_LIBC_STDLIB_INLINES

#define CYGPKG_LIBC_RAND

#ifdef CYGPKG_LIBC_RAND
#define CYGNUM_LIBC_RAND_SEED 1
#endif

#define CYGPKG_LIBC_MALLOC

#ifdef CYGPKG_LIBC_MALLOC

#undef CYGIMP_LIBC_MALLOC_CXX_DELETE_CALLS_FREE

#ifndef CYGPKG_KERNEL
# error You must have the Kernel enabled to use the C library dynamic \
 memory allocation functions
#endif

#define CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE 32768

#endif // ifdef CYGPKG_LIBC_MALLOC

#define CYGFUN_LIBC_strtod

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
            requires     CYGPKG_DEVICES
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
            
            type          count
            legal_values  1 to 0x7fffffff
            parent        CYGPKG_LIBC_STDIO
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
                            constant BUFSIZE, except that it is 0 if
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
                            of the configuration option BUFSIZE. The only
                            exception is if a user buffer is not supplied,
                            and the buffer size requested is less than
                            BUFSIZE. In this case, the buffer will
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
                             the name of a device as listed in the
                             devs/common/current/src/table.cxx file in the
                             source repository. Look at that file to see
                             list of available devices."
            
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
        
    }}CFG_DATA
*/

#define CYGPKG_LIBC_STDIO

#ifdef CYGPKG_LIBC_STDIO

#ifndef CYGPKG_DEVICES
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
# define BUFSIZE CYGNUM_LIBC_STDIO_BUFSIZE
#else
# define BUFSIZE 0
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
#define CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE        "/dev/haldiag"

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

//========================================================================

// THREAD SAFETY CONFIGURATION OPTIONS

/*

    {{CFG_DATA

        cdl_component CYGSEM_LIBC_THREAD_SAFETY {
            display      "Thread safety"
            description  "This option controls whether the C library has
                          support for thread safe operation in general.
                          This requires eCos kernel support for per-thread
                          data, and adjustment of the stack limit."
            doc          ref/ecos-ref/c-and-math-library-overview-thread-safety.html
            type         boolean
            parent       CYGPKG_LIBC
            requires     CYGPKG_KERNEL
            requires     CYGVAR_KERNEL_THREADS_DATA
            requires     CYGFUN_KERNEL_THREADS_STACK_LIMIT
        }

        cdl_option CYGSEM_LIBC_PER_THREAD_ERRNO {
            display      "Per-thread errno"
            description  "This option controls whether the standard error
                          code reporting variable errno is a per-thread
                          variable, rather than global."
            type         boolean
            parent       CYGSEM_LIBC_THREAD_SAFETY
        }

        cdl_option CYGSEM_LIBC_PER_THREAD_STRTOK {
            display      "Per-thread strtok()"
            description  "This option controls whether the string function
                          strtok() has its state recorded on a per-thread
                          basis rather than global. If this option is
                          disabled, some per-thread space can be saved.
                          Note there is also a POSIX-standard strtok_r()
                          function to achieve a similar effect with user
                          support."
            type         boolean
            parent       CYGSEM_LIBC_THREAD_SAFETY
        }

        cdl_option CYGSEM_LIBC_PER_THREAD_RAND {
            display      "Per-thread rand()"
            description  "This option controls whether the pseudo-random
                          number generation functions rand() and srand()
                          have their state recorded on a per-thread
                          basis rather than global. If this option is
                          disabled, some per-thread space can be saved.
                          Note there is also a POSIX-standard rand_r()
                          function to achieve a similar effect with user
                          support."
            type         boolean
            parent       CYGSEM_LIBC_THREAD_SAFETY
        }

        cdl_option CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS {
            display      "Thread safe I/O streams"
            description  "This option controls whether standard I/O streams
                          are thread-safe. Having this option set allows
                          the streams to be locked when accessed by
                          multiple threads simultaneously."
            type         boolean
            parent       CYGSEM_LIBC_THREAD_SAFETY
        }



    }}CFG_DATA

*/

#undef CYGSEM_LIBC_THREAD_SAFETY

#ifdef CYGSEM_LIBC_THREAD_SAFETY

#define CYGSEM_LIBC_PER_THREAD_ERRNO

#define CYGSEM_LIBC_PER_THREAD_STRTOK

#define CYGSEM_LIBC_PER_THREAD_RAND

#define CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS

// Consistency check for kernel support
# if !defined(CYGVAR_KERNEL_THREADS_DATA) || !defined(CYGFUN_KERNEL_THREADS_STACK_LIMIT)

#  error Sorry, the kernel must have per-thread data and stack limit \
 support configured to allow a thread-safe C library

# endif // if !defined(CYGVAR_KERNEL_THREADS_DATA) || 
       // !defined(CYGFUN_KERNEL_THREADS_STACK_LIMIT)

#endif // ifdef CYGSEM_LIBC_THREAD_SAFETY

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
            doc          ref/ecos-ref/c-library-startup.html.html
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

        cdl_option CYGNUM_LIBC_MAIN_STACK_SIZE {
            display        "main()'s thread stack size"
            description    "With an eCos kernel, when the cyg_iso_c_start()
                            function is used to invoke the user-supplied
                            main() function in an ISO C compatible fashion,
                            a separate thread is created to call main().
                            This option controls the size of that thread's
                            stack, which is allocated in the BSS. It
                            must be a multiple of 8."
            
            type           count
            legal_values   16 to 0x7fffffff
            parent         CYGPKG_LIBC_STARTUP
            #requires       CYGPKG_KERNEL
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
            parent         CYGPKG_LIBC_STARTUP
            legal_values   1 to 0x7fffffff
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

    }}CFG_DATA
*/

#define CYGDAT_LIBC_ARGUMENTS { "", NULL }

#define CYGNUM_LIBC_MAIN_STACK_SIZE 32768

#define CYGNUM_LIBC_ATEXIT_HANDLERS 32

#undef CYGSEM_LIBC_EXIT_STOPS_SYSTEM

#define CYGPKG_LIBC_ENVIRONMENT

#define CYGDAT_LIBC_DEFAULT_ENVIRONMENT { NULL }


//========================================================================

// TRACING OPTIONS

/*
    {{CFG_DATA

        cdl_component CYGPKG_LIBC_TRACE {
            display      "Tracing output levels in C library"
            description  "Tracing support is useful for debugging. Some
                          C library modules can be configured with
                          different levels of tracing verbosity. These
                          levels can be configured here."
            
            type         boolean
            requires     CYGDBG_USE_TRACING
            parent       CYGPKG_LIBC
        }

        cdl_option CYGNUM_LIBC_RAND_TRACE_LEVEL {
            display      "rand(), srand() and rand_r()"
            description  "Trace level for debugging the rand(), srand()
                          and rand_r() functions."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

        cdl_option CYGNUM_LIBC_ERRNO_TRACE_LEVEL {
            display      "errno"
            description  "Trace level for debugging the errno retrieval
                          mechanism in errno.cxx."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

        cdl_option CYGNUM_LIBC_BSEARCH_TRACE_LEVEL {
            display      "bsearch()"
            description  "Trace level for debugging the <stdlib.h> binary
                          search function bsearch()."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

        cdl_option CYGNUM_LIBC_QSORT_TRACE_LEVEL {
            display      "qsort()"
            description  "Trace level for debugging the <stdlib.h>
                          quicksort function qsort()."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

        cdl_option CYGNUM_LIBC_CLOCK_TRACE_LEVEL {
            display      "clock()"
            description  "Trace level for debugging the <time.h> function
                          clock()."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

        cdl_option CYGNUM_LIBC_STRTOK_TRACE_LEVEL {
            display      "strtok() and strtok_r()"
            description  "Trace level for debugging the <string.h>
                          functions strtok() and strtok_r()."
            
            type         count
            legal_values 0 to 1
            parent       CYGPKG_LIBC_TRACE
        }

    }}CFG_DATA
*/

#undef CYGPKG_LIBC_TRACE

#ifdef CYGPKG_LIBC_TRACE

#define CYGNUM_LIBC_RAND_TRACE_LEVEL 0

#define CYGNUM_LIBC_ERRNO_TRACE_LEVEL 0

#define CYGNUM_LIBC_BSEARCH_TRACE_LEVEL 0

#define CYGNUM_LIBC_QSORT_TRACE_LEVEL 0

#define CYGNUM_LIBC_CLOCK_TRACE_LEVEL 0

#define CYGNUM_LIBC_STRTOK_TRACE_LEVEL 0

#endif // ifdef CYGPKG_LIBC_TRACE

//========================================================================

// MISCELLANEOUS CONFIGURATION OPTIONS

// These are not really adjustable by the user. Do not change these unless
// you know what you are doing!

// How to define inline functions
#define CYGPRI_LIBC_INLINE extern __inline__

// How to define weak symbols - may disappear as this is only relevant for
// ELF and a.out
#define CYGPRI_LIBC_WEAK __attribute__ ((weak))

// How to define aliases
#define CYGPRI_LIBC_ALIAS(X) __attribute__ ((alias (X)))

// How to define weak aliases or an equivalent hack if not available

#define CYGPRI_LIBC_WEAK_ALIAS(X) __attribute__ ((weak, alias (X)))

// How to define functions that don't return
#define CYGPRI_LIBC_NORETURN __attribute__ ((noreturn))

#endif // CYGONCE_PKGCONF_LIBC_H multiple inclusion protection

// EOF libc.h
