dnl Process this file with aclocal to get an aclocal.m4 file. Then
dnl process that with autoconf.
dnl ====================================================================
dnl
dnl     acinclude.m4
dnl
dnl     Various autoconf macros that are shared between different
dnl     eCos packages.
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
dnl Date:	1998/12/16
dnl Version:	0.01
dnl
dnl####DESCRIPTIONEND####
dnl ====================================================================

dnl ====================================================================
dnl The AM_INIT_AUTOMAKE() will define a symbol VERSION for the
dnl package's version number. Unfortunately this symbol is rather
dnl hard to share if several different packages are involved, so this
dnl macro is used to define an alternative symbol

AC_DEFUN(CYG_AC_SUBST_VERSION,[
    AC_REQUIRE([AM_INIT_AUTOMAKE])
    ifelse($#,1,,AC_MSG_ERROR([Invalid number of arguments passed to CYG AC_SUBST_VERSION]))
    AC_DEFINE_UNQUOTED($1, "$VERSION")
])

dnl ====================================================================
dnl Provide support for an automake conditional MSVC. Also set a
dnl variable MSVC to true or false, which may help other tests.

AC_DEFUN(CYG_AC_PROG_MSVC,[
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CXX])

    AC_MSG_CHECKING("for Visual C++")
    MSVC="no";
    if test "${CC}" = "cl" ; then
       MSVC="yes"
    fi
    if test "${CXX}" = "cl" ; then
       MSVC="yes"
    fi
    AM_CONDITIONAL(MSVC, test "${MSVC}" = "yes")
    AC_MSG_RESULT(${MSVC})
])

dnl ====================================================================
dnl Set up sensible flags for the various different compilers. This
dnl is achieved by redefining CC and CXX. 
dnl
dnl A better way of doing this sort of thing would be to detect
dnl arguments to add and remove to the set of compiler flags, and
dnl then update CFLAGS and CXXFLAGS (or even better, the AM
dnl versions of those flags?)
dnl
dnl There is no point in checking the cache, this macro does
dnl not do any feature tests.
dnl
dnl The VC++ variant might benefit from -Za, but the header files
dnl shipped with the compiler are incompatible with this option.

AC_DEFUN(CYG_AC_PROG_STANDARD_COMPILER_FLAGS, [
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([CYG_AC_PROG_MSVC])

    AC_MSG_CHECKING("the default compiler flags")

    dnl Add a user-settable flag to control whether or debugging info is
    dnl incorporated at compile-time.
    dnl
    dnl NOTE: there may also have to be a flag to control whether or
    dnl the VC++ multi-threading flags are enabled.
    cygflags_enable_debug="no"
    AC_ARG_ENABLE(debug,[ --enable-debug do a debug rather than a release build],
    [case "${enableval}" in
       yes) cygflags_enable_debug="yes" ;;
       *)   cygflags_enable_debug="no" ;;
    esac])

    dnl For VC++ builds also provide a flag for ANSI vs. unicode builds.
    dnl For now this does not actually affect the compiler flags.
    cygflags_enable_ansi="no"
    if test "${MSVC}" = "yes" ; then
      AC_ARG_ENABLE(ansi,[ --enable-ansi Do an ANSI rather than a unicode build],
      [case "${enableval}" in
         yes) cygflags_enable_ansi="yes" ;;
         *)   cygflags_enable_ansi="no" ;;
      esac])
    fi

    if test "${GCC}" = "yes" ; then
        CC="$CC -Wall -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs"
        CXX="$CXX -Wall -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Woverloaded-virtual"
    elif test "${MSVC}" = "yes" ; then
        CC="$CC -nologo -W3"
        CXX="$CXX -nologo -W3 -GR -GX"
        if test "${cygflags_enable_debug}" = "yes" ; then
            CC="$CC -MDd -Zi"
            CXX="$CXX -MDd -Zi"
        else
            CC="$CC -MD -O2"
            CXX="$CXX -MD -O2"
        fi
    else
        AC_MSG_ERROR("default flags for ${CC} are not known")
    fi
    AC_MSG_RESULT(done)
])

dnl --------------------------------------------------------------------
dnl User-settable options for assertions and tracing.
dnl
dnl The settable options are:
dnl   --disable-asserts
dnl   --disable-preconditions
dnl   --disable-postconditions
dnl   --disable-invariants
dnl   --disable-loopinvariants
dnl   --disable-tracing
dnl   --disable-fntracing

