/* This file use for NCTU OSDI course */
/* It's contants file operator's wapper API */
#include <fs.h>
#include <fat/ff.h>
#include <inc/string.h>
#include <inc/stdio.h>

/* Static file objects */
FIL file_objs[FS_FD_MAX];

/* Static file system object */
FATFS fat;

/* It file object table */
struct fs_fd fd_table[FS_FD_MAX];

/* File system operator, define in fs_ops.c */
extern struct fs_ops elmfat_ops; //We use only one file system...

/* File system object, it record the operator and file system object(FATFS) */
struct fs_dev fat_fs = {
    .dev_id = 1, //In this lab we only use second IDE disk
    .path = {0}, // Not yet mount to any path
    .ops = &elmfat_ops,
    .data = &fat
};

int mapposix(int ret)
{
    int r = -ret;
    int retVal;

    switch(r)
    {
        case FR_OK:
            retVal = STATUS_OK;
            break;
        case FR_DISK_ERR:
        case FR_NOT_READY:
            retVal = STATUS_EIO;
            break;
        case FR_INT_ERR:
        case FR_INVALID_DRIVE:
            retVal = STATUS_EBADF;
            break;
        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_INVALID_NAME:
            retVal = STATUS_ENOENT;
            break;
        case FR_DENIED:
        case FR_EXIST:
            retVal = STATUS_EEXIST;
            break;
        case FR_INVALID_OBJECT:
            retVal = STATUS_ENXIO;
            break;
        case FR_WRITE_PROTECTED:
            retVal = STATUS_EROFS;
            break;
        case FR_NOT_ENABLED:
            retVal = STATUS_ENOSPC;
            break;
        case FR_NO_FILESYSTEM:
            retVal = STATUS_ENODEV;
            break;
        case FR_MKFS_ABORTED:
        case FR_INVALID_PARAMETER:
            retVal = STATUS_EINVAL;
            break;
        case FR_TIMEOUT:
        case FR_LOCKED:
        case FR_NOT_ENOUGH_CORE:
        case FR_TOO_MANY_OPEN_FILES:
            retVal = STATUS_EBUSY;
            break;
    }
    return -retVal;
}
   
/*TODO: Lab7, VFS level file API.
 *  This is a virtualize layer. Please use the function pointer
 *  under struct fs_ops to call next level functions.
 *
 *  Call flow example:
 *        ┌──────────────┐
 *        │     open     │
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │   sys_open   │  file I/O system call interface
 *        └──────────────┘
 *               ↓
 *        ╔══════════════╗
 *   ==>  ║  file_open   ║  VFS level file API
 *        ╚══════════════╝
 *               ↓
 *        ┌──────────────┐
 *        │   fat_open   │  fat level file operator
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │    f_open    │  FAT File System Module
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │    diskio    │  low level file operator
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │     disk     │  simple ATA disk dirver
 *        └──────────────┘
 */
int fs_init()
{
    int res, i;
    
    /* Initial fd_tables */
    for (i = 0; i < FS_FD_MAX; i++)
    {
        fd_table[i].flags = 0;
        fd_table[i].size = 0;
        fd_table[i].pos = 0;
        fd_table[i].type = 0;
        fd_table[i].ref_count = 0;
        fd_table[i].data = &file_objs[i];
        fd_table[i].fs = &fat_fs;
    }
    
    /* Mount fat file system at "/" */
    /* Check need mkfs or not */
    if ((res = fs_mount("elmfat", "/", NULL)) != 0)
    {
        fat_fs.ops->mkfs("elmfat");
        res = fs_mount("elmfat", "/", NULL);
        return res;
    }
    return -STATUS_EIO;
       
}

/** Mount a file system by path 
*  Note: You need compare the device_name with fat_fs.ops->dev_name and find the file system operator
*        then call ops->mount().
*
*  @param data: If you have mutilple file system it can be use for pass the file system object pointer save in fat_fs->data. 
*/
int fs_mount(const char* device_name, const char* path, const void* data)
{
    if (strcmp(device_name,"elmfat")==0)
	{
    	int ret;
    	strcpy(fat_fs.path,path);
    	ret = fat_fs.ops->mount(&fat_fs, data);
    	return mapposix(ret);
    }   
    return -STATUS_EIO;
} 

/* Note: Before call ops->open() you may copy the path and flags parameters into fd object structure */
int file_open(struct fs_fd* fd, const char *path, int flags)
{
    fd->flags = flags;
    int ret = fat_fs.ops->open(fd);
    return mapposix(ret);
}

int file_read(struct fs_fd* fd, void *buf, size_t len)
{
	int ret = fat_fs.ops->read(fd, buf, len);
    if (ret < 0)
	{
		return mapposix(ret);
	}else
	{
		return ret;
	}
}

int file_write(struct fs_fd* fd, const void *buf, size_t len)
{
    int ret = fat_fs.ops->write(fd, buf, len);
    if (ret < 0) 
	{	
		return mapposix(ret);
	}else
	{
		return ret;
	}
}

int file_close(struct fs_fd* fd)
{
    int ret = fat_fs.ops->close(fd);
    return mapposix(ret);
}

int file_lseek(struct fs_fd* fd, off_t offset)
{
    int ret = fat_fs.ops->lseek(fd, offset);
    return mapposix(ret);
}

int file_unlink( const char *path)
{
	int i;
  	int ret = fat_fs.ops->unlink(path);
	if(ret == 0)
	{
		for (i = 0; i < FS_FD_MAX; ++i)
		{
            struct fs_fd * cur_fd = &fd_table[i];
            if (strcmp(cur_fd->path, path) == 0) 
			{
                strcpy(cur_fd->path, "");
                cur_fd->ref_count = 0;
                break;
            } 
		}
	}
    return mapposix(ret);
}


/**
 * @ingroup Fd
 * This function will allocate a file descriptor.
 *
 * @return -1 on failed or the allocated file descriptor.
 */
int fd_new(void)
{
	struct fs_fd* d;
	int idx;

	/* find an empty fd entry */
	for (idx = 0; idx < FS_FD_MAX && fd_table[idx].ref_count > 0; idx++);


	/* can't find an empty fd entry */
	if (idx == FS_FD_MAX)
	{
		idx = -1;
		goto __result;
	}

	d = &(fd_table[idx]);
	d->ref_count = 1;

__result:
	return idx;
}

/**
 * @ingroup Fd
 *
 * This function will return a file descriptor structure according to file
 * descriptor.
 *
 * @return NULL on on this file descriptor or the file descriptor structure
 * pointer.
 */
struct fs_fd* fd_get(int fd)
{
	struct fs_fd* d;

	if ( fd < 0 || fd > FS_FD_MAX ) return NULL;

	d = &fd_table[fd];

	/* increase the reference count */
	d->ref_count ++;

	return d;
}

/**
 * @ingroup Fd
 *
 * This function will put the file descriptor.
 */
void fd_put(struct fs_fd* fd)
{

	fd->ref_count --;

	/* clear this fd entry */
	if ( fd->ref_count == 0 )
	{
		//memset(fd, 0, sizeof(struct fs_fd));
		memset(fd->data, 0, sizeof(FIL));
	}
};


