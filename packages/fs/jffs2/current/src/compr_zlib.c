/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001, 2002 Red Hat, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: compr_zlib.c,v 1.12 2002/01/25 01:49:26 dwmw2 Exp $
 *
 */

/* Be careful - there are three environments are supported by this file. 
 * - Linux kernel.
 * - eCos kernel.
 * - Userspace (mkfs.jffs2).
 */

#ifdef __ECOS
	/* Compiling in eCos (or RedBoot) */
#include <cyg/compress/zlib.h>
#include "jffs2port.h"
#include "nodelist.h"

#elif defined(__KERNEL__)
	/* Compiling in Linux kernel */
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mtd/compatmac.h> /* for min() */
#include <linux/slab.h>
#include <linux/jffs2.h>
#include "nodelist.h"

static void *zalloc(void *opaque, unsigned nr, unsigned size)
{
	/* How much does it request? Should we use vmalloc? Or be dynamic? */
	return kmalloc(nr * size, GFP_KERNEL);
}

static void zfree(void *opaque, void *addr)
{
	kfree(addr);
}
#ifdef CONFIG_ZLIB /* 2.5 kernels will have a shared zlib, at last. */
#include <linux/zlib.h>
#else
#include "zlib.h"
#endif

#else
	/* Compiling in Linux(ish) userspace */
#define min(x,y) ((x)<(y)?(x):(y))
#ifndef D1
#define D1(x)
#endif
#define KERN_DEBUG
#define KERN_NOTICE
#define KERN_WARNING
#define printk printf
#include <stdint.h>
#include <stdio.h>
#include <zlib.h>
#endif

	/* Plan: call deflate() with avail_in == *sourcelen, 
		avail_out = *dstlen - 12 and flush == Z_FINISH. 
		If it doesn't manage to finish,	call it again with
		avail_in == 0 and avail_out set to the remaining 12
		bytes for it to clean up. 
	   Q: Is 12 bytes sufficient?
	*/
#define STREAM_END_SPACE 12

int jffs2_zlib_compress(unsigned char *data_in, unsigned char *cpage_out, 
		   uint32_t *sourcelen, uint32_t *dstlen)
{
	z_stream strm;
	int ret;

	if (*dstlen <= STREAM_END_SPACE)
		return -1;

#ifdef __KERNEL__ /* Linux needs own alloc/free funcs */
	strm.zalloc = zalloc;
	strm.zfree = zfree;
#else /* eCos and userspace don't */
	strm.zalloc = (void *)0;
	strm.zfree = (void *)0;
#endif

	if (Z_OK != deflateInit(&strm, 3)) {
		printk(KERN_WARNING "deflateInit failed\n");
		return -1;
	}
	strm.next_in = data_in;
	strm.total_in = 0;
	
	strm.next_out = cpage_out;
	strm.total_out = 0;

	while (strm.total_out < *dstlen - STREAM_END_SPACE && strm.total_in < *sourcelen) {
		strm.avail_out = *dstlen - (strm.total_out + STREAM_END_SPACE);
		strm.avail_in = min((unsigned)(*sourcelen-strm.total_in), strm.avail_out);
		D1(printk(KERN_DEBUG "calling deflate with avail_in %d, avail_out %d\n",
			  strm.avail_in, strm.avail_out));
		ret = deflate(&strm, Z_PARTIAL_FLUSH);
		D1(printk(KERN_DEBUG "deflate returned with avail_in %d, avail_out %d, total_in %ld, total_out %ld\n", 
			  strm.avail_in, strm.avail_out, strm.total_in, strm.total_out));
		if (ret != Z_OK) {
			D1(printk(KERN_DEBUG "deflate in loop returned %d\n", ret));
			deflateEnd(&strm);
			return -1;
		}
	}
	strm.avail_out += STREAM_END_SPACE;
	strm.avail_in = 0;
	ret = deflate(&strm, Z_FINISH);
	if (ret != Z_STREAM_END) {
		D1(printk(KERN_DEBUG "final deflate returned %d\n", ret));
		deflateEnd(&strm);
		return -1;
	}
	deflateEnd(&strm);

	D1(printk(KERN_DEBUG "zlib compressed %ld bytes into %ld\n", strm.total_in, strm.total_out));

	if (strm.total_out >= strm.total_in)
		return -1;


	*dstlen = strm.total_out;
	*sourcelen = strm.total_in;
	return 0;
}

void jffs2_zlib_decompress(unsigned char *data_in, unsigned char *cpage_out,
		      uint32_t srclen, uint32_t destlen)
{
	z_stream strm;
	int ret;

#ifdef __KERNEL__
	strm.zalloc = zalloc;
	strm.zfree = zfree;
#else
	strm.zalloc = (void *)0;
	strm.zfree = (void *)0;
#endif

	if (Z_OK != inflateInit(&strm)) {
		printk(KERN_WARNING "inflateInit failed\n");
		return;
	}
	strm.next_in = data_in;
	strm.avail_in = srclen;
	strm.total_in = 0;
	
	strm.next_out = cpage_out;
	strm.avail_out = destlen;
	strm.total_out = 0;

	while((ret = inflate(&strm, Z_FINISH)) == Z_OK)
		;
	if (ret != Z_STREAM_END) {
		printk(KERN_NOTICE "inflate returned %d\n", ret);
	}
	inflateEnd(&strm);
}
