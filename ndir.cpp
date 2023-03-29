/* ndir.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include "common.h"
#include "ndir.h"

#ifdef EMULATE_NDIR
#include <io.h>

/*
 * support for Berkeley directory reading routine on a V7 file system
 */

/*
 * open a directory.
 */
DIR *opendir(const char *name)
{
    int fd = open(name, 0);

    if (fd == -1)
        return nullptr;
    DIR *dirp = (DIR *)malloc(sizeof(DIR));
    if (dirp == nullptr)
    {
        close(fd);
        return nullptr;
    }
    dirp->dd_fd = fd;
    dirp->dd_loc = 0;
    return dirp;
}

/*
 * read an old style directory entry and present it as a new one
 */
#ifndef pyr
#define	ODIRSIZ	14

struct	olddirect {
	ino_t	od_ino;
	char	od_name[ODIRSIZ];
};
#else	/* an Pyramid in the ATT universe */
#define	ODIRSIZ	248

struct	olddirect {
	long	od_ino;
	short	od_fill1, od_fill2;
	char	od_name[ODIRSIZ];
};
#endif

/*
 * get next entry in a directory.
 */
Direntry_t *readdir(DIR *dirp)
{
    static Direntry_t dir;

	for (;;) {
		if (dirp->dd_loc == 0) {
			dirp->dd_size = read(dirp->dd_fd, dirp->dd_buf,
			    DIRBLKSIZ);
			if (dirp->dd_size <= 0)
				return nullptr;
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = 0;
			continue;
		}
		struct olddirect *dp = (struct olddirect*)(dirp->dd_buf + dirp->dd_loc);
		dirp->dd_loc += sizeof(struct olddirect);
		if (dp->od_ino == 0)
			continue;
		dir.d_ino = dp->od_ino;
		strncpy(dir.d_name, dp->od_name, ODIRSIZ);
		dir.d_name[ODIRSIZ] = '\0'; /* insure null termination */
		dir.d_namlen = strlen(dir.d_name);
		dir.d_reclen = DIRSIZ(&dir);
		return &dir;
	}
}

/*
 * close a directory.
 */
void closedir(DIR *dirp)
{
	close(dirp->dd_fd);
	dirp->dd_fd = -1;
	dirp->dd_loc = 0;
	free(dirp);
}

#endif /* EMULATE_NDIR */
