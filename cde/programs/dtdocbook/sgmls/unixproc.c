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
/* $XConsortium: unixproc.c /main/3 1996/06/19 17:19:08 drk $ */
/* unixproc.c -

   Unix implementation of run_process().

     Written by James Clark (jjc@jclark.com).
*/

#include "config.h"

#ifdef SUPPORT_SUBDOC

#ifdef POSIX

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#endif /* POSIX */

#include "std.h"
#include "entity.h"
#include "appl.h"

#ifndef POSIX

#define WIFSTOPPED(s) (((s) & 0377) == 0177)
#define WIFSIGNALED(s) (((s) & 0377) != 0 && ((s) & 0377 != 0177))
#define WIFEXITED(s) (((s) & 0377) == 0)
#define WEXITSTATUS(s) (((s) >> 8) & 0377)
#define WTERMSIG(s) ((s) & 0177)
#define WSTOPSIG(s) (((s) >> 8) & 0377)
#define _SC_OPEN_MAX 0
#define sysconf(name) (20)
typedef int pid_t;

#endif /* not POSIX */

#ifndef HAVE_VFORK
#define vfork() fork()
#endif /* not HAVE_VFORK */

#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif /* HAVE_VFORK_H */

int run_process(argv)
char **argv;
{
     pid_t pid;
     int status;
     int ret;

     /* Can't trust Unix implementations to support fflush(NULL). */
     fflush(stderr);
     fflush(stdout);

     pid = vfork();
     if (pid == 0) {
	  /* child */
	  int i;
	  int open_max = (int)sysconf(_SC_OPEN_MAX);

	  for (i = 3; i < open_max; i++)
	       (void)close(i);
	  execvp(argv[0], argv);
	  appl_error(E_EXEC, argv[0], strerror(errno));
	  fflush(stderr);
	  _exit(127);
     }
     if (pid < 0) {
	  appl_error(E_FORK, strerror(errno));
	  return -1;
     }
     /* parent */
     while ((ret = wait(&status)) != pid)
	  if (ret < 0) {
	       appl_error(E_WAIT, strerror(errno));
	       return -1;
	  }
     if (WIFSIGNALED(status)) {
	  appl_error(E_SIGNAL, argv[0], WTERMSIG(status));
	  return -1;
     }
     /* Must have exited normally. */
     return WEXITSTATUS(status);
}

#endif /* SUPPORT_SUBDOC */

/*
Local Variables:
c-indent-level: 5
c-continued-statement-offset: 5
c-brace-offset: -5
c-argdecl-indent: 0
c-label-offset: -5
End:
*/
