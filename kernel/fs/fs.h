#ifndef K_FS_H
#define K_FS_H
#include <inc/types.h>

#define FS_FD_MAX 10

/* Mounted file system */
struct fs_dev
{
	uint32_t dev_id;		/* Attached device */

	char  path[32];				/* File system mount point */
	const struct fs_ops* ops;	/* Operations for file system type */

	void *data;				/* Specific file system data */
};

/* file descriptor */
struct fs_fd
{
    char path[64];					/* Name (below mount point) */
    int type;					/* Type (regular or socket) */
    int ref_count;				/* Descriptor reference count */

    struct fs_dev* fs;	/* Resident file system */

    uint32_t flags;			/* Descriptor flags */
    size_t 	size;			/* Size in bytes */
    off_t  	pos;			/* Current file position */

    void *data;					/* Specific file system data */
};

/* It's low level disk operators */
struct fs_ops
{
    char *dev_name;
    /* mount and unmount file system */
    int (*mount)	(struct fs_dev* fs, const void* data);
    //int (*unmount)	(struct fs_dev* fs);

    /* Volume Management */
    int (*mkfs)     (const char* device_name);
    //int (*statfs)   (struct fs_dev* fs, struct statfs *buf);

    /* File operators */
    int (*open)		(struct fs_fd* fd);
    int (*close)	(struct fs_fd* fd);
    int (*ioctl)	(struct fs_fd* fd, int cmd, void *args);
    int (*read)		(struct fs_fd* fd, void* buf, size_t count);
    int (*write)	(struct fs_fd* fd, const void* buf, size_t count);
    //int (*flush)    (struct fs_fd* fd);
    int (*lseek)	(struct fs_fd* fd, off_t offset);
    
    //int (*getdents)	(struct fs_fd* fd, struct dirent* dirp, uint32_t count);
    int (*unlink)	(struct fs_fd* fs, const char* pathname);

};


int fs_init();
int fs_mount(const char* device_name, const char* path, const void* data);

int file_open(struct fs_fd* fd, const char *path, int flags);
int file_close(struct fs_fd* fd);
int file_read(struct fs_fd* fd, void *buf, size_t len);
int file_write(struct fs_fd* fd, const void *buf, size_t len);

int file_lseek(struct fs_fd* fd, off_t offset);
int file_unlink(const char *path);

struct fs_fd* fd_get(int fd);
void fd_put(struct fs_fd* fd);
int fd_new(void);

#endif
