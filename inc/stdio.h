#ifndef JOS_INC_STDIO_H
#define JOS_INC_STDIO_H

#include <inc/stdarg.h>
#include <inc/types.h>
#include <inc/fs.h>

#ifndef NULL
#define NULL	((void *) 0)
#endif /* !NULL */


#ifdef USE_BUILDIN
#include <stdio.h>
#else
/* Device error codes */
#define STATUS_OK			0		/* no error */
#define STATUS_ENOENT		2		/* No such file or directory */
#define STATUS_EIO		 	5		/* I/O error */
#define STATUS_ENXIO		6		/* No such device or address */
#define STATUS_EBADF		9		/* Bad file number */
#define STATUS_EAGIAN		11		/* Try again */
#define STATUS_ENOMEM		12		/* no memory */
#define STATUS_EBUSY		16		/* Device or resource busy */
#define STATUS_EEXIST		17		/* File exists */
#define STATUS_EXDEV		18		/* Cross-device link */
#define STATUS_ENODEV		19		/* No such device */
#define STATUS_ENOTDIR		20		/* Not a directory */
#define STATUS_EISDIR		21		/* Is a directory */
#define STATUS_EINVAL		22		/* Invalid argument */
#define STATUS_ENOSPC		28		/* No space left on device */
#define STATUS_EROFS		30		/* Read-only file system */
#define STATUS_ENOSYS		38		/* Function not implemented */
#define STATUS_ENOTEMPTY	39		/* Directory not empty */

/* Operation flags */
#define O_RDONLY		0x0000000
#define O_WRONLY		0x0000001
#define O_RDWR			0x0000002
#define O_ACCMODE		0x0000003
#define O_CREAT			0x0000100
#define O_EXCL			0x0000200
#define O_TRUNC			0x0001000
#define O_APPEND		0x0002000
#define O_DIRECTORY		0x0200000

#define SEEK_SET         0
#define SEEK_CUR         1
#define SEEK_END         2

/* File flags */
#define F_OPEN			0x01000000
#define F_DIRECTORY		0x02000000
#define F_EOF			0x04000000
#define F_ERR			0x08000000

/* Stat codes */
#define S_IFMT		00170000
#define S_IFSOCK	0140000
#define S_IFLNK		0120000
#define S_IFREG		0100000
#define S_IFBLK		0060000
#define S_IFDIR  	0040000
#define S_IFCHR  	0020000
#define S_IFIFO  	0010000
#define S_ISUID  	0004000
#define S_ISGID  	0002000
#define S_ISVTX  	0001000

#endif

//lib/kbd.c
int k_getc(void);

//lib/screen.c
void	k_puts(unsigned char *text);
void	k_putch(unsigned char c);

//kernel/printf.c
void	k_puts(unsigned char *text);
int	printk(const char *fmt, ...);

// lib/printfmt.c
void	printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);
void	vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list);
int	snprintf(char *str, int size, const char *fmt, ...);
int	vsnprintf(char *str, int size, const char *fmt, va_list);

// lib/printf.c
int	cprintf(const char *fmt, ...);
int	vcprintf(const char *fmt, va_list);

// lib/readline.c
char *readline(const char *prompt);

// lib/console.c, use it in user space 
void cputchar(int ch);
int getchar(void);


// Below is stardand file APIs
// lib/syscall.c
/* file api*/
int open(const char *file, int flags, int mode);
int close(int d);
int read(int fd, void *buf, size_t len);
int write(int fd, const void *buf, size_t len);

off_t lseek(int fd, off_t offset, int whence);

int unlink(const char *pathname);

#endif /* !JOS_INC_STDIO_H */
