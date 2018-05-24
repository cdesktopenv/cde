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
/* $TOG: CmdProcess.c /main/8 1998/04/09 17:47:13 mgreess $ */
/***************************************************************************
*
* File:         CmdProcess.c
* Description:  Miscellaneous functions for the command execution system
* Language:     C
*
** (c) Copyright 1993, 1994 Hewlett-Packard Company
** (c) Copyright 1993, 1994 International Business Machines Corp.
** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
** (c) Copyright 1993, 1994 Novell, Inc.
***************************************************************************/

#include "CmdInvP.h"

#include <X11/Xproto.h>
#include <Dt/CommandM.h>
#include <Dt/DtNlUtils.h>
#include <Dt/ActionUtilP.h>
#include "myassertP.h"

#if defined(sun)
#include <limits.h>
#endif 

#if defined(__hpux)
#include <sys/getaccess.h>
#endif

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <Tt/tt_c.h>

/*
 * Global variables:
 */
Cmd_Resources cmd_Resources = { 0 };
Cmd_Globals cmd_Globals = { 0 };

/********    Static Function Declarations    ********/

static char *_GetRemoteTerminalsResource (
        		Display *display,
			char *appClass,
			char *appName);
static void GetRemoteTerminalsResource (
        		Display *display,
			char *appClass,
			char *appName);
static void GetLocalTerminalResource (
        		Display *display,
			char *appClass,
			char *appName);
static void GetWaitTimeResource (
        		Display *display,
			char *appClass,
			char *appName);
static void GetDtexecPath (
        		Display *display,
			char *appClass,
			char *appName);
static void GetExecutionHostLoggingResource (
        		Display *display,
			char *appClass,
			char *appName);
static char *GetRemoteTerminal(
			char *host);

/********    End Static Function Declarations    ********/

/******************************************************************************
 *
 * _GetRemoteTerminalsResource - gets the value of the "remoteTerminals"
 *    resource.
 *
 * PARAMETERS:
 *
 *   Display *display;       The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * RETURNS: a pointer to the "remoteTerminals" resource or NULL if the
 *   resource is not defined.
 *
 * NOTES:  The space for the returned string is "malloc'ed".  The calling
 *   function should free the string.
 *
 *****************************************************************************/

static char 
*_GetRemoteTerminalsResource (
	Display		*display,
	char		*appClass,
	char		*appName)
{
   XrmValue resource_value;
   XrmDatabase db;
   char *rep_type;
   char *name;
   char *class;
   char *terminals = NULL;

   name = XtMalloc (strlen (DtREMOTE_TERMINALS_NAME) + strlen (appName) + 4);
   sprintf (name, "%s*%s", appName, DtREMOTE_TERMINALS_NAME);

   class = XtMalloc (strlen (DtREMOTE_TERMINALS_CLASS) + strlen (appClass) + 4);
   sprintf (class, "%s*%s", appClass, DtREMOTE_TERMINALS_CLASS);

   db = XtDatabase (display);
   if (XrmGetResource (db, name, class, &rep_type, &resource_value)) 
      terminals = (char *) XtNewString (resource_value.addr);

   XtFree (name);
   XtFree (class);

   return (terminals);
}

/******************************************************************************
 *
 * GetRemoteTerminalsResource - initializes the "remoteTerminals" resource.
 *
 * PARAMETERS:
 *
 *   Display *display;       The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * MODIFIED:
 *
 *   char *cmd_Resources.remoteTerminals; -  Set to the list of remote 
 *					     terminals.
 *
 *   char *cmd_Resources.remoteHosts;     -  Set to the list of remote hosts..
 *
 *****************************************************************************/

