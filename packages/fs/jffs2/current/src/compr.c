/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * Created by Arjan van de Ven <arjanv@redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: compr.c,v 1.33 2003/11/28 17:22:54 dwmw2 Exp $
 *
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/jffs2.h>
#include "nodelist.h"

int jffs2_zlib_compress(unsigned char *data_in, unsigned char *cpage_out, uint32_t *sourcelen, uint32_t *dstlen);
void jffs2_zlib_decompress(unsigned char *data_in, unsigned char *cpage_out, uint32_t srclen, uint32_t destlen);
int jffs2_rtime_compress(unsigned char *data_in, unsigned char *cpage_out, uint32_t *sourcelen, uint32_t *dstlen);
void jffs2_rtime_decompress(unsigned char *data_in, unsigned char *cpage_out, uint32_t srclen, uint32_t destlen);
int jffs2_rubinmips_compress(unsigned char *data_in, unsigned char *cpage_out, uint32_t *sourcelen, uint32_t *dstlen);
void jffs2_rubinmips_decompress(unsigned char *data_in, unsigned char *cpage_out, uint32_t srclen, uint32_t destlen);
int jffs2_dynrubin_compress(unsigned char *data_in, unsigned char *cpage_out, uint32_t *sourcelen, uint32_t *dstlen);
void jffs2_dynrubin_decompress(unsigned char *data_in, unsigned char *cpage_out, uint32_t srclen, uint32_t destlen);


/* jffs2_compress:
 * @data: Pointer to uncompressed data
 * @cdata: Pointer to returned pointer to buffer for compressed data
 * @datalen: On entry, holds the amount of data available for compression.
 *	On exit, expected to hold the amount of data actually compressed.
 * @cdatalen: On entry, holds the amount of space available for compressed
 *	data. On exit, expected to hold the actual size of the compressed
 *	data.
 *
 * Returns: Byte to be stored with data indicating compression type used.
 * Zero is used to show that the data could not be compressed - the 
 * compressed version was actually larger than the original.
 *
 * If the cdata buffer isn't large enough to hold all the uncompressed data,
 * jffs2_compress should compress as much as will fit, and should set 
 * *datalen accordingly to show the amount of data which were compressed.
 */
unsigned char jffs2_compress(unsigned char *data_in, unsigned char **cpage_out, 
		    uint32_t *datalen, uint32_t *cdatalen)
{
#ifdef JFFS2_COMPRESSION
	int ret;

	*cpage_out = kmalloc(*cdatalen, GFP_KERNEL);
	if (!*cpage_out) {
		printk(KERN_WARNING "No memory for compressor allocation. Compression failed\n");
		goto out;
	}

#ifdef JFFS2_USE_ZLIB
	ret = jffs2_zlib_compress(data_in, *cpage_out, datalen, cdatalen);
	if (!ret) {
		return JFFS2_COMPR_ZLIB;
	}
#endif
#ifdef JFFS2_USE_DYNRUBIN
	ret = jffs2_dynrubin_compress(data_in, *cpage_out, datalen, cdatalen);
	if (!ret) {
		return JFFS2_COMPR_DYNRUBIN;
	}
#endif
#ifdef JFFS2_USE_RUBINMIPS
	ret = jffs2_rubinmips_compress(data_in, *cpage_out, datalen, cdatalen);
	if (!ret) {
		return JFFS2_COMPR_RUBINMIPS;
	}
#endif
#ifdef JFFS2_USE_RTIME
	/* rtime does manage to recompress already-compressed data */
	ret = jffs2_rtime_compress(data_in, *cpage_out, datalen, cdatalen);
	if (!ret) {
		return JFFS2_COMPR_RTIME;
	}
#endif
	kfree(*cpage_out);
#endif /* Compression */
 out:
	*cpage_out = data_in;
	*datalen = *cdatalen;
	return JFFS2_COMPR_NONE; /* We failed to compress */
}

void jffs2_free_comprbuf(unsigned char *comprbuf, unsigned char *orig)
{
	if (orig != comprbuf)
		kfree(comprbuf);
}

int jffs2_decompress(unsigned char comprtype, unsigned char *cdata_in, 
		     unsigned char *data_out, uint32_t cdatalen, uint32_t datalen)
{
	switch (comprtype) {
	case JFFS2_COMPR_NONE:
		/* This should be special-cased elsewhere, but we might as well deal with it */
		memcpy(data_out, cdata_in, datalen);
		break;

	case JFFS2_COMPR_ZERO:
		memset(data_out, 0, datalen);
		break;
#ifdef JFFS2_USE_ZLIB
	case JFFS2_COMPR_ZLIB:
		jffs2_zlib_decompress(cdata_in, data_out, cdatalen, datalen);
		break;
#endif
#ifdef JFFS2_USE_RTIME
	case JFFS2_COMPR_RTIME:
		jffs2_rtime_decompress(cdata_in, data_out, cdatalen, datalen);
		break;
#endif
#ifdef JFFS2_USE_RUBINMIPS
	case JFFS2_COMPR_RUBINMIPS:
		jffs2_rubinmips_decompress(cdata_in, data_out, cdatalen, datalen);
		break;
#endif
#ifdef JFFS2_USE_DYNRUBIN
	case JFFS2_COMPR_DYNRUBIN:

		jffs2_dynrubin_decompress(cdata_in, data_out, cdatalen, datalen);
		break;
#endif
	default:
		printk(KERN_NOTICE "Unknown JFFS2 compression type 0x%02x\n", comprtype);
		return -EIO;
	}
	return 0;
}
