/* config2.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */
#ifndef TRN_CONFIG2_H
#define TRN_CONFIG2_H

#ifdef HAS_GETPWENT
#   include <pwd.h>
#endif

#ifdef I_UNISTD
#   include <unistd.h>
#endif

#include <stdlib.h>

#ifdef USE_DEBUGGING_MALLOC
#   include "malloc.h"
#   define safemalloc malloc
#   define saferealloc realloc
#endif

#include <string.h>

#ifndef S_ISDIR
#   define S_ISDIR(m)  ( ((m) & S_IFMT) == S_IFDIR )
#endif
#ifndef S_ISCHR
#   define S_ISCHR(m)  ( ((m) & S_IFMT) == S_IFCHR )
#endif
#ifndef S_ISREG
#   define S_ISREG(m)  ( ((m) & S_IFMT) == S_IFREG )
#endif

#ifdef MSDOS
#include "msdos.h"
#endif

/* some handy defs */

using char_int = int;

#define Ctl(ch) (ch & 037)

#ifndef FILE_REF
inline bool file_ref(const char *s)
{
    return s[0] == '/';
}
#   define FILE_REF(s) file_ref(s)
#endif

/* how to open binary format files */
#ifndef FOPEN_RB
#   define FOPEN_RB "r"
#endif
#ifndef FOPEN_WB
#   define FOPEN_WB "w"
#endif

#endif