static void 
GetRemoteTerminalsResource (
	Display		*display,
	char		*appClass,
	char		*appName)
{
   int num, j;
   char *terminals;
   char *pch, *host;

   if ((terminals = _GetRemoteTerminalsResource (display, appClass, appName)) 
	== NULL) {
      /*
       * The resource was not specified, and no defaults are set.
       */
      cmd_Resources.remoteHosts = (char **) NULL;
      cmd_Resources.remoteTerminals = (char **) NULL;
      return;
   }

   /*
    * Find out how many terminals are specified by counting the number of
    * colons.
    */
   for (pch=terminals, num=0; pch != NULL ; ) {
      if ((pch = DtStrchr (pch, ':')) != NULL) {
	 /*
	  * Move past the ':'.
	  */
	 pch++;
	 num++;
      }
   }

   if (num == 0) {
      /*
       * No "host:/terminal" pairs were found.
       */
      cmd_Resources.remoteHosts = (char **) NULL;
      cmd_Resources.remoteTerminals = (char **) NULL;
      return;
   }

   /*
    * Create space for the pointers to the hosts and terminals and 
    * make room for a NULL terminator.
    *
    * Note that the host names and terminal names are "vectorized" in
    * place by replacing the ':' and ',' characters with '\0'.
    */

   cmd_Resources.remoteHosts = (char **) XtMalloc (num * sizeof(char *));
   cmd_Resources.remoteTerminals = (char **) XtMalloc (num * sizeof(char *));
   
   for (pch=terminals, j=0; j < num && pch != NULL && *pch != '\0'; j++) 
   {

      host = pch;

      if ((pch = DtStrchr (pch, ':')) == NULL)
	 break;

      /*
       * Null terminate the host and then move past the ':'.
       */
      *pch = '\0';
      pch++;

      if (*pch == '\0')
	 /* 
	  * Found a host but nothing followed it. 
	  */
	 break;

      cmd_Resources.remoteHosts[j] = host;
      cmd_Resources.remoteTerminals[j] = pch;

      /*
       * Find the end of this entry and skip past it.
       */
      if ((pch = DtStrchr (pch, ',')) == NULL) {
	 /*
	  * This is the last pair, advance "j" and break the loop.
	  */
	 j++;
	 break;
      }

      /*
       * End the terminal name.
       */
      *pch = '\0';
      pch++;
   }
   
   /*
    * NULL terminate the arrays.
    */
   cmd_Resources.remoteHosts[j] = (char *) NULL;
   cmd_Resources.remoteTerminals[j] = (char *) NULL;
}


/******************************************************************************
 *
 * GetLocalTerminalResource - initializes the "localTerminal resource.
 *
 * PARAMETERS:
 *
 *   Display *display;       The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * MODIFIED:
 *
 *   char *cmd_Resources.localTerminal; -  Set to the "localTerminal"
 *					   or to DtDEFUALT_TERMINAL.
 *
 *   Boolean cmd_Globals.terminal_ok;   -  Set to True if "localTerminal"
 *					   is executable and has the correct
 *					   permissions; False otherwise.
 *
 *****************************************************************************/

static void 
GetLocalTerminalResource (
   	Display		*display,
	char		*appClass,
	char		*appName)
{
   XrmValue resource_value;
   char *rep_type;
   char *name;
   char *class;
   XrmDatabase db = XtDatabase (display);

   name = XtMalloc (strlen (DtLOCAL_TERMINAL_NAME) + strlen (appName) + 4);
   sprintf (name, "%s*%s", appName, DtLOCAL_TERMINAL_NAME);

   class = XtMalloc (strlen (DtLOCAL_TERMINAL_CLASS) + strlen (appClass) + 4);
   sprintf (class, "%s*%s", appClass, DtLOCAL_TERMINAL_CLASS);

   if (XrmGetResource (db, name, class, &rep_type, &resource_value)) 
      cmd_Resources.localTerminal = (char *) XtNewString (resource_value.addr);
   else 
      cmd_Resources.localTerminal = XtNewString (DtTERMINAL_DEFAULT);

   /*
    * Check to see if the execvp will potentially fail because the 
    * localTerminal is not accessable or it it is not executable.  If not, 
    * log an error message.
    */
   cmd_Globals.terminal_ok = True;
   if (!(_DtCmdCheckForExecutable (cmd_Resources.localTerminal))) {
      char *errorMessage = XtMalloc(MAX_BUF_SIZE);

      cmd_Globals.terminal_ok = False;
      if (errorMessage)
      {
          (void) sprintf (errorMessage, 
	                  cmd_Globals.error_terminal, 
                          cmd_Resources.localTerminal);
          _DtCmdLogErrorMessage (errorMessage);
	  XtFree(errorMessage);
      }
   }

   XtFree (name);
   XtFree (class);
}

/******************************************************************************
 *
 * GetWaitTimeResource - initializes the "waitTime" resource.
 *
 * PARAMETERS:
 *
 *   Display *display;		The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * MODIFIED:
 *
 *   char *cmd_Resources.waitTime; -  Set to the "waitTime" resource
 *					  or "True" by default.
 *
 *****************************************************************************/

