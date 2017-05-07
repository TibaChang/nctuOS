
#ifndef FS_H
#define FS_H
#include <inc/types.h>



#ifndef DFS_PATH_MAX
#define DFS_PATH_MAX 			256
#endif


struct dirent
{
	uint8_t d_type;				/* The type of the file */
	uint8_t d_namlen;			/* The length of the not including the terminating null file name */
	uint16_t d_reclen;			/* length of this record */
	char d_name[DFS_PATH_MAX];		/* The null-terminated file name */
};

int getdents(unsigned int fd, struct dirent *dirp, unsigned int count);
#endif
