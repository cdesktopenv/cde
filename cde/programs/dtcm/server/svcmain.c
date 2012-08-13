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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: svcmain.c /main/10 1998/04/06 13:13:49 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#if defined(linux)
#define _POSIX_C_SOURCE 2
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#ifdef SVR4
#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif /* _NETINET_IN_H */
#endif

#if defined(SunOS) || defined(USL) || defined(__uxp__)
#include <netconfig.h>
#include <netdir.h>
#include <sys/stropts.h>
#include <tiuser.h>
#endif /* SunOS || USL || __uxp__ */
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rpc/rpc.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <pwd.h>
#include <grp.h>
#include "rpcextras.h"
#include "log.h"
#include "cmscalendar.h"
#include "repeat.h"
#include "lutil.h"
#include "cmsdata.h"

#ifndef	S_IRWXU
#define	S_IRWXU		(S_IRUSR|S_IWUSR|S_IXUSR)
#endif
#ifndef	S_IRWXG
#define	S_IRWXG		(S_IRGRP|S_IWGRP|S_IXGRP)
#endif
#ifndef	S_IRWXO
#define	S_IRWXO		(S_IROTH|S_IWOTH|S_IXOTH)
#endif

#define S_MASK  (S_INPUT|S_HIPRI|S_ERROR|S_HANGUP)

int debug;
static int standalone;			/* default is 0 */
static int received_sighup = 0;		/* 1 means we get SIGHUP */
static int rpc_in_process = 0;		/* 1 means processing client request */
static int garbage_collection_time = 240; /* in min; default time is 4:00am */

char	*pgname;

uid_t daemon_uid;
gid_t daemon_gid;

/*
 * get garbage collection time
 * the given string should be in the format hhmm
 * where hh is 0 - 23 and mm is 00 - 59
 */
static int
_GetGtime(char *timestr)
{
	int	hour, minute, len, i;

	if (timestr == NULL)
		goto error;

	if ((len = strlen(timestr)) > 4)
		goto error;

	for (i = 0; i < len; i++) {
		if (timestr[i] < '0' || timestr[i] > '9')
			goto error;
	}

	minute = atoi(&timestr[len - 2]);
	timestr[len - 2] = NULL;
	hour = atoi(timestr);

	if (hour > 23 || minute > 59)
		goto error;

	garbage_collection_time = hour * 60 + minute;

	return (0);

error:
	fprintf(stderr, "The time specified is invalid.\n");
	return (-1);
}

static void
parse_args(int argc, char **argv)
{
	int	opt;

	if (pgname = strrchr (argv[0], '/'))
		pgname++;
	else
		pgname = argv[0];

	while ((opt = getopt (argc, argv, "dsg:")) != -1)
	{
		switch (opt)
		{
		case 'd':
			debug = 1;
			break;
		case 's':
			standalone = 1;
			break;
		case 'g':
			if (_GetGtime(optarg))
				goto error;
			break;
		case '?':
			goto error;
		}
	}

	if (optind == argc)
		return;

#if defined(_aix)
	/*
	 * rpc.cmsd gets started by the inetd.
	 * On AIX inetd requires that two arguments be supplied to the RPC
	 * programs as follows (from the inetd.conf man page):
	 *
	 * ServerArgs      Specifies the command line arguments that the
	 * inetd daemon should use to execute the server. The maximum number
	 * of arguments is five. The first argument specifies the name of the
	 * server used.  If the SocketType parameter is sunrpc_tcp or
	 * sunrpc_udp, * the second argument specifies the program name and
	 * the third argument specifies the version of the program. For
	 * services that the inetd daemon provides internally, this field
	 * should be empty.
	 */

	else if (optind == 1 && argc >= 3)
	{
		int i,j;
		char **argv_r;
		
		if (argc == 3)
		  return;
		  
		argv_r = (char **) malloc(argc * sizeof(char *));

		argv_r[0] = argv[0];
		for (i=optind+2, j=1; i<argc; i++,j++)
		  argv_r[j] = argv[i];
		parse_args(argc-2, argv_r);

		free((void *) argv_r);
		return;
	}
#endif
		
error:
	fprintf (stderr, "Usage: %s [-d] [-s] [-g hhmm]\n", pgname);
	exit (-1);
}

