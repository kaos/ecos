/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2002 Red Hat, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id$
 *
 */

#ifndef __JFFS2_OS_ECOS_H__
#define __JFFS2_OS_ECOS_H__

#include <cyg/io/io.h>

#define CONFIG_JFFS2_FS_DEBUG 0

 /* Read-only operation not currently implemented on eCos */
#define jffs2_is_readonly(c) (0)

/* NAND flash not currently supported on eCos */
#define jffs2_can_mark_obsolete(c) (1)

#define JFFS2_INODE_INFO(i) (&(i)->jffs2_i)
#define OFNI_EDONI_2SFFJ(f)  ((struct inode *) ( ((char *)f) - ((char *)(&((struct inode *)NULL)->jffs2_i)) ) )
 
#define JFFS2_F_I_SIZE(f) (OFNI_EDONI_2SFFJ(f)->i_size)
#define JFFS2_F_I_MODE(f) (OFNI_EDONI_2SFFJ(f)->i_mode)
#define JFFS2_F_I_UID(f) (OFNI_EDONI_2SFFJ(f)->i_uid)
#define JFFS2_F_I_GID(f) (OFNI_EDONI_2SFFJ(f)->i_gid)
#define JFFS2_F_I_CTIME(f) (OFNI_EDONI_2SFFJ(f)->i_ctime)
#define JFFS2_F_I_MTIME(f) (OFNI_EDONI_2SFFJ(f)->i_mtime)
#define JFFS2_F_I_ATIME(f) (OFNI_EDONI_2SFFJ(f)->i_atime)

/* FIXME: eCos doesn't hav a concept of device major/minor numbers */
#define JFFS2_F_I_RDEV_MIN(f) (MINOR(to_kdev_t(OFNI_EDONI_2SFFJ(f)->i_rdev)))
#define JFFS2_F_I_RDEV_MAJ(f) (MAJOR(to_kdev_t(OFNI_EDONI_2SFFJ(f)->i_rdev)))

struct inode {
	//struct list_head	i_hash;
	//struct list_head	i_list;
	struct list_head	i_dentry;

	cyg_uint32		i_ino;
	atomic_t		i_count;
	//kdev_t			i_dev;
	mode_t			i_mode;
	nlink_t			i_nlink;
	uid_t			i_uid;
	gid_t			i_gid;
	kdev_t			i_rdev;
	off_t			i_size;
	time_t			i_atime;
	time_t			i_mtime;
	time_t			i_ctime;
	unsigned long		i_blksize;
	unsigned long		i_blocks;
	//unsigned long		i_version;
	//struct semaphore	i_sem;
	//struct semaphore	i_zombie;
	struct inode_operations	*i_op;
	struct file_operations	*i_fop;	// former ->i_op->default_file_ops 
	struct super_block	*i_sb;
	//wait_queue_head_t	i_wait;
	//struct file_lock	*i_flock;
	//struct address_space	*i_mapping;
	//struct address_space	i_data;	
	//struct dquot		*i_dquot[MAXQUOTAS];
	//struct pipe_inode_info	*i_pipe;
	//struct block_device	*i_bdev;

	//unsigned long		i_state;

	unsigned int		i_flags;
	//unsigned char		i_sock;

	atomic_t		i_writecount;
	//unsigned int		i_attr_flags;
	//uint32_t			i_generation;
	struct jffs2_inode_info	jffs2_i;

        struct inode *i_parent;

        struct inode *i_cache_prev;
        struct inode *i_cache_next;
};

#define JFFS2_SB_INFO(sb) (&(sb)->jffs2_sb)

#define OFNI_BS_2SFFJ(c)  ((struct super_block *) ( ((char *)c) - ((char *)(&((struct super_block *)NULL)->jffs2_sb)) ) )

struct super_block {
	unsigned long		s_blocksize;
	unsigned char		s_blocksize_bits;
	unsigned char		s_dirt;
	//struct super_operations	*s_op;
	unsigned long		s_flags;
	unsigned long		s_magic;
	//struct dentry		*s_root;
	struct inode		*s_root;
	struct jffs2_sb_info jffs2_sb;
        unsigned long       s_mount_count;
    cyg_io_handle_t     s_dev;
};

/* background.c */
static inline void jffs2_garbage_collect_trigger(struct jffs2_sb_info *c)
{
	/* We don't have a GC thread in eCos (yet) */
}

/* dir.c */
extern struct file_operations jffs2_dir_operations;
extern struct inode_operations jffs2_dir_inode_operations;

/* file.c */
extern struct file_operations jffs2_file_operations;
extern struct inode_operations jffs2_file_inode_operations;
extern struct address_space_operations jffs2_file_address_operations;
int jffs2_null_fsync(struct file *, struct dentry *, int);
int jffs2_setattr (struct dentry *dentry, struct iattr *iattr);
int jffs2_do_readpage_nolock (struct inode *inode, struct page *pg);
int jffs2_do_readpage_unlock (struct inode *inode, struct page *pg);
//int jffs2_readpage (struct file *, struct page *);
int jffs2_readpage (struct inode *d_inode, struct page *pg);
//int jffs2_prepare_write (struct file *, struct page *, unsigned, unsigned);
int jffs2_prepare_write (struct inode *d_inode, struct page *pg, unsigned start, unsigned end);
//int jffs2_commit_write (struct file *, struct page *, unsigned, unsigned);
int jffs2_commit_write (struct inode *d_inode, struct page *pg, unsigned start, unsigned end);


#endif /* __JFFS2_OS_ECOS_H__ */
