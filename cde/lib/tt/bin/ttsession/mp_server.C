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
//%%  $TOG: mp_server.C /main/10 1999/10/14 18:38:32 mgreess $ 			 				
/*
 * @(#)mp_server.C	1.119 95/09/26
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
#include "tt_options.h"
#include "mp_s_global.h"
#include "mp/mp_mp.h"
#include "mp_s_mp.h"
#include "mp_ptype.h"     
#include "mp_s_session.h"
#include "mp_typedb.h"
#include "util/copyright.h"
#include "util/tt_enumname.h"
#include "util/tt_global_env.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "db/db_server.h"
#include <locale.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define MAXARGS 256
#define MAXPIDS 256

#include <sys/resource.h>
#include <unistd.h>
#if defined(sgi) || defined(CSRG_BASED)
#include <getopt.h>
#endif

TT_INSERT_COPYRIGHT

#ifdef OPT_PATCH
static char PatchID[] = "Patch Id: 100626_03.";
static int Patch_ID100626_03;
#endif

extern char		**environ;

// flag set by the signal handler function whenever we receive a
// signal that the types database changed.
int			signal_types_changed = 0;
int			signal_toggle_trace  = 0;

// option_classing_engine determines whether we use the Classing Engine
// to read the ptype/otype database. It's starting value determines the
// default. 
int			option_classing_engine = 0;

// if equal to 1 then this option means that non process-tree
// ttsessions will fork a child and exit when the child ttsession is
// ready to service clients.
int			background_mode = 1;

// this is a special case of a process-tree session that forks a child
// ttsession (rather than forking a root process) and then prints out
// the child ttsession session id when it is ready to service clients. 
int			print_sessid = 0;

// the level of authorization we are to use. one of: "unix", "none",
// or "des". The default is "none".
char			*option_auth_level = (char *)0;

// Whether to maximize the fd limit.
int			option_maximize_procids = 0;

// the pid of the root process that gets forked in a process-tree
// session, or of the child ttsession
pid_t			forked_pid = (pid_t)-1;

void			sig_handler(int sig);

#ifdef OPT_XTHREADS
static void init_self();
#endif

// buffer used to write out the session id of a child ttsession when the
// special print_sessid option is turned on. 
char			session_buf[255];

// pipe used by print_sessid to communicate the session id from
// the child ttsession to the parent ttsession.
int			ds_fds[2];

// Sink for ttsession error output.  If 0, output goes to syslog.
FILE			*errstr = stderr;
pid_t			child_waited_for;

//
// failed_procs is used to record any forked processes that have
// exited. Essentially it is a circular list of pids. Because it is
// typically updated inside a signal handler, it cannot use
// dynamically allocated memory so it is implemented as a fixed-size
// array of pids with two pointers into the array: _tt_s_mp->fin is an
// index into the first unset slot in the array. When a child process
// exits it's pid is recorded in this slot and the _tt_s_mp->fin index
// is incremented. The other index records the last "processed" slot.
// It is the last slot that was processed by the notify_start_failure
// routine that processes this list of pids. Whenever _tt_s_mp->fin is
// not equal to _tt_s_mp->fout then new pids were added to the list
// and need to be processed by notify_start_failure. Note that the
// indexes wrap around to 0 when they reach MAXPIDS. This means that
// if more than MAXPIDS processes are entered into failed_procs before
// they are consumed by notify_start_failure then some of the failed
// processes won't get handled properly. This event was considered
// unlikely but if it starts to happen then a larger value of MAXPIDS
// should be chosen.
//
// (see notify_start_failure, sig_handler, _Tt_s_mp::main_loop, and
//  _Tt_s_mp::_Tt_s_mp) 
pid_t			failed_procs[MAXPIDS];

//
// Returns diagnosed child exit status, or -1 if we are not convinced
// the child exited.
//
int
child_exit_status(pid_t child, _Tt_wait_status status)
{
	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	} else if (WIFSIGNALED(status)) {
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 2,
				     "child (%d) exited due to signal %d%s" ),
			    child, WTERMSIG(status),
			    WCOREDUMP(status) ?
				  catgets( _ttcatd, 3, 3,
					   " (core dumped)" )
				: "" );
		return 1;
	} else {
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 4,
				     "child (%d) neither exited nor "
				     "was signaled!" ),
			    child );
		return -1;
	}
}

int main(int argc, char **argv)
{
	void		install_signal_handler();
	void		notify_start_failure();
	int		init_types();
	void		print_usage_and_exit();
	char		*cmd = (char *)0;
	char		*cargv[MAXARGS];
	int		c;
	extern char 	*optarg;
	extern int	optind;
	extern int	opterr;
	int		maxfds;
	Tt_status	status;
	_Tt_wait_status	ch_status;
	int		i;

	//
	// Initialize all the global objects needed.
	//
	_tt_global = new _Tt_global();
	_tt_s_mp = new _Tt_s_mp;
	_tt_mp = (_Tt_mp *)_tt_s_mp;
	_tt_s_mp->exit_main_loop = 1;

	//
	// parse command-line options
	//

	// used by other objects when they need to print out error
	// messages to the user.
	_tt_global->progname = argv[0];
	setlocale( LC_ALL, "" );
	_tt_openlog( _tt_global->progname, LOG_PID | LOG_CONS | LOG_NOWAIT,
		     LOG_DAEMON );

	// needed by the getopt call
	opterr = 0;
	while ((c = getopt(argc, argv, "A:pa:htvcd:sSXEN")) != -1) {
		switch (c) {
		      case 'A':
			// set maximum number of undelivered messages
			// in ttsession.
			_tt_s_mp->max_active_messages = atoi(optarg);
			break;
		      case 'a':
			// set authorization level
			option_auth_level = optarg;
			break;
		      case 'S':
			// don't fork and exit (which is the default).
			background_mode = 0;
			break;
		      case 's':
			// don't print out any status messages
			_tt_global->silent = 1;
			break;
		      case 'v':
			// print versions and exit
			_TT_PRINT_VERSIONS((char *)_tt_global->progname)
			exit(0);
		      case 'c':
			// process-tree session. Parse the rest of the
			// command-line arguments as arguments to the
			// process-tree program. This means that -c
			// should always be the last ttsession option
			// used.
			
			// set up the server session to be a
			// process-tree session.
			_tt_mp->initial_session->set_env(_TT_ENV_PROCESS_TREE,
							(char *)0);
			// set up buffer of arguments to pass to the
			// program we're going to fork as the root of
			// the process-tree
			for (i=0;argv[optind] && i < MAXARGS; i++, optind++) {
				cargv[i] = argv[optind];
			}
			if (i==MAXARGS) {
				errno = E2BIG;
				_tt_syslog( errstr, LOG_ERR, "%m" );
				exit(1);
			}
			cargv[i] = '\0';

			// if no program given then use $SHELL
			if (! cargv[0]) {
				cargv[0] = getenv("SHELL");
				cargv[1] = '\0';
			}
			cmd = cargv[0];
			background_mode = 0;
			// exit out of command-processing here
			// (otherwise, getopt will want to reparse the
			// arguments!)
			goto endopt;
		      case 'd':
			// set up an X tooltalk session to the given X
			// display.
			if (getenv("DISPLAY") == 0) {
				_tt_putenv( "DISPLAY", optarg );
			}
			_tt_mp->initial_session->set_env(_TT_ENV_X11, optarg);
			break;
		      case 't':
			// turn message tracing on.
			tt_trace_control( 1 );
			break;
		      case 'p':
			// set up a process-tree session but rather
			// than fork a program as the root of the
			// process-tree, just set up the tooltalk
			// session and then print out the session id
			// to stdout. Programs can then manually set
			// the _TT_SESSION or _SUN_TT_SESSION environment variable to
			// this value to communicate.
			_tt_mp->initial_session->set_env(_TT_ENV_PROCESS_TREE,
							 (char *)0);
			cmd = (char *)0;
			print_sessid = 1;
			break;
#ifdef OPT_CLASSING_ENGINE
		      case 'E':
			// turn on use of Classing Engine
			option_classing_engine = 1;
			break;
#endif			
		      case 'X':
			option_classing_engine = 0;
			break;
		      case 'N':
			option_maximize_procids = 1;
			break;
		      case 'h':
		      case '?':
		      default:
			// error or help request.
			print_usage_and_exit();
			break;
		}
	}
      endopt:

	if (option_maximize_procids && (_tt_zoomdtablesize() != 0)) {
		_tt_syslog( errstr, LOG_WARNING,
			    catgets( _ttcatd, 3, 5,
				     "cannot maximize clients because %m"));
	}
	// sanity check
	if (_tt_mp->initial_session->env() == _TT_ENV_LAST) {
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 6,
				     "No scope to manage.  Use -c, -p, "
				     "-d, or set $DISPLAY." ));
		exit(1);
	}

	//
	// set authorization level to either the command-line option,
	// the value of $TOOLTALK_AUTH_LEVEL or the default.
	//
	if (option_auth_level == (char *)0) {
		option_auth_level = getenv("TOOLTALK_AUTH_LEVEL");
	}
	if (option_auth_level == (char *)0) {
		_tt_mp->initial_session->set_auth_level(_TT_AUTH_ICEAUTH);
	} else if (!strcmp(option_auth_level, "unix")) {
		_tt_mp->initial_session->set_auth_level(_TT_AUTH_UNIX);
		_tt_s_mp->unix_cred_chk_flag = 1;
	} else if (!strcmp(option_auth_level, "cookie")) {
		_tt_mp->initial_session->set_auth_level(_TT_AUTH_ICEAUTH);
	} else if (!strcmp(option_auth_level,"des")) {
		_tt_mp->initial_session->set_auth_level(_TT_AUTH_DES);
	} else if (!strcmp(option_auth_level,"none")) {
		_tt_mp->initial_session->set_auth_level(_TT_AUTH_NONE);
	} else {
		print_usage_and_exit();
	}

	// check for conflicting options being set.
	if (background_mode && ! print_sessid &&
	    _tt_mp->initial_session->env() == _TT_ENV_PROCESS_TREE) {
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 7,
				     "Cannot use -S option with -c" ));
		exit(1);
	}

	install_signal_handler();

	// set up a pipe which will be used by the child ttsession to
	// communicate it's session id to the parent ttsession which
	// will then print that out and then exit. (see SIGTERM case
	// in sig_handler below).
	if (print_sessid) {
		pipe(ds_fds);
	}

	if (!background_mode || (forked_pid=fork())==0) {
		// We are here if and only if we are a ttsession process
		// that will manage a session.

		// set ourselves to be the process group leader
		// so that our parent's signals won't propagate
		// down to us.
		if (!background_mode) {
			setpgid(0,0);
		}

		// re-install signal handler
		install_signal_handler();

		// Call the s_init method for _tt_mp. This will
		// cause the initial server session to be initiated
		// (ie. setup for rpc servicing and advertising our
		// address to tooltalk clients).
		status = _tt_s_mp->s_init();

		// now we check startup status, if we found another
		// session running then we return status=2. For other
		// errors we return status=1. (See auto-start code in
		// mp_session.cc in _Tt_session::init)
		switch (status) {
		      case TT_OK:
			// rpc servicing has been initiated. Now we
			// can continue the initialization process.
			// This process is done this way because we
			// want to set up rpc servicing as soon as
			// possible so that clients can begin
			// contacting ttsession (they won't get
			// replies back until we're done with
			// initialization but that's ok).

			if (print_sessid) {
				// write our session id to a pipe that
				// was set up by our parent.

				sprintf(session_buf,"%s\n",
					(char *)(_tt_mp->initial_session->address_string()));
				write(ds_fds[1], session_buf, 255);
			}

			// initialize ptypes/otypes 
			if (! init_types()) {
				// init_types() has emitted diagnostic
				exit(1);
			}

			// if we are running as a child ttsession then
			// we're now at a point where we're ready to
			// start servicing clients so kill off our
			// parent. 
			if (background_mode) {
				// now were ready to accept requests so
				// kill the parent app.
				kill(getppid(), SIGTERM);
			}
			break;
		      case TT_ERR_SESSION:
			// couldn't initialize because there's already
			// a ttsession running in our session scope.
			exit(2);
		      case TT_ERR_NOMP:
			exit(1);
			break;
		      default:
			_tt_syslog( errstr, LOG_ERR,
				    "_Tt_s_session::s_init(): %d (%s)!",
				    status, _tt_enumname(status));
			exit(1);
		}
	} else {

		// We're running as a parent ttsession, and we will
		// not be managing a ToolTalk session. Wait for our
		// child ttsession to either exit or kill us off with
		// a SIGTERM indicating it's ready.

		if (waitpid(forked_pid, &ch_status, 0) < 0) {
			//
			// Our signal handler should have called exit()!
			//
			_tt_syslog( errstr, LOG_ERR, "wait(): %m" );
			exit(1);
		}

		// if the wait returns then child ttsession must have
		// died abnormally. Return the status returned by the
		// child process.

		if (WIFEXITED(ch_status)) {
			// An exit status==2 is returned if the child
			// ttsession already found a session that is
			// active so we avoid printing out an error
			// message in this case.
			int exitStatus = WEXITSTATUS(ch_status);
			if (exitStatus != 2) {
				_tt_syslog( errstr, LOG_ERR,
					    catgets( _ttcatd, 3, 8,
						     "child ttsession exited"
						     " with status %d" ),
					    exitStatus );
			}
			exit(exitStatus);
		} else if (WIFSIGNALED(ch_status)) {
			_tt_syslog( errstr, LOG_ERR,
				    catgets( _ttcatd, 3, 9,
					     "child ttsession exited due "
					     "to signal %d%s" ),
				    WTERMSIG(ch_status),
				    WCOREDUMP(ch_status) ?
				    	  catgets( _ttcatd, 3, 10,
						   " (core dumped)" )
				        : "" );
			exit(1);
		} else {
			_tt_syslog( errstr, LOG_ERR,
				    catgets( _ttcatd, 3, 11,
					     "child ttsession neither "
					     "exited nor was signaled!" ));
			exit(1);
		}
	}

	//
	// if we're in a process-tree session then fork the requested
	// root process (contained in "cmd"). This process will then
	// be monitored by the signal handler such that if it exits
	// then the signal handler will also abort the tooltalk
	// session. 
	//

	if (cmd != (char *)0 &&
	    _tt_mp->initial_session->env() == _TT_ENV_PROCESS_TREE) {
		int i;
		switch(forked_pid = fork()) {
		      case -1:
			_tt_syslog( errstr, LOG_ERR, "fork(): %m" );
			exit(1);
		      case 0:
			maxfds = _tt_getdtablesize();
			for (i = 3; i < maxfds; i++) {
				close(i);
			}
			_tt_restoredtablesize();
			signal(SIGHUP, SIG_IGN);
			execvp(cmd,
			       cargv);
			_tt_syslog( errstr, LOG_ERR, "execvp(): %m" );
			exit(1);
			break;
		      default:
#if !defined(OPT_BSD_WAIT)
			child_waited_for = waitpid(-1, 0, WNOHANG);
#else
			child_waited_for = wait3(&ch_status, WNOHANG, 0);
#endif
			if (child_waited_for < 0) {
				_tt_syslog( errstr, LOG_ERR, "waitpid(): %m" );
				if (errno == ECHILD) {
					exit(1);
				}
			} else if (child_waited_for == forked_pid) {
				//
				// XXX This really cannot happen here,
				// because we handle SIGCHLD, so this
				// all happens in sig_handler()
				// 
				int xstat = child_exit_status(
						child_waited_for, ch_status );
				if (xstat >= 0) {
					exit( xstat );
				}
			}
			break;
		}
	}

	if ((! background_mode) && (print_sessid)) {
		printf("%s\n",
		       (char *)_tt_mp->initial_session->address_string());
		fflush(stdout);
	}

#ifdef OPT_XTHREADS
	// Set the global lock.  This basically locks out the init_self
	// thread until it is given permission to proceed.  Locks are
	// given up just before RPC calls and upon thread exits.
	// Only this initial call should use mutex_lock as opposed
	// to _tt_global->grab_mutex, because here we do not want
	// to wait on the condition variable here.

	_tt_global->grab_mutex();
	
	// Initialize as a client of ourself.  Ignore failure.

        xthread_fork((void *(*)(void *)) init_self, NULL);
#else
	_tt_s_mp->init_self();
#endif

	//
	// main service loop. We basically check for any programs we
	// forked exiting, check for flags set by the signal handler
	// (_tt_s_mp->exit_main_loop means exit out of rpc servicing,
	// signal_types_changed means we got a signal to re-read the
	// types database). If no signal flags were set or programs
	// exited, then we just invoke the _Tt_s_mp::main_loop method
	// to service rpc requests.
	//
	if (background_mode) {
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 12, "starting" ));
		// We are in daemon mode, so error output goes to syslog.
		errstr = 0;
	}
	while (1) {
		_tt_s_mp->exit_main_loop = 0;
		if (_tt_s_mp->fout != _tt_s_mp->fin) {
			notify_start_failure();
		}
		_tt_s_mp->main_loop();
		if (_tt_s_mp->xfd == -2) {
			// X server exited
			break;
		}
		//
		// exit from main_loop method is the result of
		// a signal (see sig_handler below)
		//
		if (signal_types_changed) {
			if (init_types()) {
				_tt_syslog( errstr, LOG_ERR,
					    catgets( _ttcatd, 3, 13,
						     "have re-read types"));
			} else {
				_tt_syslog( errstr, LOG_ERR,
					    catgets( _ttcatd, 3, 14,
						     "error in types; "
						     "keeping old types" ));
			}
			signal_types_changed = 0;
		}
		if (signal_toggle_trace) {
			int was_on = tt_trace_control( -1 );
			_tt_syslog( errstr, LOG_ERR,
				    "tt_trace_control( %d )", !was_on );
			signal_toggle_trace = 0;
		}
		if (_tt_s_mp->fout != _tt_s_mp->fin) {
			notify_start_failure();
		}
	}
	delete _tt_mp;
	exit(0);
}

#ifdef OPT_XTHREADS
static void init_self()
{
	_tt_s_mp->init_self();
	xthread_exit(0);
}
#endif

// 
// This routine is called whenever _tt_s_mp->fin is not equal to
// _tt_s_mp->fout (see comment for failed_procs above). This condition
// signals that an asynchronous signal from the system came in telling us
// that a child process exited. What we need to do is search the list of
// ptypes for ptypes that are in the process of launching (see
// _Tt_ptype::launch). If we find one whose start pid matches one of the
// pids in the failed_procs array then we invoke the
// _Tt_ptype::launch_failed method to arrange for the _Tt_ptype object to
// handle this condition.  The reason that this launch failure is special
// is that a ptype that is in the process of being launched may not have
// gotten far enough to launch a process that connects with ttsession so
// the only mechanism we have to detect the failure is that the process
// exited. What this means is that a process in a ptype's start string
// cannot exit until it either connects with ttsession and replies to its
// start message or until a process it launches does so.
//
void notify_start_failure()
{
	// one of the starting ptypes failed to start.
	// Find which one it was and fail all the
	// messages waiting for it.
	// XXX: there should be better data-structure
	// support so we wouldn't have to iterate
	// through all the ptypes.
	_Tt_ptype_table_cursor		ptypes;
	int				ptype_found;

	while (_tt_s_mp->fout != _tt_s_mp->fin) {
		ptypes.reset(_tt_s_mp->ptable);
		ptype_found = 0;
		while (!ptype_found && ptypes.next()) {
			if (failed_procs[_tt_s_mp->fout]==ptypes->start_pid()) {
				if (ptypes->launching()) {
					ptypes->launch_failed();
					ptype_found = 1;
				}
			}
		}
		_tt_s_mp->fout++;
		if (_tt_s_mp->fout == MAXPIDS) {
			_tt_s_mp->fout = 0;
		}
	}
}

// 
// Install sig_handler as the handler for all the signals it handles.
//
void
install_signal_handler()
{
	_Tt_string err = "_tt_sigset(SIG";
	if (_tt_sigset(SIGHUP, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("HUP)"));
	}
	if (_tt_sigset(SIGTERM, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("TERM)") );
	}
	if (_tt_sigset(SIGINT, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("INT)") );
	}
	if (_tt_sigset(SIGUSR1, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("USR1)") );
	}
	if (_tt_sigset(SIGUSR2, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("USR2)") );
	}
	if (_tt_sigset(SIGCHLD, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("CHLD)") );
	}
	if (_tt_sigset(SIGPIPE, &sig_handler) == 0) {
		_tt_syslog( errstr, LOG_WARNING, err.cat("PIPE)") );
	}
}


// 
// Reads in the ptype/otype database from the XDR (or CE) database.
// Once the types are read in from the database the types are installed
// via the appropiate _Tt_s_mp methods install_ptable and install_otable.
// Keep a pointer to the _Tt_typedb structure so we can merge in
// more types later on via tt_session_types_load.
//
int
init_types()
{
	Tt_status		err;

#ifdef OPT_CLASSING_ENGINE
	_Tt_typedb::ce2xdr();
#endif
	_tt_s_mp->tdb = new _Tt_typedb();
	if (option_classing_engine) {
		err = _tt_s_mp->tdb->init_ce();
		if (err != TT_OK) {
			if (0==getenv("OPENWINHOME")) {
				_tt_syslog( errstr, LOG_ERR,
					    catgets( _ttcatd, 3, 15,
						     "$OPENWINHOME not set"));
			}
			return(0);
		}
	} else {
		err = _tt_s_mp->tdb->init_xdr();
		switch (err) {
		    case TT_OK:
		    case TT_ERR_NO_MATCH:
		    case TT_ERR_DBCONSIST:
		    case TT_ERR_PATH:
			break;
		    default:
			_tt_syslog( errstr, LOG_ERR,
				    "_Tt_typedb::init_xdr(): %s",
				    _tt_enumname(err));
		}
		if (err != TT_OK) {
			return(0);
		}
	}
	_tt_s_mp->install_ptable(_tt_s_mp->tdb->ptable);
	_tt_s_mp->install_otable(_tt_s_mp->tdb->otable);
	return(1);
}


// 
// Prints out a usage string and exits.
//
void
print_usage_and_exit()
{
	_tt_syslog( errstr, LOG_ERR, "%s%s%s%s%s",
		    catgets( _ttcatd, 3, 16,
"\nUsage: ttsession [-a cookie|unix|des][-d display][-spStvhNX" ),
#if defined(OPT_CLASSING_ENGINE)
		    "E",
#else
		    "",
#endif
		    catgets( _ttcatd, 3, 17,
"][-c command]\n"
" -c [command]	start a process tree session, and run command in it.\n"
"		Subsequent options are passed to command.  Default: $SHELL\n"
" -p		start a process tree session, and print its id\n"
" -d display	start an X session on display\n"
"\n"
" -a cookie|unix|des	set server authentication level\n"
" -s		silent. Don't print out any warnings\n"
" -S		don't fork into the background\n"
" -N		maximize the number of clients allowed\n"
" -t		turn on message tracing\n"
" -X		use XDR databases for static types (default)\n" ),
#if defined(OPT_CLASSING_ENGINE)
		    catgets( _ttcatd, 3, 18,
" -E		use Classing Engine for static types\n" ),
#else
		    "",
#endif
		    catgets( _ttcatd, 3, 19,
"\n"
" -v		print out version number\n"
" -h		print out this message\n"
"\n"
"Signal interface:\n"
" kill -USR1 ttsession_pid	toggle message tracing\n"
" kill -USR2 ttsession_pid	re-read static types" ) );

	exit(1);
}


// 
// Global signal handler for ttsession. All signals are handled by this
// function (ie. no signal handlers should be defined in any other files)
//
void
sig_handler(int sig)
{
	int		intrs = 100;
	pid_t		child_pid;
	_Tt_wait_status	status;

	switch (sig) {
	      case SIGPIPE:
		// usually the result of a write on a broken tcp
		// socket. Default action is to ignore it.
		break;
	      case SIGHUP:
		break;
	      case SIGCHLD:
		// a child process has exited. If the process that
		// exited is the root process of a process-tree
		// session then we exit. Otherwise, it is possibly a
		// ptype that we launched so we record its pid in the
		// failed_procs list (see comment above for
		// failed_procs). 
#if !defined(OPT_BSD_WAIT)
		// XXX: the sysv code should do the same loop as
		// below. 
		child_pid = waitpid(-1, &status, WNOHANG);
#else
		// we do an asynchronous wait on the child to get its
		// pid. However the wait3 call can be interrupted and
		// return an EINTR so we keep trying for a bounded
		// amount of time.
		while (((child_pid = wait3(&status, WNOHANG, 0)) == -1)
		       && errno == EINTR) {
			if (! intrs--) {
				_tt_syslog( errstr, LOG_ERR, "wait3(): %m" );
				break;
			}
		}
#endif // OPT_BSD_WAIT

		// check for the child pid being the root process of a
		// process-tree id and exit if it is. Otherwise record
		// the pid in the failed_procs list and set
		// _tt_s_mp->exit_main_loop to 1 to signal the main
		// event loop to break out of rpc servicing to handle
		// this condition.
		if (child_pid > 0) {
			int status2exit = child_exit_status(child_pid, status);
			int isdead = status2exit >= 0;
			if (isdead) {
				if (child_pid == forked_pid) {
					// calling free in a sig handler is a no-no
					// delete _tt_mp;
					exit(status2exit);

				}
				else
#ifdef OPT_XTHREADS
					if(child_pid !=
					   _tt_s_mp->garbage_collector_pid)
#endif
				{
					failed_procs[_tt_s_mp->fin] = child_pid;
					_tt_s_mp->fin++;
					if (_tt_s_mp->fin == MAXPIDS) {
						_tt_s_mp->fin = 0;
					}
				}
 
			}
			_tt_s_mp->exit_main_loop = 1;
			return;
		}
		_tt_syslog( errstr, LOG_WARNING, "waitpid(): %m");
		break;
	      case SIGTERM:
		// this signal is sent by a forked ttsession when it
		// is ready to start servicing clients. It is our
		// signal to exit. For the special case of a "dialin"
		// session we print out the child session's id.
		if ((background_mode) && (forked_pid > 0)) {
			if (print_sessid) {
                                memset(session_buf, 0, 255);
				read(ds_fds[0], session_buf, 255 - 1);
				printf("%s", session_buf);
			}
			// this is the signal from the forked
			// ttsession that we should exit.

			exit(0);
		}
		// The child ttsession falls through to clean itself up
	      case SIGINT:
		_tt_syslog( errstr, LOG_ERR,
			    catgets( _ttcatd, 3, 20, "exiting" ));

		// if this is a process tree system, let our kid know
		// it's time to depart. It's important to allow the
		// process tree root process to exit beforehand so
		// that the controlling terminal will get properly
		// reassigned. Otherwise, the SIGINT will cause the
		// entire process hierarchy that has the same
		// controlling terminal to exit (so for example your
		// shelltool window would die because ttsession was
		// killed in process-tree mode).
		
		if (forked_pid > 0
		    && 0 == kill(forked_pid, SIGINT)) {

			// Wait for a decent interval to give him a chance
			// to clean up

			int i;
			for (i=0; i<10; i++) {
				sleep(1);
				if (forked_pid ==
				    waitpid(forked_pid, 0, WNOHANG)) {
					break;
				}
			}
			if (i==10) {
				// Won't go away, eh?  Time to get tough.
				if (0 == kill(forked_pid, SIGKILL)) {
					waitpid(forked_pid, 0, 0);
				}
			}
		}
		// calling free in a sig handler is a no-no
		// delete _tt_mp;
		exit(1);
		break;
	      case SIGUSR1:
		// signal to toggle message tracing
		signal_toggle_trace = 1;
		break;
	      case SIGTYPES:
		// signal to reread types database
		_tt_s_mp->exit_main_loop = 1;
		signal_types_changed = 1;
		break;
	      default:
		break;
	}
}