static void
init_dir()
{
	char *dir = _DtCMS_DEFAULT_DIR;
	char msgbuf[BUFSIZ];
	int create_dir;
	struct stat info;
	mode_t mode;

	if (geteuid() != 0)
	{
		fprintf (stderr,
			"%s: must be run in super-user mode!  Exited.\n",
			pgname);
		exit (-1);
	}

	create_dir = 0;
	if (stat(dir, &info))
	{
		/* if directory does not exist, create the directory */
		if ((errno != ENOENT) || mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO))
		{
			if (errno == ENOENT)
				sprintf(msgbuf, "%s: cannot create %s.\n%s: %s",
					pgname, dir, pgname, "System error");
			else
				sprintf(msgbuf, "%s: cannot access %s.\n%s: %s",
					pgname, dir, pgname, "System error");
			perror (msgbuf);
			exit (-1);
		}
		create_dir = 1;
	}

	/* if dir is just created, we need to do chmod and chown.
	 * Otherwise, only do chmod and/or chown if permssion and/or
	 * ownership is wrong.
	 */
	mode = S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO;

	if (create_dir || info.st_mode != (mode | S_IFDIR)) {

		/* set directory permission to be "rwxrwsrwt" */
		if (chmod(dir, mode)) {
			sprintf(msgbuf, "%s: Permission on %s%s\n%s%s\n%s%s",
				pgname, dir,
				" is wrong but cannot be corrected.", pgname,
				": This might happen if you are mounting the directory.",
				pgname, ": System error");
			perror(msgbuf);
			if (create_dir)
				rmdir(dir);
			exit(-1);
		}
	}

	if (create_dir || info.st_uid!=daemon_uid || info.st_gid!=daemon_gid) {
		/* set directory ownership to: owner = 1, group = 1 */
		if (chown(dir, daemon_uid, daemon_gid)) {
			sprintf(msgbuf, "%s: Ownership on %s%s\n%s%s\n%s%s",
				pgname, dir,
				" is wrong but cannot be corrected.", pgname,
				": This might happen if you are mounting the directory.",
				pgname, ": System error");
			perror(msgbuf);
			if (create_dir)
				rmdir(dir);
			exit(-1);
		}
	}

	/* Change current directory, so core file can be dumped. */
	chdir (dir);
}

/*
 * send a SIGHUP signal to the rpc.cmsd that is already running
 */
static void
send_hup()
{
	FILE	*fp = NULL;
	char	buf[BUFSIZ];
	pid_t	pid, mypid = getpid();
	extern FILE *popen(const char *, const char *);
	extern int pclose(FILE *);

	sprintf(buf, "ps -e|grep rpc.cmsd|grep -v grep");

	if ((fp = popen(buf, "r")) == NULL) {
		if (debug)
			fprintf(stderr, "rpc.cmsd: popen failed\n");
	} else {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if ((pid = atol(buf)) != mypid) {
				if (kill(pid, SIGHUP))
				    perror("rpc.cmsd: failed to send SIGHUP");
				if (debug)
				    fprintf(stderr, "rpc.cmsd: %s %ld\n",
						"sent SIGHUP to", (long)pid);
			}
		}
		pclose(fp);
	}
}

/*
 * We only allow one rpc.cmsd to run on each machine.
 */
