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
/* $TOG: CmdMain.c /main/15 1998/04/20 12:46:37 mgreess $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/***************************************************************************
*
* File:         CmdMain.c
* Description:  Command execution system
* Language:     C
*
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
***************************************************************************/

#include "CmdInvP.h"
#include <Dt/CmdInv.h>

#include <fcntl.h>
#ifdef __apollo
#include "/sys5/usr/include/sys/termio.h"
#else
#include <termios.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef _SUN_OS   /* to get the define for NOFILE */
#include <sys/param.h>
#endif /* _SUN_OS */
#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <Dt/CommandM.h>
#include <Dt/EnvControlP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Utility.h>

#include <Dt/ActionDb.h>
#include <Dt/ActionUtilP.h>
#include "myassertP.h"
#include "DtSvcLock.h"

#include <SPC/spcE.h>
#include <SPC/spcP.h>
#include <SPC/spc-proto.h>
#include <Tt/tt_c.h>

#define MAX_EXEC_ARGS		1000    /* Maximum number of arguments for */
					/* execvp call. */
/*
 * Dtexec return status:
 */
#define COMMAND_CHECK_FAILURE           1

#ifdef __hpux
#ifdef hpV4
#define INETD_SECURITY_FILE	"/var/adm/inetd.sec"
#else /* hpV4 */
#define INETD_SECURITY_FILE	"/usr/adm/inetd.sec"
#endif /* hpV4 */
#endif /* __hpux */

#define Cmd_FreeAllocatedStringVector(sv) \
		_DtCmdFreeStringVector(sv);\
		XtFree((char *)sv);

/*
 * Global variables for the Command Invoker.
 */
static char _cmdClientHost[MAXHOSTNAMELEN];

/*
 * Static variables for the Command Invoker.
 */
static Cmd_RequestQueue *requestQueue = NULL;

/*
 * Static function declarations:.
 */

static void QueueRequest (
			SPC_Channel_Ptr channel,
			char *context,
			char *execHost,
			char *execString,
			char **argv,
                        int  windowType,
			unsigned long requestNum,
			DtSvcMsgContext replyContext,
			DtCmdInvExecuteProc success_proc,
			void *success_data,
			DtCmdInvExecuteProc failure_proc,
			void *failure_data);
static void ExecuteQueuedRequest ( 
			unsigned long requestNum);

static void FreeRequest (Cmd_RequestQueue *pNode);
			
static void DtexecTerminator (
                        SPC_Channel_Ptr cmdChannel,
                        int pid,
                        int type,
                        int cause,
                        unsigned long ind) ;
static void CheckCommandTerminator (
                        SPC_Channel_Ptr cmdChannel,
                        int pid,
                        int type,
                        int cause,
                        unsigned long ind) ;
static int DtCmdGetWindowType(
        unsigned long windowTypeMask);
static void _DtCmdInitializeErrorMessages(void);

/*
 * Command invocatin error messages.
 */
static char 	*errorExec,
		*errorSpawn,
		*errorFork,
		*errorSpcTerminator,
		*errorLength,
     		*errorRequest,
		*errorChdir,
		*errorRemoteSubprocess,
		*errorUnknownHost,
		*errorBadConnect,
		*errorBadService,
		*errorRegisterHandshake,
		*errorRegisterUsername,
		*errorRegisterNetrc,
		*errorRegisterOpen,
		*errorEnvTooBig,
		*errorInetSecurity,
		*successHost;

/*******************************************************************************
 *
 * _DtSPCSpawn()
 *	This is a wrapper around DtSPCSpawn (i.e. XeSPCSPawn) that makes sure
 *	the original environment is restored before the spawn and the DT
 *      environment is reinstated after the spawn.  It returns the value 
 *	originally returned by DtSPCSpawn.
 *
 ******************************************************************************/

int
_DtSPCSpawn( 
	char		*path, 
	char		*cwd,
	char		**args,
	char		**env,
	SPC_Channel_Ptr	chan,
	char		*execHost,
	char		*contextHost,
	char		*contextDir,
	char		*errorMessage)
{
	int	retVal;

	/*
	 * Restore the original environment
	 */
	(void) _DtEnvControl (DT_ENV_RESTORE_PRE_DT);

	/*
	 * Map some env var paths to execHost.
	 */
	(void) _DtEnvMapForRemote(execHost);

	if ((retVal = XeSPCSpawn(path,
				 cwd,
				 args,
				 env,
				 chan)) == SPC_ERROR)
	{
		switch (DtSPCErrorNumber) 
		{
			case SPC_cannot_Chdir:
	    			(void) sprintf (errorMessage, 
						errorChdir, 
						contextDir, 
						execHost);
				break;
			case SPC_Cannot_Fork:
	    			(void) sprintf (errorMessage, 
						errorFork, 
						execHost);
				break;
	 		case SPC_Env_Too_Big:
			case SPC_Arg_Too_Long:
	    			(void) sprintf (errorMessage, 
						errorLength, 
						SPC_BUFSIZ);
				break;
			default:
				/*
				 * SPC_Cannot_Exec
				 */
				(void) sprintf (errorMessage, 
						errorSpawn,
						execHost,
						path);
      		}
	}

	/*
	 * Restore some env var paths.
	 */
	(void) _DtEnvRestoreLocal();

	/*
	 * Restore the DT environment
	 */
	(void) _DtEnvControl (DT_ENV_RESTORE_POST_DT);

	/*
	 * Return the result of DtSPCSpawn
	 */
	return retVal;
}

/*******************************************************************************
 *
 * _DtSPCOpen()
 *	This is a wrapper around DtSPCOpen (i.e. XeSPCOpen) that makes sure
 *	the original environment is restored before the spawn and the DT
 *      environment is reinstated after the spawn.  It returns the value 
 *	originally returned by DtSPCOpen.
 *
 ******************************************************************************/
