dnl Process this file with aclocal to get an aclocal.m4 file. Then
dnl process that with autoconf.
dnl ====================================================================
dnl
dnl     acinclude.m4
dnl
dnl ====================================================================
dnl###COPYRIGHTBEGIN####
dnl                                                                          
dnl -------------------------------------------                              
dnl The contents of this file are subject to the Red Hat eCos Public License 
dnl Version 1.1 (the "License"); you may not use this file except in         
dnl compliance with the License.  You may obtain a copy of the License at    
dnl http://www.redhat.com/                                                   
dnl                                                                          
dnl Software distributed under the License is distributed on an "AS IS"      
dnl basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
dnl License for the specific language governing rights and limitations under 
dnl the License.                                                             
dnl                                                                          
dnl The Original Code is eCos - Embedded Configurable Operating System,      
dnl released September 30, 1998.                                             
dnl                                                                          
dnl The Initial Developer of the Original Code is Red Hat.                   
dnl Portions created by Red Hat are                                          
dnl Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
dnl All Rights Reserved.                                                     
dnl -------------------------------------------                              
dnl                                                                          
dnl####COPYRIGHTEND####
dnl ====================================================================
dnl#####DESCRIPTIONBEGIN####
dnl
dnl Author(s):	bartv
dnl Contact(s):	bartv
dnl Date:	1999/08/11
dnl Version:	0.01
dnl
dnl####DESCRIPTIONEND####
dnl ====================================================================

dnl Access shared macros.
dnl AM_CONDITIONAL needs to be mentioned here or else aclocal does not
dnl incorporate the macro into aclocal.m4
sinclude(../../../../acinclude.m4)
