#ifndef __LINUX_JFFS2PORT_H__
#define __LINUX_JFFS2PORT_H__

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
//#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>
//#include <pkgconf/fs_ram.h>

#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

#include <cyg/fileio/fileio.h>

#include <cyg/hal/drv_api.h>
#include <cyg/infra/diag.h>

#include <cyg/io/flash.h>

#include <pkgconf/fs_jffs2.h>

#include "list.h"

// ARM related constants
#define PAGE_SHIFT 0xC
#define PAGE_SIZE  (0x1 << PAGE_SHIFT)
#define PAGE_CACHE_SHIFT        PAGE_SHIFT
#define PAGE_CACHE_SIZE         PAGE_SIZE

//#define BITS_PER_LONG 32

// Linux types
#define printf diag_printf
#define printk diag_printf
#define BUG() diag_printf("BUG()\n")

#define uint8_t cyg_uint8
#define uint16_t cyg_uint16
#define uint32_t cyg_uint32
#define loff_t off_t

#define min(x,y) (x<y?x:y)
#define max(x,y) (x<y?y:x)
#define min_t(t, x,y) ((t)x<(t)y?(t)x:(t)y)

#define atomic_t int
#define atomic_inc(atom) (*atom)++
#define atomic_dec(atom) (*atom)--
#define atomic_read(atom) (*atom)

#define kmalloc(a,b) malloc(a)
#define kfree(a) free(a)

#define PageLocked(pg) 1
#define Page_Uptodate(pg) 0
#define UnlockPage(pg) 
#define PAGE_BUG(pg) BUG()
#define ClearPageUptodate(pg)
#define SetPageError(pg)
#define ClearPageError(pg)
#define SetPageUptodate(pg)

#define jiffies 100

#define ERR_PTR(err) (void*)(err)
#define PTR_ERR(err) (cyg_int32)(err)
#define IS_ERR(err) (err==NULL)

#define CURRENT_TIME cyg_timestamp()

//#define spinlock_t cyg_spinlock_t
typedef struct { } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { }
#define spin_lock_init(lock) do{} while (0)
#define spin_lock(lock) do{} while (0)
#define spin_unlock(lock) do{} while (0)
#define spin_lock_bh(lock) do{} while (0)
#define spin_unlock_bh(lock) do{} while (0)

struct semaphore {
	cyg_drv_mutex_t x;
};

#define init_MUTEX(sem) cyg_drv_mutex_init((cyg_drv_mutex_t *)sem)
#define init_MUTEX_LOCKED(sem) do { cyg_drv_mutex_init((cyg_drv_mutex_t *)sem); cyg_drv_mutex_lock((cyg_drv_mutex_t *)sem); } while(0)
#define down(sem) cyg_drv_mutex_lock((cyg_drv_mutex_t *)sem)
#define down_interruptible(sem) ({ cyg_drv_mutex_lock((cyg_drv_mutex_t *)sem), 0; })
#define up(sem) cyg_drv_mutex_unlock((cyg_drv_mutex_t *)sem)

#define wait_queue_head_t cyg_uint32
#define init_waitqueue_head(wait) do{} while (0)
#define add_wait_queue(wait,new_wait) do{} while (0)
#define remove_wait_queue(wait,old_wait) do{} while (0)
#define DECLARE_WAITQUEUE(wait,current) do{} while (0)

#define	KERN_EMERG              "<0>" // system is unusable
#define	KERN_ALERT              "<1>" // action must be taken immediately
#define	KERN_CRIT               "<2>" // critical conditions
#define	KERN_ERR                "<3>" // error conditions
#define	KERN_WARNING            "<4>" // warning conditions
#define	KERN_NOTICE             "<5>" // normal but significant condition
#define	KERN_INFO               "<6>" // informational
#define	KERN_DEBUG              "<7>" // debug-level messages

// Structures used by VFS

typedef unsigned short kdev_t;

