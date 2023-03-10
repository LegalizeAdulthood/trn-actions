/* ndir.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include "EXTERN.h"
#include "common.h"
#include "INTERN.h"
#include "ndir.h"

#ifdef EMULATE_NDIR
#include <io.h>

/*
 * support for Berkeley directory reading routine on a V7 file system
 */

/*
 * open a directory.
 */
DIR*
opendir(name)
char* name;
{
	register DIR* dirp;
	register int fd;

	if ((fd = open(name, 0)) == -1)
		return NULL;
	if ((dirp = (DIR*)malloc(sizeof(DIR))) == NULL) {
		close (fd);
		return NULL;
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
Direntry_t*
readdir(dirp)
register DIR* dirp;
{
	register struct olddirect* dp;
	static Direntry_t dir;

	for (;;) {
		if (dirp->dd_loc == 0) {
			dirp->dd_size = read(dirp->dd_fd, dirp->dd_buf,
			    DIRBLKSIZ);
			if (dirp->dd_size <= 0)
				return NULL;
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = 0;
			continue;
		}
		dp = (struct olddirect*)(dirp->dd_buf + dirp->dd_loc);
		dirp->dd_loc += sizeof(struct olddirect);
		if (dp->od_ino == 0)
			continue;
		dir.d_ino = dp->od_ino;
		strncpy(dir.d_name, dp->od_name, ODIRSIZ);
		dir.d_name[ODIRSIZ] = '\0'; /* insure null termination */
		dir.d_namlen = strlen(dir.d_name);
		dir.d_reclen = DIRSIZ(&dir);
		return (&dir);
	}
}

/*
 * close a directory.
 */
void
closedir(dirp)
register DIR* dirp;
{
	close(dirp->dd_fd);
	dirp->dd_fd = -1;
	dirp->dd_loc = 0;
	free(dirp);
}

#endif /* EMULATE_NDIR */