AC_DEFUN(CYG_AC_ARG_INFRASTRUCTURE, [

AC_REQUIRE([CYG_AC_PROG_STANDARD_COMPILER_FLAGS])

if test "${cygflags_enable_debug}" = "yes" ; then
  cyginfra_asserts=yes
  cyginfra_preconditions=yes
  cyginfra_postconditions=yes
  cyginfra_invariants=yes
  cyginfra_loopinvariants=yes
  cyginfra_tracing=yes
  cyginfra_fntracing=yes
else
  cyginfra_asserts=no
  cyginfra_preconditions=no
  cyginfra_postconditions=no
  cyginfra_invariants=no
  cyginfra_loopinvariants=no
  cyginfra_tracing=no
  cyginfra_fntracing=no
fi

AC_ARG_ENABLE(asserts,[ --disable-asserts        disable all assertions],
[case "${enableval}" in
  yes) cyginfra_asserts=yes ;;
  no)  cyginfra_asserts=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-asserts option]) ;;
esac])
if test "${cyginfra_asserts}" = "yes"; then
  AC_DEFINE(CYGDBG_USE_ASSERTS)
fi

AC_ARG_ENABLE(preconditions, [ --disable-preconditions  disable a subset of the assertions],
[case "${enableval}" in
  yes) cyginfra_preconditions=yes ;;
  no)  cyginfra_preconditions=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-preconditions option]) ;;
esac])
if test "${cyginfra_preconditions}" = "yes"; then
  AC_DEFINE(CYGDBG_INFRA_DEBUG_PRECONDITIONS)
fi

AC_ARG_ENABLE(postconditions, [ --disable-postconditions disable a subset of the assertions],
[case "${enableval}" in
  yes) cyginfra_postconditions=yes ;;
  no)  cyginfra_postconditions=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-postconditions option]) ;;
esac])
if test "${cyginfra_postconditions}" = "yes"; then
  AC_DEFINE(CYGDBG_INFRA_DEBUG_POSTCONDITIONS)
fi

AC_ARG_ENABLE(invariants, [ --disable-invariants     disable a subset of the assertions],
[case "${enableval}" in
  yes) cyginfra_invariants=yes ;;
  no)  cyginfra_invariants=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-invariants option]) ;;
esac])
if test "${cyginfra_invariants}" = "yes"; then
  AC_DEFINE(CYGDBG_INFRA_DEBUG_INVARIANTS)
fi

AC_ARG_ENABLE(loopinvariants, [ --disable-loopinvariants disable a subset of the assertions],
[case "${enableval}" in
  yes) cyginfra_loopinvariants=yes ;;
  no)  cyginfra_loopinvariants=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-loopinvariants option]) ;;
esac])
if test "${cyginfra_loopinvariants}" = "yes"; then
  AC_DEFINE(CYGDBG_INFRA_DEBUG_LOOP_INVARIANTS)
fi

AC_ARG_ENABLE(tracing,[ --disable-tracing        disable tracing],
[case "${enableval}" in
  yes) cyginfra_tracing=yes ;;
  no)  cyginfra_tracing=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-tracing option]) ;;
esac])
if test "${cyginfra_tracing}" = "yes"; then
  AC_DEFINE(CYGDBG_USE_TRACING)
fi

AC_ARG_ENABLE(fntracing,[ --disable-fntracing      disable function entry/exit tracing],
[case "${enableval}" in
  yes) cyginfra_fntracing=yes ;;
  no)  cyginfra_fntracing=no  ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-fntracing option]) ;;
esac])
if test "${cyginfra_fntracing}" = "yes"; then
  AC_DEFINE(CYGDBG_INFRA_DEBUG_FUNCTION_REPORTS)
fi

])

dnl ====================================================================
dnl Inspired by KDE's autoconfig
dnl This macro takes three Arguments like this:
dnl AC_FIND_FILE(foo.h, $incdirs, incdir)
dnl the filename to look for, the list of paths to check and
dnl the variable with the result.

AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])


