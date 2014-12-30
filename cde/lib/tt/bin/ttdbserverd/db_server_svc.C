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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
//%%  (c) Copyright 1993, 1994 Novell, Inc.
//%%  $TOG: db_server_svc.C /main/9 1999/10/12 10:01:35 mgreess $ 			 				
/*
 *  @(#)db_server_svc.C	1.54 95/06/07
 *
 *  Tool Talk Database Server
 *
 *  Copyright (c) 1992 Sun Microsystems, Inc.
 *
 *  This file contains the db server main rpc program _tt_dbserver_prog_1.
 */
#define PORTMAP

#include "tt_options.h"
#include "tt_db_partition_redirection_map.h"
#include "util/tt_global_env.h"
#include "util/tt_new.h"
#include "util/copyright.h"
#include "dm_access_cache.h"
#include "dm/dm_enums.h"
#include "db/db_server.h"
#include "db/tt_db_rpc_routines.h"
#include "tt_db_msg_q_lock.h"
#include "util/tt_log.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_file_system.h"
#include "db_server_globals.h"
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <rpc/pmap_clnt.h>
#include <isam.h>     
#include <unistd.h>
#include <sys/socket.h>
#include <termios.h>
#include <time.h>

#if defined(sgi) || defined(CSRG_BASED)
#include <getopt.h>
#endif

#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <signal.h>

#if defined(OPT_TLI)
#include <stropts.h>
#endif

#if (OPT_GARBAGE_IN_PARALLEL==1) && !defined(OPT_GARBAGE_THREADS)
#include <sys/wait.h>
#endif

#define _TT_TREC_INFO (sizeof(int) + sizeof(long) + sizeof(u_int))

extern "C" { int isclose(int); }
extern "C" { int iscntl(int, int, ...); }
extern "C" { int isopen(const char*, int); }
extern "C" { int isrewrec(int, long, char*); }
extern "C" { int iswrite(int, char*); }
extern char *optarg;
extern int opterr;
extern int _tt_run_garbage_collect(int in_parallel);
extern int _tt_run_garbage_collect(int);
static void _tt_dbserver_prog_1(struct svc_req*, SVCXPRT*);
void install_signal_handler();
void sig_handler(int sig);

#if defined(TTDB_DEBUG)
#define TTDB_DEBUG_SYSLOG(txt)	_tt_syslog(errstr, LOG_ERR, txt);
#else
#define TTDB_DEBUG_SYSLOG(txt)
#endif

TT_INSERT_COPYRIGHT

const char	*_TT_UNIX_OPT = "unix";
const char	*_TT_DES_OPT = "des";
const int	_TT_UMASK = 0022;	/* rwxr-xr-x max permissions. */

/*
 * GLOBAL variables
 */
uid_t	_tt_uid;
gid_t	_tt_gid;
gid_t	_tt_gidlist[NGROUPS];

#if defined(OPT_GARBAGE_THREADS)
mutex_t 	rpc_client_busy; // Used to sync calls with the RPC clients.
mutex_t		garbage_run_in_process;
#endif

int	_tt_auth_level;
int	_tt_gidlen;

keydesc				_tt_oid_keydesc;
_Tt_oid_access_queue_ptr	_tt_oa_cache;
_Tt_link_access_queue_ptr	_tt_la_cache;
char				_tt_log_file[MAXPATHLEN];

/* table of NetISAM db opened */
_Tt_db_info			_tt_db_table[_TT_MAX_ISFD];

// _tt_refclock is a "pseudo clock" that's just incremented every time
// a reference is made to an fd; the resulting value is placed 
// the _tt_db_table[fd].reftime field, so we can figure out which 
// fd is Least Recently Used when it comes time to re-use one.
int		_tt_refclock=0;
time_t		_tt_mtab_last_mtime;
static char	_tt_log_buf[ISMAXRECLEN];
static char	_tt_target_db[MAXPATHLEN];
int		_tt_debug_mode = 0;
int		access_checking = 1;

static bool_t	msg_q_unlock_flag = FALSE;
static bool_t	refresh_partition_redirection_map = TRUE;

// DB partition redirection map
_Tt_db_partition_redirection_map	* db_pr_map;

// Sink for error output.  If 0, output goes to syslog.
FILE		* errstr = stderr;

extern int	optind;

void sig_handler(int sig);

enum {STARTED_FROM_INETD, STARTED_FROM_SHELL} start_mode;

//
// This is used to hold the next time the automatic garbage
// collection should be run. It will be run when the current
// time is greator than or equal to next_garbage_run.
//
time_t	next_garbage_run = (time_t) 0;

int
isamfatalerror(char *msg)
{
	_tt_syslog(errstr, LOG_ERR, "NetISAM: %s", msg);
	return 1;
}

void
print_usage_and_exit()
{
	static int usage_printed = 0;

	if (!usage_printed) {

		_tt_syslog(errstr, LOG_ERR, "%s",
			   catgets(_ttcatd, 5, 7,
"Usage:\n"
"rpc.ttdbserverd [-S] [-n] [-m DTMOUNTPOINT]\n"
"rpc.ttdbserverd [-S] [-v]\n"
"rpc.ttdbserverd -G\n"));

		usage_printed = 1;

	}

	// we only exit when started from the shell -- if started by
	// inetd we're likely to just get started up again with bad options,
	// probably better to try to keep going.

	if (start_mode == STARTED_FROM_SHELL) {
		exit(1);
	}
	return;
}

