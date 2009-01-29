dnl Process this file with aclocal to get an aclocal.m4 file. Then
dnl process that with autoconf.
dnl ====================================================================
dnl
dnl     acinclude.m4
dnl
dnl     Host side implementation of the eCos infrastructure.
dnl
dnl ====================================================================
dnl ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                       
dnl -------------------------------------------                             
dnl This file is part of the eCos host tools.                               
dnl Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
dnl
dnl This program is free software; you can redistribute it and/or modify    
dnl it under the terms of the GNU General Public License as published by    
dnl the Free Software Foundation; either version 2 or (at your option) any  
dnl later version.                                                          
dnl
dnl This program is distributed in the hope that it will be useful, but     
dnl WITHOUT ANY WARRANTY; without even the implied warranty of              
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       
dnl General Public License for more details.                                
dnl
dnl You should have received a copy of the GNU General Public License       
dnl along with this program; if not, write to the                           
dnl Free Software Foundation, Inc., 51 Franklin Street,                     
dnl Fifth Floor, Boston, MA  02110-1301, USA.                               
dnl -------------------------------------------                             
dnl ####ECOSHOSTGPLCOPYRIGHTEND####                                         
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
sinclude(../../acsupport/acinclude.m4)
