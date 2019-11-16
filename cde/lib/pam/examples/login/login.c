/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: login.c /main/8 1996/10/29 13:49:30 drk $ */
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)login.c	1.63	96/02/15 SMI"	/* SVr4.0 1.43.6.26	*/

/*

		PROPRIETARY NOTICE(Combined)

This source code is unpublished proprietary information
constituting, or derived under license from AT&T's UNIX(r) System V.
In addition, portions of such source code were derived from Berkeley
4.3 BSD under license from the Regents of the University of
California.



		Copyright Notice

Notice of copyright on this source code product does not indicate
publication.

	(c) 1986, 1987, 1988, 1989, 1990, 1991, 1992 Sun Microsystems, Inc
	(c) 1983, 1984, 1985, 1986, 1987, 1988, 1989  AT&T.
		All rights reserved.
*******************************************************************
*/

/*	Copyright (c) 1987, 1988 Microsoft Corporation	*/
/*	  All Rights Reserved	*/

/*	This Module contains Proprietary Information of Microsoft  */
/*	Corporation and should be treated as Confidential.	   */

/*
 * Usage: login [ -d device ] [ name ] [ environment args ]
 *
 *
 */


/*
 *
 *			    *** Header Files ***
 *
 *
 */

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>	/* For logfile locking */
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <deflt.h>
#include <grp.h>
#include <fcntl.h>
#include <lastlog.h>
#include <termio.h>
#include <utmpx.h>
#include <dirent.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#include <ctype.h>
#include <syslog.h>
#include <ulimit.h>
#include <libgen.h>
#include <security/pam_appl.h>

/*
 *
 *	    *** Defines, Macros, and String Constants  ***
 *
 *
 */

#define	ISSUEFILE "/etc/issue"	/* file to print before prompt */
#define	NOLOGIN	"/etc/nologin"	/* file to lock users out during shutdown */

/*
 * These need to be defined for UTMP management.
 * If we add in the utility functions later, we
 * can remove them.
 */
#define	__UPDATE_ENTRY	1
#define	__LOGIN		2

/*
 * Intervals to sleep after failed login
 */
#ifndef	SLEEPTIME
#define	SLEEPTIME 4	/* sleeptime before login incorrect msg */
#endif
static int	Sleeptime = SLEEPTIME;

/*
 * seconds login disabled after allowable number of unsuccessful attempts
 */
#ifndef	DISABLETIME
#define	DISABLETIME	20
#endif
#define	MAXTRYS		5

/*
 * Login logging support
 */
#define	LOGINLOG	"/var/adm/loginlog"	/* login log file */
#define	LNAME_SIZE	20	/* size of logged logname */
#define	TTYN_SIZE	15	/* size of logged tty name */
#define	TIME_SIZE	30	/* size of logged time string */
#define	ENT_SIZE	(LNAME_SIZE + TTYN_SIZE + TIME_SIZE + 3)
#define	L_WAITTIME	5	/* waittime for log file to unlock */
#define	LOGTRYS		10	/* depth of 'try' logging */

/*
 * String manipulation macros: SCPYN, EQN and ENVSTRNCAT
 */
#define	SCPYN(a, b)	(void) strncpy(a, b, sizeof (a))
#define	EQN(a, b)	(strncmp(a, b, sizeof (a)-1) == 0)
#define	ENVSTRNCAT(to, from) {int deflen; deflen = strlen(to); \
	(void) strncpy((to)+ deflen, (from), sizeof (to) - (1 + deflen)); }

/*
 * Other macros
 */
#define	NMAX	sizeof (utmp.ut_name)
#define	HMAX	sizeof (utmp.ut_host)
#define	min(a, b)	(((a) < (b)) ? (a) : (b))

/*
 * Various useful files and string constants
 */
#define	SHELL		"/usr/bin/sh"
#define	SHELL2		"/sbin/sh"
#define	SUBLOGIN	"<!sublogin>"
#define	LASTLOG		"/var/adm/lastlog"
#define	PROG_NAME	"login"
#define	HUSHLOGIN	".hushlogin"

/*
 * Array and Buffer sizes
 */
#define	PBUFSIZE 8	/* max significant characters in a password */
#define	MAXARGS 63
#define	MAXENV 1024
#define	MAXLINE 2048

/*
 * Miscellaneous constants
 */
#define	ROOTUID		0
#define	ERROR		1
#define	OK		0
#define	LOG_ERROR	1
#define	DONT_LOG_ERROR	0
#define	TRUE		1
#define	FALSE		0

/*
 * Counters for counting the number of failed login attempts
 */
static int trys = 0;

/*
 * Externs a plenty
 */
extern	int	defopen(char *filename);
extern	int	getsecretkey();

/*
 * BSM hooks
 */
extern int audit_login_save_flags(int rflag, int hflag);
extern int audit_login_save_host(char *host);
extern int audit_login_save_ttyn(char *ttyn);
extern int audit_login_save_port(void);
extern int audit_login_success(void);
extern int audit_login_save_pw(struct passwd *pwd);
extern int audit_login_bad_pw(void);
extern int audit_login_maxtrys(void);
extern int audit_login_not_console(void);
extern int audit_login_bad_dialup(void);
extern int audit_login_maxtrys(void);

/*
 * utmp file variables
 */
static	struct	utmpx utmp;

/*
 * The current user name
 */
static	char	user_name[64];
static	char	minusnam[16] = "-";

/*
 * locale environments to be passed to shells.
 */
static char *localeenv[] = {
	"LANG",
	"LC_CTYPE", "LC_NUMERIC", "LC_TIME", "LC_COLLATE",
	"LC_MONETARY", "LC_MESSAGES", "LC_ALL", 0};
static int locale_envmatch(char *lenv, char *penv);

/*
 * Environment variable support
 */
static	char	shell[256] = { "SHELL=" };
static	char	home[MAXPATHLEN] = { "HOME=" };
static	char	term[64] = { "TERM=" };
static	char	logname[30] = { "LOGNAME=" };
static	char	timez[100] = { "TZ=" };
static	char	hertz[10] = { "HZ=" };
static	char	path[MAXPATHLEN] = { "PATH=" };
static	char	*newenv[10+MAXARGS] =
	{home, path, logname, hertz, term, 0, 0};
static	char	**envinit = newenv;
static	int	basicenv;
static	char	envblk[MAXENV];
static	char	*zero = (char *)0;
static	char 	**envp;
static	char	krb5ccname[256] = { "KRB5CCNAME=" };
static	char	krb4ccname[256] = { "KRBTKFILE=" };
#ifndef	NO_MAIL
static	char	mail[30] = { "MAIL=/var/mail/" };
#endif
extern char **environ;
char inputline[MAXLINE];


/*
 * Strings used to prompt the user.
 */
static	char	loginmsg[] = "login: ";
static	char	passwdmsg[] = "Password:";
static	char	incorrectmsg[] = "Login incorrect\n";

/*
 * Password file support
 */
static	struct	passwd *pwd;
static	char	remote_host[HMAX];

/*
 * Illegal passwd entries.
 */
static	struct	passwd nouser = { "", "no:password", ~ROOTUID };

/*
 * Log file support
 */
static	char	*log_entry[LOGTRYS];
static	int	writelog = 0;
static	int	lastlogok = 0;
static	struct lastlog ll;
static	int	dosyslog = 0;

/*
 * Default file toggles
 */
static	char	*Pndefault	= "/etc/default/login";
static	char	*Altshell	= NULL;
static	char	*Console	= NULL;
static	char	*Passreq	= NULL;
#define	DEFUMASK	022
static	mode_t	Umask		= DEFUMASK;
static	char 	*Def_tz		= NULL;
static	char 	*tmp_tz		= NULL;
static	char 	*Def_hertz	= NULL;
#define	SET_FSIZ	2			/* ulimit() command arg */
static	long	Def_ulimit	= 0;
#define	MAX_TIMEOUT	(15 * 60)
#define	DEF_TIMEOUT	(5 * 60)
static	unsigned Def_timeout	= DEF_TIMEOUT;
static	char	*Def_path	= NULL;
static	char	*Def_supath	= NULL;
#define	DEF_PATH	"/usr/bin:" 	/* same as PATH */
#define	DEF_SUPATH	"/usr/sbin:/usr/bin" /* same as ROOTPATH */

