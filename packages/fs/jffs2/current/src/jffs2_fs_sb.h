/* $Id: jffs2_fs_sb.h,v 1.18 2002/01/09 11:44:23 dwmw2 Exp $ */

#ifndef _JFFS2_FS_SB
#define _JFFS2_FS_SB

#ifdef __ECOS
#include "list.h"
#else
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <asm/semaphore.h>
#include <linux/list.h>
#endif

#define INOCACHE_HASHSIZE 1

#define JFFS2_SB_FLAG_RO 1

/* A struct for the overall file system control.  Pointers to
   jffs2_sb_info structs are named `c' in the source code.  
   Nee jffs_control
*/
struct jffs2_sb_info {
	struct mtd_info *mtd;

	uint32_t highest_ino;
	unsigned int flags;
	spinlock_t nodelist_lock;

	//	pid_t thread_pid;		/* GC thread's PID */
	struct task_struct *gc_task;	/* GC task struct */
#ifndef __ECOS
	struct semaphore gc_thread_start; /* GC thread start mutex */
	struct completion gc_thread_exit; /* GC thread exit completion port */
#endif
	//	uint32_t gc_minfree_threshold;	/* GC trigger thresholds */
	//	uint32_t gc_maxdirty_threshold;

	struct semaphore alloc_sem;	/* Used to protect all the following 
					   fields, and also to protect against
					   out-of-order writing of nodes.
					   And GC.
					*/
	uint32_t flash_size;
	uint32_t used_size;
	uint32_t dirty_size;
	uint32_t free_size;
	uint32_t erasing_size;
	uint32_t bad_size;
	uint32_t sector_size;
	//	uint32_t min_free_size;
	//	uint32_t max_chunk_size;

	uint32_t nr_free_blocks;
	uint32_t nr_erasing_blocks;

	uint32_t nr_blocks;
	struct jffs2_eraseblock *blocks;	/* The whole array of blocks. Used for getting blocks 
						 * from the offset (blocks[ofs / sector_size]) */
	struct jffs2_eraseblock *nextblock;	/* The block we're currently filling */

	struct jffs2_eraseblock *gcblock;	/* The block we're currently garbage-collecting */

	struct list_head clean_list;		/* Blocks 100% full of clean data */
	struct list_head dirty_list;		/* Blocks with some dirty space */
	struct list_head erasing_list;		/* Blocks which are currently erasing */
	struct list_head erase_pending_list;	/* Blocks which need erasing */
	struct list_head erase_complete_list;	/* Blocks which are erased and need the clean marker written to them */
	struct list_head free_list;		/* Blocks which are free and ready to be used */
	struct list_head bad_list;		/* Bad blocks. */
	struct list_head bad_used_list;		/* Bad blocks with valid data in. */

	spinlock_t erase_completion_lock;	/* Protect free_list and erasing_list 
						   against erase completion handler */
	wait_queue_head_t erase_wait;		/* For waiting for erases to complete */
	struct jffs2_inode_cache *inocache_list[INOCACHE_HASHSIZE];
	spinlock_t inocache_lock;
};

#endif /* _JFFS2_FB_SB */