static void 
GetWaitTimeResource (
   	Display		*display,
	char		*appClass,
	char		*appName)
{
   XrmValue resource_value;
   char *rep_type;
   char *name;
   char *class;
   XrmDatabase db= XtDatabase (display);

   cmd_Resources.waitTime = DtWAIT_TIME_DEFAULT;

   name = XtMalloc (strlen (DtWAIT_TIME_NAME) + strlen (appName) + 4);
   sprintf (name, "%s*%s", appName, DtWAIT_TIME_NAME);

   class = XtMalloc (strlen (DtWAIT_TIME_CLASS) + strlen (appClass) + 4);
   sprintf (class, "%s*%s", appClass, DtWAIT_TIME_CLASS);

   if (XrmGetResource (db, name, class, &rep_type, &resource_value)) {
      cmd_Resources.waitTime = atoi (resource_value.addr);
      if (cmd_Resources.waitTime < 0)
	 cmd_Resources.waitTime = DtWAIT_TIME_DEFAULT;
   }

   XtFree (name);
   XtFree (class);
}

/******************************************************************************
 *
 * GetDtexecPath - initializes the "dtexecPath" resource.
 *
 * PARAMETERS:
 *
 *   Display *display;       The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * MODIFIED:
 *
 *   char *cmd_Resources.dtexecPath; -  Set to the "dtexecPath" resource
 *					 or to DtCMD_INV_SUB_PROCESS.
 *
 *****************************************************************************/

static void 
GetDtexecPath (
   	Display		*display,
	char		*appClass,
	char		*appName)
{
   XrmValue resource_value;
   char *rep_type;
   char errorMessage [MAX_BUF_SIZE];
   char *name;
   char *class;
   XrmDatabase db = XtDatabase (display);

   name = XtMalloc (strlen (DtDTEXEC_PATH_NAME) + strlen (appName) + 4);
   sprintf (name, "%s*%s", appName, DtDTEXEC_PATH_NAME);

   class = XtMalloc (strlen (DtDTEXEC_PATH_CLASS) + strlen (appClass) + 4);
   sprintf (class, "%s*%s", appClass, DtDTEXEC_PATH_CLASS);

   if (XrmGetResource (db, name, class, &rep_type, &resource_value)) 
      cmd_Resources.dtexecPath = (char *) XtNewString (resource_value.addr);
   else 
      cmd_Resources.dtexecPath = XtNewString (DtCMD_INV_SUB_PROCESS);

   /*
    * Check to see if the execvp will potentially fail because the 
    * subprocess is not accessable or it it is not executable.  If not, 
    * log an error message.
    */
   cmd_Globals.subprocess_ok = True;
   if (!(_DtCmdCheckForExecutable (cmd_Resources.dtexecPath))) {
      char *errorMessage = XtMalloc(MAX_BUF_SIZE);

      cmd_Globals.subprocess_ok = False;
      if (errorMessage)
      {
          (void) sprintf (errorMessage, 
                          cmd_Globals.error_subprocess, 
                          cmd_Resources.dtexecPath);
          _DtCmdLogErrorMessage (errorMessage);
	  XtFree(errorMessage);
      }
   }

   XtFree (name);
   XtFree (class);
}

/******************************************************************************
 *
 * GetExecutionHostLoggingResources - initializes the "executionHostLogging"
 *   
 *
 * PARAMETERS:
 *
 *   Display *display;       	The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *   char *appName;		The client's application name.
 *
 * MODIFIED:
 *
 *   char *cmd_Resources.executionHostLogging; 
 *
 *         -  Set to the "executionHostLogging" resource or "False" by default.
 *
 *****************************************************************************/

static void 
GetExecutionHostLoggingResource (
   	Display		*display,
	char		*appClass,
	char		*appName)
{
   XrmValue resource_value;
   char *rep_type;
   char *name;
   char *class;
   XrmDatabase db= XtDatabase (display);

   cmd_Resources.executionHostLogging = False;

   name = XtMalloc (strlen (DtEXECUTION_HOST_LOGGING_NAME) + 
		    strlen (appName) + 4);
   sprintf (name, "%s*%s", appName, DtEXECUTION_HOST_LOGGING_NAME);

   class = XtMalloc (strlen (DtEXECUTION_HOST_LOGGING_CLASS) + 
		     strlen (appClass) + 4);
   sprintf (class, "%s*%s", appClass, DtEXECUTION_HOST_LOGGING_CLASS);

   if (XrmGetResource (db, name, class, &rep_type, &resource_value)) {
      if (((strcmp (resource_value.addr, "True")) == 0) ||
	  ((strcmp (resource_value.addr, "true")) == 0) ||
          ((strcmp (resource_value.addr, "TRUE")) == 0))
	 cmd_Resources.executionHostLogging = True;
      else if (((strcmp (resource_value.addr, "False")) == 0) ||
	  ((strcmp (resource_value.addr, "false")) == 0) ||
          ((strcmp (resource_value.addr, "FALSE")) == 0))
	 cmd_Resources.executionHostLogging = False;
   }

   XtFree (name);
   XtFree (class);
}