/*
 * ttyprompt will point to the environment variable TTYPROMPT.
 * TTYPROMPT is set by ttymon if ttymon already wrote out the prompt.
 */
static	char	*ttyprompt = NULL;
static	char 	*ttyn = NULL;
static	struct	group	*grpstr;
static	char	*ttygrp = "tty";
static	char	*progname = PROG_NAME;

/*
 * Pass inherited environment.  Used by telnetd in support of the telnet
 * ENVIRON option.
 */
static	int	pflag;
/*
 * Remote login support
 */
static	int	hflag, rflag;
static	char	rusername[NMAX+1], lusername[NMAX+1];
static	char	terminal[MAXPATHLEN];

/*
 * Pre-authentication flag support
 */
static	int	fflag;

static int login_conv(int num_msg, struct pam_message **msg,
    struct pam_response **response, void *appdata_ptr);

static struct pam_conv pam_conv = {login_conv, NULL};
static pam_handle_t *pamh;	/* Authentication handle */

/*
 * Function declarations
 */
static	void	turn_on_logging(void);
static	void	defaults(void);
static	void	usage(void);
static	void	process_rlogin(void);
static	void	login_authenticate();
static	void	setup_credentials(void);
static	void	adjust_nice(void);
static	void	update_utmp_entry(int sublogin);
static	void	establish_user_environment(char **renvp);
static	void	print_banner(void);
static	void	display_last_login_time(void);
static	void	exec_the_shell(void);
static	int	process_chroot_logins(void);
static 	int	chdir_to_dir_root(void);
static 	void	chdir_to_dir_user(void);
static	void	logindevperm(char *, uid_t, gid_t);
static	void	dir_dev_acc(char *, uid_t, gid_t, mode_t, char *);
static	void	check_log(void);
static	void	validate_account();
static	void	doremoteterm(char *term);
static	int	get_options(int argc, char *argv[]);
static	void	uppercaseterm(char *strp);
static	void	getstr(char *buf, int cnt, char *err);
static 	int	legalenvvar(char *s);
static	void	check_for_root_user(void);
static	void	check_for_dueling_unix(char inputline[]);
static	void	get_user_name(void);
static	void	login_exit(int exit_code);
static	int	logins_disabled(char *user_name);
static	void	log_bad_attempts(void);

/*
 *			*** main ***
 *
 *	The primary flow of control is directed in this routine.
 *	Control moves in line from top to bottom calling subfunctions
 *	which perform the bulk of the work.  Many of these calls exit
 *	when a fatal error is encountered and do not return to main.
 *
 *
 */

void
main(int argc, char *argv[], char **renvp)
{
	int sublogin;

	/*
	 * Set up Defaults and flags
	 */
	defaults();

	/*
	 * Set up default umask
	 */
	if (Umask > ((mode_t) 0777))
		Umask = DEFUMASK;
	(void) umask(Umask);

	/*
	 * Set up default timeouts and delays
	 */
	if (Def_timeout > MAX_TIMEOUT)
		Def_timeout = MAX_TIMEOUT;
	if (Sleeptime < 0 || Sleeptime > 5)
		Sleeptime = SLEEPTIME;

	(void) alarm(Def_timeout);

	/*
	 * Ignore SIGQUIT and SIGINT and set nice to 0
	 */
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGINT, SIG_IGN);
	(void) nice(0);

	/*
	 * Set flag to disable the pid check if you find that you are
	 * a subsystem login.
	 */
	sublogin = 0;
	if (*renvp && strcmp(*renvp, SUBLOGIN) == 0)
		sublogin = 1;

	/*
	 * Parse Arguments
	 */
	if (get_options(argc, argv) == -1) {
		usage();
		login_exit(1);
	}

	audit_login_save_flags(rflag, hflag);
	audit_login_save_host(remote_host);

	/*
	 * if devicename is not passed as argument, call ttyname(0)
	 */
	if (ttyn == NULL) {
		ttyn = ttyname(0);
		if (ttyn == NULL)
			ttyn = "/dev/???";
	}

	audit_login_save_ttyn(ttyn);
	audit_login_save_port();

	/*
	 * Call pam_start to initiate a PAM authentication operation
	 */

	if ((pam_start(progname, user_name, &pam_conv, &pamh))
		!= PAM_SUCCESS)
		login_exit(1);
	if ((pam_set_item(pamh, PAM_TTY, ttyn)) != PAM_SUCCESS) {
		login_exit(1);
	}
	if ((pam_set_item(pamh, PAM_RHOST, remote_host)) != PAM_SUCCESS) {
		login_exit(1);
	}

	/*
	 * Open the log file which contains a record of successful and failed
	 * login attempts
	 */
	turn_on_logging();

	/*
	 * say "hi" to syslogd ..
	 */
	openlog("login", 0, LOG_AUTH);

	/*
	 * Do special processing for -r (rlogin) flag
	 */
	if (rflag)
		process_rlogin();

	/*
	 * validate user
	 */
	/* we are already authenticated. fill in what we must, then continue */
	if (fflag) {
		if (pwd = getpwnam(user_name))
			audit_login_save_pw(pwd);
		else {
			audit_login_bad_pw();
			log_bad_attempts();
			login_exit(1);
		}
	} else {
		/*
		 * Perform the primary login authentication activity.
		 */
		login_authenticate();
	}

	/* change root login, then we exec another login and try again */
	if (process_chroot_logins() != OK)
		login_exit(1);

	/*
	 * If root login and not on system console then call exit(2)
	 */
	check_for_root_user();

	/*
	 * Check to see if a shutdown is in progress, if it is and
	 * we are not root then throw the user off the system
	 */
	if (logins_disabled(user_name) == TRUE)
		login_exit(1);

	if (pwd->pw_uid == 0) {
		if (Def_supath != NULL)
			Def_path = Def_supath;
		else
			Def_path = DEF_SUPATH;
	}

	/*
	 * Check account expiration and passwd aging
	 */
	validate_account();

	/*
	 * We only get here if we've been authenticated.
	 */
	update_utmp_entry(sublogin);

	/*
	 * Now we set up the environment for the new user, which includes
	 * the users ulimit, nice value, ownership of this tty, uid, gid,
	 * and environment variables.
	 */
	if (Def_ulimit > 0L && ulimit(SET_FSIZ, Def_ulimit) < 0L)
		(void) printf("Could not set ULIMIT to %ld\n", Def_ulimit);

	/*
	 * Set mode to r/w user & w group, owner to user and group to tty
	 */
	(void) chmod(ttyn, S_IRUSR|S_IWUSR|S_IWGRP);

	if ((grpstr = getgrnam(ttygrp)) == NULL)
		(void) chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	else
		(void) chown(ttyn, pwd->pw_uid, grpstr->gr_gid);

	logindevperm(ttyn, pwd->pw_uid, pwd->pw_gid);
	adjust_nice();		/* passwd file can specify nice value */

	/*
	 * Record successful login and fork process that records logout.
	 * We have to do this before setting credentials because we need
	 * to be root in order do a setaudit() and an audit().
	 */
	audit_login_success();

	setup_credentials();	/* Set uid/gid - exits on failure */

	pam_end(pamh, PAM_SUCCESS);	/* Done using PAM */

	/*
	 * Set up the basic environment for the exec.  This includes
	 * HOME, PATH, LOGNAME, SHELL, TERM, TZ, HZ, and MAIL.
	 */
	chdir_to_dir_user();

	establish_user_environment(renvp);

	if (pwd->pw_uid == 0)
		if (remote_host[0] && dosyslog)
			syslog(LOG_NOTICE, "ROOT LOGIN %s FROM %.*s",
			    ttyn, HMAX, remote_host);
		else if (dosyslog)
			syslog(LOG_NOTICE, "ROOT LOGIN %s", ttyn);
	closelog();

	(void) signal(SIGQUIT, SIG_DFL);
	(void) signal(SIGINT, SIG_DFL);

	/*
	 * Display some useful information to the new user like the banner
	 * and last login time if not a quiet login.
	 */

	if (access(HUSHLOGIN, F_OK) != 0) {
		print_banner();
		display_last_login_time();
	}

	/*
	 * Now fire off the shell of choice
	 */
	exec_the_shell();

	/*
	 * All done
	 */
	login_exit(1);
	/* NOTREACHED */
}


