#ifndef CRC32_H
#define CRC32_H

/* $Id: crc32.h,v 1.4 2002/01/03 15:20:44 dwmw2 Exp $ */

#ifdef __ECOS
#include "jffs2port.h"
#else
#include <linux/types.h>
#endif

extern const uint32_t crc32_table[256];

/* Return a 32-bit CRC of the contents of the buffer. */

static inline uint32_t 
crc32(uint32_t val, const void *ss, int len)
{
	return cyg_crc32_accumulate(val,(void *)ss,len);
}

#endif
