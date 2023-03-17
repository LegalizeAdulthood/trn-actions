/* term.h
 */
/* This software is copyrighted as detailed in the LICENSE file. */

EXT char ERASECH;		/* rubout character */
EXT char KILLCH;		/* line delete character */
EXT char circlebuf[PUSHSIZE];
EXT int nextin INIT(0);
EXT int nextout INIT(0);
EXT unsigned char lastchar;

/* stuff wanted by terminal mode diddling routines */

#ifdef I_TERMIO
EXT struct termio _tty, _oldtty;
#else
# ifdef I_TERMIOS
EXT struct termios _tty, _oldtty;
# else
#  ifdef I_SGTTY
EXT struct sgttyb _tty;
EXT int _res_flg INIT(0);
#  endif
# endif
#endif

EXT int _tty_ch INIT(2);
EXT bool bizarre INIT(false);		/* do we need to restore terminal? */

/* terminal mode diddling routines */

#ifdef I_TERMIO

#define crmode() ((bizarre=1),_tty.c_lflag &=~ICANON,_tty.c_cc[VMIN] = 1,ioctl(_tty_ch,TCSETAF,&_tty))
#define nocrmode() ((bizarre=1),_tty.c_lflag |= ICANON,_tty.c_cc[VEOF] = CEOF,stty(_tty_ch,&_tty))
#define echo()	 ((bizarre=1),_tty.c_lflag |= ECHO, ioctl(_tty_ch, TCSETA, &_tty))
#define noecho() ((bizarre=1),_tty.c_lflag &=~ECHO, ioctl(_tty_ch, TCSETA, &_tty))
#define nl()	 ((bizarre=1),_tty.c_iflag |= ICRNL,_tty.c_oflag |= ONLCR,ioctl(_tty_ch, TCSETAW, &_tty))
#define nonl()	 ((bizarre=1),_tty.c_iflag &=~ICRNL,_tty.c_oflag &=~ONLCR,ioctl(_tty_ch, TCSETAW, &_tty))
#define	savetty() (ioctl(_tty_ch, TCGETA, &_oldtty),ioctl(_tty_ch, TCGETA, &_tty))
#define	resetty() ((bizarre=0),ioctl(_tty_ch, TCSETAF, &_oldtty))
#define unflush_output()

#else /* !I_TERMIO */
# ifdef I_TERMIOS

#define crmode() ((bizarre=1), _tty.c_lflag &= ~ICANON,_tty.c_cc[VMIN]=1,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nocrmode() ((bizarre=1),_tty.c_lflag |= ICANON,_tty.c_cc[VEOF] = CEOF,tcsetattr(_tty_ch, TCSAFLUSH,&_tty))
#define echo()	 ((bizarre=1),_tty.c_lflag |= ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define noecho() ((bizarre=1),_tty.c_lflag &=~ECHO, tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nl()	 ((bizarre=1),_tty.c_iflag |= ICRNL,_tty.c_oflag |= ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define nonl()	 ((bizarre=1),_tty.c_iflag &=~ICRNL,_tty.c_oflag &=~ONLCR,tcsetattr(_tty_ch, TCSAFLUSH, &_tty))
#define	savetty() (tcgetattr(_tty_ch, &_oldtty),tcgetattr(_tty_ch, &_tty))
#define	resetty() ((bizarre=0),tcsetattr(_tty_ch, TCSAFLUSH, &_oldtty))
#define unflush_output()

# else /* !I_TERMIOS */
#  ifdef I_SGTTY

