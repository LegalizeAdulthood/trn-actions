/* This file Copyright 1992,1995 by Clifford A. Adams */
/* sadesc.h
 *
 */

const char *sa_get_statchars(long a, int line);
const char *sa_desc_subject(long e);
const char *sa_get_desc(long e, int line, bool trunc);
int sa_ent_lines(long);
