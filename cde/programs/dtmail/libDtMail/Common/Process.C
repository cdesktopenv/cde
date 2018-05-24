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
/*
 *+SNOTICE
 *
 *
 *	$TOG: Process.C /main/6 1998/04/06 13:26:21 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

/*
** file included for INFTIM
*/ 
#if defined(SunOS)
#include <stropts.h>
#elif defined(HPUX)
#include <sys/poll.h>
#elif defined(_AIX) || defined(__linux__)
#define INFTIM (-1)             /* Infinite timeout */
#endif

#include <sys/wait.h>

#include "Process.hh"
#include <DtMail/Threads.hh>
#include <DtMail/IO.hh>

static const int DEFAULT_SIZE = 64 << 10;

void
SetNoBlock(int fd)
{
    int flags;
    do {
	flags = fcntl(fd, F_GETFL);
    } while(flags < 0 && errno == EINTR);

    if (flags < 0) {
	return;
    }

    flags |= O_NONBLOCK;

    int status;
    do {
	status = fcntl(fd, F_SETFL, flags);
    } while (status < 0 && errno == EINTR);

    return;
}

int
RunProg(const char * program,
	char *const * argv,
	const char * stdin_data,
	const unsigned long stdin_size,
	char ** stdout_data,
	unsigned long & stdout_size,
	char ** stderr_data,
	unsigned long & stderr_size)
{
    // See if we are supposed to do I/O with the child.
    //
    int stdin_fd[2];
    int stdout_fd[2];
    int stderr_fd[2];

    size_t stdout_bufsize = 0;
    unsigned long stderr_bufsize = 0;

    if (stdin_data) {
	pipe(stdin_fd);
	SetNoBlock(stdin_fd[1]);
    }

    if (stdout_data) {
	pipe(stdout_fd);
	SetNoBlock(stdout_fd[0]);
    }

    if (stderr_data) {
	pipe(stderr_fd);
	SetNoBlock(stderr_fd[0]);
    }

    // We will fork and set up the file descriptors in the
    // child.
    //

#if defined(POSIX_THREADS)
    pid_t child = fork1();
#else
    pid_t child = fork();
#endif

    if (child < 0) {
	return(child);
    }

    if (child == 0) { // The real child process.
	if (stdin_data) {
	    SafeDup2(stdin_fd[0], STDIN_FILENO);
	    close(stdin_fd[0]);
	    close(stdin_fd[1]);
	}

	if (stdout_data) {
	    SafeDup2(stdout_fd[1], STDOUT_FILENO);
	    close(stdout_fd[0]);
	    close(stdout_fd[1]);
	}

	if (stderr_data) {
	    SafeDup2(stderr_fd[1], STDERR_FILENO);
	    close(stderr_fd[0]);
	    close(stderr_fd[1]);
	}

	SafeExecvp(program, argv);
	return(child);
    }
    else { // This is still us.

	int stdin_written = 0;

	int nfds = 1;
	pollfd fds[3];
	memset(fds, 0, sizeof(fds));

	fds[0].fd = stdin_fd[1];
	fds[0].events = POLLOUT;
	close(stdin_fd[0]);

	if (stdout_data) {
	    fds[1].fd = stdout_fd[0];
	    fds[1].events = POLLIN;
	    nfds = 2;
	    *stdout_data = (char *)malloc(DEFAULT_SIZE);
	    stdout_bufsize = DEFAULT_SIZE;
	    stdout_size = 0;
	    close(stdout_fd[1]);
	} else {
	    fds[1].fd = -1;
	}

	if (stderr_data) {
	    fds[2].fd = stderr_fd[0];
	    fds[2].events = POLLIN;
	    nfds = 3;
	    *stderr_data = (char *)malloc(DEFAULT_SIZE);
	    stderr_bufsize = DEFAULT_SIZE;
	    stderr_size = 0;
	    close(stderr_fd[1]);
	} else {
	    fds[2].fd = -1;
	}

	// set up the initial events we way we care about.  Note that
	// fds[1] and fds[2] may not be used -- nfds may be less than 3


	while (1) {
	    int result;
	    int i;

	    // check to make sure there is really some work to do
	    // walk through the fds structure.  If we get to the end
	    // without finding anything to do, i will be == nfds
	    for (i = 0; i < nfds; i++) {
		if (fds[i].fd >= 0) break;
	    }
	    if (i == nfds) {
		// there was no work to do
		break;
	    }


	    // we probably don't want to wait forever, so we can try
	    // and reap the child here, just in case it exits...
	    result = poll(fds, nfds, INFTIM);

	    if (result < 0) {
		// poll error -- what do we do?
		if (errno == EINTR) continue;

		// not much else to do -- poll really shouldn't fail...
		break;
	    }

	    if ((fds[0].revents & POLLOUT) &&
		stdin_data && stdin_written < stdin_size) {
		int status = SafeWrite(stdin_fd[1], stdin_data + stdin_written,
				       (size_t) stdin_size - stdin_written);
		if (status > 0) {
		    stdin_written += status;
		}

		if (stdin_written >= stdin_size) {
		    // we're done with the input
		    close(stdin_fd[1]);
		    fds[0].fd = -1;
		    fds[0].events = 0;
		}
	    }

	    // We will now try to read 4K from each requested file
	    // descriptor.
	    //
	    if (stdout_data && (fds[1].revents & POLLIN)) {
		int status = SafeRead(stdout_fd[0], *stdout_data + stdout_size,
				      4096);

		if (status < 0 && errno != EAGAIN) {
		    break;
		}

		if (status > 0) {
		    stdout_size += status;
		    if ((stdout_size + 4096) > stdout_bufsize) {
			stdout_bufsize += DEFAULT_SIZE;
			*stdout_data = (char *)realloc(*stdout_data, stdout_bufsize);
		    }
		}
	    }

	    if (stderr_data && (fds[2].revents & POLLIN)) {
		int status = SafeRead(stderr_fd[0], *stderr_data + stderr_size,
				      4096);

		if (status < 0 && errno != EAGAIN) {
		    break;
		}

		if (status > 0) {
		    stderr_size += status;
		}

		if ((stderr_size + 4096) > stderr_bufsize) {
		    stderr_bufsize += DEFAULT_SIZE;
		    *stderr_data =
		      (char*) realloc(*stderr_data, (size_t) stderr_bufsize);
		}
	    }

	    if ((fds[0].revents & POLLHUP) ||
		(fds[1].revents & POLLHUP) ||
		(fds[2].revents & POLLHUP)) {
		break;
	    }
	}

	int child_stat;
	SafeWaitpid(child, &child_stat, 0);
	return(child_stat);
    }
    return(child);
}
