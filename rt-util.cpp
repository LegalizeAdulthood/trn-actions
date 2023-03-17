/* rt-util.c
*  vi: set sw=4 ts=8 ai sm noet :
*/
/* This software is copyrighted as detailed in the LICENSE file. */


#include "EXTERN.h"
#include "common.h"
#include "list.h"
#include "hash.h"
#include "cache.h"
#include "ngdata.h"
#include "artio.h"
#include "rthread.h"
#include "rt-select.h"
#include "term.h"
#include "nntpclient.h"
#include "charsubst.h"
#include "datasrc.h"
#include "nntp.h"
#include "intrp.h"
#include "ng.h"
#include "util.h"
#include "util2.h"
#include "utf.h"
#include "INTERN.h"
#include "rt-util.h"
#include "rt-util.ih"

/* Name-munging routines written by Ross Ridge.
** Enhanced by Wayne Davison.
*/

/* Extract the full-name part of an email address, returning nullptr if not
** found.
*/
char *extract_name(char *name)
{
    char* s;
    char* lparen;
    char* rparen;
    char* langle;

    while (isspace(*name)) name++;

    lparen = index(name, '(');
    rparen = rindex(name, ')');
    langle = index(name, '<');
    if (!lparen && !langle)
	return nullptr;
    else if (langle && (!lparen || !rparen || lparen>langle || rparen<langle)) {
	if (langle == name)
	    return nullptr;
	*langle = '\0';
    } else {
	name = lparen;
	*name++ = '\0';
	while (isspace(*name)) name++;
	if (name == rparen)
	    return nullptr;
	if (rparen != nullptr)
	    *rparen = '\0';
    }

    if (*name == '"') {
	name++;
	while (isspace(*name)) name++;
	if ((s = rindex(name, '"')) != nullptr)
	    *s = '\0';
    }
    return name;
}