/******************************************************************************
 *
 * GetRemoteTerminal - takes a host name "host" and searches the list of remote
 *   host:terminal pairs for "host" and if found, returns the corresponding
 *   terminal emulator.
 *
 * PARAMETERS:
 *
 *   char *host;		- Name of the host to use as the key to find
 *				  the terminal emulator name.
 *
 * RETURNS:  If "host" is found, the corresponding terminal emulator is
 *   returned, otherwise "NULL" is returned.
 *
 *****************************************************************************/

static char * 
GetRemoteTerminal(
        char *host )
{
   int i;

   if (cmd_Resources.remoteHosts == NULL)
      return (NULL);

   for (i=0; cmd_Resources.remoteHosts[i] != NULL; i++) {
      if ((strcmp (host, cmd_Resources.remoteHosts[i])) == 0)
	 return (cmd_Resources.remoteTerminals[i]);
   }

   return (NULL);
}

/******************************************************************************
 *
 * _DtCmdCreateTerminalCommand - takes a windowType, execString 
 * and execParms and creates an appropriate command to execute.
 *
 * Note that if valid execParms are found, they are simply added
 * to the execution string, before the "-e" option.
 *
 * PARAMETERS:
 *
 *   char **theCommand;		- The command line to be expanded and returned.
 *   int windowType;		- The request number (window number).
 *   char *execString;		- The execution string to be expanded.
 *   char *execParms;		- The execution parameters.
 *   char *procId		- tooltalk procId for dtexec
 *   char *tmpFiles             - string containing tmp file args for dtexec
 *
 * MODIFIED:
 *
 *    char ** theCommand; 	- Contains the completed execution string.
 *
 *****************************************************************************/

void 
_DtCmdCreateTerminalCommand(
        char **theCommand,
        int windowType,
        char *execString,
        char *execParms,
	char *execHost,
        char *procId,
        char *tmpFiles)
{
   char *pGeom = NULL;
   char *pName = NULL;
   char *pTitle = NULL;
   char options[MAX_BUF_SIZE];
   char *defaultTerminal;

   strcpy(options,execParms);
   
   /*
    * Get the name of the terminal emulator to use.
    */
   if ((defaultTerminal = GetRemoteTerminal (execHost)) == NULL)
      defaultTerminal = cmd_Resources.localTerminal;

   /*
    * Create space for the expanded command line.
    */
   *theCommand = (char *) XtMalloc ( strlen (defaultTerminal) 
         + strlen(options) 
	 + strlen(cmd_Resources.dtexecPath)
         + strlen(" -open ") + 4 /* max waitTime len */
         + strlen(" -ttprocid ") + strlen(_DtActNULL_GUARD(procId))
         + strlen(_DtActNULL_GUARD(tmpFiles))
         + strlen(execString) + 9 /* 2 quotes, "-e", 7 blanks, null */ );

   if (windowType == TERMINAL) {
      (void) sprintf (*theCommand, "%s %s -e %s -open %d -ttprocid '%s' %s %s",
	              defaultTerminal, options, cmd_Resources.dtexecPath, 
                      cmd_Resources.waitTime, 
                      _DtActNULL_GUARD(procId),
                      _DtActNULL_GUARD(tmpFiles),
                      execString);
   }
   else {
      (void) sprintf (*theCommand, "%s %s -e %s -open -1 -ttprocid '%s' %s %s",
	              defaultTerminal, options, cmd_Resources.dtexecPath, 
                      _DtActNULL_GUARD(procId),
                      _DtActNULL_GUARD(tmpFiles),
		      execString);
   }

}

/******************************************************************************
 * 
 * _DtCmdCheckForExecutable - checks "fileName" to see if it is executable
 *   by the user.
 *
 * PARAMETERES:
 *
 *   char *fileName;	- The name of the file to check.
 *
 * RETURNS: "True" if the file is access'able && executable, "False" otherwise.
 *
 *****************************************************************************/

