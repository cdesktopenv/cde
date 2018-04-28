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
/* $XConsortium: CmdSpc.c /main/4 1995/10/26 15:02:34 rswiston $ */
/***************************************************************************
*
* File:         CmdSpc.c
* Description:  Messaging support for the command execution system.
* Language:     C
*
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
***************************************************************************/

#include "CmdInvP.h"

#include <Dt/UserMsg.h>
#include <Dt/CommandM.h>


/******************************************************************************
 *
 * _DtCmdSPCAddInputHandler - Register the Command Invoker AddInputHandler
 *
 * PARAMETERS: Those need by a SPC Add Input Handler.
 *
 * NOTES:
 *
 *   This function is needed by SPC to get a process termination 
 *   callback to get invoked.
 *
 *****************************************************************************/

SbInputId
_DtCmdSPCAddInputHandler ( 
	int fd,
	SbInputCallbackProc proc,
	void *data)
{
   if (cmd_Globals.app_context == NULL)
      return(XtAddInput(fd, 
			(XtPointer) XtInputReadMask, 
			proc, 
			data));
   else
      return(XtAppAddInput(cmd_Globals.app_context, 
			   fd, 
			   (XtPointer) XtInputReadMask, 
	                   proc, 
			   data));
}

/******************************************************************************
 *
 * _DtCmdSPCAddExceptionHandler - Register the Command Invoker AddExceptionHandler
 *
 * PARAMETERS: Those need by a SPC Add Exception Input Handler.
 *
 * NOTES:
 *
 *   This function is needed by SPC to get a process termination 
 *   callback to get invoked.
 *
 *****************************************************************************/

SbInputId
_DtCmdSPCAddExceptionHandler ( 
	int fd,
	SbInputCallbackProc proc,
	void *data)
{
   if (cmd_Globals.app_context == NULL)
      return(XtAddInput(fd, 
			(XtPointer) XtInputExceptMask, 
		        proc, 
			data));
   else
      return(XtAppAddInput(cmd_Globals.app_context, 
			  fd, 
			  (XtPointer) XtInputExceptMask, 
	                  proc, 
			  data));
}

/******************************************************************************
 * 
 * _DtCmdLogErrorMessage - write an error message to the default log file.
 *
 * PARAMETERS:
 *
 *   char *message;	- The error message to log.
 *
 *****************************************************************************/

void 
_DtCmdLogErrorMessage(
        char *message )
{
   _DtSimpleError (DtProgName, DtError, NULL, "%.2000s", message);
}