/* If necessary, compress a net user's full name by playing games with
** initials and the middle name(s).  If we start with "Ross Douglas Ridge"
** we try "Ross D Ridge", "Ross Ridge", "R D Ridge" and finally "R Ridge"
** before simply truncating the thing.  We also turn "R. Douglas Ridge"
** into "Douglas Ridge" and "Ross Ridge D.D.S." into "Ross Ridge" as a
** first step of the compaction, if needed.
*/
char *compress_name(char *name, int max)
{
    register char* s;
    register char* last;
    register char* mid;
    register char* d;
    register int len, namelen, midlen;
#ifdef USE_UTF_HACK
    int vis_len, vis_namelen, vis_midlen;
#else
#define vis_len len
#define vis_namelen namelen
#define vis_midlen midlen
#endif
    int notlast;

try_again:
    /* First remove white space from both ends. */
    while (isspace(*name)) name++;
    if ((len = strlen(name)) == 0)
	return name;
    s = name + len - 1;
    while (isspace(*s)) s--;
    s[1] = '\0';
#ifdef USE_UTF_HACK
    vis_len = visual_length_between(s, name) + 1;
#else
    vis_len = s - name + 1;
#endif
    if (vis_len <= max)
	return name;

    /* Look for characters that likely mean the end of the name
    ** and the start of some hopefully uninteresting additional info.
    ** Spliting at a comma is somewhat questionalble, but since
    ** "Ross Ridge, The Great HTMU" comes up much more often than 
    ** "Ridge, Ross" and since "R HTMU" is worse than "Ridge" we do
    ** it anyways.
    */
#ifdef USE_UTF_HACK
    d = name + byte_length_at(name);
#else
    d = name + 1;
#endif
    for (d = name ; ; ) {
#ifdef USE_UTF_HACK
	int w = byte_length_at(d);
	int x = byte_length_at(d + w);
#else
	int w = 1;
	int x = 2;
#endif
	char ch = d[w];
	char next = d[x];
    if (!ch) break;
	if (ch == ',' || ch == ';' || ch == '(' || ch == '@'
	 || (ch == '-' && (next == '-' || next == ' '))) {
	    *d = '\0';
	    s = d;
	    break;
	}
	d += w;
    }

    /* Find the last name */
    do {
	notlast = 0;
	while (isspace(*s)) s--;
	s[1] = '\0';
	len = s - name + 1;
#ifdef USE_UTF_HACK
	vis_len = visual_length_between(s, name) + 1;
#endif
	if (vis_len <= max)
	    return name;
	/* If the last name is an abbreviation it's not the one we want. */
	if (*s == '.')
	    notlast = 1;
	while (!isspace(*s)) {
	    if (s == name) {	/* only one name */
#ifdef USE_UTF_HACK
		terminate_string_at_visual_index(name, max);
#else
		name[max] = '\0';
#endif
		return name;
	    }
	    if (isdigit(*s))	/* probably a phone number */
		notlast = 1;	/* so chuck it */
	    s--;
	}
    } while (notlast);

    last = s-- + 1;

    /* Look for a middle name */
    while (isspace(*s)) {	/* get rid of any extra space */
	len--;	
	s--;
    }
    mid = name;
    while (!isspace(*mid)) {
#ifdef USE_UTF_HACK
	mid += byte_length_at(mid);
#else
	mid++;
#endif
    }
    namelen = mid - name + 1;
#ifdef USE_UTF_HACK
    vis_namelen = visual_length_between(mid, name) + 1;
#endif
    if (mid == s+1) {	/* no middle name */
	mid = 0;
	midlen = 0;
    } else {
	*mid++ = '\0';
	while (isspace(*mid)) {
	    len--;
#ifdef USE_UTF_HACK
	    mid += byte_length_at(mid);
#else
	    mid++;
#endif
	}
	midlen = s - mid + 2;
#ifdef USE_UTF_HACK
	vis_midlen = visual_length_between(s, mid) + 2;
#endif
	/* If first name is an initial and middle isn't and it all fits
	** without the first initial, drop it. */
	if (vis_len > max && mid != s) {
	    if (vis_len - vis_namelen <= max
	     && ((mid[1] != '.' && (!name[1] || (name[1] == '.' && !name[2])))
	      || (*mid == '"' && *s == '"'))) {
		len -= namelen;
		name = mid;
		namelen = midlen;
#ifdef USE_UTF_HACK
		vis_len = vis_namelen;
		vis_namelen = vis_midlen;
#endif
		mid = 0;
	    }
	    else if (*mid == '"' && *s == '"') {
		if (vis_midlen > max) {
		    name = mid+1;
		    *s = '\0';
		    goto try_again;
		}
		len = midlen;
		last = mid;
		namelen = 0;
		mid = 0;
#ifdef USE_UTF_HACK
		vis_len = vis_midlen;
		vis_namelen = 0;
#endif
	    }
	}
    }
    s[1] = '\0';
    if (mid && vis_len > max) {
	/* Turn middle names into intials */
	len -= s - mid + 2;
#ifdef USE_UTF_HACK
	vis_len -= visual_length_between(s, mid) + 2;
#endif
	d = s = mid;
	while (*s) {
#ifdef USE_UTF_HACK
	    int w;
	    int v;
#endif
	    if (isalpha(*s)) {
		if (d != mid) {
#ifdef USE_UTF_HACK
		    int w = byte_length_at(s);
		    memset(d, ' ', w);
		    d += w;
#else
		    *d++ = ' ';
#endif
		}
#ifdef USE_UTF_HACK
		w = byte_length_at(s);
		bcopy(s, d, w);
		d += w;
		s += w;
#else
		*d++ = *s++;
#endif
	    }
	    while (*s && !isspace(*s)) {
#ifdef USE_UTF_HACK
		s += byte_length_at(s);
#else
		s++;
#endif
	    }
	    while (isspace(*s)) s++;
	}
	if (d != mid) {
	    *d = '\0';
	    midlen = d - mid + 1;
	    len += midlen;
#ifdef USE_UTF_HACK
	    vis_midlen = visual_length_between(d, mid) + 1;
	    vis_len += vis_midlen;
#endif
	} else
	    mid = 0;
    }
    if (vis_len > max) {
	/* If the first name fits without the middle initials, drop them */
	if (mid && vis_len - vis_midlen <= max) {
	    len -= midlen;
#ifdef USE_UTF_HACK
	    vis_len -= vis_midlen;
#endif
	    mid = 0;
	} else if (namelen > 0) {
	    /* Turn the first name into an initial */
#ifdef USE_UTF_HACK
	    int w = byte_length_at(name);
	    len -= namelen - (w + 1);
	    name[w] = '\0';
	    namelen = w + 1;
	    vis_namelen = visual_width_at(name) + 1;
#else
	    len -= namelen - 2;
	    name[1] = '\0';
	    namelen = 2;
#endif
	    if (vis_len > max) {
		/* Dump the middle initials (if present) */
		if (mid) {
		    len -= midlen;
#ifdef USE_UTF_HACK
		    vis_len -= vis_midlen;
#endif
		    mid = 0;
		}
		if (vis_len > max) {
		    /* Finally just truncate the last name */
#ifdef USE_UTF_HACK
		    terminate_string_at_visual_index(last, max - 2);
#else
		    last[max - 2] = '\0';
#endif
		}
	    }
	} else {
	    namelen = 0;
#ifdef USE_UTF_HACK
	    vis_namelen = 0;
#endif
	}
    }

    /* Paste the names back together */
    d = name + namelen;
    if (namelen)
	d[-1] = ' ';
    if (mid) {
	if (d != mid)
	    strcpy(d, mid);
	d += midlen;
	d[-1] = ' ';
    }
#ifdef USE_UTF_HACK
    /* FIXME - need to move into some function */
    do {
	int i;
	int j;
	for (i = j = 0; last[i] && j < max; ) {
	    int w = byte_length_at(last + i);
	    int v = visual_width_at(last + i);
	if (w == 0 || j + v > max) break;
	    bcopy(last+i, d+i, w);
	    i += w;
	    j += v;
	}
	d[i] = '\0';
    } while (0);
#else
    safecpy(d, last, max);	/* "max - (d-name)" would be overkill */
#endif
    return name;
}
#undef vis_len
#undef vis_namelen
#undef vis_midlen