/*
 *			*** Utility functions ***
 */



/*
 * donothing & catch	- Signal catching functions
 */

/*ARGSUSED*/
static void
donothing(int sig)
{
	if (pamh)
		pam_end(pamh, PAM_ABORT);
}

/*
 *			*** Bad login logging support ***
 */

/*
 * badlogin() 		- log to the log file 'trys'
 *			  unsuccessful attempts
 */

static void
badlogin(void)
{
	int retval, count1, fildes;

	/*
	 * Tries to open the log file. If succeed, lock it and write
	 * in the failed attempts
	 */
	if ((fildes = open(LOGINLOG, O_APPEND|O_WRONLY)) != -1) {

		(void) sigset(SIGALRM, donothing);
		(void) alarm(L_WAITTIME);
		retval = lockf(fildes, F_LOCK, 0L);
		(void) alarm(0);
		(void) sigset(SIGALRM, SIG_DFL);
		if (retval == 0) {
			for (count1 = 0; count1 < trys; count1++)
				(void) write(fildes, log_entry[count1],
				    (unsigned) strlen(log_entry[count1]));
			(void) lockf(fildes, F_ULOCK, 0L);
		}
		(void) close(fildes);
	}
}


/*
 * log_bad_attempts 	- log each bad login attempt - called from
 *			  login_authenticate.  Exits when the maximum attempt
 *			  count is exceeded.
 */

static void
log_bad_attempts(void)
{
	time_t timenow;

	if (writelog == 1 && trys < LOGTRYS) {
		(void) time(&timenow);
		(void) strncat(log_entry[trys], user_name, LNAME_SIZE);
		(void) strncat(log_entry[trys], ":", (size_t) 1);
		(void) strncat(log_entry[trys], ttyn, TTYN_SIZE);
		(void) strncat(log_entry[trys], ":", (size_t) 1);
		(void) strncat(log_entry[trys], ctime(&timenow), TIME_SIZE);
		trys++;
	}
}


/*
 * turn_on_logging 	- if the logfile exist, turn on attempt logging and
 *			  initialize the string storage area
 */

static void
turn_on_logging(void)
{
	struct stat dbuf;
	int i;

	if (stat(LOGINLOG, &dbuf) == 0) {
		writelog = 1;
		for (i = 0; i < LOGTRYS; i++) {
			if (!(log_entry[i] = malloc((size_t) ENT_SIZE))) {
				writelog = 0;
				break;
			}
			*log_entry[i] = '\0';
		}
	}
}


/*
 * login_conv():
 *	This is the conv (conversation) function called from
 *	a PAM authentication module to print error messages
 *	or garner information from the user.
 */
static int
login_conv(int num_msg, struct pam_message **msg,
    struct pam_response **response, void *appdata_ptr)
{
	struct pam_message	*m;
	struct pam_response	*r;
	char 			*temp;
	int			k, i;

	if (num_msg <= 0)
		return (PAM_CONV_ERR);

	*response = calloc(num_msg, sizeof (struct pam_response));
	if (*response == NULL)
		return (PAM_BUF_ERR);

	k = num_msg;
	m = *msg;
	r = *response;
	while (k--) {

		switch (m->msg_style) {

		case PAM_PROMPT_ECHO_OFF:
			temp = getpass(m->msg);
			if (temp != NULL) {
				r->resp = strdup(temp);
				if (r->resp == NULL) {
					/* free responses */
					r = *response;
					for (i = 0; i < num_msg; i++, r++) {
						if (r->resp)
							free(r->resp);
					}
					free(*response);
					*response = NULL;
					return (PAM_BUF_ERR);
				}
			}

			m++;
			r++;
			break;

		case PAM_PROMPT_ECHO_ON:
			if (m->msg != NULL)
				(void) fputs(m->msg, stdout);
			r->resp = calloc(1, PAM_MAX_RESP_SIZE);
			if (r->resp == NULL) {
				/* free responses */
				r = *response;
				for (i = 0; i < num_msg; i++, r++) {
					if (r->resp)
						free(r->resp);
				}
				free(*response);
				*response = NULL;
				return (PAM_BUF_ERR);
			}
			if (fgets(r->resp, PAM_MAX_RESP_SIZE-1, stdin)) {
				int len;
				r->resp[PAM_MAX_RESP_SIZE-1] = NULL;
				len = strlen(r->resp);
				if (r->resp[len-1] == '\n')
					r->resp[len-1] = '\0';
			} else {
				login_exit(1);
			}
			m++;
			r++;
			break;

		case PAM_ERROR_MSG:
			if (m->msg != NULL) {
				(void) fputs(m->msg, stderr);
				(void) fputs("\n", stderr);
			}
			m++;
			r++;
			break;
		case PAM_TEXT_INFO:
			if (m->msg != NULL) {
				(void) fputs(m->msg, stdout);
				(void) fputs("\n", stdout);
			}
			m++;
			r++;
			break;

		default:
			break;
		}
	}
	return (PAM_SUCCESS);
}

/*
 * verify_passwd	- Checks the users password
 *			  Returns: OK if password check successful,
 *			  ERROR if password check fails.
 */

static int
verify_passwd()
{
	int flags;
	int error;
	char *user;

	/*
	 * PAM authenticates the user for us.
	 */

	error = pam_authenticate(pamh, 0);

	/* get the user_name from the pam handle */
	pam_get_item(pamh, PAM_USER, (void**)&user);
	SCPYN(user_name, user);
	check_for_dueling_unix(user_name);

	if (error != PAM_SUCCESS) {
		/* something bad has happended - bye bye */
		if (error == PAM_ABORT) {
			audit_login_bad_pw();
			log_bad_attempts();
			(void) sleep(DISABLETIME);
			(void) printf(incorrectmsg);
			login_exit(1);
		}
		return (error);
	}

	if ((pwd = getpwnam(user_name)) == NULL) {
		audit_login_bad_pw();
		log_bad_attempts();
		return (PAM_USER_UNKNOWN);
	}

	audit_login_save_pw(pwd);

	return (error);
}

/*
 * quotec		- Called by getargs
 */

static int
quotec(void)
{
	int c, i, num;

	switch (c = getc(stdin)) {

		case 'n':
			c = '\n';
			break;

		case 'r':
			c = '\r';
			break;

		case 'v':
			c = '\013';
			break;

		case 'b':
			c = '\b';
			break;

		case 't':
			c = '\t';
			break;

		case 'f':
			c = '\f';
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			for (num = 0, i = 0; i < 3; i++) {
				num = num * 8 + (c - '0');
				if ((c = getc(stdin)) < '0' || c > '7')
					break;
			}
			(void) ungetc(c, stdin);
			c = num & 0377;
			break;

		default:
			break;
	}
	return (c);
}

/*
 * getargs		- returns an input line.  Exits if EOF encountered.
 */
#define	WHITESPACE	0
#define	ARGUMENT	1