dnl --------------------------------------------------------------------
dnl Convert a cygwin pathname to something acceptable to VC++ (but
dnl still invoked from bash and cygwin's make). This means using
dnl the cygpath utility and then translating any backslashes into
dnl forward slashes to avoid confusing make.

AC_DEFUN(CYG_AC_MSVC_PATH, [
    AC_REQUIRE([CYG_AC_PROG_MSVC])
    ifelse($#, 1, , AC_MSG_ERROR("Invalid number of arguments passed to CYG AC_MSVC_PATH"))

    if test "${MSVC}" = "yes" ; then
      $1=`cygpath -w ${$1} | tr \\\\\\\\ /`
    fi
])


dnl --------------------------------------------------------------------
dnl Work out details of the Tcl installation that should be used.
dnl This adds two command-line options, --with-tcl=<prefix> to
dnl specify the Tcl install directory, and --with-tcl-version=<vsn>
dnl to control which version of Tcl should be used.
dnl
dnl On Unix systems and under cygwin there should be a file
dnl $(tcl_prefix)/lib/tclConfig.sh containing all the information
dnl needed for Tcl. This file are consulted and the appropriate
dnl variables extracted.
dnl
dnl To confuse matters, subtly different naming conventions are used
dnl under Unix and NT. Under Unix the Tcl library will be called
dnl libtcl8.0.a, libtcl8.1.a, etc. with a dot between the major and
dnl minor version. Under NT (including cygwin) the library will be
dnl called tcl80.lib, dnl tcl81.lib, libtcl80.a, libtcl81.a, etc.
dnl without a dot.
dnl
dnl The macro should really assume that it is running inside devo,
dnl with tcl and tcl8.1 directories present and already configured.
dnl This is tricky for a number of reasons, including confusion about
dnl the meaning of top_srcdir.
dnl
dnl This macro defines four new shell variables which will be
dnl substituted (typically into Makefile.in):
dnl
dnl    cyg_ac_tcl_version	- the version of Tcl that is to be used
dnl    cyg_ac_tcl_incdir	- location of <tcl.h> etc al
dnl    cyg_ac_tcl_libdir	- location of libtcl.a/tcl.lib etc.
dnl    cyg_ac_tcl_libs		- all of the libraries that are needed for Tcl

AC_DEFUN(CYG_AC_PATH_TCL, [

AC_REQUIRE([CYG_AC_PROG_MSVC])
AC_REQUIRE([AC_CYGWIN])

dnl Look for the version of Tcl. If none is specified, default to
dnl 8.0 under VC++ and cygwin, nothing under Unix. A version has to be
dnl specified under NT because there is no symbolic link from libtcl.a
dnl to whatever happens to be the most recent installed version.

AC_MSG_CHECKING(for Tcl version)
AC_ARG_WITH(tcl-version,[ --with-tcl-version=<vsn> version of Tcl to be used],[
  cyg_ac_tcl_version=${with_tcl_version}
],[
  if test "${MSVC}" = "yes" ; then
    cyg_ac_tcl_version=80
  else
    if test "${ac_cv_cygwin}" = "yes" ; then
      cyg_ac_tcl_version=80
    else
      cyg_ac_tcl_version=""
    fi
  fi
])
AC_MSG_RESULT(${cyg_ac_tcl_version})
AC_SUBST(cyg_ac_tcl_version)

dnl Where is the Tcl installation? By default assume Tcl is already
dnl installed in the same place as the eCos host-side.

AC_ARG_WITH(tcl,[ --with-tcl-header=<path>        location of Tcl header])
AC_ARG_WITH(tcl,[ --with-tcl-lib=<path>        location of Tcl libraries])
AC_ARG_WITH(tcl,[ --with-tcl=<path>        location of Tcl header and libraries])

AC_MSG_CHECKING(for Tcl installation)

tcl_incdirs="${with_tcl_header} ${with_tcl}/include ${prefix}/include /usr/include/tcl$cyg_ac_tcl_version /usr/include"
AC_FIND_FILE(tcl.h, $tcl_incdirs, cyg_ac_tcl_incdir)

dnl Sanity check, make sure that there is a tcl.h header file.
dnl If not then there is no point in proceeding.
if test \! -r ${cyg_ac_tcl_incdir}/tcl.h ; then
  AC_MSG_ERROR(unable to locate Tcl header file tcl.h)
fi

tcl_libdirs="${with_tcl_lib} ${with_tcl}/lib ${prefix}/lib /usr/lib/tcl$cyg_ac_tcl_version /usr/lib"
AC_FIND_FILE(tclConfig.sh, $tcl_libdirs, cyg_ac_tcl_libdir)

AC_MSG_RESULT(${cyg_ac_tcl_libdir})

dnl If using VC++ then there is no tclConfig.sh file, so the
dnl library information has to be hard-wired. Otherwise ther
dnl should be a tclConfig.sh file containing the necessary information.
if test "${MSVC}" = "yes" ; then
  cyg_ac_tcl_libs="tcl${cyg_ac_tcl_version}.lib"
  CYG_AC_MSVC_PATH(cyg_ac_tcl_incdir)
  CYG_AC_MSVC_PATH(cyg_ac_tcl_libdir)
else
  if test \! -r ${cyg_ac_tcl_libdir}/tclConfig.sh ; then
    AC_MSG_ERROR(unable to locate Tcl config file tclConfig.sh)
  else
    . ${cyg_ac_tcl_libdir}/tclConfig.sh
    cyg_ac_tcl_libs="-ltcl${cyg_ac_tcl_version} ${TCL_LIBS}"
  fi
fi

AC_SUBST(cyg_ac_tcl_incdir)
AC_SUBST(cyg_ac_tcl_libdir)
AC_SUBST(cyg_ac_tcl_libs)
AC_SUBST(cyg_ac_tk_libs)
])