/* Compress an email address, trying to keep as much of the local part of
** the addresses as possible.  The order of precence is @ ! %, but
** @ % ! may be better...
*/
char *compress_address(char *name, int max)
{
    char* s;
    char* at;
    char* bang;
    char* hack;
    char* start;
    int len;

    /* Remove white space from both ends. */
    while (isspace(*name)) name++;
    if ((len = strlen(name)) == 0)
	return name;
    s = name + len - 1;
    while (isspace(*s)) s--;
    s[1] = '\0';
    if (*name == '<') {
	name++;
	if (*s == '>')
	    *s-- = '\0';
    }
    if ((len = s - name + 1) <= max)
	return name;

    at = bang = hack = nullptr;
    for (s = name + 1; *s; s++) {
	/* If there's whitespace in the middle then it's probably not
	** really an email address. */
	if (isspace(*s)) {
	    name[max] = '\0';
	    return name;
	}
	switch (*s) {
	  case '@':
	    if (at == nullptr) {
		at = s;
	    }
	    break;
	  case '!':
	    if (at == nullptr) {
		bang = s;
		hack = nullptr;
	    }
	    break;
	  case '%':
	    if (at == nullptr && hack == nullptr)
		hack = s;
	    break;
	}
    }
    if (at == nullptr)
	at = name + len;

    if (hack != nullptr) {
	if (bang != nullptr) {
	    if (at - bang - 1 >= max)
		start = bang + 1;
	    else if (at - name >= max)
		start = at - max;
	    else
		start = name;
	} else
	    start = name;
    } else if (bang != nullptr) {
	if (at - name >= max)
	    start = at - max;
	else
	    start = name;
    } else
	start = name;
    if (len - (start - name) > max)
	start[max] = '\0';
    return start;
}