int
#if defined(OPT_GARBAGE_THREADS)
main(int argc, char** argv)
#else
main(int argc, char** argv, char **envp)
#endif
{
#if !defined(OPT_GARBAGE_THREADS)
	global_argv = argv;
	global_envp = envp;
#endif
	char *progname;
	void install_signal_handler();
	SVCXPRT *transp;
	struct sockaddr_in saddr;
#if defined(DEC) || defined(HPUX)
	int asize = sizeof(saddr);
#else
# if defined(linux) || defined(CSRG_BASED) || defined(sun)
	socklen_t asize = sizeof(saddr);
# else
	size_t asize = sizeof(saddr);
# endif
#endif
	int is_aix = 0;

	int do_garbage_collect = 0;

#if defined(OPT_TLI)
	char mname[FMNAMESZ + 1];
	struct t_info info;
#endif

	memset(&saddr, 0, sizeof(saddr));

#if defined(OPT_TLI)
	memset((char *)&info,0,sizeof info);
#endif

#ifdef _AIX
        is_aix = 1;
#endif

	setlocale( LC_ALL, "" );
	_tt_openlog( argv[0], LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_DAEMON );

// First, we have to determine if we are started from inetd or from the shell.
// Normal installations would start from inetd, but we do start from the shell
// when testing; also we\'d like to allow installations to start out of the
// init process (rc.local, rc?.d) if they\'d rather.

// The way to determine if we\'re started from inetd is to check if fd 0 is
// a connection endpoint (socket or TLI).  If so, that endpoint has been
// set up as an RPC connection by inetd and we should use it instead of
// creating our own.  If fd 0 is NOT a endpoint, we assume we\'re started
// from the shell, and create a new endpoint and redirect the portmapper to it.

// This is complicated by the fact that the inetd.conf file in Solaris 2.3
// and Solaris 2.3. specifies that rpc.ttdbserverd is rpc/tcp, not rpc/tli.
// We want this version of rpc.ttdbserverd to run on Solaris 2.3, so we have
// to cater to it by checking for a tcp socket being passed in; if there is
// such, we do some streams magic to convert it into a TLI endpoint.

// This is further complicated by having to run on systems that don\'t
// have TIRPC yet.

        signal(SIGTTOU, SIG_IGN);
	if (getsockname(0, (struct sockaddr *) &saddr, &asize) == 0) {
	
		//  We were given a socket.  This means  we were started
		// from inetd.
 		if (saddr.sin_family != AF_INET) {
 			_tt_syslog(0, LOG_ERR, "saddr.sin_family != AF_INET");
 			exit(1);
 		}

		start_mode = STARTED_FROM_INETD;
		errstr = 0;

#if defined(OPT_TLI)

		// We were given a socket but need a TLI endpoint.  
		// Fortunately STREAMS caters to folks like us by making
		// it dead easy to change one into another.
 
 		if (!ioctl(0, I_LOOK, mname) && (!strcmp(mname, "sockmod"))) {
 			if (ioctl(0, I_POP, 0) || ioctl(0, I_PUSH, "timod")) {
 				_tt_syslog(errstr, LOG_ERR, "ioctl(0, I_POP, 0)"
					   " || ioctl(0, I_PUSH, \"timod\")");
 				exit(1);
 			}
 		}
	
	} else if (0==t_getinfo(0, &info)) {
		
		// We were given a TLI endpoint. This means we were started
		// from inetd.  The TLI endpoint is suitable for later use.

		start_mode = STARTED_FROM_INETD;
		errstr = 0;

#endif /* defined(OPT_TLI) */

	} else {

		// fd 0 is not a communications endpoint, so we must not
		// have been started from inetd.

		start_mode = STARTED_FROM_SHELL;
	}
		

	// At this point start_mode is set to tell us who started us.
	// If start_mode is STARTED_FROM_INETD, then fd 0 is the 
	// endpoint to use; if OPT_TLI is defined, fd 0 is a TLI endpoint;
	// if OPT_TLI is not defined, fd 0 is a socket.

	// it might be considered appropriate to fork into the background
	// here if started from the shell here, but we never have before,
	// we are usually started from inetd.  The usual reason we\'re 
	// started from a a shell is for debugging, where forking into the
	// background is a big pain, and we\'d have to have an option like
	// the -S one on ttsession to skip the fork.

        _tt_global = new _Tt_global;


	// Create the DB parition redirection map
	db_pr_map = new _Tt_db_partition_redirection_map;

	/* set minimum authentication requirement */
        _tt_auth_level = AUTH_UNIX;	/* default to UNIX authentication */

	progname = *argv;

	if (argc>1) {
		opterr = 0;
		int c;
// remove authentication option
//		int c = getopt(argc, argv, "vm:t:dna:");
		while ((c = getopt(argc, argv, "vm:t:dnSG")) != -1) {
			switch (c) {
			      case 'v':
				_TT_PRINT_VERSIONS(progname)
				exit(0);
			      case 'm':
				_tt_putenv("DTMOUNTPOINT", optarg);
				break;
			      case 'n':
				access_checking = 0;
				break;
			      case 'a':
				// this is now disabled (see comment above)
				if (strcmp(optarg, _TT_DES_OPT) == 0) {
					_tt_auth_level = AUTH_DES;
				} else if (strcmp(optarg, _TT_UNIX_OPT) != 0) {
					print_usage_and_exit();
				}
				break;
			      case 't':
				break;
			      case 'd':
				break;
			      case 'S':
				_tt_debug_mode = 1;
				break;
			      case 'G':
				do_garbage_collect = 1;
				break;
			      case '?':
			      default:
				print_usage_and_exit();
			}
		}

        	// The, er, unusual inetd.conf syntax on the IBM platform
        	// makes it necessary to *not* check for extraneous
		// command-line args on IBM when started out of inetd.
		// Since I need to check the value of a cpp macro
		// -- _AIX -- and a program variable -- start_mode --
		// use another "temp" program variable -- is_aix.
	
        	if (optind < argc &&
		    !(is_aix && start_mode == STARTED_FROM_INETD))
		{
			print_usage_and_exit();
		}
        }

	//
	// Start the garbage collection running.
	//
	if (do_garbage_collect) {

		//
		// As the garbage collection flag is set, we
		// start the garbage cleanup only (no server-d).
		//
		_tt_run_garbage_collect(FALSE);// Not in parallel.
	}

	//
	// Lock down the RPC interface until we are ready.
	//
	LOCK_RPC();

	/* setup oid key descriptor for access control */
	
	_tt_oid_keydesc.k_flags = ISNODUPS;
	_tt_oid_keydesc.k_nparts = 1;
	_tt_oid_keydesc.k_part[0].kp_start = 0;
	_tt_oid_keydesc.k_part[0].kp_leng = OID_KEY_LENGTH;
	_tt_oid_keydesc.k_part[0].kp_type = BINTYPE;
	
	install_signal_handler();
	
	/* set NetISAM fatal error handler */
	if (iscntl(ALLISFD, ISCNTL_FATAL, isamfatalerror) == -1) {
		_tt_syslog(errstr, LOG_ERR,
			   "iscntl(ALLISFD, ISCNTL_FATAL,) == -1");
		exit(1);
	}
	
	/* setup database file creation mask */
	mode_t pmask = umask(_TT_UMASK);
	
	/* initialize access control cache */
	_tt_oa_cache = new _Tt_oid_access_queue();
	_tt_la_cache = new _Tt_link_access_queue();
	
	/* initialize opened database table */
	
	for (int i = 0; i < _TT_MAX_ISFD; i++) {
		_tt_db_table[i].db_path = 0;
		_tt_db_table[i].opener_uid = (uid_t)-1;
		_tt_db_table[i].reftime = 0;
		_tt_db_table[i].server_has_open = 0;
		_tt_db_table[i].client_has_open = 0;
	}
	

	// If we were started from inetd, inetd has already done all
	// the negotiation with the portmapper.  We simply create a RPC
	// connection over the endpoint that inetd handed us.  If
	// we were started from the shell, we create a RPC connection
	// with RPC_ANYSOCK (to create a new endpoint) and register
	// that with the portmapper.
							
	switch (start_mode) {
	    case STARTED_FROM_INETD:
#if defined(OPT_TLI)
		transp = svc_tli_create(0,
					(struct netconfig *)0,
					(struct t_bind *)0,
					0, 0);
		if (transp == NULL) {
			_tt_syslog(errstr, LOG_ERR, "svc_tli_create() == 0");
			exit(1);
		}
#else
		transp = svctcp_create(0,0,0);
		if (transp == NULL) {
			_tt_syslog(errstr, LOG_ERR, "svctcp_create() == 0");
			exit(1);
		}
#endif				/*OPT_TLI*/
		if (!svc_register(transp, TT_DBSERVER_PROG, TT_DBSERVER_VERS,
#ifdef _AIX
				  (void(*)())
#endif
				  _tt_dbserver_prog_1,
				  0)) // this zero means leave portmapper alone
		{
			_tt_syslog(errstr, LOG_ERR, "svc_register() == 0");
		}
		break;	       
								
	    case STARTED_FROM_SHELL:

		// Go into the background, unless the debug_mode -S
		// switch was set.

		if (!_tt_debug_mode) {
			if (0!=fork()) exit(0);

#if defined(__osf__) || defined(CSRG_BASED)
			setsid();
#else
 			setpgrp();
#endif // __osf__

			close(0); close(1); close(2);

		}

		// Evict any previous user of our RPC number and tell
		// the portmapper we are here.

		(void)pmap_unset(TT_DBSERVER_PROG, TT_DBSERVER_VERS);
		// XXX: really ought to use svc_tli_create if OPT_TLI is set
		// but that requires searching for a suitable transport..
		transp = svctcp_create(RPC_ANYSOCK, 0, 0);
		if (transp == NULL) {
			_tt_syslog(errstr, LOG_ERR, "svctcp_create() == 0");
			exit(1);
		}
								
		if (!svc_register(transp, TT_DBSERVER_PROG, TT_DBSERVER_VERS,
#ifdef _AIX
				  (void(*)())
#endif
				  _tt_dbserver_prog_1, IPPROTO_TCP)) {
			_tt_syslog(errstr, LOG_ERR, "svc_register() == 0");
			exit(1);
		}
		break;
	}

	// Maximize the number of possible file descriptors.
	_tt_zoomdtablesize();

	UNLOCK_RPC();

	svc_run();
	_tt_syslog(errstr, LOG_ERR, "svc_run()");
	exit(1);
	return 1;
}