SPC_Channel_Ptr
_DtSPCOpen(
	char *hostname, 
	int iomode, 
	char *errorMessage)
{
	SPC_Channel_Ptr	chan;
	_Xgetpwparams	pwd_buf;
	struct passwd *	pwd_ret;

	/*
	 * Restore the original environment
	 */
	(void) _DtEnvControl (DT_ENV_RESTORE_PRE_DT);

	/*
	 * Map some env var paths to execHost.
	 */
	(void) _DtEnvMapForRemote(hostname);

	if ((chan = XeSPCOpen(hostname, iomode)) == SPC_ERROR)
	{
		uid_t		this_uid;
		char		*username;

		switch (DtSPCErrorNumber) 
		{
			case SPC_Unknown_Host:
				(void) sprintf (errorMessage, 
						errorUnknownHost,
						hostname);
				break;
			case SPC_Bad_Connect:
				(void) sprintf (errorMessage, 
						errorBadConnect,
						hostname,
						SPC_SERVICE,
						_cmdClientHost);
				break;
			case SPC_Bad_Service:
				(void) sprintf (errorMessage, 
						errorBadService,
						SPC_SERVICE,
						_cmdClientHost);
				break;
			case SPC_Register_Handshake:
				this_uid = getuid();
				if((pwd_ret = _XGetpwuid(this_uid, pwd_buf)) == 
				   NULL)
					username = NULL;
				else
					username = pwd_ret->pw_name;
				(void) sprintf (errorMessage, 
						errorRegisterHandshake,
						hostname,
						username,
						this_uid,
						_cmdClientHost,
						hostname);
				break;
			case SPC_Register_Username:
				this_uid = getuid();
				if((pwd_ret = _XGetpwuid(this_uid, pwd_buf)) == 
				   NULL)
					username = NULL;
				else
					username = pwd_ret->pw_name;
				(void) sprintf (errorMessage, 
						errorRegisterUsername,
						hostname,
						username);
				break;
			case SPC_Register_Netrc:
				(void) sprintf (errorMessage, 
						errorRegisterNetrc,
						hostname);
				break;
			case SPC_Env_Too_Big:
				(void) sprintf (errorMessage, 
						errorEnvTooBig,
						hostname,
						SPC_BUFSIZ);
				break;
			case SPC_Connection_EOF:
#ifdef __hpux
				(void) sprintf (errorMessage, 
						errorInetSecurity,
						hostname,
						_cmdClientHost,
						SPC_SERVICE,
						_cmdClientHost,
						SPC_SERVICE,
						INETD_SECURITY_FILE,
						hostname);

#else /* __hpux */
				(void) sprintf (errorMessage, 
						errorBadConnect,
						hostname,
						SPC_SERVICE,
						_cmdClientHost);
#endif /* __hpux */
				break;
			default:
				/*
				 * SPC_Register_Open:
				 */
				(void) sprintf (errorMessage, 
						errorRegisterOpen,
						hostname);
      		}
      	}
	
	/*
	 * Restore some env var paths.
	 */
	(void) _DtEnvRestoreLocal();

	/*
	 * Restore the DT environment
	 */
	(void) _DtEnvControl (DT_ENV_RESTORE_POST_DT);
	
	return chan;
}


/******************************************************************************
 *
 * QueueRequest - takes the "state" from a request and puts it on the
 *   "requestQueue".
 *
 * PARAMETERS:
 *
 *	SPC_Channel_Ptr channel;		- Spcd channel id.
 *	char *context;				- Context for SPCSpawn.
 *	char *execHost;				- The execution host.
 *	char *execString;			- The execution string.
 *	char **argv;				- Arg vector for SPCSpawn.
 *						  (Arg vector is XtFree'd)
 *      int  windowType;                        - window type of queued command.
 *	unsigned long requestNum;		- Id number into the queue.
 *	DtSvcMsgContext replyContext;		- Reply info.
 *	DtCmdInvExecuteProc success_proc;	- Success callback.
 *	void *success_data;			- Success client_data.
 *	DtCmdInvExecuteProc failure_proc;	- Failure callback.
 *	void *failure_data;			- Failure client_data.
 *
 * MODIFIED:
 *
 *   Cmd_RequestQueue *requestQueue;		- This request to added.
 *
 *****************************************************************************/

static void
QueueRequest (
	SPC_Channel_Ptr channel,
	char *context,
	char *execHost,
	char *execString,
	char **argv,
        int  winType,
	unsigned long requestNum,
	DtSvcMsgContext replyContext,
	DtCmdInvExecuteProc success_proc,
	void *success_data,
	DtCmdInvExecuteProc failure_proc,
	void *failure_data)

{
   Cmd_RequestQueue *pNode;
   Cmd_RequestQueue *pNewNode;

   pNewNode = (Cmd_RequestQueue *) XtMalloc (sizeof (Cmd_RequestQueue));

   pNewNode->next = (Cmd_RequestQueue *) NULL;
   pNewNode->channel = channel;
   pNewNode->context = XtNewString (context);
   pNewNode->exec_host = XtNewString (execHost);
   pNewNode->exec_string = XtNewString (execString);
   pNewNode->argv = argv;
   pNewNode->winType = winType;
   pNewNode->request_num = requestNum;
   if (replyContext == NULL)
      pNewNode->replyContext = NULL;
   else
      pNewNode->replyContext = replyContext;
   pNewNode->success_proc = success_proc;
   pNewNode->success_data = success_data;
   pNewNode->failure_proc = failure_proc;
   pNewNode->failure_data = failure_data;

   if (requestQueue == NULL) 
   {
      requestQueue = pNewNode;
      return;
   }

   /*
    * Find the End Of the Queue and link in the NewNode.
    */
   for (pNode = requestQueue; pNode->next != NULL; pNode = pNode->next);

   pNode->next = pNewNode;

}