static int
lock_it()
{
	char *dir = _DtCMS_DEFAULT_DIR;
	char	buff [MAXPATHLEN];
	int	error;
	int	fd;
#ifdef SVR4
	struct flock locker;
	locker.l_type = F_WRLCK;
	locker.l_whence = 0;
	locker.l_start = 0;
	locker.l_len = 0;
#endif /* SVR4 */

	strcpy (buff, dir);
	strcat (buff, "/.lock.");

	/* 
	 * /var/spool might be mounted.  Use .lock.hostname to
	 * prevent more than one cms running in each host.
	 */
	strcat(buff, _DtCmGetLocalHost());

	fd = open(buff, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	if (fd < 0)
	{
		perror (buff);
		exit (-1);
	}

	/*
	 * Note, we have to use flock() instead of lockf() because cms process
	 * is run in each host.
	 */
#ifdef SVR4
	if (fcntl (fd, F_SETLK, &locker) != 0)
#else
	if (flock (fd, LOCK_EX|LOCK_NB) != 0)
#endif /* SVR4 */
	{
		error = errno;

		close(fd);

		if (error != EWOULDBLOCK && error != EACCES) {

			perror ("rpc.cmsd: failed to lock lockfile");
			fprintf(stderr, "error = %d\n", error);
			exit (-1);

		} else {
			if (debug)
				fprintf(stderr, "rpc.cmsd: %s\n",
				    "lock_it failed due to another process");
			
			/* cms has been running.... */
			return(error);
		}
	}

	return (0);
}

static void
program(struct svc_req *rqstp, register SVCXPRT *transp)
{
        char *result;
        char *argument = NULL;
        program_handle ph = getph();
        struct rpcgen_table *proc;

	/* set rpc_in_process so that sighup handler won't exit right away */
	rpc_in_process = 1;
 
        /* first do some bounds checking: */
        if (rqstp->rq_vers >= ph->nvers) {
                svcerr_noproc(transp);
                goto done;
        }
        if (ph->prog[rqstp->rq_vers].nproc == 0) {
                svcerr_noproc(transp);
                goto done;
        }
        if (rqstp->rq_proc >= ph->prog[rqstp->rq_vers].nproc) {
                svcerr_noproc(transp);
                goto done;
        }

	if (rqstp->rq_proc == NULLPROC) {
		if (debug) fprintf(stderr, "rpc.cmsd: ping\n");
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
		goto done;
	}

        /* assert - the program number, version and proc numbers are valid */
        proc = &(ph->prog[rqstp->rq_vers].vers[rqstp->rq_proc]);
        argument = (char*)calloc(proc->len_arg, sizeof(char));
        if (!svc_getargs(transp, proc->xdr_arg, argument)) {
                svcerr_decode(transp);
                goto done;
        }

        result = (*proc->proc)(argument, rqstp);
        if (result != NULL && !svc_sendreply(transp, proc->xdr_res, result)) {
                svcerr_systemerr(transp);
        }

        if (!svc_freeargs(transp, proc->xdr_arg, argument)) {
                (void)fprintf(stderr, "unable to free arguments");
                exit(1);
        }
        free(argument);

done:
	rpc_in_process = 0;

	/* exit if we have received the SIGHUP signal */
	if (received_sighup == 1) {
		if (debug)
			fprintf(stderr, "rpc.cmsd: received SIGHUP, %s",
				"exiting after finished processing\n");
		exit(0);
	}
}

/*
 * Signal handler for SIGHUP.
 * If we are in the middle of processing a client request,
 * finish processing before we exit.
 */
static void
sighup_handler(int sig_num)
{
	if (debug)
		fprintf(stderr, "rpc.cmsd: sighup received\n");

	if (rpc_in_process == 0) {
		if (debug)
			fprintf(stderr, "rpc.cmsd: exit from sighup_handler\n");
		exit(0);
	} else {
		if (debug)
			fprintf(stderr, "rpc.cmsd: set received_sighup to 1\n");
		received_sighup = 1;
	}
}

/*
 * garbage_collection_time (in min) is the time to do garbage collection
 * each day
 * This routine returns the difference between the first garbage collection
 * time and now so that the calling routine can set the alarm.
 */
static int
_GetFirstGarbageCollectionTime()
{
	int n=0, midnight=0, gtime=0;

	n = time(0);

	/* try today first */
	midnight = next_ndays(n, 0);
	gtime = next_nmins(midnight, garbage_collection_time);

	if (gtime < n) {
		/* the first garbage collection will be done tomorrow */
		midnight = next_ndays(n, 1);
		gtime = next_nmins(midnight, garbage_collection_time);
	}

	return (gtime - n);
}

static void
init_alarm()
{
	int next;
	extern void garbage_collect();
	extern void debug_switch();

#if defined(SVR4) && !defined(linux)
	extern void (*sigset(int, void (*)(int)))(int);
	sigset(SIGUSR1, garbage_collect);
	sigset(SIGALRM, garbage_collect);
	sigset(SIGUSR2, debug_switch);
#else
	signal(SIGUSR1, garbage_collect);
	signal(SIGALRM, garbage_collect);
	signal(SIGUSR2, debug_switch);
#endif /* SVR4 */

	next = _GetFirstGarbageCollectionTime();
	alarm((unsigned) next);
}

main(int argc, char **argv)
{
        u_long version;
        program_handle ph = newph();
	struct passwd *pw;
	struct group *gr;
	struct rlimit rl;
	struct sockaddr_in saddr;
	int asize = sizeof (saddr);
	SVCXPRT *tcp_transp = (SVCXPRT *)-1;
	SVCXPRT *udp_transp = (SVCXPRT *)-1;
	int	fd, error;

#if defined(SunOS) || defined(USL) || defined(__uxp__)
	struct netconfig *nconf_udp;
	struct netconfig *nconf_tcp;
	struct t_info info;
#if !defined(USL) || (defined(USL) && (OSMAJORVERSION > 1))
	char mname[FMNAMESZ+1];
#endif
#endif /* SunOS || USL */

	pw = (struct passwd *)getpwnam("daemon");
	gr = (struct group *)getgrnam("daemon");
	if (pw != NULL) 
		daemon_uid = pw->pw_uid;
	else
		daemon_uid = 1;
	if (gr != NULL)
		daemon_gid = gr->gr_gid;
	else 
		daemon_gid = 1;

	parse_args(argc, argv);

	/* check to see if we are started by inetd */
	if (getsockname(0, (struct sockaddr *)&saddr, &asize) == 0) {

		standalone = 0;

#if defined(SunOS) || defined(USL) || defined(__uxp__)
#if !defined(USL) || (defined(USL) && (OSMAJORVERSION > 1))
		/* we need a TLI endpoint rather than a socket */
		if (ioctl(0, I_LOOK, mname) != 0) {
			perror("rpc.cmsd: ioctl failed to get module name");
			exit(1);
		}
		if (strcmp(mname, "sockmod") == 0) {
			/* Change socket fd to TLI fd */
			if (ioctl(0, I_POP, 0) || ioctl(0, I_PUSH, "timod")) {
				perror("rpc.cmsd: ioctl I_POP/I_PUSH failed");
				exit(1);
			}
		} else if (strcmp(mname, "timod") != 0) {
			fprintf(stderr, "rpc.cmsd: fd 0 is not timod\n");
			exit(1);
		}
#else  /* !USL || (USL && OSMAJORVERSION > 1) */
		if (ioctl(0, I_POP, 0) || ioctl(0, I_PUSH, "timod")) {
			perror("rpc.cmsd: ioctl I_POP/I_PUSH failed");
			exit(1);
		}
#endif /* !USL || (USL && OSMAJORVERSION > 1) */

	} else if (t_getinfo(0, &info) == 0) {
		standalone = 0;

#endif /* SunOS || USL */

	} else
		standalone = 1;

	/*
	 * if it is started by inetd, make stderr to be
	 * output to console.
	 */
	if (!standalone) {
		if ((fd = open ("/dev/console", O_WRONLY)) >= 0) {
			if (fd != 2) {
				dup2(fd, 2);
				close (fd);
			}
		}
	}

	/* Set up private directory and switch euid/egid to daemon. */
	umask (S_IWOTH);
	init_dir();

	/* Don't allow multiple cms processes running in the same host. */
	if ((error = lock_it()) != 0 && !standalone) {
		/* we are invoked by inetd but another rpc.cmsd
		 * is alreay running, so send SIGHUP to it
		 */

		send_hup();

		/* try to lock it again */
		if (lock_it() != 0) {
			if (debug)
				fprintf(stderr, "cm: rpc.cmsd is still running\n");
			exit(0);
		}

	} else if (error != 0) {
		fprintf(stderr, "rpc.cmsd: rpc.cmsd is already running.\n");
		exit(0);
	}

	/* use signal because we only need it once */
	signal(SIGHUP, sighup_handler);


#if defined(SunOS) || defined(USL) || defined(__uxp__)
	/* raise the soft limit of number of file descriptor */
	/* this is to prevent the backend from running out of open file des */
	getrlimit(RLIMIT_NOFILE, &rl);
	rl.rlim_cur = (rl.rlim_max <= 256) ? rl.rlim_max : 256;
	setrlimit(RLIMIT_NOFILE, &rl);
#endif

#if defined(SunOS) || defined(USL) || defined(__uxp__)
	nconf_udp = getnetconfigent("udp");
	nconf_tcp = getnetconfigent("tcp");

	for (version = 0; version < ph->nvers; version++) {
		/* don't register unsupported versions: */
		if (ph->prog[version].nproc == 0) continue;

		if (standalone) {
			rpcb_unset(ph->program_num, version, NULL);
			if (debug)
				fprintf(stderr,
					"rpc.cmsd: rpcb_unset for version %ld\n",
					version);
		}

		/* brought up by inetd, use fd 0 which must be a TLI fd */
		if (udp_transp == (SVCXPRT *)-1) {
			udp_transp = svc_tli_create(standalone ? RPC_ANYFD : 0,
				nconf_udp, (struct t_bind*) NULL, 0, 0); 

			if (udp_transp == NULL) {
				t_error("rtable_main.c: svc_tli_create(udp)");
				exit(2);
			}
		}

		if (svc_reg(udp_transp, ph->program_num, version, program,
				standalone ? nconf_udp : NULL) == 0) {
			t_error("rtable_main.c: svc_reg");
			exit(3);
		}

		/* Set up tcp for calls that potentially return */
		/* large amount of data.  This transport is not */
		/* registered with inetd so need to register it */
		/* with rpcbind ourselves.			*/

		rpcb_unset(ph->program_num, version, nconf_tcp);

		if (tcp_transp == (SVCXPRT *)-1) {
			tcp_transp = svc_tli_create(RPC_ANYFD, nconf_tcp,
					(struct t_bind *)NULL, 0, 0);

			if (tcp_transp == NULL) {
				t_error("rtable_main.c: svc_til_create(tcp)");
				exit(2);
			}
		}

		if (svc_reg(tcp_transp, ph->program_num, version, program,
				nconf_tcp) == 0) {
			t_error("rtable_main.c: svc_reg(tcp)");
			exit(3);
		}
	}/*for*/

	if (nconf_udp)
		freenetconfigent(nconf_udp);
	if (nconf_tcp)
		freenetconfigent(nconf_tcp);

#else

	for (version = 0; version < ph->nvers; version++) {
		/* don't register unsupported versions: */
		if (ph->prog[version].nproc == 0) continue;

#ifndef HPUX
		if (standalone)
#endif
			(void) pmap_unset(ph->program_num, version);

		if (udp_transp == (SVCXPRT *)-1) {
			udp_transp = svcudp_create(standalone ? RPC_ANYSOCK : 0
#if defined(_AIX) || defined(hpV4) || defined(__osf__) || defined(linux) || \
	defined(CSRG_BASED)
					);
#else
					,0,0);
#endif
			if (udp_transp == NULL) {
				(void)fprintf(stderr,
				"rtable_main.c: cannot create udp service.\n");
				exit(1);
                	}
		}

#ifndef HPUX
		if (!svc_register(udp_transp, ph->program_num, version, program,
				standalone ? IPPROTO_UDP : 0)) {
#else
		if (!svc_register(udp_transp, ph->program_num, version, program,
				IPPROTO_UDP)) {
#endif
			(void)fprintf(stderr, "rtable_main.c: unable to register");
			exit(1);
		}

		/* Set up tcp for calls that potentially return */
		/* large amount of data.  This transport is not */
		/* registered with inetd so need to register it */
		/* with rpcbind ourselves.			*/

		if (tcp_transp == (SVCXPRT *)-1) {
			tcp_transp = svctcp_create(RPC_ANYSOCK, 0, 0);
			if (tcp_transp == NULL) {
				(void)fprintf(stderr,
				"rtable_main.c: cannot create tcp service.\n");
				exit(1);
			}
		}

		if (!svc_register(tcp_transp, ph->program_num, version, program,
				IPPROTO_TCP)) {
			(void)fprintf(stderr, "rtable_main.c: unable to register(tcp)");
			exit(1);
		}
	}

#endif /* SunOS || USL */

#ifndef AIX
#ifdef HPUX
	setgid (daemon_gid);
	setuid (daemon_uid);
#else
	setegid (daemon_gid);
	seteuid (daemon_uid);
#endif /* HPUX */
#endif /* AIX */

	init_time();
	init_alarm();
	_DtCm_init_hash();

	svc_run();

	(void)fprintf(stderr, "rpc.cmsd: svc_run returned\n");
	return(1);
}