#define raw()	 ((bizarre=1),_tty.sg_flags|=RAW, stty(_tty_ch,&_tty))
#define noraw()	 ((bizarre=1),_tty.sg_flags&=~RAW,stty(_tty_ch,&_tty))
#define crmode() ((bizarre=1),_tty.sg_flags |= CBREAK, stty(_tty_ch,&_tty))
#define nocrmode() ((bizarre=1),_tty.sg_flags &= ~CBREAK,stty(_tty_ch,&_tty))
#define echo()	 ((bizarre=1),_tty.sg_flags |= ECHO, stty(_tty_ch, &_tty))
#define noecho() ((bizarre=1),_tty.sg_flags &= ~ECHO, stty(_tty_ch, &_tty))
#define nl()	 ((bizarre=1),_tty.sg_flags |= CRMOD,stty(_tty_ch, &_tty))
#define nonl()	 ((bizarre=1),_tty.sg_flags &= ~CRMOD, stty(_tty_ch, &_tty))
#define	savetty() (gtty(_tty_ch, &_tty), _res_flg = _tty.sg_flags)
#define	resetty() ((bizarre=0),_tty.sg_flags = _res_flg, stty(_tty_ch, &_tty))
#   ifdef LFLUSHO
EXT int lflusho INIT(LFLUSHO);
#define unflush_output() (ioctl(_tty_ch,TIOCLBIC,&lflusho))
#   else /*! LFLUSHO */
#define unflush_output()
#   endif
#  else
#   ifdef MSDOS

#define crmode() (bizarre=1)
#define nocrmode() (bizarre=1)
#define echo()	 (bizarre=1)
#define noecho() (bizarre=1)
#define nl()	 (bizarre=1)
#define nonl()	 (bizarre=1)
#define	savetty()
#define	resetty() (bizarre=0)
#define unflush_output()
#   else /* !MSDOS */
// ..."Don't know how to define the term macros!"
#   endif /* !MSDOS */
#  endif /* !I_SGTTY */
# endif /* !I_TERMIOS */

#endif /* !I_TERMIO */

#ifdef TIOCSTI
#define forceme(c) ioctl(_tty_ch,TIOCSTI,c) /* pass character in " " */
#else
#define forceme(c)
#endif

/* termcap stuff */

/*
 * NOTE: if you don't have termlib you'll either have to define these strings
 *    and the tputs routine, or you'll have to redefine the macros below
 */

#ifdef HAS_TERMLIB
EXT int tc_GT;				/* hardware tabs */
EXT char* tc_BC INIT(NULL);		/* backspace character */
EXT char* tc_UP INIT(NULL);		/* move cursor up one line */
EXT char* tc_CR INIT(NULL);		/* get to left margin, somehow */
EXT char* tc_VB INIT(NULL);		/* visible bell */
EXT char* tc_CL INIT(NULL);		/* home and clear screen */
EXT char* tc_CE INIT(NULL);		/* clear to end of line */
EXT char* tc_TI INIT(NULL);		/* initialize terminal */
EXT char* tc_TE INIT(NULL);		/* reset terminal */
EXT char* tc_KS INIT(NULL);		/* enter `keypad transmit' mode */
EXT char* tc_KE INIT(NULL);		/* exit `keypad transmit' mode */
EXT char* tc_CM INIT(NULL);		/* cursor motion */
EXT char* tc_HO INIT(NULL);		/* home cursor */
EXT char* tc_IL INIT(NULL);		/* insert line */
EXT char* tc_CD INIT(NULL);		/* clear to end of display */
EXT char* tc_SO INIT(NULL);		/* begin standout mode */
EXT char* tc_SE INIT(NULL);		/* end standout mode */
EXT int tc_SG INIT(0);			/* blanks left by SO and SE */
EXT char* tc_US INIT(NULL);		/* start underline mode */
EXT char* tc_UE INIT(NULL);		/* end underline mode */
EXT char* tc_UC INIT(NULL);		/* underline a character,
						 if that's how it's done */
EXT int tc_UG INIT(0);			/* blanks left by US and UE */
EXT bool tc_AM INIT(false);		/* does terminal have automatic
								 margins? */
EXT bool tc_XN INIT(false);		/* does it eat 1st newline after
							 automatic wrap? */
EXT char tc_PC INIT(0);			/* pad character for use by tputs() */

#ifdef _POSIX_SOURCE
EXT speed_t outspeed INIT(0);		/* terminal output speed, */
#else
EXT long outspeed INIT(0);		/* 	for use by tputs() */
#endif

EXT int fire_is_out INIT(1);
EXT int tc_LINES INIT(0), tc_COLS INIT(0);/* size of screen */
EXT int g_term_line, g_term_col;	/* position of cursor */
EXT int term_scrolled;			/* how many lines scrolled away */
EXT int just_a_sec INIT(960);		/* 1 sec at current baud rate */
					/* (number of nulls) */