/******************************************************************************
 *
 * ExecuteQueuedRequest - given a key into the requestQueue (requestNum)
 *   find the appropriate request and execute it.
 *
 * PARAMETERS:
 *
 *   unsigned long 	requestNum;		- Key into the requestQueue.
 *
 * MODIFIED:
 *
 *   Cmd_RequestQueue	*requestQueue;		- The executed request gets
 *						  freed.
 *
 *****************************************************************************/

static void
ExecuteQueuedRequest (
	unsigned long requestNum)
{
   char			*errorMessage;
   Boolean 		success = True;
   Cmd_RequestQueue 	*prev  = NULL;
   Cmd_RequestQueue 	*pNode = requestQueue;
   unsigned long	iomode;

   for (; pNode != NULL; pNode = pNode->next)
   {
       if ( pNode->request_num == requestNum )
       {
           /*
            * Pluck pNode out of the queue
            */
           if (prev)
               prev->next = pNode->next;
           else
               requestQueue = pNode->next;

           pNode->next = NULL;
           break;
       }
       /*
        * Move alone to the next node
        */
       prev = pNode;
   }

   if (pNode == NULL)
      return;

   errorMessage = XtMalloc (MAX_BUF_SIZE * sizeof (char));

   /*
    *  Reopen SPC Channel
    */


   iomode = ( SPCIO_NOIO 
            | SPCIO_SYNC_TERMINATOR 
            | SPCIO_FORCE_CONTEXT );

   
   if ((pNode->channel = (_DtSPCOpen(pNode->exec_host, 
          iomode,
	  errorMessage))) == SPC_ERROR) 
      {
	 success = False;
      }

   if ( success )
       if ((_DtSPCSpawn(pNode->argv[0], pNode->context, pNode->argv, NULL, 
		    pNode->channel, pNode->exec_host, NULL, NULL,
		    errorMessage)) == SPC_ERROR) 
       {
	  success = False;
	  if (DtSPCErrorNumber != SPC_Arg_Too_Long)
	     DtSPCClose(pNode->channel);
       }

   if (success && pNode->success_proc != NULL)
   {
      if (cmd_Resources.executionHostLogging && pNode->success_data != NULL)
      {
	 CallbackData *data;
	 data = (CallbackData *) pNode->success_data;
	 (void) sprintf (errorMessage, successHost, 
			 data->actionLabel, pNode->exec_host);
	 _DtCmdLogErrorMessage (errorMessage);
      }

      (*pNode->success_proc) (NULL, pNode->success_data);
   }
   else if (!success)
   {
      if (cmd_Resources.executionHostLogging)
      {
	 if (DtSPCErrorNumber == SPC_Arg_Too_Long)
	 {
	    int cmdlen,i;
	    char *cmdp;	/* pointer to complete command string */
	    char *tmp_message;

	    /*
	     * The message should include all of the command because 
	     * the problem may be with some on the internally generated 
	     * parts of the command (e.g. the terminal emulator and args).  
	     * This means going through all of argv to determine the 
	     * length of the string.
	     */
	    for (cmdlen = 0, i = 0; pNode->argv[i]; i++) {
	       cmdlen+=strlen(pNode->argv[i]);
	       cmdlen+=2; /* make room for a space + string terminator */
	    }
	 
	    tmp_message = (char *) XtMalloc (strlen (errorSpawn) + 
				             strlen (pNode->exec_host) +
				             cmdlen + 4);
	    cmdp = (char *) XtMalloc(cmdlen + 1);
	    *cmdp = '\0';
	    for (i = 0; pNode->argv[i]; i++) {
	       strcat(cmdp,pNode->argv[i]);
	       strcat(cmdp, " ");
	    }
	    (void) sprintf (tmp_message, errorSpawn, pNode->exec_host, cmdp);
	    _DtCmdLogErrorMessage (tmp_message);
	    XtFree(cmdp);
	    XtFree(tmp_message);
	 }
      }
      if (pNode->failure_proc != NULL)
	 (*pNode->failure_proc) (errorMessage, pNode->failure_data);
   }

   XtFree ((char *)errorMessage);
   FreeRequest (pNode);
}

Cmd_RequestQueue *
_DtCmdGetQueueHead(void)
{
	return requestQueue;
}

/******************************************************************************
 *
 * FreeRequest - Frees the malloced data associated with the node.
 *               and frees the node.
 *
 * PARAMETERS:
 *
 *  Cmd_RequestQueue *pNode
 *
 *
 *****************************************************************************/

static void
FreeRequest (Cmd_RequestQueue *pNode)
{

   if (pNode == NULL)
      return;

   XtFree (pNode->context);
   XtFree (pNode->exec_host);
   XtFree (pNode->exec_string);
   Cmd_FreeAllocatedStringVector (pNode->argv);

   XtFree ((char *) pNode);
}

/******************************************************************************
 *
 * _DtCmdCommandInvokerExecute - executes a request on the specified host.
 *
 * RETURNS:  int
 *
 *   _CMD_EXECUTE_SUCCESS	- successful execution
 *   _CMD_EXECUTE_FAILURE	- execution failed
 *   _CMD_EXECUTE_QUEUED	- the request was queued
 *   _CMD_EXECUTE_FATAL		- the request contains invalid information
 *
 *
 *****************************************************************************/

