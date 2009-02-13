eCos Configuration Tool version 3
Copyright (c) Free Software Foundation Inc., 2001-2009
======================================================


Introduction
============

  Welcome to the eCos Configuration Tool, a graphical tool to assist in
  the configuration and building of a customised version of the eCos
  real-time operating system.

  This is a cross-platform version built using the wxWidgets toolkit. The
  tool uses the GTK+ 2.x widget set on Linux, and the Win32 API on Windows
  2000, Windows XP and Windows Vista. Windows 95, Windows 98 and Windows ME
  are not supported.


Running the eCos Configuration Tool
===================================

  You can invoke the tool with zero, one, or two parameters. The two
  parameters can be the location of the repository and/or the location of
  a save file (extension .ecc).

  For detailed information about the eCos Configuration Tool, please refer
  to the eCos User Guide which may be invoked from the Help menu.


Frequently Asked Questions
==========================

Q:  Why does the eCos Configuration Tool use wxWidgets?

A:  wxWidgets is an open source, mature multi-platform GUI toolkit for
    C++. It makes platform-independence relatively easy to achieve,
    whilst remaining compatibility with the look and feel of GTK+ and
    Win32 on their respective platforms. Using a platform-independent
    API will make it easier to port the eCos Configuration Tool to other
    platforms if needed.

Q:  Where can I get more information about wxWidgets?

A:  The wxWidgets web site is at http://www.wxwidgets.org. wxWidgets
    distributions include documentation in a variety of formats.

Q:  How can I help improve the Configuration Tool?

A:  All help is very welcome: please see the FAQ at
    http://ecos.sourceware.org/ for details of how to contribute.

Q:  Who do I contact when things go wrong?

A:  Please discuss problems on the ecos-discuss mailing list:
    http://ecos.sourceware.org/intouch.html


Building the eCos Configuration Tool
====================================

  The build system is subject to change.

  You will need to configure and build the eCos host infrastructure first.
  This is described in the README.host file at the top level of the eCos
  directory hierarchy. The examples below assume that the eCos host
  infrastructure has been configured with:

      --prefix=/opt/ecos/ecos-3.0/tools

  There is no 'configure' step for building the eCos Configuration Tool.

  You will need to pass the wxWidgets installation directory (WXDIR), the
  eCos host infrastructure installation directory (INSTALLDIR) and the OS
  type (OSTYPE) to the GNU make file (makefile.gnu). The version 3 tool
  was developed for wxWidgets 2.8.x configured with "--disable-sockets" and
  will not build correctly with earlier versions.

  Examples:

  1. To build and install the configtool on Cygwin for debugging purposes:

     $ mkdir builddir
     $ cd builddir
     $ ECOSTOOLSPREFIX=/opt/ecos/ecos-3.0/tools
     $ make -f ${ECOSTOOLSPREFIX}/src/tools/configtool/standalone/wxwin/makefile.gnu \
         install WXDIR=/path/to/wxMSW INSTALLDIR=$ECOSTOOLSPREFIX OSTYPE=cygwin DEBUG=1

  2. To build and install the configtool on Linux for release:

     $ mkdir builddir
     $ cd builddir
     $ ECOSTOOLSPREFIX=/opt/ecos/ecos-3.0/tools
     $ make -f ${ECOSTOOLSPREFIX}/src/tools/configtool/standalone/wxwin/makefile.gnu \
         install WXDIR=/path/to/wxGTK INSTALLDIR=$ECOSTOOLSPREFIX OSTYPE=linux-gnu
