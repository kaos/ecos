#=========================================================================
#
#    makefile
#
#    language/c/libm/src
#
#=========================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.0 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://sourceware.cygnus.com/ecos                                        
#                                                                          
# Software distributed under the License is distributed on an       
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
#=========================================================================

PACKAGE       := libm
include ../../../../../pkgconf/pkgconf.mak
include ../../../../../pkgconf/system.mak

LIBRARY       := libtarget.a
COMPILE       := misc/matherr.c misc/standard.c misc/compatmode.cxx         \
                 misc/infconst.c                                            \
                                                                            \
                 double/ieee754-core/e_acos.c double/ieee754-core/e_asin.c  \
                 double/ieee754-core/e_atan2.c double/ieee754-core/e_cosh.c \
                 double/ieee754-core/e_exp.c double/ieee754-core/e_fmod.c   \
                 double/ieee754-core/e_log.c double/ieee754-core/e_log10.c  \
                 double/ieee754-core/e_pow.c                                \
                 double/ieee754-core/e_rem_pio2.c                           \
                 double/ieee754-core/e_scalb.c double/ieee754-core/e_sinh.c \
                 double/ieee754-core/e_sqrt.c                               \
                                                                            \
                 double/ieee754-api/w_acos.c double/ieee754-api/w_asin.c    \
                 double/ieee754-api/w_atan2.c double/ieee754-api/w_cosh.c   \
                 double/ieee754-api/w_exp.c double/ieee754-api/w_fmod.c     \
                 double/ieee754-api/w_log.c double/ieee754-api/w_log10.c    \
                 double/ieee754-api/w_pow.c double/ieee754-api/w_scalb.c    \
                 double/ieee754-api/w_sinh.c double/ieee754-api/w_sqrt.c    \
                                                                            \
                 double/internal/k_cos.c double/internal/k_rem_pio2.c       \
                 double/internal/k_sin.c double/internal/k_tan.c            \
                                                                            \
                 double/portable-api/s_atan.c double/portable-api/s_ceil.c  \
                 double/portable-api/s_copysign.c                           \
                 double/portable-api/s_cos.c double/portable-api/s_expm1.c  \
                 double/portable-api/s_fabs.c double/portable-api/s_finite.c\
                 double/portable-api/s_floor.c double/portable-api/s_frexp.c\
                 double/portable-api/s_isnan.c double/portable-api/s_ldexp.c\
                 double/portable-api/s_rint.c double/portable-api/s_scalbn.c\
                 double/portable-api/s_sin.c double/portable-api/s_tan.c    \
                 double/portable-api/s_modf.c double/portable-api/s_tanh.c



OTHER_OBJS    :=
OTHER_TARGETS :=
OTHER_CLEAN   :=


include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src