/* Fit the author name in <max> chars.  Uses the comment portion if present
** and pads with spaces.
*/
char *compress_from(char *from, int size)
{
    static char lbuf[LBUFLEN];
    char* s = from? from : nullstr;
    int len, vis_len;

#ifdef CHARSUBST
    strcharsubst(lbuf, s, sizeof lbuf, *charsubst);
#else
    safecpy(lbuf, s, sizeof lbuf);
#endif
    if ((s = extract_name(lbuf)) != nullptr)
	s = compress_name(s, size);
    else
	s = compress_address(lbuf, size);
#ifdef USE_UTF_HACK
    len = strlen(s);
    vis_len = visual_length_of(s);
#else
    len = strlen(s);
    vis_len = len;
#endif
    if (!len) {
	strcpy(s,"NO NAME");
	len = 7;
    }
    while (vis_len < size && len < sizeof lbuf - 1) {
	s[len++] = ' ';
	vis_len++;
    }
    s[len] = '\0';
    return s;
}

/* Fit the date in <max> chars. */
char *compress_date(ARTICLE *ap, int size)
{
    char* s;
    char* t;

    strncpy(t = cmd_buf, ctime(&ap->date), size);
    if ((s = index(t, '\n')) != nullptr)
	*s = '\0';
    t[size] = '\0';
    return t;
}

#define EQ(x,y) ((isupper(x) ? tolower(x) : (x)) == (y))

/* Parse the subject to look for any "Re[:^]"s at the start.
** Returns true if a Re was found.  If strp is non-nullptr, it
** will be set to the start of the interesting characters.
*/
bool subject_has_Re(char *str, char **strp)
{
    bool has_Re = 0;

    while (*str && AT_GREY_SPACE(str)) str++;
    while (EQ(str[0], 'r') && EQ(str[1], 'e')) {	/* check for Re: */
      register char* cp = str + 2;
	if (*cp == '^') {				/* allow Re^2: */
	    while (*++cp <= '9' && *cp >= '0')
		;
	}
	if (*cp != ':')
	    break;
	while (*++cp && AT_GREY_SPACE(cp)) ;
	str = cp;
	has_Re = 1;
    }
    if (strp)
	*strp = str;
    return has_Re;
}

/* Output a subject in <max> chars.  Does intelligent trimming that tries to
** save the last two words on the line, excluding "(was: blah)" if needed.
*/
char *compress_subj(ARTICLE *ap, int max)
{
    register char* cp;
    register int len;
    ARTICLE* first;

    if (!ap)
	return "<MISSING>";

    /* Put a preceeding '>' on subjects that are replies to other articles */
    cp = buf;
    first = (ThreadedGroup? ap->subj->thread : ap->subj->articles);
    if (ap != first || (ap->flags & AF_HAS_RE)
     || (!(ap->flags&AF_UNREAD) ^ sel_rereading))
	*cp++ = '>';
#ifdef CHARSUBST
    strcharsubst(cp, ap->subj->str + 4, (sizeof buf) - (cp-buf), *charsubst);
#else
    safecpy(cp, ap->subj->str + 4, (sizeof buf) - (cp-buf));
#endif

    /* Remove "(was: oldsubject)", because we already know the old subjects.
    ** Also match "(Re: oldsubject)".  Allow possible spaces after the ('s.
    */
    for (cp = buf; (cp = index(cp+1, '(')) != nullptr;) {
	while (*++cp == ' ') ;
	if (EQ(cp[0], 'w') && EQ(cp[1], 'a') && EQ(cp[2], 's')
	 && (cp[3] == ':' || cp[3] == ' ')) {
	    *--cp = '\0';
	    break;
	}
	if (EQ(cp[0], 'r') && EQ(cp[1], 'e')
	 && ((cp[2]==':' && cp[3]==' ') || (cp[2]=='^' && cp[4]==':'))) {
	    *--cp = '\0';
	    break;
	}
    }
    len = strlen(buf);
    if (!unbroken_subjects && len > max) {
	char* last_word;
	/* Try to include the last two words on the line while trimming */ 
	if ((last_word = rindex(buf, ' ')) != nullptr) {
	    char* next_to_last;
	    *last_word = '\0';
	    if ((next_to_last = rindex(buf, ' ')) != nullptr) {
		if (next_to_last-buf >= len - max + 3 + 10-1)
		    cp = next_to_last;
		else
		    cp = last_word;
	    } else
		cp = last_word;
	    *last_word = ' ';
	    if (cp-buf >= len - max + 3 + 10-1) {
		char* s = buf + max - (len-(cp-buf)+3);
		*s++ = '.'; *s++ = '.'; *s++ = '.';
		safecpy(s, cp + 1, max);
		len = max;
	    }
	}
    }
    if (len > max)
	buf[max] = '\0';
    return buf;
}

