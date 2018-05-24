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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tttrace.C /main/9 1999/10/14 18:38:47 mgreess $ 			 				
/*
 * @(#)tttrace.C	1.29 95/05/02
 *
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#if defined(__linux__)
# include <sys/poll.h>
#else
# include <poll.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "api/c/tt_c.h"
#include "util/tt_string.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "tttrace_objs.h"
#include "tt_options.h"

#define SESSION_TRACE_OP	"Session_Trace"

static _Tt_string	progname = "tttrace";
pid_t			forked_pid = (pid_t) -1;
int			makequit = 0;
int			poll_timeout = -1;
int			exit_status = 0;
int			ttfd = 0;

static void	install_signal_handler();
static void	sig_handler(int);
static pid_t	do_fork(_Tt_trace_optobj&);
static void	send_session_trace(_Tt_trace_optobj&);
static int	tail_pipe(int, _Tt_string&, pid_t, int);
static int	open_pipe(_Tt_string&, int*);
static void	send_on_exit();



int main(int argc, char **argv)
{
	void			print_usage_and_exit();
	int			status = 0;
	int			pipe_fd = -1;
	_Tt_string		temp_name;	// name of FIFO file
	_Tt_trace_optobj	myopts;		// processes command-line options
	setlocale( LC_ALL, "" );

	install_signal_handler();
	
	//
	// parse command-line options
	//
	
	switch(myopts.getopts(argc, argv)) {
	    case 0:
		break;
	    case 1:
		print_usage_and_exit();
		break;
	    case 2:
		exit(2);
	    default:
		break;
	}

	// Open the pipe for reading, if requested

	if (myopts.pipe_name().len()) {
		status = open_pipe(myopts.pipe_name(), &pipe_fd);
	}

	if (status == 0) {
	
		// Do fork, or send session_trace requests
				
		pid_t child_pid = 0;
		int sink;
		switch(myopts.operation_mode()) {
		    case FORK_COMMAND:
			(void) putenv(myopts.envstr());
			child_pid = do_fork(myopts);
			sink = STDERR_FILENO;
			break;
		    case SESSION_TRACE:
			send_session_trace(myopts);
			sink = STDOUT_FILENO;
			break;
		    default:
			exit(2);
			break;
		}
			
		_Tt_string outfile;
		if (myopts.outfile( outfile ) && (outfile == "-")) {
			sink = STDOUT_FILENO;
		}

		status = tail_pipe(pipe_fd, myopts.pipe_name(), child_pid, sink);

		if (pipe_fd > 0) {
			
			if (unlink( myopts.pipe_name()) != 0) {
				_Tt_string msg = progname.cat( ": " );
				msg = msg.cat( myopts.pipe_name() );
				perror( msg );
			}
		}
	}
	
	exit( status );
}

// 
// Install sig_handler as the handler for all the signals it handles.
//
static void
install_signal_handler()
{
	_Tt_string err;
	err = err.cat( ": _tt_sigset(SIG" );
	if (_tt_sigset(SIGHUP, &sig_handler) == 0) {
		perror( (char *) err.cat("HUP)") );
	}
	if (_tt_sigset(SIGTERM, &sig_handler) == 0) {
		perror( (char *) err.cat("TERM)") );
	}
	if (_tt_sigset(SIGINT, &sig_handler) == 0) {
		perror( (char *) err.cat("INT)") );
	}
	if (_tt_sigset(SIGUSR1, &sig_handler) == 0) {
		perror( (char *) err.cat("USR1)") );
	}
	if (_tt_sigset(SIGUSR2, &sig_handler) == 0) {
		perror( (char *) err.cat("USR2)") );
	}
	if (_tt_sigset(SIGCHLD, &sig_handler) == 0) {
		perror( (char *) err.cat("CHLD)") );
	}
	if (_tt_sigset(SIGPIPE, &sig_handler) == 0) {
		perror( (char *) err.cat("PIPE)") );
	}
}

// 
// Prints out a usage string and exits.
//
void
print_usage_and_exit()
{
	fprintf(stderr,
		catgets(_ttcatd, 9, 2,
			"Usage: %s [-0FCa][-o outfile] [-S session | command [options]]\n"
			"       %s [-e script | -f scriptfile][-S session | command [options]]\n"
			" -0		Turn off message tracing in session, or run command\n"
			"		without message tracing (i.e. only API tracing)\n"
			" -F		Follow all children forked by command or subsequently\n"
			"		started in session by ttsession(1)\n"
			" -C		Do not trace ToolTalk API calls\n"
			" -a		Print all attributes, arguments, and context slots of\n"
			"		traced messages.  Default is single-line summary.\n"
			" -e script	Read tttracefile(4) settings from script\n"
			" -f scriptfile	Read tttracefile(4) settings from scriptfile. \"-\": stdin.\n"
			" -o outfile	Output. \"-\": stdout. default: stdout for session tracing,\n"
			"		stderr (of tttrace) for command tracing\n"
			" -S session	Session to trace.  default: see tt_default_session()\n"
			" command	ToolTalk client command to invoke and trace\n"),
		(char *) progname, (char *) progname);
	exit(1);
}

// 
// Global signal handler for tttrace. All signals are handled by this
// function (ie. no signal handlers should be defined in any other files)
//
static void
sig_handler(int sig)
{
	int status;
	pid_t child;
	switch (sig) {
	    case SIGPIPE:
	    case SIGHUP:
		break;
	    case SIGCHLD:
		child = waitpid( -1, &status, WNOHANG );
		if ((child > 0) && (WIFEXITED(status))) {
			exit_status = WEXITSTATUS(status);
			poll_timeout = 2;
			makequit = 1;
		}
		break;
	    case SIGTERM:
	    case SIGINT:
		makequit = 1;
		break;
	}
}

static pid_t do_fork(_Tt_trace_optobj& myopts)
{
	int		i;
	int		_tt_getdtablesize(void);
	int		_tt_restoredtablesize(void);
	int		maxfds;	  // max TT fd's
	_Tt_string	cmd;
	
	switch(forked_pid = fork()) {
	    case -1:
		fprintf(stderr,"%s: fork(): %s\n", (char *)progname,
			strerror(errno));
		exit(2);
	    case 0:		// child
		maxfds = _tt_getdtablesize();
		for (i = 3; i < maxfds; i++) {
			close(i);
		}
		_tt_restoredtablesize();
		signal(SIGHUP, SIG_IGN);
		(void) myopts.command(cmd); // existence of cmd already checked
		execvp((char *) cmd, (char * const*)myopts.cargv());
		perror((char *) progname);
		exit(1);
	    default:		// parent -- wait on child process
		break;
	}
	return forked_pid;
}

static void send_session_trace(_Tt_trace_optobj& myopts)
{
	int			timeout = 180; // 3-minute timeout
	int			script_stat;
	size_t			num_fd = 1;
	struct pollfd		fds[1];
	_Tt_string		tmp;
	Tt_callback_action	tttrace_callback(Tt_message, Tt_pattern);
	
	Tt_message		msg;
	
	int mark = tt_mark();
	
	// We must send the message to the specified session.  This routine
	// is only called when there is an explicit session in the
	// options list.
				
	myopts.session(tmp);
	
	ttfd = tt_fd();
	
	tt_session_join((char *) tmp);
	msg = tt_prequest_create(TT_SESSION, SESSION_TRACE_OP);
	
	tt_message_arg_add(msg, TT_IN, "string", (char *) 0);
	script_stat = myopts.script(tmp);
	if (script_stat == 1) {			// inline script
		tt_message_arg_val_set(msg, 0, (char *) tmp);
	}
	else if (script_stat == 2) {		// script is in filename
		tt_message_file_set(msg, (char *) tmp);
	}
	
	tt_message_callback_add(msg, tttrace_callback);
	Tt_status mstat = tt_message_send(msg);
	
	if (mstat != TT_OK) {
		fprintf(stderr, "%s: tt_message_send(): %s\n",
			(char *) progname, tt_status_message(mstat));
		exit(2);
	}
	
	fds[0].fd = ttfd;
	fds[0].events = POLLIN | POLLPRI;
	fds[0].revents = 0;
	int rcode = poll(fds, num_fd, timeout);
	if (rcode == -1) {
		fprintf(stderr, "%s: Session_Trace: %s\n",
			(char *) progname, strerror(ETIMEDOUT));
		exit(2);
	}
	
	Tt_message inmsg = tt_message_receive();
	
	// Make sure the stop-tracing message is sent upon exit
		
	send_on_exit();
	
	tt_release(mark);
}

Tt_callback_action tttrace_callback(Tt_message msg, Tt_pattern)
{
	Tt_status mstat = (Tt_status) tt_message_status(msg);
	if (mstat == TT_ERR_NO_MATCH) {
		
		// ttsession does not recognize this message, which
		// means an incompatible version of ttsession is being
		// used
					
		fprintf(stderr,	catgets(_ttcatd, 9, 3,
					"%s: session <%s> does not support "
					"Session_Trace.  Use kill -USR1 instead. "
					"See ttsession(1).\n"),
			(char *) progname, tt_message_session(msg) );
		exit(5);
	}
	else if (mstat != TT_OK) {
		fprintf(stderr, "%s: Session_Trace: %s\n",
			(char *) progname, tt_status_message(mstat));
		exit(2);
	}
	
	return TT_CALLBACK_PROCESSED;
}

static void send_on_exit()
{
	Tt_message msg = tt_prequest_create(TT_SESSION, SESSION_TRACE_OP);
	tt_message_arg_add(msg, TT_IN, "string",
			   "version 1; states none; functions none" );
	tt_message_send_on_exit(msg);
}

#define MAXLINE 255

static int open_pipe(_Tt_string& pipe_name, int* fd)
{
	int quit = 0;
	int exit_status = 0;
	
	while ((! quit) && (*fd <= 0)) {
		*fd = open((char *) pipe_name, O_RDONLY | O_NDELAY);
		if ((*fd < 0) && (errno != EINTR)) {
			_Tt_string msg = progname.cat( ": " ).cat(pipe_name);
			perror(msg);
			exit_status = 2;
		}
		quit = 1;
	}
	
	return exit_status;
}


// No silly cracks about this routine's name, please!

static int tail_pipe(int fd, _Tt_string& pipenm, pid_t child_pid, int sink)
{
	int		bufsize = MAXLINE;
	int		nbytes;
	int		quit = 0;
	struct pollfd	fds[2];
	char		buf[MAXLINE];
	
	if (pipenm.len() == 0) {
		int done = 0;
		while (!done) {
			pid_t child_waited_for = waitpid( child_pid, 0, 0 );
			if (child_waited_for < 0 && errno == EINTR) continue;
			if (child_waited_for < 0 && errno == ECHILD) break;
			if (child_waited_for < 0) {
				fprintf(stderr, "%s: waitpid(): ",
					(char *) progname);
				perror(0);
				if(errno == ECHILD){
					exit(2);
				}
			}
			done = 1;
		}
	} else {
		
		while (!quit) {
			fds[0].fd = fd;
			fds[0].events = POLLIN;
			fds[0].revents = 0;
			fds[1].fd = ttfd;
			fds[1].events = POLLIN;
			fds[1].revents = 0;
			
			int numfds = poll(fds, ttfd ? 2 : 1, poll_timeout);
			if (numfds < 0) {
				if (errno != EINTR) {
					_Tt_string msg =
						progname.cat( ": poll()" );
					perror( msg );
					exit_status = 2;
					quit = 1;
				}
				else if (makequit) {
					quit = 1;
				}
				continue;
			}
			else if (numfds == 0) {
				quit = 1;
				continue;
			}
			
			if (fds[ 0 ].revents & POLLHUP) {
				if (makequit) {
					
					// Child has exited -- do a last read
					while ((nbytes = read(fd,
							      buf,
							      bufsize)) > 0) {
						write(sink, buf, nbytes);
					}
					quit = 1;
				}
				else {
					pid_t child_waited_for = waitpid(child_pid,
									 0, 0 );
					if (child_waited_for == child_pid) {

						quit = 1;
					}
					else {
					
						// Tracing is temporarily stopped.
						// Periodically wake up to see if it
						// has started again.
								
						sleep( 1 );
					}
				}
			}
			else if (fds[ 0 ].revents & POLLIN) {
				if (makequit) {
					quit = 1;
				}
				else {
					nbytes = read(fd, buf, bufsize);
					if (nbytes > 0) {
						write(sink, buf, nbytes);
					}
				}
			}
			if (fds[ 1 ].revents & POLLIN) {
				Tt_message msg = tt_message_receive();
				if (tt_ptr_error(msg) == TT_ERR_NOMP) {
					quit = 1;
				}
			}
		}
		if (fd > 0) {
			close(fd);
		}
	}
	return exit_status;
}
