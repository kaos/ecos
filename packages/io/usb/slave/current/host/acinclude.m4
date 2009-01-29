dnl Process this file with aclocal to get an aclocal.m4 file. Then
dnl process that with autoconf.
dnl ====================================================================
dnl
dnl     acinclude.m4
dnl
dnl ====================================================================
dnl ####ECOSGPLCOPYRIGHTBEGIN####                                           
dnl -------------------------------------------                             
dnl This file is part of eCos, the Embedded Configurable Operating System.  
dnl Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
dnl
dnl eCos is free software; you can redistribute it and/or modify it under   
dnl the terms of the GNU General Public License as published by the Free    
dnl Software Foundation; either version 2 or (at your option) any later     
dnl version.                                                                
dnl
dnl eCos is distributed in the hope that it will be useful, but WITHOUT     
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or   
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   
dnl for more details.                                                       
dnl
dnl You should have received a copy of the GNU General Public License       
dnl along with eCos; if not, write to the Free Software Foundation, Inc.,   
dnl 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           
dnl
dnl As a special exception, if other files instantiate templates or use     
dnl macros or inline functions from this file, or you compile this file     
dnl and link it with other works to produce a work based on this file,      
dnl this file does not by itself cause the resulting work to be covered by  
dnl the GNU General Public License. However the source code for this file   
dnl must still be made available in accordance with section (3) of the GNU  
dnl General Public License v2.                                              
dnl
dnl This exception does not invalidate any other reasons why a work based   
dnl on this file might be covered by the GNU General Public License.        
dnl -------------------------------------------                             
dnl ####ECOSGPLCOPYRIGHTEND####                                             
dnl ====================================================================
dnl#####DESCRIPTIONBEGIN####
dnl
dnl Author(s):	bartv
dnl Contact(s):	bartv
dnl Date:	2002/01/10
dnl Version:	0.01
dnl
dnl####DESCRIPTIONEND####
dnl ====================================================================

dnl Access shared macros.
dnl AM_CONDITIONAL needs to be mentioned here or else aclocal does not
dnl incorporate the macro into aclocal.m4
sinclude(../../../../../../acsupport/acinclude.m4)