int
_DtCmdCommandInvokerExecute (
	char *errorMessage,		/* MODIFIED */
	DtSvcMsgContext replyContext,	/* OBSOLETE -- always NULL */
	int  winMask,
	char *contextHost,
	char *contextDir,
	char *contextFile,		/* OBSOLETE -- always NULL */
	char *execParms,
	char *execHost,
	char *execString,
        char *procId,
        char *tmpFiles,
	DtCmdInvExecuteProc success_proc,
	void *success_data,
	DtCmdInvExecuteProc failure_proc,
	void *failure_data)

{
   int ioMode, i, index1;
   int windowType;
   pid_t commandPid;
   char context[MAXPATHLEN];
   char tmpDir [MAXPATHLEN];
   char **commandArray;
   SPC_Channel_Ptr cmdChannel;
   char *theCommand = NULL;
   Boolean terminalRequest = False;
   char *commandArray2[MAX_EXEC_ARGS];
   Boolean localExecution = True;
   Boolean xhostError;
   static unsigned long requestNum = 0;
   char *toolRequest = NULL;	/* backward compatibility kludge */


   myassert( !(contextFile && replyContext) );

   /*
    * Check for a valid window-type.
    * This check is probably redundant but it converts the mask bits into
    * small integer values used by the rest of the command invoker code.
    */
   if ((windowType=
	DtCmdGetWindowType(winMask))== -1) 
   {
      (void) sprintf (errorMessage, errorRequest, toolRequest, 
		      DtTERMINAL, DtPERM_TERMINAL, DtOUTPUT_ONLY, 
		      DtSHARED_OUTPUT, "" /* Obsolete shell window */,
                      DtNO_STDIO);
      return (_CMD_EXECUTE_FATAL);
   }

   /* 
    * Create the command to be exec'ed.
    */
   if (windowType == PERM_TERMINAL || windowType == TERMINAL) 
   {
      _DtCmdCreateTerminalCommand (&theCommand, windowType, execString, 
	 execParms, execHost, procId, tmpFiles);
      terminalRequest = True;
   }
   else 
   {
      /* 
       * NO-STDIO || START-SESSION request.
       */
      
      theCommand = XtMalloc( 
         + strlen (cmd_Resources.dtexecPath)
         + strlen(" -open ") + 4 /* waitTime len */
         + strlen(" -ttprocid ") + strlen(_DtActNULL_GUARD(procId))
         + strlen(_DtActNULL_GUARD(tmpFiles)) 
         + strlen (execString) 
         + 5 /* for 2 quotes,2 blanks,null */);

      sprintf(theCommand,"%s -open %d -ttprocid '%s' %s %s",
          cmd_Resources.dtexecPath,
          0 /* wait time zero for NO_STDIO */,
          _DtActNULL_GUARD(procId),
          _DtActNULL_GUARD(tmpFiles), 
          execString);

   }

   /*
    * See if the request requires Remote Execution.
    */
   localExecution = _DtIsSameHost(execHost,NULL);

   /*
    * If this is a terminalRequest and the Command Invoker subprocess
    * is not executable, return now.
    */
   if (localExecution && terminalRequest && !cmd_Globals.subprocess_ok) 
   {
      if (!(_DtCmdCheckForExecutable (cmd_Resources.dtexecPath))) 
      {
	 (void) sprintf (errorMessage, cmd_Globals.error_subprocess,
			 cmd_Resources.dtexecPath);
	 XtFree ((char *) theCommand);
	 return (_CMD_EXECUTE_FAILURE);
      }
      else
	 cmd_Globals.subprocess_ok = True;
   }

   /*
    * If this is a terminalRequest and the terminal emulator
    * is not executable, return now.
    */
   if (localExecution && terminalRequest && !cmd_Globals.terminal_ok) 
   {
      if (!(_DtCmdCheckForExecutable (cmd_Resources.localTerminal))) 
      {
	 (void) sprintf (errorMessage, cmd_Globals.error_terminal, 
			 cmd_Resources.localTerminal);
	 XtFree ((char *) theCommand);
	 return (_CMD_EXECUTE_FAILURE);
      }
      else
	 cmd_Globals.terminal_ok = True;
   }

   /* 
    * Break the command into something execvp or SPCSpawn can handle
    * and then free "theCommand" if this is a termianl-based request.
    */
   commandArray = (char **) XtMalloc (MAX_EXEC_ARGS * sizeof (char *));
   _DtCmdStringToArrayOfStrings (theCommand, commandArray);

   XtFree (theCommand);

   if (!localExecution) 
   {
      char *netfile;
      char *argv[4];
      char *tmp;

      /* REMOTE Execution */

      ioMode = SPCIO_NOIO | SPCIO_SYNC_TERMINATOR | SPCIO_FORCE_CONTEXT;

      if ((cmdChannel = (_DtSPCOpen(execHost, 
			 ioMode, 
			 errorMessage))) == SPC_ERROR) 
      {
	 Cmd_FreeAllocatedStringVector (commandArray);
	 return (_CMD_EXECUTE_FAILURE);
      }

      /* Old syntax should no longer appear in contextHost/Dir */
      myassert( (contextHost?*contextHost != '*':1) && 
		(contextDir?*contextDir != '*':1) );
      /* 
       * Create a "netfile" for the cwd to be used.
       */
      netfile = (char *) tt_host_file_netfile (
	 ((contextHost == NULL) ? execHost : contextHost),
	 ((contextDir  == NULL) ? (char *) getenv ("HOME") : contextDir));

      if (tt_pointer_error (netfile) != TT_OK) {
	 (void) sprintf (errorMessage, cmd_Globals.error_directory_name_map,
	    ((contextDir  == NULL) ? (char *) getenv ("HOME") : contextDir),
	    ((contextHost == NULL) ? execHost : contextHost),
	    tt_status_message (tt_pointer_error(netfile)));
	 Cmd_FreeAllocatedStringVector (commandArray);
	 return (_CMD_EXECUTE_FAILURE);
      }
      (void) strcpy (context, netfile);
      tt_free (netfile);

     /*
      * First check to see if the "dtexecPath" is executable on
      * the remote execution host by executing it with no
      * options which will cause it to immediately die.
      *
      * There is no need to set up termination handler for this
      * because we don't care when it dies, we only care about
      * whether or not it can be executed.
      */

     argv[0] = cmd_Resources.dtexecPath;
     argv[1] = (char *) NULL;

     if ((_DtSPCSpawn(argv[0], context, argv, NULL, cmdChannel,
		      execHost, contextHost, contextDir, errorMessage)) 
	  == SPC_ERROR) 
     {
	if (DtSPCErrorNumber != SPC_cannot_Chdir &&
	    DtSPCErrorNumber != SPC_Cannot_Fork  &&
	    DtSPCErrorNumber != SPC_Env_Too_Big  &&
	    DtSPCErrorNumber != SPC_Arg_Too_Long)
	   /*
	    * The Error message must mention that the dtexec
	    * process is not executable so must overwrite the
	    * error message returned by the Spawn function with
	    * an appropriate message.
	    */
	   (void) sprintf (errorMessage, errorRemoteSubprocess, execHost, 
			   cmd_Resources.dtexecPath);
	DtSPCClose(cmdChannel);
	Cmd_FreeAllocatedStringVector (commandArray);
	return (_CMD_EXECUTE_FAILURE);
     }

     /* The dtexec process is now known to exist on the remote host */

     /*
      *  Now run a test to see if the command is executable
      *  on this exec host.
      */
     _DtCmdStringToArrayOfStrings (execString, commandArray2);

     tmp = (char *) XtMalloc (strlen (commandArray2[0]) +
			     strlen ("whence ") + 2);
     (void) sprintf (tmp, "whence %s", commandArray2[0]);

     _DtCmdFreeStringVector (commandArray2);

     argv[0] = "ksh";
     argv[1] = "-c";
     argv[2] = tmp;
     argv[3] = (char *) NULL;

     /*
      * Reopen the channel
      */
     if ((cmdChannel = (_DtSPCOpen(execHost, 
			 ioMode, 
			 errorMessage))) == SPC_ERROR) 
      {
	 Cmd_FreeAllocatedStringVector (commandArray);
	 return (_CMD_EXECUTE_FAILURE);
      }

      /* 
       * Set up a callback to be invoked when the test command 
       * terminates.
       */
      _DtSvcProcessLock();
      if ((DtSPCRegisterTerminator(cmdChannel,
		    (SPC_TerminateHandlerType) CheckCommandTerminator,
		    (void *) ++requestNum)) == SPC_ERROR) 
      {
	DtSPCClose(cmdChannel);
	Cmd_FreeAllocatedStringVector (commandArray);
	(void) strcpy (errorMessage, errorSpcTerminator);
	XtFree ((char *) tmp);
	_DtSvcProcessUnlock();
	return (_CMD_EXECUTE_FAILURE);
      }
      
      if ((_DtSPCSpawn(argv[0], context, argv, NULL, cmdChannel,
		      execHost, contextHost, contextDir, errorMessage)) 
	  == SPC_ERROR) 
      {
	DtSPCClose(cmdChannel);
	(void) sprintf (errorMessage, errorRemoteSubprocess, execHost,
                argv[0]);
	Cmd_FreeAllocatedStringVector (commandArray);
	XtFree ((char *) tmp);
	_DtSvcProcessUnlock();
	return (_CMD_EXECUTE_FAILURE);
      }
      /* 
       * The command line checking process has been spawned.  
       * There is nothing left to do but to queue the request
       * and return to the client's main loop.  The command
       * line will be executed after the above spawned process
       * terminates.
       */
      QueueRequest (cmdChannel, context, execHost, execString, 
	      commandArray, windowType, requestNum, replyContext,
              success_proc, success_data, failure_proc, failure_data);
      _DtSvcProcessUnlock();
      XtFree(tmp);
      return (_CMD_EXECUTE_QUEUED);
   }
   else 
   {   	 
      /* LOCAL Execution */

      /* 
       * Must first check to see if the execvp will potentially fail.
       *
       * Since the terminal emulator is pre-appended onto the execution
       * string, don't want to check it (should have been done during
       * startup (in _DtInitializeCommandInvoker)) but must check the
       * execution string that was passed in as part of the message.
       */
     /* Break the command into something execvp can handle */
       _DtCmdStringToArrayOfStrings (execString, commandArray2);

      if (!_DtCmdCheckForExecutable (commandArray2[0])) 
      {
         (void) sprintf (errorMessage, errorExec, commandArray2[0]);
	 Cmd_FreeAllocatedStringVector (commandArray);
	 _DtCmdFreeStringVector (commandArray2);
	 return (_CMD_EXECUTE_FAILURE);
      }
      _DtCmdFreeStringVector (commandArray2);

      /*
       * Save the current directory and then "chdir" to the directory
       * to do the execution.  If the chdir fails, return.
       */
      if(NULL == getcwd (tmpDir, MAXPATHLEN))
      {
	 perror(strerror(errno));
	 return (_CMD_EXECUTE_FAILURE);  
      }

      if (!_DtCmdValidDir (_cmdClientHost, contextDir, contextHost)) 
      {
	 Cmd_FreeAllocatedStringVector (commandArray);
	 (void) sprintf (errorMessage, errorChdir, contextDir, execHost);
	 if(-1 == chdir (tmpDir)) {
            perror(strerror(errno));
         }
	 return (_CMD_EXECUTE_FAILURE);
      }

      /*
       * Restore the original environment and remove any DT
       * specific environment variables that were added.
       */
      (void) _DtEnvControl (DT_ENV_RESTORE_PRE_DT);

      /* Fork and then execvp the execution string */
      for (index1 = 0; (index1 < 10) && 
	       ((commandPid = fork ()) < 0); index1++) 
      {
	 /* Out of resources ? */
	 if (errno != EAGAIN) 
	    break;
	 /* If not out of resources, sleep and try again */
	 (void) sleep ((unsigned long) 2);
      }

      if (commandPid < 0) 
      {
	 Cmd_FreeAllocatedStringVector (commandArray);
	 if(-1 == chdir (tmpDir)) {
	     perror(strerror(errno)); 
         }
         (void) sprintf(errorMessage, errorFork, execHost);
         (void) _DtEnvControl (DT_ENV_RESTORE_POST_DT);
	 return (_CMD_EXECUTE_FAILURE);
      }

      if (commandPid == 0) 
      {

#if defined(__hp_osf) || defined(__osf__) || defined(CSRG_BASED)
         setsid() ;
#else
         (void) setpgrp ();
#endif
        
	 if (!terminalRequest ) 
	 {
	    int fd;

	    /*
	     * Close stdout and redirect it to /dev/null.  If this
	     * is not done and the request writes to stdout, the
	     * output will be queued in an "unlinked" file in
	     * /tmp until the client using this code terminates.
	     */
	    if ((fd = open ("/dev/null", O_RDWR)) > 0)
	       (void) dup2 (fd, fileno (stdout));
	 }

	 /* 
	  * Mark file descriptiors >=3 as "Close on Exec".
	  */
	 {
	   long	open_max;

	   open_max = sysconf(_SC_OPEN_MAX);
	   if (open_max == -1)
	   {
#ifdef _SUN_OS
             open_max = NOFILE;
#else
#if defined(USL) || defined(__uxp__) || defined(_AIX)
             open_max = FOPEN_MAX;
#else
             open_max = FD_SETSIZE;
#endif
#endif /* _SUN_OS */
	   }

           for (i=3; i < open_max; i++)
	       (void) fcntl (i, F_SETFD, 1);
	 }


	 (void) execvp (commandArray[0], commandArray);
	 
	 /* Should never get here, but if you do, must exit */

	 /*
	  * The following message will be written to the errorlog
	  * file if the request is not a terminal requests or
	  * to the terminal window if the request requires a
	  * terminal.
	  */
	 (void) sprintf (errorMessage, errorExec, commandArray[0]);
	 (void) printf ("%s\n", errorMessage);
	 (void) _exit (1);
      }

      /*
       * Restore the pre-fork environment.
       */
      (void) chdir (tmpDir);
      (void) _DtEnvControl (DT_ENV_RESTORE_POST_DT);
   }

   Cmd_FreeAllocatedStringVector (commandArray);

   return (_CMD_EXECUTE_SUCCESS);
}

