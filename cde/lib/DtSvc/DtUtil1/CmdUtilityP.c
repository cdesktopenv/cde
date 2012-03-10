/* $XConsortium: CmdUtilityP.c /main/4 1995/10/26 15:02:50 rswiston $ */
/***************************************************************************
*
* File:         CmdUtilityP.c
* Description:  Private Utility routines for the command invocation system.
* Language:     C
*
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
***************************************************************************/

#include "CmdInvP.h"

#include <stdio.h>
#include <sys/param.h>

#ifdef __apollo
#include "/sys5/usr/include/unistd.h"
#else
#include <unistd.h>
#endif
#include <limits.h>
#include <sys/stat.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Dt/CommandM.h>

/*****************************************************************************
 * 
 * _DtCmdBuildPathList - this functions builds a list of the path names
 *   that are part of the $PATH environment variable.  This is done once
 *   during initialization and the list is then searched whenever
 *   a command is going to be executed.
 *
 * MODIFIED:
 *
 *   char **cmd_Globals.path_list; 	- Initialized or set to NULL if 
 *					  $PATH is undefined
 *
 *****************************************************************************/

void 
_DtCmdBuildPathList( void )
{
   int i=0;
   char *path;
   int indx;
   int  n=0, pathLen;
   char *pend, 		/* Points to the beginning of a directory */
	*pbeg;		/* Points to the end of a directory */

   /* Get the PATH */
   if ((path = (char *) (getenv ("PATH"))) == NULL) {
      cmd_Globals.path_list = (char **) NULL;
      return;
   }

   pathLen = strlen(path);
   pbeg = path;

   while (i < pathLen) {
      n++;
      cmd_Globals.path_list = (char **) XtRealloc ((char *)cmd_Globals.path_list, n * sizeof (char *)); 

      if ((indx = DtStrcspn (pbeg, ":")) >= pathLen) {
	 /* At the end of the path */
	 i = pathLen;
	 pend = (char *) pbeg + indx;
      }
      else {
	 /* Found a ":" */
	 pend = (char *) pbeg + indx;
	 i += (pend - pbeg) +1;
      }

      cmd_Globals.path_list[n-1] = (char *) XtMalloc (((pend - pbeg) + 1) * 
						       sizeof(char));
      (void) strncpy (cmd_Globals.path_list[n-1], pbeg, (pend - pbeg));
      /* Strncpy does not put a '\0' at the EOS if s2 >= s1 */
      cmd_Globals.path_list[n-1][pend-pbeg] = '\0';

      /* Move past the ":" */
      if (i < pathLen) 
	 pbeg = pend +1;
   }

   /* May need to NULL terminate cmd_Globals.path_list */
   if (n > 0) {
      n++;
      cmd_Globals.path_list = (char **) XtRealloc ((char *) cmd_Globals.path_list, n * sizeof (char *));
      cmd_Globals.path_list [n-1] = (char *) NULL;
   }
}