/* Modified version of a spinner originally found in Clifford Adams' strn. */

static char *spinchars;
static int spin_level INIT(0);	/* used to allow non-interfering nested spins */
static int spin_mode;
static int spin_place;		/* represents place in spinchars array */
static int spin_pos;		/* the last spinbar position we drew */
static ART_NUM spin_art;
static ART_POS spin_tell;

void setspin(int mode)
{
    switch (mode) {
      case SPIN_FOREGROUND:
      case SPIN_BACKGROUND:
      case SPIN_BARGRAPH:
	if (!spin_level++) {
	    if ((spin_art = openart) != 0 && artfp)
		spin_tell = tellart();
	    spin_count = 0;
	    spin_place = 0;
	}
	if (spin_mode == SPIN_BARGRAPH)
	    mode = SPIN_BARGRAPH;
	if (mode == SPIN_BARGRAPH) {
	    if (spin_mode != SPIN_BARGRAPH) {
		int i;
#ifdef VERBOSE
		spin_marks = (verbose? 25 : 10);
#else
		spin_marks = 25;
#endif
		printf(" [%*s]", spin_marks, nullstr);
		for (i = spin_marks + 1; i--; ) backspace();
		fflush(stdout);
	    }
	    spin_pos = 0;
	}
	spinchars = "|/-\\";
	spin_mode = mode;
	break;
      case SPIN_POP:
      case SPIN_OFF:
	if (spin_mode == SPIN_BARGRAPH) {
	    spin_level = 1;
	    spin(10000);
	    if (spin_count >= spin_todo)
		spin_char = ']';
	    spin_count--;
	    spin_mode = SPIN_FOREGROUND;
	}
	if (mode == SPIN_POP && --spin_level > 0)
	    break;
	spin_level = 0;
	if (spin_place) {	/* we have spun at least once */
	    putchar(spin_char); /* get rid of spin character */
	    backspace();
	    fflush(stdout);
	    spin_place = 0;
	}
	if (spin_art) {
	    artopen(spin_art,spin_tell);   /* do not screw up the pager */
	    spin_art = 0;
	}
	spin_mode = SPIN_OFF;
	spin_char = ' ';
	break;
    }
}

/* modulus for the spin... */
void spin(int count)
{
    if (!spin_level)
	return;
    switch (spin_mode) {
      case SPIN_BACKGROUND:
	if (!bkgnd_spinner)
	    return;
	if (!(++spin_count % count)) {
	    putchar(spinchars[++spin_place % 4]);
	    backspace();
	    fflush(stdout);
	}
	break;
      case SPIN_FOREGROUND:
	if (!(++spin_count % count)) {
	    putchar('.');
	    fflush(stdout);
	}
	break;
      case SPIN_BARGRAPH: {
	int new_pos;

	if (spin_todo == 0)
	    break;		/* bail out rather than crash */
	new_pos = (int)((long)spin_marks * ++spin_count / spin_todo);
	if (spin_pos < new_pos && spin_count <= spin_todo+1) {
	    do {
		putchar('*');
	    } while (++spin_pos < new_pos);
	    spin_place = 0;
	    fflush(stdout);
	}
	else if (!(spin_count % count)) {
	    putchar(spinchars[++spin_place % 4]);
	    backspace();
	    fflush(stdout);
	}
	break;
      }
    }
}

bool inbackground()
{
    return spin_mode == SPIN_BACKGROUND;
}

static int	prior_perform_cnt;
static time_t	prior_now;
static long	ps_sel;
static long	ps_cnt;
static long	ps_missing;