struct qstr {
	const unsigned char * name;
	unsigned int len;
	unsigned int hash;
};

#define DNAME_INLINE_LEN 16

struct dentry {
	atomic_t d_count;
	unsigned int d_flags;
	struct inode  * d_inode;	/* Where the name belongs to - NULL is negative */
	struct dentry * d_parent;	/* parent directory */
	struct list_head d_hash;	/* lookup hash list */
	struct list_head d_child;	/* child of parent list */
	struct list_head d_subdirs;	/* our children */
	struct list_head d_alias;	/* inode alias list */
	struct qstr d_name;
	struct dentry_operations  *d_op;
	struct super_block * d_sb;	/* The root of the dentry tree */
	unsigned char d_iname[DNAME_INLINE_LEN]; /* small names */
};

struct file {
	struct dentry		*f_dentry;
	unsigned int 		f_flags;
	mode_t			    f_mode;
	loff_t			    f_pos;
	unsigned long 		f_reada, f_ramax, f_raend, f_ralen, f_rawin;
};

#define ATTR_MODE	1
#define ATTR_UID	2
#define ATTR_GID	4
#define ATTR_SIZE	8
#define ATTR_ATIME	16
#define ATTR_MTIME	32
#define ATTR_CTIME	64
#define ATTR_ATIME_SET	128
#define ATTR_MTIME_SET	256
#define ATTR_FORCE	512	/* Not a change, but a change it */
#define ATTR_ATTR_FLAG	1024

typedef unsigned short umode_t;

struct iattr {
        unsigned int ia_valid;
	umode_t		ia_mode;
	uid_t		ia_uid;
	gid_t		ia_gid;
	loff_t		ia_size;
	time_t		ia_atime;
	time_t		ia_mtime;
	time_t		ia_ctime;
};

struct page {
	unsigned long index;
	void *virtual;
};

struct nameidata {
	struct dentry *dentry;
	struct qstr last;
	unsigned int flags;
	int last_type;
};

struct file_operations {
	//struct module *owner;
	//loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	//ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	int (*readdir) (struct file *, char *, int);
	//unsigned int (*poll) (struct file *, struct poll_table_struct *);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
	//int (*mmap) (struct file *, struct vm_area_struct *);
	//int (*open) (struct inode *, struct file *);
	//int (*flush) (struct file *);
	//int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, struct dentry *, int datasync);
	//int (*fasync) (int, struct file *, int);
	//int (*lock) (struct file *, int, struct file_lock *);
	//ssize_t (*readv) (struct file *, const struct iovec *, unsigned long, loff_t *);
	//ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
};

struct inode_operations {
	int (*create) (struct inode *,struct dentry *,int);
	struct dentry * (*lookup) (struct inode *,struct dentry *);
	int (*link) (struct dentry *,struct inode *,struct dentry *);
	int (*unlink) (struct inode *,struct dentry *);
	int (*symlink) (struct inode *,struct dentry *,const char *);
	int (*mkdir) (struct inode *,struct dentry *,int);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct inode *,struct dentry *,int,int);
	int (*rename) (struct inode *, struct dentry *,
			struct inode *, struct dentry *);
	int (*readlink) (struct dentry *, char *,int);
	int (*follow_link) (struct dentry *, struct nameidata *);
	//void (*truncate) (struct inode *);
	int (*permission) (struct inode *, int);
	//int (*revalidate) (struct dentry *);
	int (*setattr) (struct dentry *, struct iattr *);
	//int (*getattr) (struct dentry *, struct iattr *);
};

/*
 * File types
 */
#define DT_UNKNOWN	0
#define DT_DIR		4
#define DT_REG		8

struct iovec {
        void *iov_base;
        ssize_t iov_len; 
};

// FIXME: we must provide some translation so that Linux-created jffs2
// partitions can be read on eCos and vice versa