static char **
getargs(char *in_line)
{
	static char envbuf[MAXLINE];
	static char *args[MAXARGS];
	char *ptr, **answer;
	int c;
	int state;

	for (ptr = envbuf; ptr < &envbuf[sizeof (envbuf)]; /* cstyle */)
		*ptr++ = '\0';

	for (answer = args; answer < &args[MAXARGS]; /* cstyle */)
		*answer++ = (char *)NULL;

	for (ptr = envbuf, answer = &args[0], state = WHITESPACE;
		(c = getc(stdin)) != EOF; /* cstyle */) {

		*(in_line++) = c;
		switch (c) {

		case '\n':
			if (ptr == &envbuf[0])
				return ((char **)NULL);
			return (&args[0]);

		case ' ':
		case '\t':
			if (state == ARGUMENT) {
				*ptr++ = '\0';
				state = WHITESPACE;
			}
			break;

		case '\\':
			c = quotec();

		default:
			if (state == WHITESPACE) {
				*answer++ = ptr;
				state = ARGUMENT;
			}
			*ptr++ = c;
		}

		/*
		 * If the buffer is full, force the next character to be read to
		 * be a <newline>.
		 */
		if (ptr == &envbuf[sizeof (envbuf)-1]) {
			(void) ungetc('\n', stdin);
			(void) putc('\n', stdout);
		}
	}

	/*
	 * If we left loop because an EOF was received, exit immediately.
	 */
	login_exit(0);
	/* NOTREACHED */
}

/*
 * get_user_name	- Gets the user name either passed in, or from the
 *			  login: prompt.
 */

static void
get_user_name()
{
	FILE	*fp;
	int gotname = 0;

	if ((fp = fopen(ISSUEFILE, "r")) != NULL) {
		char    *ptr, buffer[BUFSIZ];
		while ((ptr = fgets(buffer, sizeof (buffer),
					fp)) != NULL) {
			(void) fputs(ptr, stdout);
		}
		(void) fclose(fp);
	}

	/*
	 * if TTYPROMPT is not set, use our own prompt
	 * otherwise, use ttyprompt. We just set PAM_USER_PROMPT
	 * and let the module do the prompting.
	 */

	if ((ttyprompt == NULL) || (*ttyprompt == '\0'))
		(void) pam_set_item(pamh, PAM_USER_PROMPT, (void *)loginmsg);
	else
		(void) pam_set_item(pamh, PAM_USER_PROMPT, (void *)ttyprompt);

	envp = &zero; /* XXX: is this right? */
}


/*
 * Check_for_dueling_unix   -	Check to see if the another login is talking
 *				to the line we've got open as a login port
 *				Exits if we're talking to another unix system
 */

static void
check_for_dueling_unix(char *inputline)
{
	if (EQN(loginmsg, inputline) || EQN(passwdmsg, inputline) ||
	    EQN(incorrectmsg, inputline)) {
		(void) printf("Looking at a login line.\n");
		login_exit(8);
	}
}

/*
 * logins_disabled - 	if the file /etc/nologin exists and the user is not
 *			root then do not permit them to login
 */
static int
logins_disabled(char *user_name)
{
	FILE	*nlfd;
	int	c;
	if (!EQN("root", user_name) &&
			((nlfd = fopen(NOLOGIN, "r")) != (FILE *)NULL)) {
		while ((c = getc(nlfd)) != EOF)
			putchar(c);
		(void) fflush(stdout);
		sleep(5);
		return (TRUE);
	}
	return (FALSE);
}

/*
 * check_for_root_user	- Checks if we're getting a root login on the console
 *			  Exits if root login not on system console.
 *
 */

static void
check_for_root_user(void)
{
	if (pwd->pw_uid == 0) {
		if ((Console != NULL) && (strcmp(ttyn, Console) != 0)) {
			audit_login_not_console();
			(void) printf("Not on system console\n");
			login_exit(10);
		}
	}
}


static char *illegal[] = {
	"SHELL=",
	"HOME=",
	"LOGNAME=",
#ifndef	NO_MAIL
	"MAIL=",
#endif
	"CDPATH=",
	"IFS=",
	"PATH=",
	0
};

/*
 * legalenvvar		- Is it legal to insert this environmental variable?
 */

static int
legalenvvar(char *s)
{
	char **p;

	for (p = illegal; *p; p++)
		if (strncmp(s, *p, strlen(*p)) == 0)
			return (0);

	if (s[0] == 'L' && s[1] == 'D' && s[2] == '_')
		return (0);

	return (1);
}


/*
 * getstr		- Get a string from standard input
 *			  Calls exit if read(2) fails.
 */

static void
getstr(char *buf, int cnt, char *err)
{
	char c;

	do {
		if (read(0, &c, 1) != 1)
			login_exit(1);
		*buf++ = c;
	} while (cnt > 1 && c != 0);

	*buf = 0;
	err = err; 	/* For lint */
}


/*
 * uppercaseterm 	- if all input char are upper case
 *			  set the corresponding termio
 */

static void
uppercaseterm(char *strp)
{
	int 	upper = 0;
	int 	lower = 0;
	char	*sp;
	struct	termio	termio;

	for (sp = strp; *sp; sp++) {
		if (islower(*sp))
			lower++;
		else if (isupper(*sp))
			upper++;
	}

	if (upper > 0 && lower == 0) {
		(void) ioctl(0, TCGETA, &termio);
		termio.c_iflag |= IUCLC;
		termio.c_oflag |= OLCUC;
		termio.c_lflag |= XCASE;
		(void) ioctl(0, TCSETAW, &termio);
		for (sp = strp; *sp; sp++)
			if (*sp >= 'A' && *sp <= 'Z') *sp += ('a' - 'A');
	}
}


/*
 * defaults 		- read defaults
 */

static void
defaults(void)
{
	extern int defcntl();
	int  flags;
	char *ptr;

	if (defopen(Pndefault) == 0) {
		/*
		 * ignore case
		 */
		flags = defcntl(DC_GETFLAGS, 0);
		TURNOFF(flags, DC_CASE);
		defcntl(DC_SETFLAGS, flags);

		if ((Console = defread("CONSOLE=")) != NULL)
			Console = strdup(Console);

		if ((Altshell = defread("ALTSHELL=")) != NULL)
			Altshell = strdup(Altshell);

		if ((Passreq = defread("PASSREQ=")) != NULL)
			Passreq = strdup(Passreq);

		if ((Def_tz = defread("TIMEZONE=")) != NULL)
			Def_tz = strdup(Def_tz);

		if ((Def_hertz = defread("HZ=")) != NULL)
			Def_hertz = strdup(Def_hertz);

		if ((Def_path   = defread("PATH=")) != NULL)
			Def_path = strdup(Def_path);

		if ((Def_supath = defread("SUPATH=")) != NULL)
			Def_supath = strdup(Def_supath);

		if ((ptr = defread("ULIMIT=")) != NULL)
			Def_ulimit = atol(ptr);

		if ((ptr = defread("TIMEOUT=")) != NULL)
			Def_timeout = (unsigned) atoi(ptr);

		if ((ptr = defread("UMASK=")) != NULL) {
		        long long_umask;
			if (sscanf(ptr, "%lo", &long_umask) != 1)
				Umask = DEFUMASK;
			else
				Umask = (mode_t) long_umask;
		}

		if ((ptr = defread("SLEEPTIME=")) != NULL)
			Sleeptime = atoi(ptr);

		if ((ptr = defread("SYSLOG=")) != NULL)
			dosyslog = strcmp(ptr, "YES") == 0;

		(void) defopen((char *)NULL);
	}
}


/*
 * get_options(argc, argv)
 * 				- parse the cmd line.
 *				- return 0 if successful, -1 if failed.
 *				Calls login_exit() on misuse of -r and -h flags
 */

