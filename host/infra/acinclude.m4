dnl Process this file with aclocal to get an aclocal.m4 file. Then
dnl process that with autoconf.
dnl ====================================================================
dnl
dnl     acinclude.m4
dnl
dnl     Host side implementation of the eCos infrastructure.
dnl
dnl ====================================================================
dnl####COPYRIGHTBEGIN####
dnl                                                                         
dnl ----------------------------------------------------------------------------
dnl Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
dnl
dnl This file is part of the eCos host tools.
dnl
dnl This program is free software; you can redistribute it and/or modify it 
dnl under the terms of the GNU General Public License as published by the Free 
dnl Software Foundation; either version 2 of the License, or (at your option) 
dnl any later version.
dnl 
dnl This program is distributed in the hope that it will be useful, but WITHOUT 
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
dnl more details.
dnl 
dnl You should have received a copy of the GNU General Public License along with
dnl this program; if not, write to the Free Software Foundation, Inc., 
dnl 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
dnl
dnl ----------------------------------------------------------------------------
dnl                                                                          
dnl####COPYRIGHTEND####
dnl ====================================================================
dnl#####DESCRIPTIONBEGIN####
dnl
dnl Author(s):	bartv
dnl Contact(s):	bartv
dnl Date:	1998/07/14
dnl Version:	0.01
dnl
dnl####DESCRIPTIONEND####
dnl ====================================================================

dnl Access shared macros.
dnl AM_CONDITIONAL needs to be mentioned here or else aclocal does not
dnl incorporate the macro into aclocal.m4
sinclude(../acinclude.m4)


dnl ====================================================================
dnl Look for a 64 bit data type. It is necessary to check both C and C++
dnl compilers.
dnl
dnl A better implementation would check whether or not AC_PROG_CC and
dnl AC_PROG_CXX have been invoked and only test the appropriate
dnl compiler.

AC_DEFUN(CYG_AC_TYPE_64bit, [
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CXX])

    AC_CACHE_CHECK("for a 64 bit data type",cyg_ac_cv_type_64bit,[
        for type in "long" "long long" "__int64"; do
            AC_LANG_SAVE
	    AC_LANG_C
            AC_TRY_RUN([
                main() {
                    return 8 != sizeof($type);
                }
	    ],ctype_64bit=$type,ctype_64bit="unknown",ctype_64bit="unknown")
	    AC_LANG_CPLUSPLUS
            AC_TRY_RUN([
                int main(int argc, char ** argv) {
                    return 8 != sizeof($type);
                }
	    ],cxxtype_64bit=$type,cxxtype_64bit="unknown",cxxtype_64bit="unknown")
	    AC_LANG_RESTORE
            if test "${ctype_64bit}" = "${type}" -a "${cxxtype_64bit}" = "${type}"; then
                cyg_ac_cv_type_64bit="${type}"
                break
            fi
        done
    ])
    if test "${cyg_ac_cv_type_64bit}" = ""; then
        AC_MSG_ERROR(Unable to figure out how to do 64 bit arithmetic)
    else
        if test "${cyg_ac_cv_type_64bit}" != "long long"; then
            AC_DEFINE_UNQUOTED(cyg_halint64,${cyg_ac_cv_type_64bit})
            AC_DEFINE_UNQUOTED(cyg_halcount64,${cyg_ac_cv_type_64bit})
        fi
    fi
])

dnl ====================================================================
dnl Check that both the C and C++ compilers support __PRETTY_FUNCTION__

AC_DEFUN(CYG_AC_C_PRETTY_FUNCTION,[
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CXX])

    AC_CACHE_CHECK("for __PRETTY_FUNCTION__ support",cyg_ac_cv_c_pretty_function,[
        AC_LANG_SAVE
        AC_LANG_C
        AC_TRY_LINK(
            [#include <stdio.h>],
            [puts(__PRETTY_FUNCTION__);],
            c_ok="yes",
            c_ok="no"
        )
        AC_LANG_CPLUSPLUS
        AC_TRY_LINK(
            [#include <cstdio>],
            [puts(__PRETTY_FUNCTION__);],
            cxx_ok="yes",
            cxx_ok="no"
        )
        AC_LANG_RESTORE
        if test "${c_ok}" = "yes" -a "${cxx_ok}" = "yes"; then
            cyg_ac_cv_c_pretty_function="yes"
        fi
    ])
    if test "${cyg_ac_cv_c_pretty_function}" = "yes"; then
        AC_DEFINE(CYGDBG_INFRA_DEBUG_FUNCTION_PSEUDOMACRO)
    fi
])




