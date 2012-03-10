/* $XConsortium: Process.h /main/4 1996/10/01 16:10:06 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

class Process {

   int NumProcs;
   pid_t last_pid;
   char **pprocs;
   char *procs;
   char *last_proc;
   char *GetByPid(pid_t);
   uid_t uid;
   pid_t pid;
   pid_t ppid;

 public:

   Process();
   ~Process();

   pid_t Parent(pid_t pid);
   uid_t UID(pid_t pid);
   char *Command(pid_t pid);
};

#endif // PROCESS_H
