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
/* $TOG: MkDir.c /main/8 1999/12/09 13:06:55 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           MkDir.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains functions to make directories.
 *
 *   FUNCTIONS: RunFileCommand
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <sys/param.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#include <unistd.h>
#include <limits.h>
#ifdef __hpux
#include <sys/getaccess.h>
#endif

#include <Xm/Xm.h>

#include <Dt/EnvControlP.h>
#include <Dt/DtNlUtils.h>
#include "Encaps.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"

/************************************************************************
 *
 *  RunFileCommand
 *    This function is called to fork a process and run a command in the
 *    event that PAM does not have the appropriate capabilities (i.e. PAM
 *    isn't superuser and superuser privilege is required).  PAM waits 
 *    for completion of the forked command process.
 *
 *  PARAMETERS:
 *
 *    command_path -- this is a pointer to the full pathname of the
 *    command to be run.
 *
 *    argument1 -- this is a pointer to the first command argument.
 *
 *    argument2 -- this is a pointer to the second command argument.
 *
 *    argument3 -- this is a pointer to the third command argument.
 *
 *  RETURN VALUE:
 *
 *    If the command could not be run or if it exited with a non-zero
 *    value then a non-zero value is returned.
 *
 ************************************************************************/

int
RunFileCommand(
        register char *command_path,
        register char *argument1,
        register char *argument2,
        register char *argument3)
{
   static char *pname = "RunFileCommand";
   register int child;           /* process id of command process */
   register int wait_return;     /* return value from wait */
            int exit_value;      /* command exit value */
   register char *command_name;  /* pointer to the command name */
   register int i;
   void (*oldSig)();
  
   /* prepare to catch the command termination */

   oldSig = signal (SIGCHLD, SIG_DFL);

   /* fork a process to run command */

   if ((child = fork ()) < 0)	/* fork failed */
   {
      (void) signal (SIGCHLD, oldSig);
      return (-1);
   }

   if (child != 0)		/* parend (PAM) process */
   {
      DBGFORK(("%s:  forked child<%d>\n", pname, child));

      do			/* wait for completion of command */
      {
         wait_return = wait (&exit_value);
      } while (wait_return != child);

      (void) signal (SIGCHLD, oldSig); /* child stopped or terminated */

      return (exit_value);      /* if exit_value == 0 then success */
   }
    

   DBGFORK(("%s:  child forked\n", pname));

   /*  child (command) process  */

   /*  redirect stdin, stdout, stderr to /dev/null  */

   for (i = 0; i < 3; i++)
   {
      (void) close (i);
      (void) open ("/dev/null", O_RDWR);
   }


   /*  set pointer to simple command name  */

   if ((command_name = (char *)strrchr (command_path, '/')) == 0)
      command_name = command_path;
   else
      command_name++;

   _DtEnvControl(DT_ENV_RESTORE_PRE_DT);
   (void) execl (command_path, command_name, argument1, argument2, argument3,0);
    
   DBGFORK(("%s:  child exiting\n", pname));

   exit (-1);                     /* error exit */
}
