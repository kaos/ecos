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
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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

ifdef CYG_HAL_ARM
# Work-around for PR 19698
s_atan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ceil.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_copysign.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_cos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_expm1.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_fabs.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_finite.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_floor.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_frexp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_isnan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ldexp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_rint.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_scalbn.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_sin.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_tan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_modf.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_tanh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_asinh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_cbrt.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_erf.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ilogb.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_log1p.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_logb.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_nextafter.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_significand.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop

e_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
w_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_acos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_acosh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_asin.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_atan2.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_atanh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_cosh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_exp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_hypot.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_j0.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_j1.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_jn.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_lgamma_r.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_log.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_log10.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_pow.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_rem_pio2.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_remainder.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_sinh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_sqrt.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
k_cos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
k_tan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
endif

ifdef CYG_HAL_SPARCLITE
# Work-around for PR 19698
s_atan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ceil.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_copysign.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_cos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_expm1.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_fabs.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_finite.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_floor.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_frexp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_isnan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ldexp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_rint.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_scalbn.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_sin.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_tan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_modf.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_tanh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_asinh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_cbrt.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_erf.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_ilogb.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_log1p.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_logb.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_nextafter.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
s_significand.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop

e_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
w_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_acos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_acosh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_asin.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_atan2.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_atanh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_cosh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_exp.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_fmod.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_hypot.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_j0.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_j1.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_jn.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_lgamma_r.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_log.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_log10.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_pow.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_rem_pio2.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_remainder.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_sinh.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
e_sqrt.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
k_cos.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
k_tan.c-CFLAGS += -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fdelayed-branch -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop
endif

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

