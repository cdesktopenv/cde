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
/* $TOG: DtTermLogit.c /main/6 1998/07/23 18:08:59 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/times.h>
#include <pwd.h>

#include <sys/utsname.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define	LOG_HOST	"hpcvusj.cv.hp.com"
#define	LOG_ADDR	"15.0.209.35"
#define	LOG_PORT	"4444"
#define	WAIT_INTERVAL	5
#define	RETRIES		5

/*
#define	DEBUG_LOGIT
*/
#ifdef	DEBUG_LOGIT
#define	logitmain	main
#endif	/* DEBUG_LOGIT */

#ifdef	LOG_USAGE
void LogStart(int noFork, int argc, char **argv);
void LogFinish(int noFork, int sessions);
void LogBumpSessionCount(int count);
static void doLog(int noFork, char *msg);
static void ding(int sig);
static void autoLogFinish();

static time_t startTime;
static char *id = (char *) 0;
static int sessionCount = 0;
static int savedNoFork = 0;
static int sequence = 0;

void
LogBumpSessionCount(int count)
{
    sessionCount += count;
}

void
LogStart(int noFork, int argc, char **argv)
{
    char buffer[BUFSIZ];
    char *c1;
    char *c2;
    char *tstring;
    struct utsname uts;
    struct passwd *pw;
    int i1;

    if (getenv("DTTERM_NOLOG")) {
	return;
    }

    /* save away noFork... */
    savedNoFork = noFork;

    *buffer = '\0';
    (void) strcat(buffer, "START");
    (void) strcat(buffer, " USER=\"");
    if (c1 = getlogin()) {
	(void) strcat(buffer, c1);
    } else {
	if ((pw = getpwuid(getuid())) && pw->pw_name && *pw->pw_name) {
	    (void) strcat(buffer, pw->pw_name);
	} else {
	    (void) strcat(buffer, "???");
	}
    }

    (void) strcat(buffer, "\"" );
    if (uname(&uts) != -1) {
	(void) strcat(buffer, " UNAME=\"");
	(void) strcat(buffer, uts.sysname);
	(void) strcat(buffer, " ");
	(void) strcat(buffer, uts.nodename);
	(void) strcat(buffer, " ");
	(void) strcat(buffer, uts.release);
	(void) strcat(buffer, " ");
	(void) strcat(buffer, uts.version);
	(void) strcat(buffer, " ");
	(void) strcat(buffer, uts.machine);
	(void) strcat(buffer, "\"");
    }

    (void) time(&startTime);
    tstring = ctime(&startTime);
    /* remove the trailing '\n'... */
    tstring[strlen(tstring) - 1] = '\0';
    (void) strcat(buffer, " TIME=\"");
    (void) strcat(buffer, tstring);
    (void) strcat(buffer, "\"");

    (void) strcat(buffer, " ARGS=\"");
    c1 = buffer + strlen(buffer);
    for (i1 = 0; i1 < argc; i1++) {
	if (i1 > 0) {
	    *c1++ = ' ';
	}

	for (c2 = argv[i1]; *c2; c2++) {
	    if (iscntrl(*c2)) {
		*c1++ = '^';
		*c1++ = '@' + *c2;
	    } else if (isprint(*c2)) {
		switch (*c2) {
		case '"' :
		case '\\' :
		case ' ' :
		    *c1++ = '\\';
		    *c1++ = *c2;
		    break;

		default:
		    *c1++ = *c2;
		    break;
		}
	    } else {
		*c1++ = '\\';
		*c1++ = '0' + (*c2 / 0100) & 07;
		*c1++ = '0' + (*c2 / 0010) & 07;
		*c1++ = '0' + (*c2 / 0001) & 07;
	    }
	}
    }
    *c1++ = '"';
    *c1++ = '\0';

    (void) doLog(noFork, buffer);
    atexit(autoLogFinish);
}

void
LogFinish(int noFork, int sessions)
{
    char buffer[BUFSIZ];
    char buffer2[BUFSIZ];
    time_t now;
    long cpuTime;
    int i1;
    struct tms tms;
    long clkTick = 0;

    if (getenv("DTTERM_NOLOG")) {
	return;
    }

    *buffer = '\0';
    (void) strcat(buffer, "FINISH");

    (void) sprintf(buffer2, " SESSIONS=\"%d\"", sessions);
    (void) strcat(buffer, buffer2);

    (void) time(&now);
    (void) sprintf(buffer2, " ELAPSED=\"%ld\"", now - startTime);
    (void) strcat(buffer, buffer2);

    (void) times(&tms);

    clkTick = sysconf(_SC_CLK_TCK);
    if (clkTick) {
	cpuTime = (tms.tms_utime = tms.tms_stime) / clkTick;
    } else {
	cpuTime = -1;
    }
	
    (void) sprintf(buffer2, " CPU=\"%ld\"", cpuTime);
    (void) strcat(buffer, buffer2);

    (void) doLog(noFork, buffer);
}

static void
autoLogFinish()
{
    (void) LogFinish(savedNoFork, sessionCount);
}