/******************************************************************************
 *
 * CheckCommandTerminator - this is the SPC termination callback 
 *   that is invoked when the command line checking process terminates.  
 *
 *   When this callback is invoked, the next step is to check the
 *   exit status of the remote checking process and if the "command_
 *   line" is executable, execute the QueuedRequest.  Otherwise,
 *   return.
 *
 * PARAMETERS:  This parameters for this callback are those defined by
 *   the type "SPC_TerminateHandlerType".  Most are not used.
 *
 *****************************************************************************/

static void 
CheckCommandTerminator(
        SPC_Channel_Ptr cmdChannel,
        int pid, 			/* NOT USED */
        int type, 			/* NOT USED */
        int cause,			/* Exit value of the remote process. */
        unsigned long requestNum)	/* Specifies the request number. */
{
   Boolean xhostError;
   char errorMessage[MAX_BUF_SIZE];
   Cmd_RequestQueue *prev  = NULL;
   Cmd_RequestQueue *pNode = requestQueue;

   DtSPCClose(cmdChannel);

   /*
    * Must find the node in the queue.
    */
   for (; pNode != NULL; pNode = pNode->next)
   {
       if ( pNode->request_num == requestNum )
       {
           /*
            * Pluck pNode out of the queue
            */
           if (prev)
               prev->next = pNode->next;
           else
               requestQueue = pNode->next;

           pNode->next = NULL;
           break;
       }
       /*
        * Move alone to the next node
        */
       prev = pNode;
   }

   if (pNode == NULL)
      return;

   /*
    * Check the exit status of the remote process.
    */
   if (cause == COMMAND_CHECK_FAILURE)
   {
      if (pNode->failure_proc != NULL)
      {
	 (void) sprintf (errorMessage, errorSpawn, pNode->exec_host, 
			 pNode->exec_string);
	 if (cmd_Resources.executionHostLogging)
	    _DtCmdLogErrorMessage (errorMessage);
	 (*pNode->failure_proc) (errorMessage, pNode->failure_data);
      }

      FreeRequest (pNode);

      return;
   }

    /*
     * Re-queue this node -- we will execute the command
     */
    pNode->next = requestQueue;
    requestQueue = pNode;

    ExecuteQueuedRequest (requestNum);
}

