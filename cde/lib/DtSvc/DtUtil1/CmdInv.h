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
/* $XConsortium: CmdInv.h /main/6 1996/01/23 10:41:03 barstow $ */
/***************************************************************************
*
* File:         CmdInv.h
* Description:  Public header for the command invocation system.
* Language:     C
*
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
***************************************************************************/

#ifndef _CmdInv_h
#define _CmdInv_h

#include <X11/Xlib.h>
#include <Dt/Message.h>
#include <Dt/Spc.h>

/******************************************************************************
 *
 * DtCmdInvExecuteProc - This type is is used to define the parameters
 *   needed in the callback functions for success and failure notification 
 *   of a Command Invoker execution call. 
 *
 *****************************************************************************/

typedef void (*DtCmdInvExecuteProc) (
	char *message,		/* NULL if the request is successful.  
				 * Otherwise an error message. */
	void *client_data
);

/******************************************************************************
 *
 * Function:  void _DtInitializeCommandInvoker (
 *	Display *display,
 *	char *toolClass,
 *	char *applicationClass,
 *	DtSvcReceiveProc reloadDBHandler,
 *	XtAppContext appContext)
 *
 * Parameters:		
 *
 *	display		 - The X server display connection.
 *			
 *	toolClass	 - The BMS tool class of the client.
 *
 *	applicationClass - The application class of the client (see 
 *			   XtInitialize).  This is needed to add the 
 *			   Command Invoker's resources to the client's
 *			   resources.
 *			
 *	reloadDBHandler  - Function to be called if a RELOAD-TYPES-DB 
 *			   request is made.  If the client does not read
 *			   DT action and/or filetype databases, NULL must
 *			   be used.
 *
 *      appContext	 - The client's application context.  Must be NULL
 *			   if the client does not use an application 
 *                         context.
 *
 * Purpose:		
 *
 * 	This function allows a client to internalize the functionality
 *	of the DT "Command Invoker".  By using this library and the
 *   	Action Library, requests for the Command Invoker will be done
 *	internally instead of sending a request to a separate Command
 *	Invoker process.
 *
 *	For local execution, the "fork" and "execvp" system calls are
 *	used.  For remote execution, the "SPCD" is used.
 *			
 *****************************************************************************/

extern void
_DtInitializeCommandInvoker(
	Display *display,
	char *toolClass,
	char *applicationClass,
	DtSvcReceiveProc reloadDBHandler,
	XtAppContext appContext);

/******************************************************************************
 *
 * Function:  void _DtCommandInvokerExecute (
 *	char *request_name,
 *	char *context_host,
 *	char *context_dir,
 *	char *context_file,
 *	char *exec_parameters,
 *	char *exec_host,
 *	char *exec_string,
 *	DtCmdInvExecProc success_proc,
 *	void *success_data,
 *	DtCmdInvExecProc failure_proc,
 *	void *failure_data)
 *
 * Parameters:		
 *
 *	request_name	- The request name (defined in "CommandM.h").
 *			
 *	context_host	- Name of the host where request is executed from.  If
 *			  NULL, the "exec_host" parameter is used.  Note: this
 *			  should not be confused with execution host, described
 *			  below.
 *
 *	context_dir	- Directory where the request should be executed.  If
 *			  NULL, the HOME directory is used.
 *
 *	context_file	- Not currently used by the Command Invoker.
 *
 *	exec_parameters - Command Invoker execution parameters.  See External
 *			  Specification for more information.
 *			  If set to NULL, "-" is used.
 *
 *	exec_host	- Name of the host where the request is executed.
 *
 *	exec_string	- The command line to execute.
 *
 *	success_proc	- The function to be invoked if the request is 
 *			  successfully executed.
 *
 *	success_data	- Client data for successful execution.
 *
 *	failure_proc	- The function to be invoked if an attempt to exeucte
 *			  the request fails.
 *
 *	failure_data	- Client data for unsuccessful execution.
 *
 * Purpose:		
 *
 * 	This function allows a client to use the DT "Command Invoker"
 *	Library for its' process execution.  This function is intended
 *	for processes which do not use the Action Library.
 *
 *	For local execution, the "fork" and "execvp" system calls are
 *	used.  For remote execution, the "SPCD" is used.
 *
 * Notes:
 *
 *      This function must be preceded by a call to
 *	"_DtInitializeCommandInvoker".
 *			
 *****************************************************************************/

extern void
_DtCommandInvokerExecute(
	char *request_name,
	char *context_host,
	char *context_dir,
	char *context_file,
	char *exec_parameters,
	char *exec_host,
	char *exec_string,
	DtCmdInvExecuteProc success_proc,
	void *success_data,
	DtCmdInvExecuteProc failure_proc,
	void *failure_data);

extern SPC_Channel_Ptr _DtSPCOpen( char *hostname,
         int iomode,
         char *errorMessage);				/* MODIFIED */

extern int _DtSPCSpawn(
        char                    *path,
        char                    *context,
        char                    **args,
        char                    **env,
        SPC_Channel_Ptr         chan,
        char                    *execHost,
        char                    *contextHost,
        char                    *contextDir,
        char                    *errorMessage);		/* MODIFIED */

#endif /* _CmdInv_h */
