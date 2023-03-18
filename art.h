/* art.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

/* do_article() return values */

enum
{
    DA_NORM = 0,
    DA_RAISE = 1,
    DA_CLEAN = 2,
    DA_TOEND = 3
};

EXT ART_LINE highlight INIT(-1);/* next line to be highlighted */
EXT ART_LINE first_view;
EXT ART_POS raw_artsize;	/* size in bytes of raw article */
EXT ART_POS artsize;		/* size in bytes of article */
EXT char art_line[LBUFLEN];	/* place for article lines */

EXT int gline INIT(0);
EXT ART_POS innersearch INIT(0); /* artpos of end of line we want to visit */
EXT ART_LINE innerlight INIT(0); /* highlight position for innersearch or 0 */
EXT char hide_everything INIT(0);/* if set, do not write page now, */
				 /* ...but execute char when done with page */

void art_init();
int do_article();
bool maybe_set_color(char *cp, bool backsearch);
int page_switch();
bool innermore();
void pager_mouse(int, int, int, int, int, int);