static	int
get_options(int argc, char *argv[])
{
	int	c;
	int	errflg = 0;

	while ((c = getopt(argc, argv, "f:h:r:pad:")) != -1) {
		switch (c) {
			case 'a':
				break;

			case 'd':
				/*
				 * Must be root to pass in device name
				 * otherwise we exit() as punishment for trying.
				 */
				if (getuid() != 0 || geteuid() != 0) {
					login_exit(1);	/* sigh */
					/*NOTREACHED*/
				}
				ttyn = optarg;
				break;

			case 'h':
				if (hflag || rflag) {
					(void) fprintf(stderr,
					    "Only one of -r and -h allowed\n");
					login_exit(1);
				}
				hflag++;
				SCPYN(remote_host, optarg);
				if (argv[optind]) {
					if (argv[optind][0] != '-')
						SCPYN(terminal, argv[optind]);
					optind++;
				}
				progname = "telnet";
				break;

			case 'r':
				if (hflag || rflag) {
					(void) fprintf(stderr,
					    "Only one of -r and -h allowed\n");
					login_exit(1);
				}
				rflag++;
				SCPYN(remote_host, optarg);
				progname = "rlogin";
				break;

			case 'p':
				pflag++;
				break;

			case 'f':
				/*
				 * Must be root to bypass authentication
				 * otherwise we exit() as punishment for trying.
				 */
				if (getuid() != 0 || geteuid() != 0) {
					login_exit(1);	/* sigh */
					/*NOTREACHED*/
				}
				/* save fflag user name for future use */
				SCPYN(user_name, optarg);
				fflag = 1;
				break;
			default:
				errflg++;
				break;
		} 	/* end switch */
	} 		/* end while */

	/*
	 * get the prompt set by ttymon
	 */
	ttyprompt = getenv("TTYPROMPT");

	if ((ttyprompt != NULL) && (*ttyprompt != '\0')) {
		/*
		 * if ttyprompt is set, there should be data on
		 * the stream already.
		 */
		if ((envp = getargs(inputline)) != (char **)NULL) {
			uppercaseterm(*envp);
			/*
			 * don't get name if name passed as argument.
			 */
			SCPYN(user_name, *envp++);
		}
	} else if (optind < argc) {
		SCPYN(user_name, argv[optind]);
		(void) strcpy(inputline, user_name);
		(void) strcat(inputline, "   \n");
		envp = &argv[optind+1];
	}

	if (errflg)
		return (-1);
	return (0);
}

/*
 * usage		- Print usage message
 *
 */

static void
usage(void)
{
	(void) fprintf(stderr,
	    "Usage:\tlogin [-h|-r] [ name [ env-var ... ]]\n");
}

/*
 * 			*** Remote login support ***
 *
 */


/*
 * doremoteterm		- Sets the appropriate ioctls for a remote terminal
 */

static char	*speeds[] = {
	"0", "50", "75", "110", "134", "150", "200", "300",
	"600", "1200", "1800", "2400", "4800", "9600", "19200", "38400",
	"57600", "76800", "115200", "153600", "230400", "307200", "460800"
};

#define	NSPEEDS	(sizeof (speeds) / sizeof (speeds[0]))


static void
doremoteterm(char *term)
{
	struct termios tp;
	char *cp = strchr(term, '/'), **cpp;
	char *speed;

	(void) ioctl(0, TCGETS, &tp);

	if (cp) {
		*cp++ = '\0';
		speed = cp;
		cp = strchr(speed, '/');

		if (cp)
			*cp++ = '\0';

		for (cpp = speeds; cpp < &speeds[NSPEEDS]; cpp++)
			if (strcmp(*cpp, speed) == 0) {
				cfsetospeed(&tp, cpp-speeds);
				break;
			}
	}

	tp.c_lflag |= ECHO|ICANON;
	tp.c_iflag |= IGNPAR|ICRNL;

	(void) ioctl(0, TCSETS, &tp);

}


/*
 * Process_rlogin		- Does the work that rlogin and telnet
 *				  need done
 */

static void
process_rlogin(void)
{

	getstr(rusername, sizeof (rusername), "remuser");
	getstr(lusername, sizeof (lusername), "locuser");
	getstr(terminal, sizeof (terminal), "Terminal type");

	/* fflag has precedence over stuff passed by rlogind */
	if (fflag || getuid()) {
		pwd = &nouser;
		doremoteterm(terminal);
		return;
	} else {
		if (pam_set_item(pamh, PAM_USER, lusername) != PAM_SUCCESS)
			login_exit(1);
		pwd = getpwnam(lusername);
		if (pwd == NULL) {
			pwd = &nouser;
			doremoteterm(terminal);
			return;
		}
	}

	doremoteterm(terminal);

	/*
	 * Update PAM on the user name
	 */
	if (pam_set_item(pamh, PAM_USER, lusername) != PAM_SUCCESS)
		login_exit(1);

	if (pam_set_item(pamh, PAM_RUSER, rusername) != PAM_SUCCESS)
		login_exit(1);

	SCPYN(user_name, lusername);
	envp = &zero;
	lusername[0] = '\0';
}

/*
 *		*** Account validation routines ***
 *
 */

/*
 * validate_account		- This is the PAM version of validate.
 */

static void
validate_account()
{
	int 	error;
	int	n;
	int	flag;

	(void) alarm(0);	/* give user time to come up with password */

	check_log();

	if ((Passreq != NULL) && (!strcasecmp("YES", Passreq)))
		flag = PAM_DISALLOW_NULL_AUTHTOK;
	else
		flag = 0;

	if ((error = pam_acct_mgmt(pamh, flag)) != PAM_SUCCESS) {
		if (error == PAM_AUTHTOKEN_REQD) {
			(void) printf("Choose a new password.\n");

			if ((error = pam_chauthtok(pamh, 0)) != PAM_SUCCESS) {
				if (dosyslog)
					syslog(LOG_CRIT,
						"change password failure: %s",
						pam_strerror(pamh, error));
				login_exit(1);
			}
		} else {
			(void) printf(incorrectmsg);
			if (dosyslog)
				syslog(LOG_CRIT,
					"login account failure: %s",
						pam_strerror(pamh, error));
			login_exit(1);
		}
	}
}

/*
 * Check_log	- This is really a hack because PAM checks the log, but login
 *		  wants to know if the log is okay and PAM doesn't have
 *		  a module independent way of handing this info back.
 */

static void
check_log(void)
{
	int fdl;
	long long offset;

	offset = (long long) pwd->pw_uid * (long long) sizeof (struct lastlog);

	if ((fdl = open(LASTLOG, O_RDWR|O_CREAT, 0444)) >= 0) {
		if (llseek(fdl, offset, SEEK_SET) == offset &&
		    read(fdl, (char *)&ll, sizeof (ll)) == sizeof (ll) &&
		    ll.ll_time != 0)
			lastlogok = 1;
		(void) close(fdl);
	}
}


/*
 * chdir_to_dir_root	- Attempts to chdir us to the home directory.
 *			  defaults to "/" if it can't cd to the home
 *			  directory, and returns ERROR if it can't do that.
 */

static int
chdir_to_dir_root(void)
{
	if (chdir(pwd->pw_dir) < 0) {
		if (chdir("/") < 0) {
			(void) printf("No directory!\n");
			return (ERROR);
		}
	}

	return (OK);
}


/*
 * chdir_to_dir_user	- Now chdir after setuid/setgid have happened to
 *			  place us in the user's home directory just in
 *			  case it was protected and the first chdir failed.
 *			  No chdir errors should happen at this point because
 *			  all failures should have happened on the first
 *			  time around.
 */

static void
chdir_to_dir_user(void)
{
	if (chdir(pwd->pw_dir) < 0) {
		if (chdir("/") < 0) {
			(void) printf("No directory!\n");
			/*
			 * This probably won't work since we can't get to /.
			 */
			if (remote_host[0] && dosyslog) {
				syslog(LOG_CRIT,
				"REPEATED LOGIN FAILURES ON %s FROM %.*s",
				ttyn, HMAX, remote_host);
			} else if (dosyslog) {
				syslog(LOG_CRIT,
				"REPEATED LOGIN FAILURES ON %s", ttyn);
			}
			closelog();
			(void) sleep(DISABLETIME);
			exit(1);
		} else {
			(void) printf("No directory! Logging in with home=/\n");
			pwd->pw_dir = "/";
		}
	}
}


