/* rt-wumpus.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */

void init_tree();
ARTICLE *get_tree_artp(int x, int y);
int tree_puts(char *, ART_LINE, int);
int finish_tree(ART_LINE);
void entire_tree(ARTICLE *ap);
char thread_letter(ARTICLE *ap);