static void
doLog(int noFork, char *msg)
{
    char *sbuffer;
    char rbuffer[BUFSIZ];
    char thisId[BUFSIZ];
    static int s = -1;
    int i1;
    int len;
    char *c1;
    int retries = RETRIES;
    struct hostent *hp;
    static struct sockaddr_in myaddr_in;
    static struct sockaddr_in servaddr_in;
    struct sigaction sa;
    struct sigaction oldSa;
    time_t now;
    pid_t pid;
    int doRead;

    sbuffer = malloc(2*BUFSIZ);
    if (!id) {
	(void) time(&now);
	(void) sprintf(sbuffer, "%ld.%ld", now, (long)getpid());
	id = strdup(sbuffer);
    }
	
    /* bump the sequence number before we fork()...  */
    (void) sequence++;

    if (noFork) {
	pid = 0;
    } else {
	pid = fork();
    }

    if (pid != 0) {
	return;
    }

    if (s < 0) {
	(void) memset(&servaddr_in, '\0', sizeof(servaddr_in));
	(void) memset(&myaddr_in, '\0', sizeof(myaddr_in));

	/* set up the server address... */
	servaddr_in.sin_family = AF_INET;
	hp = gethostbyname((c1 = getenv("DTTERM_LOG_HOST")) ? c1 : LOG_HOST);

	if (hp) {
	    servaddr_in.sin_addr.s_addr =
		    ((struct in_addr *)(hp->h_addr))->s_addr;
	} else {
	    servaddr_in.sin_addr.s_addr =
		    inet_addr((c1 = getenv("DTTERM_LOG_ADDR")) ? c1 : LOG_ADDR);
	}
	    
	servaddr_in.sin_port =
		atoi((c1 = getenv("DTTERM_LOG_PORT")) ? c1 : LOG_PORT);

	/* create the socket... */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
	    if (!noFork) {
		(void) _exit(1);
	    }
	    return;
	}

	/* bind to some local address so we can get a reply... */
	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_port = 0;
	myaddr_in.sin_addr.s_addr = INADDR_ANY;
	if (bind(s, &myaddr_in, sizeof(myaddr_in)) == -1) {
	    if (!noFork) {
		(void) _exit(1);
	    }
	    (void) close(s);
	    s = -1;
	    return;
	}
    }

    if (!noFork) {
	for (i1 = 0; i1 < _NFILE; i1++) {
	    if (i1 != s) {
		(void) close(i1);
	    }
	}
    }

    (void) sprintf(thisId, "ID=\"%s.%d\"", id, sequence);
    (void) sprintf(sbuffer, "%s %s", thisId, msg);

    /* set up a signal handler... */
    (void) sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = ding;

    (void) sigaction(SIGALRM, &sa, &oldSa);

    /* try to send the message... */
    while (retries > 0) {
	if (sendto(s, sbuffer, strlen(sbuffer), 0, &servaddr_in,
		sizeof(servaddr_in)) < 0) {
	    if (!noFork) {
		(void) _exit(1);
	    }
	    (void) close(s);
	    s = -1;
	    return;
	}

	/* set a timeout... */
	(void) alarm(WAIT_INTERVAL);

	for (doRead = 1; doRead; ) {
	    if ((len = recv(s, rbuffer, sizeof(rbuffer) -1, 0)) < 0) {
		if (errno == EINTR) {
		    if (--retries < 0) {
			/* give up... */
			if (!noFork) {
			    (void) _exit(1);
			}
			(void) alarm(0);
			(void) close(s);
			s = -1;
			return;
		    }

		    /* we need to resend before doing another read... */
		    doRead = 0;
		} else {
		    /* give up... */
		    if (!noFork) {
			(void) _exit(1);
		    }
		    (void) alarm(0);
		    (void) close(s);
		    s = -1;
		    return;
		}
	    } else {
		/* got ack... */
		(void) alarm(0);

		/* null term the string... */
		rbuffer[len] = '\0';

		/* compare it against the id... */
		if (!strncmp(rbuffer, thisId, strlen(thisId))) {
		    /* match... */
		    doRead = 0;
		    retries = 0;
		    break;
		}
		/* ignore it and get the next one... */
	    }
	}
    }

    /* clear the alarm and re-install the old signal handler... */
    (void) alarm(0);
    (void) sigaction(SIGALRM, &oldSa, (struct sigaction *) 0);
    if (!noFork) {
	_exit(0);
    }
    free(sbuffer);
    return;
}
		    
static void
ding(int sig)
{
    /* since we are using sigaction, we don't need to reinstall
     * ourself...
     */
}

#ifdef	DEBUG_LOGIT
int
logitmain(int argc, char **argv)
{
    int i1;
    int noFork = 0;
    extern char *optarg;
    extern int optind, optopt;

    while(EOF != (i1 = getopt(argc, argv, "f"))) {
	switch(i1) {
	case 'f' :
	    noFork = 1;
	    break;
	}
    }

    (void) LogStart(noFork, argc, argv);
    (void) sleep(5);
    /*
    (void) LogFinish(noFork, 123);
    */
}
#endif	/* DEBUG_LOGIT */
#endif	/* LOG_USAGE */