/******************************************************************************
 * 
 * DtCmdGetWindowType - given a window-type mask, determine its'
 *   internal window type number.
 *
 * It should not be possible for this to be called with a bogus mask, 
 *
 * PARAMETERS:
 *
 *   unsigned long mask;	- The request window type mask
 *
 * RETURNS: The window type if one if found, otherwise "-1".
 *
 *****************************************************************************/

static int 
DtCmdGetWindowType(
        unsigned long windowTypeMask) 
{
	int winTypeNum = -1;

	/*
	 * Determine the winType number recognized by dtexec
	 * The expected wintype input here is the value of the
	 * wintype bits in the action mask.  Convert this to the
	 * simple integers expected by dtexec.
	 */

	switch ( windowTypeMask )
	{
	case _DtAct_NO_STDIO_BIT:
		winTypeNum = 0;
		break;
	case _DtAct_TERMINAL_BIT:
		winTypeNum = TERMINAL;
		break;
	case _DtAct_PERM_TERM_BIT:
		winTypeNum = PERM_TERMINAL;
		break;
	default:
		myassert(0);	/* should never get here */
		winTypeNum = PERM_TERMINAL;
		break;
	} 

	
	return winTypeNum;
}

/******************************************************************************
 *
 * _DtActionCommandInvoke
 * ----------------------
 *	This is the primary entry point into the command invoker portion
 *	of the Dt Services library.  Command Actions are routed to this
 *	entry point.  This function is used to invoke both local and remote 
 *	commands.
 *
 *	If logging is turned on, the success or failure message is logged.
 ******************************************************************************/
