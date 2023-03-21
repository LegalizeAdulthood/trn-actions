/* last.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */
#ifndef LAST_H
#define LAST_H

extern char *g_lastngname; /* last newsgroup read */
extern long g_lasttime;    /* time last we ran */
extern long g_lastactsiz;  /* last known size of active file */
extern long g_lastnewtime; /* time of last newgroup request */
extern long g_lastextranum;

void last_init();
void readlast();
void writelast();

#endif
