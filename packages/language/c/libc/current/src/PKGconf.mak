#=========================================================================
#
#    makefile
#
#    language/c/libc/src
#
#=========================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
#=========================================================================

PACKAGE       := libc
include ../../../../../pkgconf/pkgconf.mak

LIBRARY       := libtarget.a
COMPILE       := ctype/isalnum.cxx ctype/isalpha.cxx   ctype/iscntrl.cxx  \
                 ctype/isdigit.cxx ctype/isgraph.cxx   ctype/islower.cxx  \
                 ctype/isprint.cxx ctype/ispunct.cxx   ctype/isspace.cxx  \
                 ctype/isupper.cxx ctype/isxdigit.cxx  ctype/tolower.cxx  \
                 ctype/toupper.cxx                                        \
                                                                          \
                 errno/errno.cxx                                          \
                                                                          \
                 i18n/locale.cxx                                          \
                                                                          \
                 setjmp/longjmp.cxx                                       \
                                                                          \
                 stdio/common/fclose.cxx       stdio/common/fflush.cxx    \
                 stdio/common/fopen.cxx        stdio/common/freopen.cxx   \
                 stdio/common/setvbuf.cxx      stdio/common/snprintf.cxx  \
                 stdio/common/sprintf.cxx      stdio/common/sscanf.cxx    \
                 stdio/common/stdioinlines.cxx stdio/common/stdiosupp.cxx \
                 stdio/common/stream.cxx       stdio/common/streambuf.cxx \
                 stdio/common/ungetc.cxx       stdio/common/vsnprintf.cxx \
                 stdio/common/vsscanf.cxx      stdio/input/fgetc.cxx      \
                 stdio/input/fgets.cxx         stdio/input/fread.cxx      \
                 stdio/input/fscanf.cxx        stdio/input/gets.cxx       \
                 stdio/input/scanf.cxx         stdio/input/vfscanf.cxx    \
                 stdio/output/fnprintf.cxx     stdio/output/fprintf.cxx   \
                 stdio/output/fputc.cxx        stdio/output/fputs.cxx     \
                 stdio/output/fwrite.cxx       stdio/output/printf.cxx    \
                 stdio/output/vfnprintf.cxx                               \
                                                                          \
                 stdlib/_exit.cxx   stdlib/abort.cxx    stdlib/abs.cxx    \
                 stdlib/atexit.cxx  stdlib/atof.cxx     stdlib/atoi.cxx   \
                 stdlib/atol.cxx    stdlib/bsearch.cxx  stdlib/div.cxx    \
                 stdlib/exit.cxx    stdlib/getenv.cxx   stdlib/labs.cxx   \
                 stdlib/ldiv.cxx    stdlib/malloc.cxx   stdlib/qsort.cxx  \
                 stdlib/rand.cxx    stdlib/strtod.cxx   stdlib/strtol.cxx \
                 stdlib/strtoul.cxx                                       \
                                                                          \
                 string/memchr.cxx  string/memcmp.cxx  string/memmove.cxx \
                 string/strcat.cxx  string/strchr.cxx  string/strcmp.cxx  \
                 string/strcoll.cxx string/strcpy.cxx  string/strcspn.cxx \
                 string/strlen.cxx  string/strncat.cxx string/strncmp.cxx \
                 string/strncpy.cxx string/strpbrk.cxx string/strrchr.cxx \
                 string/strspn.cxx  string/strstr.cxx  string/strsuppt.cxx\
                 string/strtok.cxx  string/strxfrm.cxx                    \
                                                                          \
                 support/clibdata.cxx          support/cstartup.cxx       \
                                                                          \
                 time/clock.cxx

OTHER_OBJS    :=
OTHER_TARGETS :=
OTHER_CLEAN   :=

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

