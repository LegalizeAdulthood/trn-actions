/* rt-page.h
*/
/* This software is copyrighted as detailed in the LICENSE file. */
#ifndef TRN_RT_PAGE_H
#define TRN_RT_PAGE_H

#include "opt.h"
#include "rt-select.h"

enum : bool
{
    PRESERVE_PAGE = false,
    FILL_LAST_PAGE = true
};

struct ADDGROUP;
struct ARTICLE;
struct MULTIRC;
struct NGDATA;
struct SUBJECT;
struct UNIV_ITEM;

enum
{
    MAX_SEL = 99
};

union SEL_UNION
{
    ARTICLE     *ap;
    SUBJECT     *sp;
    ADDGROUP    *gp;
    MULTIRC     *mp;
    NGDATA      *np;
    UNIV_ITEM   *un;
    option_index op;
};

struct SEL_ITEM
{
    SEL_UNION u;
    int line;
    int sel;
};

extern SEL_ITEM g_sel_items[MAX_SEL];
extern int g_sel_total_obj_cnt;
extern int g_sel_prior_obj_cnt;
extern int g_sel_page_obj_cnt;
extern int g_sel_page_item_cnt;
extern ARTICLE **g_sel_page_app;
extern ARTICLE **g_sel_next_app;
extern ARTICLE *g_sel_last_ap;
extern SUBJECT *g_sel_page_sp;
extern SUBJECT *g_sel_next_sp;
extern SUBJECT *g_sel_last_sp;
extern char *g_sel_grp_dmode;
extern char *g_sel_art_dmode;

bool set_sel_mode(char_int ch);
char *get_sel_order(sel_mode smode);
bool set_sel_order(sel_mode smode, const char *str);
bool set_sel_sort(sel_mode smode, char_int ch);
void set_selector(sel_mode smode, sel_sort_mode ssort);
void init_pages(bool fill_last_page);
bool first_page();
bool last_page();
bool next_page();
bool prev_page();
bool calc_page(SEL_UNION u);
void display_page_title(bool home_only);
void display_page();
void update_page();
void output_sel(int ix, int sel, bool update);
void display_option(int op, int item_index);

#endif
