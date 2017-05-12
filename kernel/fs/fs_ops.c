/* This file use for NCTU OSDI course */
/* It's contants fat file system operators */

#include <inc/stdio.h>
#include <fs.h>
#include <fat/ff.h>
#include <diskio.h>

extern struct fs_dev fat_fs;

/*TODO: Lab7, fat level file operator.
 *       Implement below functions to support basic file system operators by using the elmfat's API(f_xxx).
 *       Reference: http://elm-chan.org/fsw/ff/00index_e.html (or under doc directory (doc/00index_e.html))
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
 *        ┌──────────────┐
 *        │  file_open   │  VFS level file API
 *        └──────────────┘
 *               ↓
 *        ╔══════════════╗
 *   ==>  ║   fat_open   ║  fat level file operator
 *        ╚══════════════╝
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

/* Note: 1. Get FATFS object from fs->data
*        2. Check fs->path parameter then call f_mount.
*/
int fat_mount(struct fs_dev *fs, const void* data)
{
	if(!fs->path)
	{
		printk("mount path is NULL\n");
	}
	int res = f_mount((fs->data), (fs->path), 1);
	return -res;
}

/* Note: Just call f_mkfs at root path '/' */
int fat_mkfs(const char* device_name)
{
	return f_mkfs("/", 0, 0);
}

/* Note: Convert the POSIX's open flag to elmfat's flag.
*        Example: if file->flags == O_RDONLY then open_mode = FA_READ
*                 if file->flags & O_APPEND then f_seek the file to end after f_open
*/
int fat_open(struct fs_fd* file)
{
    unsigned char open_mode = 0;
    uint32_t flag = file->flags;
    FIL *object;
    if (flag == O_RDONLY) 
	{
		open_mode |= FA_READ;
	}
    if (flag & O_WRONLY)
	{
		open_mode |= FA_WRITE;
	}
    if (flag & O_RDWR)
	{
		open_mode |= (FA_READ | FA_WRITE);
	}
    if (flag & O_ACCMODE)
	{
		open_mode &= 0x3;//get LSB 2 bits
	}
    if (flag & O_CREAT)
    {
        if (!(flag & O_TRUNC))
            open_mode |= FA_CREATE_NEW;
    }
    if (flag & O_EXCL) 
	{
		open_mode |= 0;//atomic operation,do nothing
	}
    if (flag & O_TRUNC) 
	{
		open_mode |= FA_CREATE_ALWAYS;
	}

	if(!file->path)
	{
		printk("mount path is NULL\n");
	}
    int ret = f_open(file->data, file->path, open_mode);
    if (flag & O_APPEND)
    {
		object = file->data;
        int size = object->obj.objsize;
        fat_lseek(file, size);
    }
    return -ret;
}

int fat_close(struct fs_fd* file)
{
    int ret = f_close(file->data);
    return -ret;
}

int fat_read(struct fs_fd* file, void* buf, size_t count)
{
    FIL* data = file->data;
    UINT bw;
    int ret_val = -1;
	printk("[FAT_READ] START pos=%d, count=%d\n",file->pos,count);
    ret_val = f_read(data, buf, count, &bw);
    if (ret_val != 0)
        return -ret_val;

    file->pos += bw;
	printk("[FAT_READ] END pos=%d, count=%d\n",file->pos,count);
    return bw;
}

int fat_write(struct fs_fd* file, const void* buf, size_t count)
{
    FIL* data = file->data;
    UINT bw; 
    int ret_val = -1; 
	printk("[FAT_WRITE] START  pos=%d, count=%d\n",file->pos,count);
    ret_val = f_write(data, buf, count, &bw);

    if (ret_val != 0)
	{
		printk("[FAT_WRITE] failed\n");
        return -ret_val;
	}
    
    int next_pos = file->pos + bw; 
    int size_offset = 0;
    if (next_pos > file->size) 
        size_offset = next_pos - file->size;
    file->size += size_offset;   // update size of file
    file->pos = next_pos;
    
	printk("[FAT_WRITE] END  pos=%d, count=%d\n",file->pos,count);
    return bw; 
}

int fat_lseek(struct fs_fd* file, off_t offset)
{
    int ret = f_lseek(file->data, offset);
	if(ret != 0)
	{
		return -ret;
	}
    return 0;
}

int fat_unlink( const char *pathname)
{
    int ret = f_unlink(pathname);
	if(ret != 0)
	{
		return -ret;
	}
    return 0;
}

struct fs_ops elmfat_ops = {
    .dev_name = "elmfat",
    .mount = fat_mount,
    .mkfs = fat_mkfs,
    .open = fat_open,
    .close = fat_close,
    .read = fat_read,
    .write = fat_write,
    .lseek = fat_lseek,
    .unlink = fat_unlink
};