int
_DtActionCommandInvoke(
	long wintype,
        char * cwdHost,
        char * cwdDir,
        char * execString,
        char * termOpts,
        char * execHost,
        char * procId,
        char * tmpFiles,
        void (*success_proc)(),
        void *success_data,
        void (*failure_proc)(),
        void *failure_data)
{
	int status;
	char errorMessage[MAX_BUF_SIZE * 2];

	status = _DtCmdCommandInvokerExecute (errorMessage, NULL, 
			       wintype, 
			       cwdHost, cwdDir, NULL,
			       termOpts, execHost, execString,
                               procId, tmpFiles,
			       success_proc, success_data,
			       failure_proc, failure_data);
	switch (status)
	{
	case _CMD_EXECUTE_SUCCESS :
		if (cmd_Resources.executionHostLogging && success_data != NULL)
		{
			CallbackData *data;
			data = (CallbackData *) success_data;
			(void) sprintf (errorMessage, successHost, 
				data->actionLabel, execHost);
			_DtCmdLogErrorMessage (errorMessage);
		}
		if (success_proc != NULL)
			(*success_proc) (NULL, success_data);
		break;
	case _CMD_EXECUTE_QUEUED :
		 /*
		  * Return for now and when the termination handler
		  * gets hit, the queued request will be executed.
		  */
		 break;
	default :
		 /*
		  * _CMD_EXECUTE_FAILURE or _CMD_EXECUTE_FATAL
		  */
		if (cmd_Resources.executionHostLogging)
			_DtCmdLogErrorMessage (errorMessage);
		if (failure_proc != NULL)
			(*failure_proc) (errorMessage, failure_data);
		break;
	}

        return (status == _CMD_EXECUTE_QUEUED) ? 1 : 0;
}

/******************************************************************************
 *
 * _DtCommandInvokerExecute -
 *
 *   This function allows a client to use the DT "Command Invoker"
 *   Library for its' process execution.  This function is intended
 *   for processes which do not use the Action Library.
 *
 *   For local execution, the "fork" and "execvp" system calls are
 *   used.  For remote execution, the "SPCD" is used.
 *
 * RETURNS: Nothing directly.  However, if the command is successfully 
 *   executed, the "success_proc" function is executed; otherwise, 
 *   the "failure_proc" function is executed.
 *
 * NOTE: This API is NOT public it is only here for use by the Session
 *       manager which uses to start up remote clients at session restore
 *	 time (see SmRestore.c).  
 *
 *****************************************************************************/

void
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
	void *failure_data)

{
   _DtSvcAppLock(cmd_Globals.app_context);

   _DtActionCommandInvoke(_DtAct_NO_STDIO_BIT,context_host,
		context_dir, exec_string, exec_parameters, exec_host,
                NULL, NULL,
		success_proc,success_data, failure_proc,failure_data);
   _DtSvcAppUnlock(cmd_Globals.app_context);
		
}

/******************************************************************************
 *
 * _DtInitializeCommandInvoker - initialize some global variables and
 *   and call the appropriate initialization routines.
 *
 * PARAMETERS:
 *
 *   Display *display;		- The X server connection.
 *
 *   char *toolClass;		- The client's tool class.
 *
 *   char *appClass;		- The client's application class.
 *
 *   DtSvcMessageProc reloadDBHandler; - The callback function for handling
 *					 a "RELOAD-TYPES-DB" request. (OBSOLETE)
 *
 *   XtAppContext appContext;	- The client's application context.
 *
 * MODIFIED:
 *
 *   SbInputId (*SbAddInput_hookfn);		- Set to _DtCmdSPCAddInputHandler
 *
 *   SbInputId (*SbRemoveInput_hookfn);		- Set to XtRemoveInput

 *   SbInputId (*SbAddException_hookfn);	- Set to _DtCmdSPCAddExceptionHandler
 *   SbInputId (*SbRemoveException_hookfn);	- Set to XtRemoveInput
 *
 *****************************************************************************/

void 
_DtInitializeCommandInvoker(
	Display *display,
	char *toolClass,	/* ignored */
	char *appClass,		/* ignored */
	DtSvcMessageProc reloadDBHandler,	/* OBSOLETE -- ignored */
	XtAppContext appContext)
{
   static	int beenCalled = 0;

   _DtSvcAppLock(appContext);

   /*
    * Prevent repeat calls
    */
   _DtSvcProcessLock();
   if ( beenCalled ) {
        _DtSvcProcessUnlock();
	return;
      }

   beenCalled++;

   cmd_Globals.app_context = appContext;

   SbAddInput_hookfn = _DtCmdSPCAddInputHandler;

   SbRemoveInput_hookfn = XtRemoveInput;

   SbAddException_hookfn = _DtCmdSPCAddExceptionHandler;

   SbRemoveException_hookfn = XtRemoveInput;

   _DtCmdBuildPathList ();

   _DtCmdInitializeErrorMessages ();

   /*
    * Must get the name of the invoking host, so that requests
    * can be checked for remote execution.
    */
   if ((DtGetShortHostname(_cmdClientHost, MAXHOSTNAMELEN)) == -1) {
      _DtCmdLogErrorMessage ("Cannot determine the local host name.\n");
   }

   _DtCmdGetResources (display);

   _DtSvcProcessUnlock();
   _DtSvcAppUnlock(appContext);
}