Boolean 
_DtCmdCheckForExecutable(
        char *fileName )
{
   char **ppch;
   int access;


   /* First check to see if the fileName contains the complete path
    * and is access'able.
    */

   switch (access = _DtExecuteAccess(fileName)) 
   {
        /* execution access allowed for this process */
       case 1: return True;
           break;
	/* file found -- not executable by this process */
       case 0: return False;
           break;
        /* file not found -- yet */
       case -1: /* fall through */
       default:
           break;
   }

   /* Append fileName to each entry in "cmd_Globals.path_list" and then see
    * if that is accessable. 
    */

   for (ppch = cmd_Globals.path_list; *ppch != NULL; ppch++) 
   {
      char *longName = XtMalloc(MAXPATHLEN);

      if (longName)
      {
          (void) sprintf (longName, "%s/%s", *ppch, fileName);
          switch (access = _DtExecuteAccess(longName)) 
          {
               /* execution access allowed for this process */
              case 1: return True;
                  break;
   	      /* file found -- not executable by this process */
              case 0: return False;
                  break;
              /* file not found -- yet */
              case -1: /* fall through */
              default:
                  break;
          }
	  XtFree(longName);
      }
   }

   return (False);
}

/******************************************************************************
 *
 * _DtCmdValidDir - takes the client host, context directory and a context host
 *   and checks to see if the directory is valid.
 *
 * PARAMETERS:
 *
 *   char *clientHost;	- Host where the client is running.
 *
 *   char *contextDir;		- Directory to "chdir" to.
 *
 *   char *contextHost;		- Host where 'contextDir' resides.
 *
 * NOTES:
 *
 *   If the context directory is NULL, "chdir" is not run and "True" is
 *   returned.
 *
 * RETURNS:  "True" if "chdir" succeeds and "False" otherwise.
 *
 *****************************************************************************/

Boolean 
_DtCmdValidDir(
        char *clientHost,
        char *contextDir,
	char *contextHost )
{
   int 		retValue = 0;

   if (contextDir == NULL) 
      /*
       * Stay in the same directory.
       */
      return (True);
   if (contextHost == NULL)
      /*
       * Change to 'contextDir'.
       */
      retValue = chdir (contextDir);
   else if (_DtIsSameHost (clientHost, contextHost))
      /*
       * Just change to 'contextDir'.
       */
      retValue = chdir (contextDir);
   else {
      /*
       * Need to create a pathname for the directory.
       */
      char	*netfile;
      char	*path;
      char	*errorMessage = XtMalloc(MAX_BUF_SIZE);

      /* Old syntax should no longer appear in contextHost/Dir */
      myassert( *contextHost != '*' && *contextDir != '*' );
      netfile = (char *) tt_host_file_netfile (contextHost, contextDir);
      if (tt_pointer_error (netfile) != TT_OK) {
	 (void) sprintf (errorMessage, cmd_Globals.error_directory_name_map, 
			 contextDir, contextHost,
			 tt_status_message (tt_pointer_error(netfile)));
	 _DtCmdLogErrorMessage (errorMessage);
	 return (False);
      }

      path = (char *) tt_netfile_file (netfile);
      tt_free (netfile);
      if (tt_pointer_error (path) != TT_OK) {
	 (void) sprintf (errorMessage, cmd_Globals.error_directory_name_map, 
			 contextDir, contextHost,
			 tt_status_message (tt_pointer_error(path)));
	 _DtCmdLogErrorMessage (errorMessage);
	 return (False);
      }

      retValue = chdir (path);
      tt_free (path);
      if (errorMessage) XtFree(errorMessage);
   }

   if (retValue == -1)
      return (False);
   else
      return (True);
}

/******************************************************************************
 *
 * _DtCmdGetResources - initializes the Command Invoker's resources.
 *
 * PARAMETERS:
 *
 *   Display *display;		The connection to the X server.
 *
 *   char *appClass;		The client's application class.
 *
 *****************************************************************************/

void
_DtCmdGetResources (
	Display		*display)
{
   char *appClass = NULL;
   char *appName = NULL;

   /*
    * Determine the name and class for this client.
    */
   XtGetApplicationNameAndClass (display, &appName, &appClass);

   GetLocalTerminalResource (display, appClass, appName);

   GetRemoteTerminalsResource (display, appClass, appName);

   GetWaitTimeResource (display, appClass, appName);

   GetDtexecPath (display, appClass, appName);

   GetExecutionHostLoggingResource (display, appClass, appName);
}