/*
 * login_authenticate	- Performs the main authentication work
 *			  1. Prints the login prompt
 *			  2. Requests and verifys the password
 *			  3. Checks the port password
 */

static void
login_authenticate()
{
	int cnt = 1;
	char *user;
	int err;

	do {
		/* if scheme broken, then nothing to do but quit */
		if (pam_get_item(pamh, PAM_USER, (void **)&user)
							!= PAM_SUCCESS)
			exit(1);

		/*
		 * only get name from utility if it is not already
		 * supplied by pam_start or a pam_set_item.
		 */
		if (!user || !user[0]) {
			/* use call back to get user name */
			get_user_name();
		}

		err = verify_passwd();
		if ((err == PAM_SUCCESS) && (chdir_to_dir_root() == OK)) {
			cnt = 0;
			break;
		}

		/* scheme has kept count of retries. time to bail */
		if (err == PAM_MAXTRIES)
			cnt = MAXTRYS;

		/* only sleep after first bad passwd */
		if (cnt)
			(void) sleep(Sleeptime);
		(void) printf(incorrectmsg);
		user_name[0] = '\0';	/* is this needed? */
		/* force name to be null in this case */
		if (pam_set_item(pamh, PAM_USER, NULL) != PAM_SUCCESS)
			login_exit(1);
		if (pam_set_item(pamh, PAM_RUSER, NULL) != PAM_SUCCESS)
			login_exit(1);
	} while (cnt++ < MAXTRYS);

	if (cnt >= MAXTRYS) {
		audit_login_maxtrys();
		/*
		 * If logging is turned on, output the
		 * string storage area to the log file,
		 * and sleep for DISABLETIME
		 * seconds before exiting.
		 */
		if (writelog)
			badlogin();
		if (remote_host[0] && dosyslog) {
			syslog(LOG_CRIT,
				"REPEATED LOGIN FAILURES ON %s FROM %.*s",
				ttyn, HMAX, remote_host);
		} else if (dosyslog) {
			syslog(LOG_CRIT,
				"REPEATED LOGIN FAILURES ON %s", ttyn);
		}
		(void) sleep(DISABLETIME);
		exit(1);
	}

}

/*
 * 			*** Credential Related routines ***
 *
 */

/*
 * setup_credentials		- sets the group ID, initializes the groups
 *				  and sets up the secretkey.
 *				  Exits if a failure occurrs.
 */


/*
 * setup_credentials		- PAM does all the work for us on this one.
 */

static void
setup_credentials(void)
{
	int 	error = 0;
	int	flags;

	/* set the real (and effective) GID */
	if (setgid(pwd->pw_gid) == -1) {
		login_exit(1);
	}

	/*
	 * Initialize the supplementary group access list.
	 */
	if (!user_name)
		login_exit(1);
	if (initgroups(user_name, pwd->pw_gid) == -1) {
		login_exit(1);
	}

	/* XXX really should be after setgid */
	if ((error = pam_setcred(pamh, PAM_ESTABLISH_CRED)) != PAM_SUCCESS) {
		login_exit(error);
	}

	/* set the real (and effective) UID */
	if (setuid(pwd->pw_uid) == -1) {
		login_exit(1);
	}

}

/*
 *
 *		*** Routines to get a new user set up and running ***
 *
 *			Things to do when starting up a new user:
 *				adjust_nice
 *				update_utmp_entry
 *				establish_user_environment
 *				print_banner
 *				display_last_login_time
 *				exec_the_shell
 *
 */


/*
 * adjust_nice		- Set the nice (process priority) value if the
 *			  gecos value contains an appropriate value.
 */

static void
adjust_nice(void)
{
	int pri, mflg, i;

	if (strncmp("pri=", pwd->pw_gecos, 4) == 0) {
		pri = 0;
		mflg = 0;
		i = 4;

		if (pwd->pw_gecos[i] == '-') {
			mflg++;
			i++;
		}

		while (pwd->pw_gecos[i] >= '0' && pwd->pw_gecos[i] <= '9')
			pri = (pri * 10) + pwd->pw_gecos[i++] - '0';

		if (mflg)
			pri = -pri;

		(void) nice(pri);
	}
}

/*
 * update_utmp_entry	- Searchs for the correct utmp entry, making an
 *			  entry there if it finds one, otherwise exits.
 */

static void
update_utmp_entry(int sublogin)
{
	char	*user;
	int	error;
	static char	*errmsg	= "No utmpx entry. "
		"You must exec \"login\" from the lowest level \"shell\".";
	int	   tmplen;
	struct utmpx  *u = (struct utmpx *)0;
	struct utmpx  utmp;
	char	  *ttyntail;


	/*
	 * If we're not a sublogin then
	 * we'll get an error back if our PID doesn't match the PID of the
	 * entry we are updating, otherwise if its a sublogin the flags
	 * field is set to 0, which means we just write a matching entry
	 * (without checking the pid), or a new entry if an entry doesn't
	 * exist.
	 */

	if (error = pam_open_session(pamh, 0) != PAM_SUCCESS) {
		login_exit(1);
	}

	if ((error = pam_get_item(pamh, PAM_USER, (void **) &user))
							!= PAM_SUCCESS) {
		login_exit(1);
	}

	(void) memset((void *)&utmp, 0, sizeof (utmp));
	(void) time(&utmp.ut_tv.tv_sec);
	utmp.ut_pid = getpid();

	if (rflag || hflag) {
		SCPYN(utmp.ut_host, remote_host);
		tmplen = strlen(remote_host) + 1;
		if (tmplen < sizeof (utmp.ut_host))
			utmp.ut_syslen = tmplen;
		else
			utmp.ut_syslen = sizeof (utmp.ut_host);
	} else {
		utmp.ut_syslen = 0;
	}

	SCPYN(utmp.ut_user, user);

	/* skip over "/dev/" */
	ttyntail = basename(ttyn);

	while ((u = getutxent()) != NULL) {
		if ((u->ut_type == INIT_PROCESS ||
			u->ut_type == LOGIN_PROCESS ||
			u->ut_type == USER_PROCESS) &&
			((sublogin && strncmp(u->ut_line, ttyntail,
			sizeof (u->ut_line)) == 0) ||
			u->ut_pid == utmp.ut_pid)) {
			SCPYN(utmp.ut_line, (ttyn+sizeof ("/dev/")-1));
			(void) memcpy(utmp.ut_id, u->ut_id,
							sizeof (utmp.ut_id));
			utmp.ut_type = USER_PROCESS;
			pututxline(&utmp);
			break;
		}
	}
	endutxent();

	if (u == (struct utmpx *)NULL) {
		if (!sublogin) {
			/*
			 * no utmp entry already setup
			 * (init or rlogind/telnetd)
			 */
			(void) puts(errmsg);
			login_exit(1);
		}
	} else {
		/* Now attempt to write out this entry to the wtmp file if */
		/* we were successful in getting it from the utmp file and */
		/* the wtmp file exists.				   */
		updwtmpx(WTMPX_FILE, &utmp);
	}
}



/*
 * process_chroot_logins 	- Chroots to the specified subdirectory and
 *				  re executes login.
 */