void perform_status_init(long cnt)
{
    perform_cnt = 0;
    error_occurred = false;
    subjline = nullptr;
    page_line = 1;
    performed_article_loop = true;

    prior_perform_cnt = 0;
    prior_now = 0;
    ps_sel = selected_count;
    ps_cnt = cnt;
    ps_missing = missing_count;

    spin_count = 0;
    spin_place = 0;
    spinchars = "v>^<";
}

void perform_status(long cnt, int spin)
{
    long kills, sels, missing;
    time_t now;

    if (!(++spin_count % spin)) {
	putchar(spinchars[++spin_place % 4]);
	backspace();
	fflush(stdout);
    }

    if (perform_cnt == prior_perform_cnt)
	return;

    now = time((time_t*)nullptr);
    if (now - prior_now < 2)
	return;

    prior_now = now;
    prior_perform_cnt = perform_cnt;

    missing = missing_count - ps_missing;
    kills = ps_cnt - cnt - missing;
    sels = selected_count - ps_sel;

    if (!(kills | sels))
	return;

    carriage_return();
    if (perform_cnt != sels  && perform_cnt != -sels
     && perform_cnt != kills && perform_cnt != -kills)
	printf("M:%d ", perform_cnt);
    if (kills)
	printf("K:%ld ", kills);
    if (sels)
	printf("S:%ld ", sels);
#if 0
    if (missing > 0)
	printf("(M: %ld) ", missing);
#endif
    erase_eol();
    fflush(stdout);
}

static char *output_change(char *cp, long num, char *obj_type, char *modifier, char *action)
{
    bool neg;
    char* s;

    if (num < 0) {
	num *= -1;
	neg = 1;
    }
    else
	neg = 0;

    if (cp != msg) {
	*cp++ = ',';
	*cp++ = ' ';
    }
    sprintf(cp, "%ld ", num);
    if (obj_type)
	sprintf(cp+=strlen(cp), "%s%s ", obj_type, PLURAL(num));
    cp += strlen(cp);
    if ((s = modifier) != nullptr) {
	*cp++ = ' ';
	if (num != 1)
	    while (*s++ != '|') ;
	while (*s && *s != '|') *cp++ = *s++;
	*cp++ = ' ';
    }
    s = action;
    if (!neg)
	while (*s++ != '|') ;
    while (*s && *s != '|') *cp++ = *s++;
    s++;
    if (neg)
	while (*s++ != '|') ;
    while (*s) *cp++ = *s++;

    *cp = '\0';
    return cp;
}

int perform_status_end(long cnt, char *obj_type)
{
    long kills, sels, missing;
    char* cp = msg;
    bool article_status = (*obj_type == 'a');

    if (perform_cnt == 0) {
	sprintf(msg, "No %ss affected.", obj_type);
	return 0;
    }

    missing = missing_count - ps_missing;
    kills = ps_cnt - cnt - missing;
    sels = selected_count - ps_sel;

    if (!performed_article_loop)
	cp = output_change(cp, (long)perform_cnt,
			   sel_mode == SM_THREAD? "thread" : "subject",
			   (char*)nullptr, "ERR|match|ed");
    else if (perform_cnt != sels  && perform_cnt != -sels
	  && perform_cnt != kills && perform_cnt != -kills) {
	cp = output_change(cp, (long)perform_cnt, obj_type, (char*)nullptr,
			   "ERR|match|ed");
	obj_type = nullptr;
    }
    if (kills) {
	cp = output_change(cp, kills, obj_type, (char*)nullptr,
			   article_status? "un||killed" : "more|less|");
	obj_type = nullptr;
    }
    if (sels) {
	cp = output_change(cp, sels, obj_type, (char*)nullptr, "de||selected");
	obj_type = nullptr;
    }
    if (article_status && missing > 0) {
	*cp++ = '(';
	cp = output_change(cp, missing, obj_type, "was|were", "ERR|missing|");
	*cp++ = ')';
    }

    strcpy(cp, ".");

    /* If we only selected/deselected things, return 1, else 2 */
    return (kills | missing) == 0? 1 : 2;
}