#define S_IFMT 0xF9 // Quick fix for clash on ecos / linux mode bits
#define S_IFDIR __stat_mode_DIR
#define S_IFREG __stat_mode_REG
#define S_IFBLK __stat_mode_BLK
#define S_IFCHR __stat_mode_CHR
#define S_IFLNK 0x101
#define S_IFSOCK 0x102
#define S_IFIFO __stat_mode_FIFO
#define S_IRUGO 0x103
#define S_IXUGO 0x104
#define S_IRWXUGO 0x105


// called by JFFS2

#define to_kdev_t(rdev) (rdev)
#define MAJOR(rdev) (rdev)>>8
#define MINOR(rdev) (rdev)

#define page_address(page)	((page)->virtual)

static __inline__ void * kmap(struct page * page) {
	return page_address(page);
}

#define kunmap(page) do { } while (0)

//struct page * read_cache_page(cyg_uint32 start, void * f, struct inode * i);
struct page *read_cache_page(unsigned long index, int (*filler)(void *,struct page*), void *data);
void page_cache_release(struct page * pg);

struct inode * new_inode(struct super_block *sb);
struct inode * iget(struct super_block *sb, cyg_uint32 ino);
void iput(struct inode * i);
void make_bad_inode(struct inode * inode);
int is_bad_inode(struct inode * inode);

#define insert_inode_hash(inode) do { } while (0)

#define d_alloc_root(root_inode) root_inode

#define flush_dcache_page(page) do { } while (0)

struct jffs2_sb_info;
struct jffs2_eraseblock;

cyg_bool jffs2_flash_read(struct jffs2_sb_info *c, cyg_uint32 read_buffer_offset, const size_t size, size_t * return_size, char * write_buffer);
cyg_bool jffs2_flash_write(struct jffs2_sb_info *c, cyg_uint32 write_buffer_offset, const size_t size, size_t * return_size, char * read_buffer);
int jffs2_flash_writev(struct jffs2_sb_info *c, const struct iovec *vecs, unsigned long count, loff_t to, size_t *retlen);
cyg_bool jffs2_flash_erase(struct jffs2_sb_info *c, struct jffs2_eraseblock *jeb);

static inline void wake_up(wait_queue_head_t *erase_wait)
{ /* Only used for waking up threads blocks on erases. Not used in eCos */ }

// calls to JFFS2

// dir-ecos.c
struct inode *jffs2_lookup(struct inode *dir_i, struct qstr *name);
int jffs2_readdir (struct inode *d_inode, unsigned long f_pos, char *nbuf, int nlen);
int jffs2_create(struct inode *dir_i, struct qstr *d_name, int mode, struct inode **new_i);
int jffs2_mkdir (struct inode *dir_i, struct qstr *d_name, int mode, struct inode **new_i);
int jffs2_link (struct inode *old_d_inode, struct inode *dir_i, struct qstr *d_name);
int jffs2_unlink(struct inode *dir_i, struct inode *d_inode, struct qstr *d_name);
int jffs2_rmdir (struct inode *dir_i, struct inode *d_inode, struct qstr *d_name);
int jffs2_rename (struct inode *old_dir_i, struct inode *d_inode, struct qstr *old_d_name,
                        struct inode *new_dir_i, struct qstr *new_d_name);

#define init_name_hash()		0
static inline unsigned long partial_name_hash(unsigned long c, unsigned long prevhash)
{
	prevhash = (prevhash << 4) | (prevhash >> (8*sizeof(unsigned long)-4));
	return prevhash ^ c;
}

static inline unsigned long end_name_hash(unsigned long hash)
{
	if (sizeof(hash) > sizeof(unsigned int))
		hash += hash >> 4*sizeof(hash);
	return (unsigned int) hash;
}

static inline unsigned int full_name_hash(const unsigned char * name, unsigned int len) {

	unsigned long hash = init_name_hash();
	while (len--)
		hash = partial_name_hash(*name++, hash);
	return end_name_hash(hash);
}

#endif /* __LINUX_JFFS2PORT_H__ */










