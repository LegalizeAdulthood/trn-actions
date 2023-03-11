/* rt-wumpus.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */

EXTERN_C_BEGIN

void init_tree _((void));
ARTICLE* get_tree_artp _((int,int));
int tree_puts _((char*,ART_LINE,int));
int finish_tree _((ART_LINE));
void entire_tree _((ARTICLE*));
char thread_letter _((ARTICLE*));

EXTERN_C_END
