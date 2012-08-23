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
/* $XConsortium: Process.C /main/3 1996/10/01 16:10:01 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Process.h"
#include "Invoke.h"

#include <string.h>
#include <stdlib.h>

Process::Process()
{
#ifdef aix
   (void)Invoke("ps -e -F \"pid ppid uid command\"", &procs);
#elif defined(__FreeBSD__)
   (void)Invoke("/bin/ps ax -o pid,ppid,uid,comm", &procs);
#else
   (void)Invoke("/bin/ps -el | awk '{printf(\"%s %s %s %s\\n\",$4,$5,$3,$NF)}'",
		&procs);
#endif
   pprocs = (char **)malloc(sizeof(char *));
   NumProcs = 0;
   strtok(procs, "\n");
   while(pprocs[NumProcs] = strtok(NULL, "\n"))
    {
      NumProcs++;
      pprocs = (char **)realloc(pprocs, sizeof(char *) * (NumProcs + 1));
    }
   last_pid = -1;
}

Process::~Process()
{
   free(procs);
   free(pprocs);
}

char *Process::GetByPid(pid_t _pid)
{
   int i;

   if (last_pid == _pid)
      return last_proc;
   if (_pid)
      for (i = 0; i < NumProcs; i++)
       {
	 long long_pid, long_ppid, long_uid;
	 sscanf(pprocs[i], "%ld %ld %ld", &long_pid, &long_ppid, &long_uid);
	 pid = (pid_t)long_pid;
	 ppid = (pid_t)long_ppid;
	 uid = (uid_t)long_uid;

         if (_pid == pid)
	  {
	     last_pid = _pid;
	     last_proc = pprocs[i];
	     return pprocs[i];
	  }
       }
   return NULL;
}

pid_t Process::Parent(pid_t pid)
{
   char *proc = GetByPid(pid);
   if (proc)
      return ppid;
   else
      return (pid_t)-1;
}

uid_t Process::UID(pid_t pid)
{
   char *proc = GetByPid(pid);
   if (proc)
      return uid;
   else
      return (uid_t)-1;
}

char *Process::Command(pid_t _pid)
{
   char *proc = GetByPid(_pid);
   if (proc)
    {
      char *s;
      // Find first field
      for (s = proc; *s == ' '; s++)
	 ;
      for ( ; *s != ' '; s++)
	 ;
      // Find second field
      for ( ; *s == ' '; s++)
	 ;
      for ( ; *s != ' '; s++)
	 ;
      // Find third field
      for ( ; *s == ' '; s++)
	 ;
      for ( ; *s != ' '; s++)
	 ;
      // Find fourth field
      for ( ; *s == ' '; s++)
	 ;
      return s;
    }
   else
      return NULL;
}