int
_tt_write_trans_record(int isfd, _Tt_trans_record *trec)
{
	LOCK_RPC();
	/* if new record, writes it; otherwise updates it */
	isreclen = trec->rec.rec_len;
	if (trec->newp) {
		if (iswrite(isfd, trec->rec.rec_val) == -1) {
			UNLOCK_RPC();
			return 0;
		}
	} else {
		if (isrewrec(isfd, trec->recnum, trec->rec.rec_val) == -1) {
			UNLOCK_RPC();
			return 0;
		}
	}
	UNLOCK_RPC();
	return 1;
}

/*
*  _tt_dbserver_prog_cleanup - closes the log file and the target database file
*  when the database transacation processing fails.
*/

void
_tt_dbserver_prog_cleanup(int log_fd, int isfd = -1)
{
	LOCK_RPC();
	/* close the log file */
	if ((log_fd != -1) && (close(log_fd) == -1)) {
		_tt_syslog(errstr, LOG_ERR, "close(log_fd): %m");
	}
	/* close the target NetISAM database */
	if ((isfd != -1) && (cached_isclose(isfd) == -1)) {
		_tt_syslog(errstr, LOG_ERR, "cached_isclose() == -1");
	}
	UNLOCK_RPC();
}

/*
*  _tt_process_transaction - process the log file by reading records stored
*  in the log file and writing them out to the target database.  If the
*  transaction flag is set, then the transaction was not successfully committed.
*  In this case, the log file is removed.  If the transaction flag is clear,
*  then process the log file and then removes it.
*/

void
_tt_process_transaction()
{
	int log_fd;

	LOCK_RPC();

	if (_tt_log_file[0] == '\0') {
		UNLOCK_RPC();
		return;
	}
	if ((log_fd = open(_tt_log_file, O_RDWR | O_CREAT, S_IREAD + S_IWRITE))
	    == -1)
	{
		_tt_syslog(errstr, LOG_ERR, "open(\"%s\"): %m", _tt_log_file);
		UNLOCK_RPC();
		return;
	}
	/* Turn on close-on-exec */
	fcntl(log_fd, F_SETFD, 1);

	/* reset to beginning of log file */
	off_t offset;
	if ((offset = lseek(log_fd, 0, SEEK_SET)) == -1) {
		_tt_syslog(errstr, LOG_ERR, "lseek(): %m");
		UNLOCK_RPC();
		return;
	}
	/* Check the transaction flag */
	int nbytes = read(log_fd, _tt_log_buf, sizeof(int));
	if (nbytes < sizeof(int)) {
		_tt_syslog(errstr, LOG_ERR, "read(): %m");
		_tt_dbserver_prog_cleanup(log_fd);
		UNLOCK_RPC();
		return;
	}
	offset = sizeof(int);
	int flag = *((int *)_tt_log_buf);
	if (flag == 1) { /* transaction failed */
		_tt_dbserver_prog_cleanup(log_fd);
		if (unlink(_tt_log_file) == -1) { /* remove log file */
			_tt_syslog(errstr, LOG_ERR, "unlink(\"%s\"): %m",
				   _tt_log_file);
		}
		UNLOCK_RPC();
		return;
	}
	/* get the target database's path from the log file */
	nbytes = read(log_fd, _tt_log_buf, MAXPATHLEN+1);
	if (nbytes == -1) {
		_tt_syslog(errstr, LOG_ERR, "read(): %m");
		_tt_dbserver_prog_cleanup(log_fd);
		UNLOCK_RPC();
		return;
	}
	strcpy(_tt_target_db, _tt_log_buf);
	/* open the NetISAM transaction target database */
	int isfd = cached_isopen(_tt_target_db, ISINOUT+ISFIXLEN+ISMANULOCK);
	if (isfd == -1) {
		_tt_syslog(errstr, LOG_ERR, "isopen(): %d", iserrno);
		_tt_dbserver_prog_cleanup(log_fd, isfd);
		UNLOCK_RPC();
		return;
	}
	/* set the log seek pointer to the first record */
	offset += strlen(_tt_target_db) + 1;
	if ((offset = lseek(log_fd, offset, SEEK_SET))
	    == -1) {
		_tt_syslog(errstr, LOG_ERR, "lseek(): %m");
		_tt_dbserver_prog_cleanup(log_fd, isfd);
		UNLOCK_RPC();
		return;
	}
	/* gets the records from the log file and writes them to the
		target database */
	char *buf_rec;
	_Tt_trans_record trec;
	nbytes = read(log_fd, _tt_log_buf, _TT_TREC_INFO);
	while (nbytes) {
		if (nbytes < _TT_TREC_INFO) {
			_tt_syslog(errstr, LOG_ERR, "read(): %m");
			_tt_dbserver_prog_cleanup(log_fd, isfd);
			UNLOCK_RPC();
			return;
		}
		/* process the batch of records just read */
		buf_rec = _tt_log_buf;
		trec.newp = *((int *) buf_rec);
		buf_rec += sizeof(int);
		trec.recnum = *((long *) buf_rec);
		buf_rec += sizeof(long);
		trec.rec.rec_len = *((u_int *) buf_rec);
		buf_rec += sizeof(u_int);
		/* read the record as a whole */
		nbytes = read(log_fd, _tt_log_buf, trec.rec.rec_len);
		if (nbytes < trec.rec.rec_len) {
			_tt_syslog(errstr, LOG_ERR, "read(): %m");
			_tt_dbserver_prog_cleanup(log_fd, isfd);
			UNLOCK_RPC();
			return;
		}
		trec.rec.rec_val = _tt_log_buf;
		if (_tt_write_trans_record(isfd, &trec) == 0) {
			_tt_dbserver_prog_cleanup(log_fd, isfd);
			UNLOCK_RPC();
			return;
		}
		nbytes = read(log_fd, _tt_log_buf, _TT_TREC_INFO);
	}
	_tt_dbserver_prog_cleanup(log_fd, isfd);
	/* remove the log file (not until actually close it) */
	if (unlink(_tt_log_file) == -1) {
		_tt_syslog(errstr, LOG_ERR, "unlink(\"%s\"): %m", _tt_log_file);
	}
	UNLOCK_RPC();
	return;
}