static int
process_chroot_logins(void)
{
	/*
	 * If the shell field starts with a '*', do a chroot to the home
	 * directory and perform a new login.
	 */

	if (*pwd->pw_shell == '*') {
		pam_end(pamh, PAM_SUCCESS);	/* Done using PAM */
		if (chroot(pwd->pw_dir) < 0) {
			(void) printf("No Root Directory\n");
			return (ERROR);
		}
		/*
		 * Set the environment flag <!sublogin> so that the next login
		 * knows that it is a sublogin.
		 */
		envinit[0] = SUBLOGIN;
		envinit[1] = (char *)NULL;
		(void) printf("Subsystem root: %s\n", pwd->pw_dir);
		(void) execle("/usr/bin/login", "login", (char *)0,
			&envinit[0]);
		(void) execle("/etc/login", "login", (char *)0, &envinit[0]);
		(void) printf("No /usr/bin/login or /etc/login on root\n");
		login_exit(1);
	}
	return (OK);
}

/*
 * establish_user_environment	- Set up the new users environment
 */

static void
establish_user_environment(char **renvp)
{
	int i, j, k, l_index, length;
	char *ptr;
	char *endptr;
	char **lenvp;
	char *krb5p;
	char *krb4p;

	lenvp = environ;
	while (*lenvp++)
		;
	envinit = (char **) calloc(lenvp - environ + 10
		+ MAXARGS, sizeof (char *));
	if (envinit == NULL) {
		(void) printf("Calloc failed - out of swap space.\n");
		login_exit(8);
	}
	memcpy(envinit, newenv, sizeof (newenv));

	/* Set up environment */
	if (rflag) {
		ENVSTRNCAT(term, terminal);
	} else if (hflag) {
		/* XXX: this causes an environment variable with the value */
		/* "" to be created later on, because terminal is "". */
		(void) strncpy(term, terminal, sizeof (term) - 1);
	} else {
		char *tp = getenv("TERM");

		if ((tp != NULL) && (*tp != '\0'))
			ENVSTRNCAT(term, tp);
	}

	ENVSTRNCAT(logname, pwd->pw_name);

	/*
	 * There are three places to get timezone info.  init.c sets
	 * TZ if the file /etc/TIMEZONE contains a value for TZ.
	 * login.c looks in the file /etc/default/login for a
	 * variable called TIMEZONE being set.  If TIMEZONE has a
	 *  value, TZ is set to that value; no environment variable
	 * TIMEZONE is set, only TZ.  If neither of these methods
	 * work to set TZ, then the library routines  will default
	 * to using the file /usr/lib/locale/TZ/localtime.
	 *
	 * There is a priority set up here.  If /etc/TIMEZONE has
	 * a value for TZ, that value remains top priority.  If the
	 * file /etc/default/login has TIMEZONE set, that has second
	 * highest priority not overriding the value of TZ in
	 * /etc/TIMEZONE.  The reason for this priority is that the
	 * file /etc/TIMEZONE is supposed to be sourced by
	 * /etc/profile.  We are doing the "sourcing" prematurely in
	 * init.c.  Additionally, a login C shell doesn't source the
	 * file /etc/profile thus not sourcing /etc/TIMEZONE thus not
	 * allowing an administrator to globally set TZ for all users
	 */
	if (Def_tz != NULL)	/* Is there a TZ from defaults/login? */
		tmp_tz = Def_tz;

	if ((Def_tz = getenv("TZ")) != NULL) {
		ENVSTRNCAT(timez, Def_tz);
	} else if (tmp_tz != NULL) {
		Def_tz = tmp_tz;
		ENVSTRNCAT(timez, Def_tz);
	}

	if (Def_hertz == NULL)
		(void) sprintf(hertz + strlen(hertz), "%u", HZ);
	else
		ENVSTRNCAT(hertz, Def_hertz);

	if (Def_path == NULL)
		(void) strcat(path, DEF_PATH);
	else
		ENVSTRNCAT(path, Def_path);

	ENVSTRNCAT(home, pwd->pw_dir);

	/*
	 * Find the end of the basic environment
	 */
	for (basicenv = 0; envinit[basicenv] != NULL; basicenv++)
		;

	/*
	 * If TZ has a value, add it.
	 */
	if (strcmp(timez, "TZ=") != 0)
		envinit[basicenv++] = timez;

	if (*pwd->pw_shell == '\0') {
		/*
		 * If possible, use the primary default shell,
		 * otherwise, use the secondary one.
		 */
		if (access(SHELL, X_OK) == 0)
			pwd->pw_shell = SHELL;
		else
			pwd->pw_shell = SHELL2;
	} else if (Altshell != NULL && strcmp(Altshell, "YES") == 0) {
		envinit[basicenv++] = shell;
		ENVSTRNCAT(shell, pwd->pw_shell);
	}

#ifndef	NO_MAIL
	envinit[basicenv++] = mail;
	(void) strcat(mail, pwd->pw_name);
#endif

	/*
	 * Pick up locale environment variables, if any.
	 */
	lenvp = renvp;
	while (*lenvp != NULL) {
		j = 0;
		while (localeenv[j] != 0) {
			/*
			 * locale_envmatch() returns 1 if
			 * *lenvp is localenev[j] and valid.
			 */
			if (locale_envmatch(localeenv[j], *lenvp) == 1) {
				envinit[basicenv++] = *lenvp;
				break;
			}
			j++;
		}
		lenvp++;
	}

	/*
	 * If '-p' flag, then try to pass on allowable environment
	 * variables.  Note that by processing this first, what is
	 * passed on the final "login:" line may over-ride the invocation
	 * values.  XXX is this correct?
	 */
	if (pflag) {
		for (lenvp = renvp; *lenvp; lenvp++) {
			if (!legalenvvar(*lenvp)) {
				continue;
			}
			/*
			 * If this isn't 'xxx=yyy', skip it.  XXX
			 */
			if ((endptr = strchr(*lenvp, '=')) == NULL) {
				continue;
			}
			length = endptr + 1 - *lenvp;
			for (j = 0; j < basicenv; j++) {
				if (strncmp(envinit[j], *lenvp, length) == 0) {
					/*
					 * Replace previously established value
					 */
					envinit[j] = *lenvp;
					break;
				}
			}
			if (j == basicenv) {
				/*
				 * It's a new definition, so add it at the end.
				 */
				envinit[basicenv++] = *lenvp;
			}
		}
	}

	/*
	 * Add in all the environment variables picked up from the
	 * argument list to "login" or from the user response to the
	 * "login" request.
	 */

	for (j = 0, k = 0, l_index = 0, ptr = &envblk[0];
		*envp && j < (MAXARGS-1); j++, envp++) {

		/*
		 * Scan each string provided.  If it doesn't have the
		 * format xxx=yyy, then add the string "Ln=" to the beginning.
		 */
		if ((endptr = strchr(*envp, '=')) == NULL) {
			envinit[basicenv+k] = ptr;
			(void) sprintf(ptr, "L%d=%s", l_index, *envp);

			/*
			 * Advance "ptr" to the beginning of the
			 * next argument.
			 */
			while (*ptr++)
				;
			k++;
			l_index++;
		} else  {
			if (!legalenvvar(*envp)) { /* this env var permited? */
				continue;
			} else {

				/*
				 * Check to see whether this string replaces
				 * any previously defined string
				 */
				for (i = 0, length = endptr + 1 - *envp;
					i < basicenv + k; i++) {
				    if (strncmp(*envp, envinit[i], length)
						== 0) {
					envinit[i] = *envp;
					break;
				    }
				}

				/*
				 * If it doesn't, place it at the end of
				 * environment array.
				 */
				if (i == basicenv+k) {
					envinit[basicenv+k] = *envp;
					k++;
				}
			}
		}
	}		/* for (j = 0 ... ) */

	/*
	 * Add DCE/Kerberos cred name, if any.
	 * XXX - The module specific stuff should be removed from login
	 * program eventually. This is better placed in DCE module.
	 * So far, we haven't come up with a way to deal with these
	 * environment variables.
	 */

	krb5p = getenv("KRB5CCNAME");

	if ((krb5p != NULL) && (*krb5p != '\0')) {
		ENVSTRNCAT(krb5ccname, krb5p);
		envinit[basicenv++] = krb5ccname;
	}

	krb4p = getenv("KRBTKFILE");

	if ((krb4p != NULL) && (*krb4p != '\0')) {
		ENVSTRNCAT(krb4ccname, krb4p);
		envinit[basicenv++] = krb4ccname;
	}

	/*
	 * Switch to the new environment.
	 */
	environ = envinit;
}

