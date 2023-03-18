/* nntplist.c
 */
/* This software is copyrighted as detailed in the LICENSE file. */


#include "EXTERN.h"
#include "common.h"
#include "env.h"
#include "util2.h"
#include "util3.h"
#include "wildmat.h"
#include "nntpclient.h"
#include "nntpinit.h"
#include "INTERN.h"
#include "common.h"

void Usage();

char* server_name;
char* nntp_auth_file;

int debug = 0;			/* make nntpclient.c happy */

int main(int argc, char *argv[])
{
    char command[32];
    char* action = nullptr;
    char* wildarg = nullptr;
    char* cp;
    FILE* in_fp;
    register FILE* out_fp = nullptr;

    while (--argc) {
	if (**++argv == '-') {
	    switch (*++*argv) {
	    case 'o':
		if (out_fp || !--argc)
		    Usage();
		out_fp = fopen(*++argv, "w");
		if (out_fp == nullptr) {
		    perror(*argv);
		    exit(1);
		}
		break;
	    case 'x':
		if (wildarg || !--argc)
		    Usage();
		wildarg = *++argv;
		break;
	    default:
		Usage();
		/* NO RETURN */
	    }
	}
	else if (!action)
	    action = *argv;
	else
	    Usage();
    }
    if (action && strcaseEQ(action,"active"))
	action = nullptr;
    if (!out_fp)
	out_fp = stdout;

    char tcbuf[1024];
    tcbuf[0] = 0;
    env_init(tcbuf, true);

    cp = getenv("NNTPSERVER");
    if (!cp) {
	cp = filexp(SERVER_NAME);
	if (*cp == '/')
	    cp = nntp_servername(cp);
    }
    if (strNE(cp,"local")) {
	server_name = savestr(cp);
	cp = index(server_name, ';');
	if (!cp)
	    cp = index(server_name, ':');
	if (cp) {
	    *cp = '\0';
	    nntplink.port_number = atoi(cp+1);
	}
	nntp_auth_file = filexp(NNTP_AUTH_FILE);
	if ((cp = getenv("NNTP_FORCE_AUTH")) != nullptr
	 && (*cp == 'y' || *cp == 'Y'))
	    nntplink.flags |= NNTP_FORCE_AUTH_NEEDED;
    }

    if (server_name) {
	if (init_nntp() < 0
	 || nntp_connect(server_name,false) <= 0)
	    exit(1);
	if (action)
	    sprintf(command,"LIST %s",action);
	else
	    strcpy(command,"LIST");
	if (wildarg)
	    sprintf(command+strlen(command)," %s",wildarg);
	if (nntp_command(command) <= 0)
	    exit(1);
#ifdef HAS_SIGHOLD
	sighold(SIGINT);
#endif
	if (nntp_check() <= 0) {
	    fprintf(stderr,"nntplist: Can't get %s file from server.\n",action? action : "active");
	    fprintf(stderr, "Server said: %s\n", g_ser_line);
	    finalize(1);
	}
	while (nntp_gets(g_ser_line, sizeof g_ser_line) == 1) {
	    if (nntp_at_list_end(g_ser_line))
		break;
	    fputs(g_ser_line, out_fp);
	    putc('\n', out_fp);
	}

#ifdef HAS_SIGHOLD
	sigrelse(SIGINT);
#endif
	nntp_close(true);
	cleanup_nntp();
    }
    else {
	cp = nullptr;
	if (!action)
	    cp = ACTIVE;
	else if (strcaseEQ(action,"active.times"))
	    cp = ACTIVE_TIMES;
	else if (strcaseEQ(action,"newsgroups"))
	    cp = GROUPDESC;
	else if (strcaseEQ(action,"subscriptions"))
	    cp = SUBSCRIPTIONS;
	else if (strcaseEQ(action,"overview.fmt"))
	    cp = OVERVIEW_FMT;
	if (!cp || !*cp) {
	    fprintf(stderr, "Don't know how to list `%s' from your local system.\n",
		    action);
	    exit(1);
	}
	if ((in_fp = fopen(filexp(cp), "r")) == nullptr) {
	    fprintf(stderr,"Unable to open `%s'.\n", cp);
	    exit(1);
	}
	while (fgets(g_ser_line, sizeof g_ser_line, in_fp)) {
	    if (wildarg) {
		for (cp = g_ser_line; *cp && !isspace(*cp); cp++) ;
		if (!cp)
		    continue;
		*cp = '\0';
		if (!wildmat(g_ser_line, wildarg))
		    continue;
		*cp = ' ';
	    }
	    fputs(g_ser_line, out_fp);
	}
    }
    return 0;
}

void Usage()
{
    fprintf(stderr, "Usage: nntplist [-x WildSpec] [-o OutputFile] [type]\n\n"
                    "Where type is any of the LIST command arguments your server accepts.\n");
    exit(1);
}

#ifdef SUPPORT_NNTP
int nntp_handle_timeout()
{
    fputs("\n503 Server timed out.\n",stderr);
    return -2;
}
#endif