/*
*  _tt_dbserver_prog_1 - the rpc db server program
*/

static void
_tt_dbserver_prog_1(struct svc_req *rqstp, SVCXPRT *transp)
{
	union arg_union {
		char *_tt_min_auth_level_1_arg;
		_Tt_isaddindex_args _tt_isaddindex_1_arg;
		_Tt_isbuild_args _tt_isbuild_1_arg;
		int _tt_isclose_1_arg;
		_Tt_iscntl_args _tt_iscntl_1_arg;
		_Tt_isdelrec_args _tt_isdelrec_1_arg;
		char *_tt_iserase_1_arg;
		_Tt_isopen_args _tt_isopen_1_arg;
		_Tt_isread_args _tt_isread_1_arg;
		_Tt_isrewrec_args _tt_isrewrec_1_arg;
		_Tt_isstart_args _tt_isstart_1_arg;
		_Tt_iswrite_args _tt_iswrite_1_arg;
		_Tt_test_and_set_args _tt_test_and_set_1_arg;
		_Tt_transaction_args _tt_transaction_1_arg;
		char *_tt_mfs_1_arg;
		_Tt_oidaccess_args _tt_getoidaccess_1_arg;
		_Tt_oidaccess_args _tt_setoiduser_1_arg;
		_Tt_oidaccess_args _tt_setoidgroup_1_arg;
		_Tt_oidaccess_args _tt_setoidmode_1_arg;
		_Tt_spec_props _tt_readspec_1_arg;
		_Tt_spec_props _tt_writespec_1_arg;
		_Tt_session_args _tt_addsession_1_arg;
		_Tt_session_args _tt_delsession_1_arg;
		_Tt_spec_props _tt_gettype_1_arg;
		char *_tt_get_file_partition_1_arg;
		_tt_create_file_args _tt_create_file_1_arg;
		_tt_create_obj_args _tt_create_obj_1_arg;
		_tt_remove_file_args _tt_remove_file_1_arg;
		_tt_remove_obj_args _tt_remove_obj_1_arg;
		_tt_move_file_args _tt_move_file_1_arg;
		_tt_set_file_props_args _tt_set_file_props_1_arg;
		_tt_set_file_prop_args _tt_set_file_prop_1_arg;
		_tt_add_file_prop_args _tt_add_file_prop_1_arg;
		_tt_del_file_prop_args _tt_delete_file_prop_1_arg;
		_tt_get_file_prop_args _tt_get_file_prop_1_arg;
		_tt_get_file_props_args _tt_get_file_props_1_arg;
		_tt_get_file_objs_args _tt_get_file_objs_1_arg;
		_tt_set_file_access_args _tt_set_file_access_1_arg;
		_tt_get_file_access_args _tt_get_file_access_1_arg;
		_tt_set_obj_props_args _tt_set_obj_props_1_arg;
		_tt_set_obj_prop_args _tt_set_obj_prop_1_arg;
		_tt_add_obj_prop_args _tt_add_obj_prop_1_arg;
		_tt_del_obj_prop_args _tt_delete_obj_prop_1_arg;
		_tt_get_obj_prop_args _tt_get_obj_prop_1_arg;
		_tt_get_obj_props_args _tt_get_obj_props_1_arg;
		_tt_set_obj_type_args _tt_set_obj_type_1_arg;
		_tt_get_obj_type_args _tt_get_obj_type_1_arg;
		_tt_set_obj_file_args _tt_set_obj_file_1_arg;
		_tt_get_obj_file_args _tt_get_obj_file_1_arg;
		_tt_set_obj_access_args _tt_set_obj_access_1_arg;
		_tt_get_obj_access_args _tt_get_obj_access_1_arg;
		_tt_is_file_in_db_args _tt_is_file_in_db_1_arg;
		_tt_is_obj_in_db_args _tt_is_obj_in_db_1_arg;
		_tt_queue_msg_args _tt_queue_message_1_arg;
		_tt_dequeue_msgs_args _tt_dequeue_messages_1_arg;
		_tt_file_netfile_args _tt_file_netfile_1_arg;
		_tt_file_netfile_args _tt_netfile_file_1_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();
	
	struct authunix_parms *unix_cred;
#if defined(OPT_SECURE_RPC)
	struct authdes_cred *des_cred;
#endif

	// Increment the counter for the number of RPC calls
	// handled during the life of this process.
	_tt_global->event_counter++;
	
	LOCK_RPC();
	if (rqstp->rq_proc == NULLPROC) {
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
		UNLOCK_RPC();
		return;
	} else if ((rqstp->rq_proc != _TT_MIN_AUTH_LEVEL) &&
		   (rqstp->rq_proc != _TT_MFS) &&
		   (rqstp->rq_proc != TT_GET_MIN_AUTH_LEVEL) &&
		   (rqstp->rq_proc != TT_GET_FILE_PARTITION) &&
		   (rqstp->rq_proc != TT_IS_FILE_IN_DB) &&
		   (rqstp->rq_proc != TT_IS_OBJ_IN_DB)) {
		/* extract the uid and gids from the client's credential */
		switch (rqstp->rq_cred.oa_flavor) {
		    case AUTH_UNIX:
			/* Check authentication level */
			if (_tt_auth_level == AUTH_DES) {
				svcerr_weakauth(transp);
				UNLOCK_RPC();
				return;
			}
			unix_cred = (struct authunix_parms*) rqstp->rq_clntcred;
			_tt_uid = unix_cred->aup_uid;
			_tt_gid = unix_cred->aup_gid;
			_tt_gidlen = unix_cred->aup_len;
			int i;
			for (i = 0; i < _tt_gidlen; i++) {
				_tt_gidlist[i] = unix_cred->aup_gids[i];
			}
			break;
#if defined(OPT_SECURE_RPC)
		    case AUTH_DES:
			des_cred = (struct authdes_cred*) rqstp->rq_clntcred;
#if defined(OPT_BUG_SUNOS_4) && !defined(__GNUG__)
#	define CAST_IN_SUNOS_4 (int *)
#else
#	define CAST_IN_SUNOS_4
#endif
			if (!netname2user(des_cred->adc_fullname.name,
					  CAST_IN_SUNOS_4 &_tt_uid,
					  CAST_IN_SUNOS_4 &_tt_gid,
					  &_tt_gidlen,
					  CAST_IN_SUNOS_4 _tt_gidlist))
				{
					svcerr_systemerr(transp);
					UNLOCK_RPC();
					return;
				}
			break;
#endif			
			
#ifdef AUTH_NONE
		    case AUTH_NONE:
#else
		    case AUTH_NULL:
#endif
		    default:
			svcerr_weakauth(transp);
			UNLOCK_RPC();
			return;
		}
	}
	
	/* Bind the service procedure */
	switch (rqstp->rq_proc) {
	    case _TT_MIN_AUTH_LEVEL:
		TTDB_DEBUG_SYSLOG("_TT_MIN_AUTH_LEVEL");
		xdr_argument = (bool_t (*)()) xdr_wrapstring;
		xdr_result = (bool_t (*)()) xdr_int;
		local = (char *(*)()) _tt_min_auth_level_1;
		break;
		
	    case _TT_ISADDINDEX:
		TTDB_DEBUG_SYSLOG("_TT_ISADDINDEX");
		xdr_argument = (bool_t (*)()) xdr_Tt_isaddindex_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isaddindex_1;
		break;
		
	    case _TT_ISBUILD:
		TTDB_DEBUG_SYSLOG("_TT_ISBUILD");
		xdr_argument = (bool_t (*)()) xdr_Tt_isbuild_args;
		xdr_result =(bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isbuild_1;
		break;
		
	    case _TT_ISCLOSE:
		TTDB_DEBUG_SYSLOG("_TT_ISCLOSE");
		xdr_argument = (bool_t (*)()) xdr_int;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isclose_1;
		break;
		
	    case _TT_ISCNTL:
		TTDB_DEBUG_SYSLOG("_TT_ISCNTL");
		xdr_argument = (bool_t (*)()) xdr_Tt_iscntl_args;
		xdr_result = (bool_t (*)()) xdr_Tt_iscntl_results;
		local = (char *(*)()) _tt_iscntl_1;
		break;
		
	    case _TT_ISDELREC:
		TTDB_DEBUG_SYSLOG("_TT_ISDELREC");
		xdr_argument = (bool_t (*)()) xdr_Tt_isdelrec_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isdelrec_1;
		break;
		
	    case _TT_ISERASE:
		TTDB_DEBUG_SYSLOG("_TT_ISERASE");
		xdr_argument = (bool_t (*)()) xdr_wrapstring;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_iserase_1;
		break;
		
	    case _TT_ISOPEN:
		TTDB_DEBUG_SYSLOG("_TT_ISOPEN");
		xdr_argument = (bool_t (*)()) xdr_Tt_isopen_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isopen_1;
		break;
		
	    case _TT_ISREAD:
		TTDB_DEBUG_SYSLOG("_TT_ISREAD");
		xdr_argument = (bool_t (*)()) xdr_Tt_isread_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isread_results;
		local = (char *(*)()) _tt_isread_1;
		break;
		
	    case _TT_ISREWREC:
		TTDB_DEBUG_SYSLOG("_TT_ISREWREC");
		xdr_argument = (bool_t (*)()) xdr_Tt_isrewrec_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isrewrec_1;
		break;
		
	    case _TT_ISSTART:
		TTDB_DEBUG_SYSLOG("_TT_ISSTART");
		xdr_argument = (bool_t (*)()) xdr_Tt_isstart_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_isstart_1;
		break;
		
	    case _TT_ISWRITE:
		TTDB_DEBUG_SYSLOG("_TT_ISWRITE");
		xdr_argument = (bool_t (*)()) xdr_Tt_iswrite_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_iswrite_1;
		break;
		
	    case _TT_TEST_AND_SET:
		TTDB_DEBUG_SYSLOG("_TT_TEST_AND_SET");
		xdr_argument = (bool_t (*)()) xdr_Tt_test_and_set_args;
		xdr_result = (bool_t (*)()) xdr_Tt_test_and_set_results;
		local = (char *(*)()) _tt_test_and_set_1;
		break;
		
	    case _TT_TRANSACTION:
		TTDB_DEBUG_SYSLOG("_TT_TRANSACTION");
		xdr_argument = (bool_t (*)()) xdr_Tt_transaction_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_transaction_1;
		break;
		
	    case _TT_MFS:
		TTDB_DEBUG_SYSLOG("_TT_MFS");
		xdr_argument = (bool_t (*)()) xdr_wrapstring;
		xdr_result = (bool_t (*)()) xdr_wrapstring;
		local = (char *(*)()) _tt_mfs_1;
		break;
		
	    case _TT_GETOIDACCESS:
		TTDB_DEBUG_SYSLOG("_TT_GETOIDACCESS");
		xdr_argument = (bool_t (*)()) xdr_Tt_oidaccess_args;
		xdr_result = (bool_t (*)()) xdr_Tt_oidaccess_results;
		local = (char *(*)()) _tt_getoidaccess_1;
		break;
		
	    case _TT_SETOIDUSER:
		TTDB_DEBUG_SYSLOG("_TT_SETOIDUSER");
		xdr_argument = (bool_t (*)()) xdr_Tt_oidaccess_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_setoiduser_1;
		break;
		
	    case _TT_SETOIDGROUP:
		TTDB_DEBUG_SYSLOG("_TT_SETOIDGROUP");
		xdr_argument = (bool_t (*)()) xdr_Tt_oidaccess_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_setoidgroup_1;
		break;
		
	    case _TT_SETOIDMODE:
		TTDB_DEBUG_SYSLOG("_TT_SETOIDMODE");
		xdr_argument = (bool_t (*)()) xdr_Tt_oidaccess_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_setoidmode_1;
		break;
		
	    case _TT_READSPEC:
		TTDB_DEBUG_SYSLOG("_TT_READSPEC");
		xdr_argument = (bool_t (*)()) xdr_Tt_spec_props;
		xdr_result = (bool_t (*)()) xdr_Tt_spec_props;
		local = (char *(*)()) _tt_readspec_1;
		break;
		
	    case _TT_WRITESPEC:
		TTDB_DEBUG_SYSLOG("_TT_WRITESPEC");
		xdr_argument = (bool_t (*)()) xdr_Tt_spec_props;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_writespec_1;
		break;
		
	    case _TT_ADDSESSION:
		TTDB_DEBUG_SYSLOG("_TT_ADDSESSION");
		xdr_argument = (bool_t (*)()) xdr_Tt_session_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_addsession_1;
		break;
		
	    case _TT_DELSESSION:
		TTDB_DEBUG_SYSLOG("_TT_DELSESSION");
		xdr_argument = (bool_t (*)()) xdr_Tt_session_args;
		xdr_result = (bool_t (*)()) xdr_Tt_isam_results;
		local = (char *(*)()) _tt_delsession_1;
		break;
		
	    case _TT_GETTYPE:
		TTDB_DEBUG_SYSLOG("_TT_GETTYPE");
		xdr_argument = (bool_t (*)()) xdr_Tt_spec_props;
		xdr_result = (bool_t (*)()) xdr_Tt_spec_props;
		local = (char *(*)()) _tt_gettype_1;
		break;
		
	    case TT_GET_MIN_AUTH_LEVEL:
		TTDB_DEBUG_SYSLOG("TT_GET_MIN_AUTH_LEVEL");
		xdr_argument = (bool_t (*)()) xdr_void;
		xdr_result = (bool_t (*)()) xdr_tt_auth_level_results;
		local = (char *(*)()) _tt_get_min_auth_level_1;
		break;
		
	    case TT_GET_FILE_PARTITION:
		TTDB_DEBUG_SYSLOG("TT_GET_FILE_PARTITION");
		xdr_argument = (bool_t (*)()) xdr_wrapstring;
		xdr_result = (bool_t (*)()) xdr_tt_file_partition_results;
		local = (char *(*)()) _tt_get_file_partition_1;
		break;
		
	    case TT_CREATE_FILE:
		TTDB_DEBUG_SYSLOG("TT_CREATE_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_create_file_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_create_file_1;
		break;
		
	    case TT_CREATE_OBJ:
		TTDB_DEBUG_SYSLOG("TT_CREATE_OBJ");
		xdr_argument = (bool_t (*)()) xdr_tt_create_obj_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_create_obj_1;
		break;
		
	    case TT_REMOVE_FILE:
		TTDB_DEBUG_SYSLOG("TT_REMOVE_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_remove_file_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_remove_file_1;
		break;
		
	    case TT_REMOVE_OBJ:
		TTDB_DEBUG_SYSLOG("TT_REMOVE_OBJ");
		xdr_argument = (bool_t (*)()) xdr_tt_remove_obj_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_remove_obj_1;
		break;
		
	    case TT_MOVE_FILE:
		TTDB_DEBUG_SYSLOG("TT_MOVE_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_move_file_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_move_file_1;
		break;
		
	    case TT_SET_FILE_PROPS:
		TTDB_DEBUG_SYSLOG("TT_SET_FILE_PROPS");
		xdr_argument = (bool_t (*)()) xdr_tt_set_file_props_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_set_file_props_1;
		break;
		
	    case TT_SET_FILE_PROP:
		TTDB_DEBUG_SYSLOG("TT_SET_FILE_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_set_file_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_set_file_prop_1;
		break;
	    case TT_ADD_FILE_PROP:
		TTDB_DEBUG_SYSLOG("TT_ADD_FILE_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_add_file_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_add_file_prop_1;
		break;
		
	    case TT_DELETE_FILE_PROP:
		TTDB_DEBUG_SYSLOG("TT_DELETE_FILE_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_del_file_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_cache_results;
		local = (char *(*)()) _tt_delete_file_prop_1;
		break;
		
	    case TT_GET_FILE_PROP:
		TTDB_DEBUG_SYSLOG("TT_GET_FILE_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_get_file_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_prop_results;
		local = (char *(*)()) _tt_get_file_prop_1;
		break;
		
	    case TT_GET_FILE_PROPS:
		TTDB_DEBUG_SYSLOG("TT_GET_FILE_PROPS");
		xdr_argument = (bool_t (*)()) xdr_tt_get_file_props_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_props_results;
		local = (char *(*)()) _tt_get_file_props_1;
		break;
		
	    case TT_GET_FILE_OBJS:
		TTDB_DEBUG_SYSLOG("TT_GET_FILE_OBJS");
		xdr_argument = (bool_t (*)()) xdr_tt_get_file_objs_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_objs_results;
		local = (char *(*)()) _tt_get_file_objs_1;
		break;
		
	    case TT_SET_FILE_ACCESS:
		TTDB_DEBUG_SYSLOG("TT_SET_FILE_ACCESS");
		xdr_argument = (bool_t (*)()) xdr_tt_set_file_access_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_set_file_access_1;
		break;
		
	    case TT_GET_FILE_ACCESS:
		TTDB_DEBUG_SYSLOG("TT_GET_FILE_ACCESS");
		xdr_argument = (bool_t (*)()) xdr_tt_get_file_access_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_access_results;
		local = (char *(*)()) _tt_get_file_access_1;
		break;
		
	    case TT_SET_OBJ_PROPS:
		TTDB_DEBUG_SYSLOG("TT_SET_OBJ_PROPS");
		xdr_argument = (bool_t (*)()) xdr_tt_set_obj_props_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_props_results;
		local = (char *(*)()) _tt_set_obj_props_1;
		break;
		
	    case TT_SET_OBJ_PROP:
		TTDB_DEBUG_SYSLOG("TT_SET_OBJ_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_set_obj_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_props_results;
		local = (char *(*)()) _tt_set_obj_prop_1;
		break;
		
	    case TT_ADD_OBJ_PROP:
		TTDB_DEBUG_SYSLOG("TT_ADD_OBJ_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_add_obj_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_props_results;
		local = (char *(*)()) _tt_add_obj_prop_1;
		break;
		
	    case TT_DELETE_OBJ_PROP:
		TTDB_DEBUG_SYSLOG("TT_DELETE_OBJ_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_del_obj_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_props_results;
		local = (char *(*)()) _tt_delete_obj_prop_1;
		break;
		
	    case TT_GET_OBJ_PROP:
		TTDB_DEBUG_SYSLOG("TT_GET_OBJ_PROP");
		xdr_argument = (bool_t (*)()) xdr_tt_get_obj_prop_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_prop_results;
		local = (char *(*)()) _tt_get_obj_prop_1;
		break;
		
	    case TT_GET_OBJ_PROPS:
		TTDB_DEBUG_SYSLOG("TT_GET_OBJ_PROPS");
		xdr_argument = (bool_t (*)()) xdr_tt_get_obj_props_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_props_results;
		local = (char *(*)()) _tt_get_obj_props_1;
		break;
		
	    case TT_SET_OBJ_TYPE:
		TTDB_DEBUG_SYSLOG("TT_SET_OBJ_TYPE");
		xdr_argument = (bool_t (*)()) xdr_tt_set_obj_type_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_set_obj_type_1;
		break;
		
	    case TT_GET_OBJ_TYPE:
		TTDB_DEBUG_SYSLOG("TT_GET_OBJ_TYPE");
		xdr_argument = (bool_t (*)()) xdr_wrapstring;
		xdr_result = (bool_t (*)()) xdr_tt_obj_type_results;
		local = (char *(*)()) _tt_get_obj_type_1;
		break;
		
	    case TT_SET_OBJ_FILE:
		TTDB_DEBUG_SYSLOG("TT_SET_OBJ_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_set_obj_file_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_set_obj_file_1;
		break;
		
	    case TT_GET_OBJ_FILE:
		TTDB_DEBUG_SYSLOG("TT_GET_OBJ_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_get_obj_file_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_file_results;
		local = (char *(*)()) _tt_get_obj_file_1;
		break;
		
	    case TT_SET_OBJ_ACCESS:
		TTDB_DEBUG_SYSLOG("TT_SET_OBJ_ACCESS");
		xdr_argument = (bool_t (*)()) xdr_tt_set_obj_access_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_set_obj_access_1;
		break;
		
	    case TT_GET_OBJ_ACCESS:
		TTDB_DEBUG_SYSLOG("TT_GET_OBJ_ACCESS");
		xdr_argument = (bool_t (*)()) xdr_tt_get_obj_access_args;
		xdr_result = (bool_t (*)()) xdr_tt_obj_access_results;
		local = (char *(*)()) _tt_get_obj_access_1;
		break;
		
	    case TT_IS_FILE_IN_DB:
		TTDB_DEBUG_SYSLOG("TT_IS_FILE_IN_DB");
		xdr_argument = (bool_t (*)()) xdr_tt_is_file_in_db_args;
		xdr_result = (bool_t (*)()) xdr_tt_is_file_in_db_results;
		local = (char *(*)()) _tt_is_file_in_db_1;
		break;
		
	    case TT_IS_OBJ_IN_DB:
		TTDB_DEBUG_SYSLOG("TT_IS_OBJ_IN_DB");
		xdr_argument = (bool_t (*)()) xdr_tt_is_obj_in_db_args;
		xdr_result = (bool_t (*)()) xdr_tt_is_obj_in_db_results;
		local = (char *(*)()) _tt_is_obj_in_db_1;
		break;
		
	    case TT_QUEUE_MESSAGE:
		TTDB_DEBUG_SYSLOG("TT_QUEUE_MESSAGE");
		xdr_argument = (bool_t (*)()) xdr_tt_queue_msg_args;
		xdr_result = (bool_t (*)()) xdr_tt_db_results;
		local = (char *(*)()) _tt_queue_message_1;
		break;
		
	    case TT_DEQUEUE_MESSAGES:
		TTDB_DEBUG_SYSLOG("TT_DEQUEUE_MESSAGES");
		xdr_argument = (bool_t (*)()) xdr_tt_dequeue_msgs_args;
		xdr_result = (bool_t (*)()) xdr_tt_dequeue_msgs_results;
		local = (char *(*)()) _tt_dequeue_messages_1;
		break;
		
	    case TTDB_FILE_NETFILE:
		TTDB_DEBUG_SYSLOG("TTDB_FILE_NETFILE");
		xdr_argument = (bool_t (*)()) xdr_tt_file_netfile_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_netfile_results;
		local = (char *(*)()) _tt_file_netfile_1;
		break;
		
	    case TTDB_NETFILE_FILE:
		TTDB_DEBUG_SYSLOG("TTDB_NETFILE_FILE");
		xdr_argument = (bool_t (*)()) xdr_tt_file_netfile_args;
		xdr_result = (bool_t (*)()) xdr_tt_file_netfile_results;
		local = (char *(*)()) _tt_netfile_file_1;
		break;

	    case TT_GET_ALL_SESSIONS:
		TTDB_DEBUG_SYSLOG("TT_GET_ALL_SESSIONS");
		xdr_argument = (bool_t (*)())xdr_tt_get_all_sessions_args;
		xdr_result = (bool_t (*)())xdr_tt_get_all_sessions_results;
		local = (char *(*)()) _tt_get_all_sessions_1;
		break;

	    case TT_GARBAGE_COLLECT:
		TTDB_DEBUG_SYSLOG("TT_GARBAGE_COLLECT");
		xdr_argument = (bool_t (*)())xdr_void;
		xdr_result = (bool_t (*)())xdr_tt_garbage_collect_results;
		local = (char *(*)()) _tt_garbage_collect_1;
		break;

	    case TT_DELETE_SESSION:
		TTDB_DEBUG_SYSLOG("TT_DELETE_SESSION");
		xdr_argument = (bool_t (*)())xdr_tt_delete_session_args;
		xdr_result = (bool_t (*)())xdr_tt_delete_session_results;
		local = (char *(*)()) _tt_delete_session_1;
		break;

	    default:
		svcerr_noproc(transp);
		UNLOCK_RPC();
		return;
	}
	/* get the input arguments */
	memset((char *)&argument, 0, sizeof(argument));
	if (!svc_getargs(transp, (xdrproc_t)xdr_argument, (caddr_t)&argument)) {
		svcerr_decode(transp);
		UNLOCK_RPC();
		return;
	}
	char *(*local_t)(caddr_t, SVCXPRT *) = (char *(*)(caddr_t, SVCXPRT *))
		local;
	
	/* Check global message queue unlock flag */
	if (msg_q_unlock_flag == TRUE) {
		msg_q_unlock_flag = FALSE;
		
		_Tt_db_msg_q_lock locks;
		locks.unsetAllLocks();
	}
	
	if (refresh_partition_redirection_map) {
		refresh_partition_redirection_map = FALSE;	
		db_pr_map->refresh();
	}
	
	/* call the service procedure */
	result = (*local_t)((caddr_t)&argument, transp);

	
	/* return the results to client */
	if ((result != NULL) && !svc_sendreply(transp,
					       (xdrproc_t)xdr_result,
					       (caddr_t)result)) {
		svcerr_systemerr(transp);
	}
	
	/* free memory */
	if (!svc_freeargs(transp,
			  (xdrproc_t)xdr_argument,
			  (caddr_t)&argument)) {
		_tt_syslog(errstr, LOG_ERR, "svc_freeargs() == 0");
		exit(1);
	}
	
	/* process the transaction log file if there were a transaction */
	if (rqstp->rq_proc == _TT_TRANSACTION) {
		_tt_process_transaction();
	}

	//
	// Free the results
		switch (rqstp->rq_proc) {
		    case TT_GET_MIN_AUTH_LEVEL:
			break;
			
		    case TT_GET_FILE_PARTITION:
			if (result) {
				if (((_tt_file_partition_results *)result)->partition) {
					free(((_tt_file_partition_results *)result)->partition);
				}
				if (((_tt_file_partition_results *)result)->network_path) {
					free(((_tt_file_partition_results *)result)->network_path);
				}
			}
			break;
			
		    case TT_CREATE_FILE:
			break;
			
		    case TT_CREATE_OBJ:
			break;
			
		    case TT_REMOVE_FILE:
			break;
			
		    case TT_REMOVE_OBJ:
			break;
			
		    case TT_MOVE_FILE:
			break;
			
		    case TT_SET_FILE_PROPS:
			break;
			
		    case TT_SET_FILE_PROP:
			break;
			
		    case TT_ADD_FILE_PROP:
			break;
			
		    case TT_DELETE_FILE_PROP:
			break;
			
		    case TT_GET_FILE_PROP:
			if (result) {
				_tt_free_rpc_property(((_tt_file_prop_results *)
						       result)->property);
			}
			break;
			
		    case TT_GET_FILE_PROPS:
			if (result) {
				_tt_free_rpc_properties(((_tt_file_props_results *)
							 result)->properties);
			}
			break;
			
		    case TT_GET_FILE_OBJS:
			if (result) {
				_tt_free_rpc_strings(((_tt_file_objs_results *)
						      result)->objids);
			}
			break;
			
		    case TT_SET_FILE_ACCESS:
			break;
			
		    case TT_GET_FILE_ACCESS:
			break;
			
		    case TT_SET_OBJ_PROPS:
			break;
			
		    case TT_SET_OBJ_PROP:
			break;
			
		    case TT_ADD_OBJ_PROP:
			break;
			
		    case TT_DELETE_OBJ_PROP:
			break;
			
		    case TT_GET_OBJ_PROP:
			if (result) {
				_tt_free_rpc_property(((_tt_obj_prop_results *)
						       result)->property);
			}
			break;
			
		    case TT_GET_OBJ_PROPS:
			if (result) {
				_tt_free_rpc_properties(((_tt_obj_props_results *)
							 result)->properties);
			}
			break;
			
		    case TT_SET_OBJ_TYPE:
			break;
			
		    case TT_GET_OBJ_TYPE:
			if (result) {
				free(((_tt_obj_type_results *)result)->otype);
			}
			break;
			
		    case TT_SET_OBJ_FILE:
			break;
			
		    case TT_GET_OBJ_FILE:
			if (result) {
				free(((_tt_obj_file_results *)result)->file);
			}
			break;
			
		    case TT_SET_OBJ_ACCESS:
			break;
			
		    case TT_GET_OBJ_ACCESS:
			break;
			
		    case TT_IS_FILE_IN_DB:
			break;
			
		    case TT_IS_OBJ_IN_DB:
			if (result && ((_tt_is_obj_in_db_results *)
				       result)->forward_pointer) {
				free(((_tt_is_obj_in_db_results *)
				      result)->forward_pointer);
			}
			break;
			
		    case TT_QUEUE_MESSAGE:
			break;
			
		    case TT_DEQUEUE_MESSAGES:
			if (result) {
				_tt_free_rpc_messages(((_tt_dequeue_msgs_results *)
						       result)->messages);
			}
			break;
			
		    default:
			break;
		}

	UNLOCK_RPC();

#if defined(OPT_AUTO_GARBAGE_COLLECT)
#if !defined(OPT_GARBAGE_THREADS)
	if (!_tt_debug_mode) {
#endif
		//
		// The results have been passed back to the client program.
		// Now check for garbage collection at some pre-determined
		// interval (_TT_GARBAGE_COLLECTION_FREQUENCY).
		//
		time_t	the_time_now = time(0);

		//
		// Wait at least 5 Min. after startup.
		//
		if (_tt_global->next_garbage_run == -1) {
		  _tt_global->next_garbage_run = the_time_now + (60 * 5);
		}

		if (the_time_now > _tt_global->next_garbage_run) {
			_tt_global->next_garbage_run
			  = the_time_now + _TT_GARBAGE_COLLECTION_FREQUENCY;

			//
			// Run the cleanup in parallal so the server can
			// keep running.
			_tt_run_garbage_collect(TRUE);	// In parallel
		}
#if !defined(OPT_GARBAGE_THREADS)
        }
#endif
#endif /*OPT_AUTO_GARBAGE_COLLECT*/
	return;
}

void
install_signal_handler()
{
	// It's important to ignore SIGPIPES so we don't die when clients do.
	// SIGUSR1 is used to turn logging on or off.
	//
	// SIGHUP probably *ought* to be used to force any caches to flush
	// For robustness, ought to catch SIGINT/TERM so we finish the current
	// RPC call and write out files..
	//

	_tt_sigset(SIGUSR1, &sig_handler);
	_tt_sigset(SIGUSR2, &sig_handler);
	_tt_sigset(SIGPIPE, &sig_handler);

#if !defined(OPT_GARBAGE_THREADS)
	_tt_sigset(SIGCHLD, &sig_handler);
#endif
	return;
}

void
sig_handler(int sig)
{
	switch (sig) {
	      case SIGPIPE:
		// If a client goes away, unlock the message q locks.
		// Rationale: It's better to have potential collisions
		// between old and new clients then lock up the DB
		// indefinitely.
		msg_q_unlock_flag = TRUE;
		break;
		
	      case SIGUSR1:
		// turn trace on (level 1) if it was off. turn trace off
		// if it was on.
		break;
		
	      case SIGUSR2:
		refresh_partition_redirection_map = TRUE;	
		break;
		
#if !defined(OPT_GARBAGE_THREADS)
	      case SIGCHLD:
		waitpid(-1, NULL, WNOHANG);	// Reap and run.
		_tt_garbage_id = -1;		// Mark as done.
		break;
#endif
	      default:
		break;
	}
	return;
}