/*
 * print_banner		- Print the banner at start up
 *			   Do not turn on DOBANNER ifdef.  This is not
 *			   relevant to SunOS.
 */

static void
print_banner(void)
{
#ifdef DOBANNER
	uname(&un);
#if i386
	(void) printf("UNIX System V/386 Release %s\n%s\n"
	    "Copyright (C) 1984, 1986, 1987, 1988 AT&T\n"
	    "Copyright (C) 1987, 1988 Microsoft Corp.\nAll Rights Reserved\n",
		un.release, un.nodename);
#elif sun
	(void) printf("SunOS Release %s Sun Microsystems %s\n%s\n"
	    "Copyright (c) 1984, 1986, 1987, 1988 AT&T\n"
	    "Copyright (c) 1988, 1989, 1990, 1991 Sun Microsystems\n"
	    "All Rights Reserved\n",
		un.release, un.machine, un.nodename);
#else
	(void) printf("UNIX System V Release %s AT&T %s\n%s\n"
	    "Copyright (c) 1984, 1986, 1987, 1988 AT&T\nAll Rights Reserved\n",
		un.release, un.machine, un.nodename);
#endif /* i386 */
#endif /* DOBANNER */
}

/*
 * display_last_login_time	- Advise the user the time and date
 *				  that this login-id was last used.
 */

static void
display_last_login_time(void)
{
	if (lastlogok) {
		(void) printf("Last login: %.*s ", 24-5, ctime(&ll.ll_time));

		if (*ll.ll_host != '\0')
			(void) printf("from %.*s\n", sizeof (ll.ll_host),
					    ll.ll_host);
		else
			(void) printf("on %.*s\n", sizeof (ll.ll_line),
					    ll.ll_line);
	}
}

/*
 * exec_the_shell	- invoke the specified shell or start up program
 */

static void
exec_the_shell(void)
{
	char *endptr;
	int i;

	(void) strcat(minusnam, basename(pwd->pw_shell));

	/*
	 * Exec the shell
	 */
	(void) execl(pwd->pw_shell, minusnam, (char *)0);

	/*
	 * pwd->pw_shell was not an executable object file, maybe it
	 * is a shell proceedure or a command line with arguments.
	 * If so, turn off the SHELL= environment variable.
	 */
	for (i = 0; envinit[i] != NULL; ++i) {
		if ((envinit[i] == shell) &&
		    ((endptr = strchr(shell, '=')) != NULL))
			(*++endptr) = '\0';
		}

	if (access(pwd->pw_shell, R_OK|X_OK) == 0) {
		(void) execl(SHELL, "sh", pwd->pw_shell, (char *)0);
		(void) execl(SHELL2, "sh", pwd->pw_shell, (char *)0);
	}

	(void) printf("No shell\n");
}

/*
 * login_exit		- Call exit()  and terminate.
 *			  This function is here for PAM so cleanup can
 *			  be done before the process exits.
 */
static void
login_exit(int exit_code)
{
	if (pamh)
		pam_end(pamh, PAM_ABORT);
	exit(exit_code);
	/*NOTREACHED*/
}

/*
 * Check if lenv and penv matches or not.
 */
static int
locale_envmatch(char *lenv, char *penv)
{
	while ((*lenv == *penv) && *lenv && *penv != '=') {
		lenv++;
		penv++;
	}

	/*
	 * '/' is eliminated for security reason.
	 */
	if (*lenv == '\0' && *penv == '=' && *(penv + 1) != '/')
		return (1);
	return (0);
}

/*
 * logindevperm - change owner/group/permissions of devices
 * list in /etc/logindevperm.  (Code derived from set_fb_attrs()
 * in 4.x usr/src/bin/login.c and usr/src/etc/getty/main.c.)
 */

#define	MAX_LINELEN 	256
#define	LOGINDEVPERM	"/etc/logindevperm"
#define	DIRWILD		"/*"			/* directory wildcard */
#define	DIRWLDLEN	2			/* strlen(DIRWILD) */

static void
logindevperm(char *ttyn, uid_t uid, gid_t gid)
{
	char *field_delims = " \t\n";
	char *permfile = LOGINDEVPERM;
	char line[MAX_LINELEN];
	char *console;
	char *mode_str;
	char *dev_list;
	char *device;
	char *ptr;
	int mode;
	FILE *fp;
	size_t l;
	int lineno;

	if ((fp = fopen(permfile, "r")) == NULL)
		return;

	lineno = 0;
	while (fgets(line, MAX_LINELEN, fp) != NULL) {
		lineno++;

		if ((ptr = strchr(line, '#')) != NULL)
			*ptr = '\0';	/* handle comments */

		if ((console = strtok(line, field_delims)) == NULL)
			continue;	/* ignore blank lines */

		if (strcmp(console, ttyn) != 0)
			continue;	/* not our tty, skip */

		mode_str = strtok((char *)NULL, field_delims);
		if (mode_str == NULL) {
			(void) fprintf(stderr,
			    "%s: line %d, invalid entry -- %s\n", permfile,
			    lineno, line);
			continue;
		}

		/* convert string to octal value */
		mode = strtol(mode_str, &ptr, 8);
		if (mode < 0 || mode > 0777 || *ptr != '\0') {
			(void) fprintf(stderr,
			    "%s: line %d, invalid mode -- %s\n", permfile,
			    lineno, mode_str);
			continue;
		}

		dev_list = strtok((char *)NULL, field_delims);
		if (dev_list == NULL) {
			(void) fprintf(stderr,
			    "%s: line %d, %s -- empty device list\n",
			    permfile, lineno, console);
			continue;
		}

		device = strtok(dev_list, ":");
		while (device != NULL) {
			l = strlen(device);
			ptr = &device[l - DIRWLDLEN];
			if ((l > DIRWLDLEN) && (strcmp(ptr, DIRWILD) == 0)) {
				*ptr = '\0';	/* chop off wildcard */
				dir_dev_acc(device, uid, gid, mode, permfile);
			} else {
				/*
				 * change the owner/group/permission;
				 * nonexistent devices are ignored
				 */
				if (chown(device, uid, gid) == -1) {
					if (errno != ENOENT) {
						(void) fprintf(stderr, "%s: ",
						    permfile);
						perror(device);
					}
				} else {
					if ((chmod(device, mode) == -1) &&
					    (errno != ENOENT)) {
						(void) fprintf(stderr, "%s: ",
						    permfile);
						perror(device);
					}
				}
			}
			device = strtok((char *)NULL, ":");
		}
	}
	(void) fclose(fp);
}

/*
 * Apply owner/group/perms to all files (except "." and "..")
 * in a directory.
 */
static void
dir_dev_acc(char *dir, uid_t uid, gid_t gid, mode_t mode, char *permfile)
{
	DIR *dirp;
	struct dirent *direntp;
	char *name, path[MAX_LINELEN + MAXNAMELEN];

	dirp = opendir(dir);
	if (dirp == NULL)
		return;

	while ((direntp = readdir(dirp)) != NULL) {
		name = direntp->d_name;
		if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0))
			continue;

		(void) sprintf(path, "%s/%s", dir, name);
		if (chown(path, uid, gid) == -1) {
			if (errno != ENOENT) {
				(void) fprintf(stderr, "%s: ", permfile);
				perror(path);
			}
		} else {
			if ((chmod(path, mode) == -1) && (errno != ENOENT)) {
				(void) fprintf(stderr, "%s: ", permfile);
				perror(path);
			}
		}
	}
	(void) closedir(dirp);
}