/*****************************************************************************
 *
 * _DtCmdInitializeErrorMessages - initializes all of the command invoker's
 *   error messages.
 *
 * PARAMETERS: None.
 *
 * MODIFIED:  all of the Command Invoker's error messages are initialized.
 *
 *****************************************************************************/

static void 
_DtCmdInitializeErrorMessages( void )
{

   /* 
    * Non-Fatal -> Abort the request
    */
   errorChdir = strdup (((char *)Dt11GETMESSAGE(3, 2, "An attempt to change to the following directory:\n\n  %s\n\nfrom host \"%s\" failed.\n\nCheck the spelling and permissions and make sure the directory exists.")));

   errorSpawn = strdup (((char *)Dt11GETMESSAGE(3, 5, "An attempt to execute the following command on host\n\"%s\" failed:\n\n  %s\n\nCheck that the program exists, has the correct\npermissions and is executable.")));

   errorExec = strdup (((char *)Dt11GETMESSAGE(3, 6, "An attempt to execute the following command failed:\n\n  %s\n\nCheck that the program exists, has the correct\npermissions and is executable.")));

   cmd_Globals.error_terminal = strdup (((char *)Dt11GETMESSAGE(3, 7, "This action cannot be started because the following\nterminal emulator cannot be executed:\n\n  %s\n\nCheck that the program exists, has the correct permissions\nand is executable.  This problem may have occurred because the\nprogram is not in your \"PATH\".")));

   errorLength = strdup (((char *)Dt11GETMESSAGE(3, 9, "The total number of characters in this action exceeds the limit of \"%d\".\n\nYou may need to break the action into more than one action.")));

   errorFork = strdup (((char *)Dt11GETMESSAGE(3, 11, "An attempt to start a new process on host \"%s\" failed.\n\nTo continue, you may need to stop an unneeded process on this host.")));

   errorRequest = strdup (((char *)Dt11GETMESSAGE(3, 17, "This action's WINDOW_TYPE \"%s\" is un-recognized.\n\nThe WINDOW_TYPE should be one of the following:\n\n  %s, %s, %s,\n   %s, %s, or %s\n")));

   cmd_Globals.error_subprocess = strdup (((char *)Dt11GETMESSAGE(3, 18, "This action cannot be started because the DT subprocess program:\n\n   %s\n\ncannot be executed.  Check that the program has the correct\npermissions and is executable.")));

   errorRemoteSubprocess = strdup (((char *)Dt11GETMESSAGE(3, 20, "This action cannot be executed on host \"%s\"\nbecause the following required program either\ndoes not exist or it is not executable:\n\n   %s\n")));

	/*
	 * The following errors may occur when a SPC
	 * channel is opened.
	 */
   errorUnknownHost = XtNewString (((char *)Dt11GETMESSAGE(3, 24, "This action cannot be executed because\nhost \"%s\" cannot be reached.")));

   errorBadConnect = XtNewString (((char *)Dt11GETMESSAGE(3, 25, "This action cannot be executed on host \"%s\" because the\n\"%s\" service is not properly configured on this host.")));

   errorBadService = XtNewString (((char *)Dt11GETMESSAGE(3, 26, "This action cannot be executed because the \"%s\"\nservice is not configured on host \"%s\".")));

   errorRegisterHandshake = XtNewString (((char *)Dt11GETMESSAGE(3, 27, "This action cannot be executed on host \"%s\" because user\n\"%s\" has a user id of \"%d\" on host \"%s\" and this does\nnot match the username and user id on the action\ninvocation host \"%s\".")));

   errorRegisterUsername = XtNewString (((char *)Dt11GETMESSAGE(3, 28, "This action cannot be executed on host \"%s\" because\nuser \"%s\" does not have an account on this host.")));

   errorRegisterNetrc = XtNewString (((char *)Dt11GETMESSAGE(3, 29, "This action cannot be executed on host \"%s\" because\na pathname to the authentication file cannot be created.")));

   errorRegisterOpen = XtNewString (((char *)Dt11GETMESSAGE(3, 30, "This action cannot be executed on host \"%s\" because\nthe authentication file on this host cannot be opened.\n\nThis may be caused by your network home not being\nproperly configured.")));

   errorEnvTooBig = XtNewString (((char *)Dt11GETMESSAGE(3, 31, "This action cannot be executed on host \"%s\" because\nthe environment exceeds \"%d\" bytes.")));

   errorInetSecurity = XtNewString (((char *)Dt11GETMESSAGE(3, 32, "This action cannot be executed on host \"%s\" because\nhost \"%s\" is not authorized to use the \"%s\" service.\n\nTo fix this, add host \"%s\" to the \"%s\" service\nentry in file \"%s\" on host \"%s\".")));

   /* 
    * Do not post a dialog, write to the error log file only.
    */

   errorSpcTerminator = strdup (((char *)Dt11GETMESSAGE(3, 15, "An attempt to register the output log from a remote host failed.\n\nTo continue, you may need to stop an existing process.")));

   successHost = strdup (((char *)Dt11GETMESSAGE(3, 21, "The action \"%s\" was successfully executed on host \"%s\".")));

   cmd_Globals.error_directory_name_map = strdup (((char *)Dt11GETMESSAGE(3, 22, "The directory \"%s\" on host \"%s\"\ncould not be converted to a network path.\n(%s)")));

}
