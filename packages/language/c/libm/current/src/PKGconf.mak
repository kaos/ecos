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

# Work-around for PR 19698

SPECIAL_FLAGS := -O1 -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fstrength-reduce -fthread-jumps -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop

ifdef CYG_HAL_SPARCLITE
SPECIAL_FLAGS += -fdelayed-branch
endif
ifdef CYG_HAL_MIPS_TX39
SPECIAL_FLAGS += -fdelayed-branch
endif
ifdef CYG_HAL_POWERPC
SPECIAL_FLAGS += -fdelayed-branch
endif

s_atan.c-CFLAGS += $(SPECIAL_FLAGS)
s_ceil.c-CFLAGS += $(SPECIAL_FLAGS)
s_copysign.c-CFLAGS += $(SPECIAL_FLAGS)
s_cos.c-CFLAGS += $(SPECIAL_FLAGS)
s_expm1.c-CFLAGS += $(SPECIAL_FLAGS)
s_fabs.c-CFLAGS += $(SPECIAL_FLAGS)
s_finite.c-CFLAGS += $(SPECIAL_FLAGS)
s_floor.c-CFLAGS += $(SPECIAL_FLAGS)
s_frexp.c-CFLAGS += $(SPECIAL_FLAGS)
s_isnan.c-CFLAGS += $(SPECIAL_FLAGS)
s_ldexp.c-CFLAGS += $(SPECIAL_FLAGS)
s_rint.c-CFLAGS += $(SPECIAL_FLAGS)
s_scalbn.c-CFLAGS += $(SPECIAL_FLAGS)
s_sin.c-CFLAGS += $(SPECIAL_FLAGS)
s_tan.c-CFLAGS += $(SPECIAL_FLAGS)
s_modf.c-CFLAGS += $(SPECIAL_FLAGS)
s_tanh.c-CFLAGS += $(SPECIAL_FLAGS)
s_asinh.c-CFLAGS += $(SPECIAL_FLAGS)
s_cbrt.c-CFLAGS += $(SPECIAL_FLAGS)
s_erf.c-CFLAGS += $(SPECIAL_FLAGS)
s_ilogb.c-CFLAGS += $(SPECIAL_FLAGS)
s_log1p.c-CFLAGS += $(SPECIAL_FLAGS)
s_logb.c-CFLAGS += $(SPECIAL_FLAGS)
s_nextafter.c-CFLAGS += $(SPECIAL_FLAGS)
s_significand.c-CFLAGS += $(SPECIAL_FLAGS)
e_fmod.c-CFLAGS += $(SPECIAL_FLAGS)
w_fmod.c-CFLAGS += $(SPECIAL_FLAGS)
e_acos.c-CFLAGS += $(SPECIAL_FLAGS)
e_acosh.c-CFLAGS += $(SPECIAL_FLAGS)
e_asin.c-CFLAGS += $(SPECIAL_FLAGS)
e_atan2.c-CFLAGS += $(SPECIAL_FLAGS)
e_atanh.c-CFLAGS += $(SPECIAL_FLAGS)
e_cosh.c-CFLAGS += $(SPECIAL_FLAGS)
e_exp.c-CFLAGS += $(SPECIAL_FLAGS)
e_fmod.c-CFLAGS += $(SPECIAL_FLAGS)
e_hypot.c-CFLAGS += $(SPECIAL_FLAGS)
e_j0.c-CFLAGS += $(SPECIAL_FLAGS)
e_j1.c-CFLAGS += $(SPECIAL_FLAGS)
e_jn.c-CFLAGS += $(SPECIAL_FLAGS)
e_lgamma_r.c-CFLAGS += $(SPECIAL_FLAGS)
e_log.c-CFLAGS += $(SPECIAL_FLAGS)
e_log10.c-CFLAGS += $(SPECIAL_FLAGS)
e_pow.c-CFLAGS += $(SPECIAL_FLAGS)
e_rem_pio2.c-CFLAGS += $(SPECIAL_FLAGS)
e_remainder.c-CFLAGS += $(SPECIAL_FLAGS)
e_sinh.c-CFLAGS += $(SPECIAL_FLAGS)
e_sqrt.c-CFLAGS += $(SPECIAL_FLAGS)
k_cos.c-CFLAGS += $(SPECIAL_FLAGS)
k_tan.c-CFLAGS += $(SPECIAL_FLAGS)

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src