/* define a few handy macros */
inline void termdown(int x)
{
    g_term_line += x;
    g_term_col = 0;
}
inline void newline()
{
    g_term_line++;
    g_term_col=0;
    putchar('\n');
    FLUSH;
}
#define backspace() tputs(tc_BC,0,putchr) FLUSH
#define erase_eol() tputs(tc_CE,1,putchr) FLUSH
#define clear_rest() tputs(tc_CD,tc_LINES,putchr) FLUSH
#define maybe_eol() if(erase_screen&&erase_each_line)tputs(tc_CE,1,putchr) FLUSH
#define underline() tputs(tc_US,1,putchr) FLUSH
#define un_underline() fire_is_out|=UNDERLINE, tputs(tc_UE,1,putchr) FLUSH
#define underchar() tputs(tc_UC,0,putchr) FLUSH
#define standout() tputs(tc_SO,1,putchr) FLUSH
#define un_standout() fire_is_out|=STANDOUT, tputs(tc_SE,1,putchr) FLUSH
#define up_line() g_term_line--, tputs(tc_UP,1,putchr) FLUSH
#define insert_line() tputs(tc_IL,1,putchr) FLUSH
#define carriage_return() g_term_col=0, tputs(tc_CR,1,putchr) FLUSH
#define dingaling() tputs(tc_VB,1,putchr) FLUSH
#else /* !HAS_TERMLIB */
//..."Don't know how to define the term macros!"
#endif /* !HAS_TERMLIB */

#define input_pending() finput_pending(true)
#define macro_pending() finput_pending(false)

EXT int page_line INIT(1);	/* line number for paging in
				 print_line (origin 1) */
EXT bool error_occurred INIT(false);

EXT char* mousebar_btns;
EXT int mousebar_cnt INIT(0);
EXT int mousebar_start INIT(0);
EXT int mousebar_width INIT(0);
EXT bool xmouse_is_on INIT(false);
EXT bool mouse_is_down INIT(false);

void term_init(void);
void term_set(char *);
void set_macro(char *, char *);
void arrow_macros(char *);
void mac_line(char *, char *, int);
void show_macros(void);
void set_mode(char_int, char_int);
int putchr(char_int);
void hide_pending(void);
bool finput_pending(bool check_term);
bool finish_command(int);
char *edit_buf(char *, char *);
bool finish_dblchar(void);
void eat_typeahead(void);
void save_typeahead(char *, int);
void settle_down(void);
Signal_t alarm_catcher(int);
int read_tty(char *, int);
#if !defined(FIONREAD) && !defined(HAS_RDCHK) && !defined(MSDOS)
int circfill(void);
#endif
void pushchar(char_int);
void underprint(char *);
#ifdef NOFIREWORKS
void no_sofire(void);
void no_ulfire(void);
#endif
void getcmd(char *);
void pushstring(char *, char_int);
int get_anything(void);
int pause_getcmd(void);
void in_char(char *, char_int, char *);
void in_answer(char *, char_int);
bool in_choice(char *, char *, char *, char_int);
int print_lines(char *, int);
int check_page_line(void);
void page_start(void);
void errormsg(char *);
void warnmsg(char *);
void pad(int);
#ifdef VERIFY
void printcmd(void);
#endif
void rubout(void);
void reprint(void);
void erase_line(bool to_eos);
void clear(void);
void home_cursor(void);
void goto_xy(int, int);
#ifdef SIGWINCH
Signal_t winch_catcher(int);
#endif
void termlib_init(void);
void termlib_reset(void);
#ifdef NBG_SIGIO
Signal_t waitkey_sig_handler(int);
#endif
bool wait_key_pause(int);
void xmouse_init(char *);
void xmouse_check(void);
void xmouse_on(void);
void xmouse_off(void);
void draw_mousebar(int limit, bool restore_cursor);
bool check_mousebar(int, int, int, int, int, int);
void add_tc_string(char *, char *);
char *tc_color_capability(char *);
#ifdef MSDOS
int tputs(char *, int, int (*)(char_int));
char *tgoto(char *, int, int);
#endif
