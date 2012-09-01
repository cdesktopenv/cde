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
/*
 * File:         spc-exec.c $TOG: spc-exec.c /main/9 1998/10/26 17:22:38 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <limits.h>
#include <stdlib.h>
#include <locale.h>

#include <SPC/spcP.h>
#include <bms/MemoryMgr.h> 

#include <SPC/spc-proto.h>

#ifdef SVR4
#include <unistd.h>
#endif

#include <Tt/tt_c.h>
#include "DtSvcLock.h"

/* Global vars. */

SPC_Connection_Ptr write_terminator=NULL, read_terminator=NULL;

/*
 * Forward declarations
 */
static char *get_path_from_context ( 
	char            *context );
static int remove_variable(
  	char		*string );
static void resolve_variable_reference(
  	char		**string );

/*
 * This array contains the process id's of the sub-processes
 * started by the daemon.  When a sub-process terminates, its
 * entry will be set to SPCD_DEAD_PROCESS.  This list of pid's
 * is kept beause when the exit timer expires, if the daemon
 * has no sub-processes running, it will exit.
 */
pid_t *SPC_pid_list = NULL;

/*
 * This global variable is set by the daemon when the client
 * connects.
 */
int SPC_client_version_number = SPC_PROTOCOL_VERSION;

/*
 * If this variable is not NULL, it will contain the name of 
 * the mount point environment variable plus its value thus it
 * ready for 'putenv'.
 */
char *SPC_mount_point_env_var = NULL;

/* External definitions */

extern XeChar spc_logging;
extern XeString *environ;

/*
 * Routines for handling Sub-Processes
 */

/*
 **
 ** Initialize synchronous terminators.
 **
*/

/*----------------------------------------------------------------------+*/
SPC_Setup_Synchronous_Terminator(void)
/*----------------------------------------------------------------------+*/
{
  int pipes[2];

  _DtSvcProcessLock();
  if(write_terminator) {
    _DtSvcProcessUnlock();
    return(TRUE);
  }
  
  if(pipe(pipes)<0) {
    SPC_Error(SPC_No_Pipe);
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }

  if((write_terminator=SPC_Alloc_Connection())==SPC_ERROR) {
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
  SPC_Add_Connection(write_terminator);
  
  if((read_terminator=SPC_Alloc_Connection())==SPC_ERROR) {
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
  SPC_Add_Connection(read_terminator);

  write_terminator->sid=pipes[WRITE_SIDE];
  write_terminator->connected=TRUE;

  read_terminator->sid=pipes[READ_SIDE];
  read_terminator->connected=TRUE;
  SPC_XtAddInput(NULL, &read_terminator->termination_id, read_terminator->sid,
		 SPC_Conditional_Packet_Handler, SPC_Terminator);
  
  _DtSvcProcessUnlock();
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
SPC_Connection_Ptr SPC_Channel_Terminator_Connection(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
  if(IS_REMOTE(channel))
    return(channel->connection);
  else
    return(read_terminator);
}
  
/*----------------------------------------------------------------------+*/
void SPC_Close_Unused(void)
/*----------------------------------------------------------------------+*/
{
  /* Close any and all unused file descriptors */
  int fd;
 
  for (fd = STDERR + 1; fd < max_fds; fd++) spc_close(fd);
}

/*----------------------------------------------------------------------+*/
SPC_MakeSystemCommand(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{

  XeString shell;
  XeString *argv;
  XeString *tmp_argv;
  XeChar newargtwo[_POSIX_ARG_MAX];
  int argtwolen=0, tmplen=0;
  
  /* Allocate our memory up front */

  argv=Alloc_Argv(4);

  newargtwo[argtwolen]=0;

  /* copy path into newargtwo */
  
  strncat(newargtwo, channel->path, _POSIX_ARG_MAX-1);
  strcat(newargtwo, (XeString)" ");
  argtwolen=strlen(newargtwo);
  
  /* copy argv into newargtwo */
  for(tmp_argv=channel->argv; tmp_argv && *tmp_argv; tmp_argv++) {
    tmplen=strlen(*tmp_argv)+1;              /* Room for extra space */
    if((tmplen+argtwolen)<_POSIX_ARG_MAX-1) {
      strcat(newargtwo, *tmp_argv);
      strcat(newargtwo, (XeString)" ");
      argtwolen += tmplen;
    } else {
      XeChar *errbuf;

      errbuf = malloc(sizeof(XeChar) * 100);
      if (errbuf)
      {
        SPC_Free_Envp(argv);
        sprintf(errbuf,"(%d chars), max. length is %d",tmplen,_POSIX_ARG_MAX);
        SPC_Error(SPC_Arg_Too_Long, tmp_argv, _POSIX_ARG_MAX);
	free(errbuf);
      }
      return(SPC_ERROR);
    }
  }
       
  /* get a shell --
     First use the value of $SB_SHELL (if any), 
     then try $SHELL,
     then use DEFAULT_SHELL
     */

  if(!(shell=getenv((XeString)"SB_SHELL")))
    if(!(shell=getenv((XeString)"SHELL")))
      shell = DEFAULT_SHELL;
  
  /* setup argv properly */
  
  argv[0]=SPC_copy_string(shell);
  argv[1]=SPC_copy_string((XeString)"-c");
  argv[2]=SPC_copy_string(newargtwo);
  argv[3]=NULL;
  channel->argv = argv;
  channel->IOMode |= SPCIO_DEALLOC_ARGV;
  
  /* Now set this shell as the path */

  channel->path = shell;
  
  return(TRUE);
}

/*
 * Routines for handling child process termination
 */

/*----------------------------------------------------------------------+*/
/* This is the right way according to the Spec 1170 */
void SPC_Child_Terminated(int i)
/*----------------------------------------------------------------------+*/
{
  /* This catches signals for sub-process termination */
  int type, cause, status;
  pid_t wait_pid, pid;
  SPC_Channel_Ptr channel;
  protocol_request     req, *prot;
  buffered_data        data, *pdata;
  int length;
  int indx;
  int saved_errno = errno;

  prot  = (&req);
  pdata = (&data);

  prot->dataptr=pdata;

  wait_pid = -1;
  while(pid = waitpid(wait_pid, &status, WNOHANG)) {
    if((pid == -1 && errno == ECHILD) || pid == 0) {
      /* no more children.  Return */
      errno = saved_errno;
      return;
    }
    
    /* Okay, we got the process ID of a terminated child.  Find the
       channel associated with this PID. */
    channel=SPC_Find_PID(pid);
#ifdef DEBUG
    fprintf(stderr, (XeString)"got SIGCHLD, pid: %d, channel: %p\n", pid, channel);
#endif

    if(!channel) {
      continue;
    }

    _DtSvcProcessLock();
    /*
     * Look for this process in the pid list.  If found, mark it
     * as done.
     */
    if (SPC_pid_list != NULL) {
      for (indx=0; SPC_pid_list[indx] != 0; indx++)
        if (SPC_pid_list[indx] == pid) {
	   SPC_pid_list[indx] = SPCD_DEAD_PROCESS;
	   break;
	}
    }
    _DtSvcProcessUnlock();

    /* We have the channel.  Mark it as being closed. */

    channel->status     = status;

    /* If we this channel is set up for synchronous termination,
       write the protocol request to record that this guy died.
       Otherwise, call the termination handler directly. */
    
    if(IS_SPCIO_SYNC_TERM(channel->IOMode)) {

      /* This code is basically what SPC_Write_Protocol_Request does.
	 It is replicated here because a call to SPC_W_P_R would have
	 to be re-enterant if we called it here, and SPC_W_P_R is not
	 re-enterant at this time. */
      
      SPC_Reset_Protocol_Ptr(prot, channel, APPLICATION_DIED, 0);
      pdata->len=WRITE_APPLICATION_DIED(pdata, status);
      length=WRITE_HEADER(pdata, channel->cid,
			  prot->request_type,
			  pdata->len,
			  0);
      pdata->data[length]=(XeChar)' ';
      length=pdata->len+REQUEST_HEADER_LENGTH;
      if(write(write_terminator->sid, pdata->data, length)==ERROR)
	SPC_Error(SPC_Internal_Error);
      pdata->offset=REQUEST_HEADER_LENGTH;
      print_protocol_request((XeString) (XeString)"  <-- INTERNAL APPLICATION_DIED", prot);
    }
    else {
      SPC_Change_State(channel, 0, -1, 0);
      if(channel->Terminate_Handler) {
	XeSPCGetProcessStatus(channel, &type, &cause);
	(* channel->Terminate_Handler)
	  (channel, channel->pid, type, cause, channel->Terminate_Data);
      }
    }
    /* Loop around & get another PID */
  }
  errno = saved_errno;
}

/*
 ***
 *** Check to see if a given path names an executable file.  Thus, we
 *** need to know if the file exists.  If it is a directory, we want
 *** to fail.  Otherwise, we want to us the system rules for checking
 *** on the file, and thus the 'access' call.
 ***
*/

static Boolean executable_predicate(XeString path, XeString dir, XeString file)
{

  struct stat file_status;
  
  dir=dir;
  file=file;

  if(stat(path, &file_status) != 0)
    return(FALSE);
  
  if(S_ISDIR(file_status.st_mode))
    return(FALSE);

  return(access(path, X_OK | F_OK) == 0);
}

/*----------------------------------------------------------------------+*/
int exec_proc_local_channel_object(SPC_Channel_Ptr channel)
/*----------------------------------------------------------------------+*/
{
    sigset_t newsigmask, oldsigmask;
    pid_t pid;
    int result;
    XeString *envp;
    XeString dir = XeString_NULL;
    int retval;
    int i, reuse_pid = 0;
    
    call_parent_method(channel, exec_proc, (channel), result);
    
    if(result==SPC_ERROR)
	return(SPC_ERROR);

    /* Check to see if the channel pathname points to a valid executable.
       We do this by using the _path_search function.  If the channel
       has a PATH variable set in its local environment, use it,
       otherwise use the "global" environment.  We can accomplish this
       by using the spc_getenv call in the _path_search call.  If the
       channel doesn't have a PATH variable, then spc_getenv will
       return NULL, which indicates use of the global environment.
   */

    if(!_path_search(SPC_Getenv("PATH", channel->envp),
		    channel->path,
		    executable_predicate)) {
      SPC_Error(SPC_Cannot_Exec, channel->path);
      return(SPC_ERROR);
    }
       
    /* If we were passed a host:dir to cd to, make sure it exists. */
    /* We want to do this before we fork the child.                */

    if((channel->context_dir) && (channel->context_dir[0])) {
	struct stat	stat_info;
	Boolean ok = FALSE;
	
	_DtSvcProcessLock();
	if (SPC_client_version_number < SPC_PROTOCOL_VERSION_CDE_BASE)
	   dir = get_path_from_context(channel->context_dir);
	else {
	   /*
	    * context_dir is actually a "netfile" so it needs to
	    * be converted to a "real" path.
	    */
	   dir = (char *) tt_netfile_file (channel->context_dir);
	   if (tt_ptr_error (dir) != TT_OK) 
	      dir = NULL;
	}
	_DtSvcProcessUnlock();

	if (dir == NULL)
	    /* can't make connection ... */;
	else if (stat(dir,&stat_info) != 0)	
	    /* directory not there */;

	else if ((stat_info.st_mode & S_IFDIR) == 0)
	    /* path is not a directory ... */;
	else
	    ok = TRUE;
	
	if (!ok && IS_SPCIO_FORCE_CONTEXT(channel->IOMode)) {
	    if (dir != NULL && (strcmp (dir, channel->context_dir) != 0))
	       SPC_Error(SPC_cannot_Chdir, dir);
	    SPC_Error(SPC_cannot_Chdir, channel->context_dir);
	    XeFree(dir);
	    dir = XeString_NULL;
	    return(SPC_ERROR);
	}
    }

    if(mempf0(channel, pre_fork)==SPC_ERROR)
	return(SPC_ERROR);
    
    /* When using HP NLIO (xj0input) we have a problem.  Namely,  */
    /* the xj0 processs uses signal() to deal with SIGCLD which   */	
    /* is incompatible with sigaction/sigprogmask/etc.  Even      */
    /* though xj0 resets the signal handler, since the signal     */
    /* routines are incompatible, our original handler gets lost. */
    /* Hence, we need to reset it.  We do it here everytime we    */
    /* fork a child just to be on the safe side.                  */

    SPC_ResetTerminator();
    
    sigemptyset(&newsigmask);
    sigemptyset(&oldsigmask);
    sigaddset(&newsigmask, SIGCHLD);
    
    if (sigprocmask(SIG_BLOCK, &newsigmask, &oldsigmask) == ERROR)
	return(SPC_ERROR);
    
    pid = channel->pid = fork();
    
    /*
     * Must save this pid so that when the daemon's timer goes off,
     * if there has been no activity and there are no sub-processes
     * running, the daemon can exit.
     */
    i = 0;
    _DtSvcProcessLock();
    if (SPC_pid_list == NULL)
      /*
       * Create the first block plus the NULL terminator.
       */
      SPC_pid_list = (pid_t *) malloc (2 * sizeof (pid_t));
    else {
      /*
       * If a dead pid entry exists, reuse it; otherwise, must create 
       * room for the new pid.
       */
      for (i = 0; SPC_pid_list[i] != 0; i++)
        if (SPC_pid_list[i] == SPCD_DEAD_PROCESS) {
	  SPC_pid_list[i] = pid;
	  reuse_pid = 1;
	  break;
	}
      if (!reuse_pid)
	SPC_pid_list = (pid_t *) realloc (SPC_pid_list, 
					   (i+2) * sizeof (pid_t));
    }
    if (!reuse_pid) {
      SPC_pid_list[i] = pid;
      SPC_pid_list[i+1] = 0;
    }
    _DtSvcProcessUnlock();

    if (pid) {
	XeFree(dir);
	
	/* Did we really fork? */
	if (pid == ERROR) {
	  SPC_Error(SPC_Cannot_Fork);
	  retval = SPC_ERROR;
	} else {
	  /* Do any set up for the parent process here */
	  mempf1(channel, post_fork, pid);
	  retval = TRUE;
	}
	
	/* Reinstate the old signal mask (unblock SIGCLD). */
	
	sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
	return(retval);
    }
    else {
	/* Child process: connect wires, make environment and exec sub-process */
	
	sigprocmask(SIG_SETMASK, &oldsigmask, (sigset_t *)NULL);
	
	/* Make sure the child is the process group leader.  In the case of
	   ptys, we also want to break the current terminal affiliation.
	   We want to be the process group leader so XeSPCKillProcess (which
	   does a kill(-pid, 9)) will kill all processes associated with us.
	   
	   For PTY's, we need to break the terminal affiliation so the next
	   open (which will be a pty) will cause us to become affiliated with
	   the pty.  We do this so when the parent process closes the master
	   side of the pty, the slave side processes get SIGHUP.  If they
	   ignore SIGHUP, they will never die.  So it goes...
	   */

	if(IS_SPCIO_PTY(channel->IOMode))
	    setsid();
	else {
	    pid_t tmppid = getpid();
	    if(setpgid(tmppid, tmppid) == -1) 
		fprintf(stderr, (XeString)"setpgid failed, errno: %d\n", errno);
	}
	
	/* Connect wires to sub-process standard files */
	result=mempf1(channel, post_fork, pid);
	
	if(result!=SPC_ERROR) {
	    int		indx = -1;
	    int		i;
	    char	**ppch;

	    /*
	     * Before adding in the list of environment variables 
	     * from the environment variable files, must search 
	     * the list for LANG definitions.  If found, the
	     * last definition must be putenv'ed to assure the 
	     * multi-byte parsing code is using the correct locale.
	     */
	    for (i = 0, ppch = channel->envp; *ppch; *ppch++, i++)
	       if (!strncmp (*ppch, "LANG=", 5))
		  indx = i;

	    if (indx != -1)
	       resolve_variable_reference (&channel->envp[indx]);

	    _DtSvcProcessLock();
	    if (!setlocale (LC_CTYPE, ""))
	       /*
		* setlocale failed - log a message but execute 
		* the command anyway.
		*/ 
	       if (SPC_Print_Protocol != NULL)
		  (void) fprintf(SPC_Print_Protocol,
		      "+++> Failed to 'setlocale'; LANG = %s\n",
		      getenv("LANG"));
	    /* Mix in the stashed environment */
	    
	    for(envp=channel->envp; *envp; envp++)
		resolve_variable_reference(&*envp);

	    if (SPC_mount_point_env_var != NULL)
		/*
		 * The mount point environment variable was 
		 * inherited by the daemon or was given to the
		 * daemon via the command line.  In either case
		 * this subprocess must inherit the daemon's
		 * value.
		 */
		(void) putenv (SPC_mount_point_env_var);
	    _DtSvcProcessUnlock();

	    /* Connect to the context directory */
	    /* We have already validated this guy exists */

	    if(dir) 
		Xechdir(dir);
	}
	
	XeFree(dir);

	if(result!=SPC_ERROR) {
	    /* Execute */
	    /* Compiler barfs without cast ? */
#if defined(__hpux_8_0) || defined(__aix)
            result=execvp(channel->path, channel->argv);
#else
	    result=execvp(channel->path, channel->argv);
#endif
	    /* If we return from exec, it failed */
	    SPC_Error(SPC_Cannot_Exec, channel->path);
	}
	
	/* We want to get rid of this child image (carefully) */
	_exit(42);
	
    }
}


/****************************************************************************
 *
 * get_path_from_context - given a 'context' string in the following form:
 * 
 *     [host:]path
 *
 * return the path component.
 *
 * NOTE - the caller must free the returned string.
 * 
 * Parameters:
 *
 *    char	*context 	- the context string to parse
 *
 * Return Value:
 *
 *    A NULL if a pathname cannot be constructed.
 *
 ****************************************************************************/

static char *get_path_from_context (
	char 		*context)
{
   char			*host = NULL;
   char			*file = NULL;
   char			*netfile = NULL;
   char			*path = NULL;
   char			tmp[MAXPATHLEN];
   char			*pch;

   /*
    * Break context into its host and file parts.
    */
   if (context == NULL)
      return (NULL);

   (void) strcpy (tmp, context);
   file = tmp;

   if ((pch = (char *) strchr (tmp, ':')) != NULL) {
      host = tmp;
      file = pch + 1;
      *pch = '\000';
   }

   if (!host)
      return (strdup (file));

   netfile = (char *) tt_host_file_netfile (host, file);
   if (tt_ptr_error (netfile) != TT_OK) {
      SPC_Error (SPC_Cannot_Create_Netfilename, context, host);
      return (NULL);
   }

   path = (char *) tt_netfile_file (netfile);
   tt_free (netfile);
   if (tt_ptr_error (path) != TT_OK) {
      SPC_Error (SPC_Cannot_Create_Netfilename, context, host);
      return (NULL);
   }

   return (path);
}


/**************************************************************************
 *
 * remove_variable ()
 *
 * This takes a string of the format:
 *
 *    var_name=some_value | <remove_variable_keyword> var_name
 *  
 * and if the second form is found, 'var_name' is removed from
 * the environment.
 *
 * Parameters:
 *
 *    char	*string 	see the format above.
 *
 * Return Value: 
 *
 *    int	0 if 'string' contains a 'remove variable' command
 *		1 if string does not contain a 'remove variable' command
 *
 * Modified:
 *
 *    char	**environ	'var_name' is removed from the environment
 *
 **************************************************************************/

static int
remove_variable(
  	char		*string)
{
   char 	*pch;
   char 	**ppch;
   char 	**ppch2;
   char 	*tmp_var;
   int  	tmp_len;

   /*
    * If string contains some white space before the variable
    * or keyword, skip the white space.
    */
   pch = string;
   while (
#ifdef NLS16
          ((mblen (pch, MB_CUR_MAX) == 1)) &&
#endif	/* NLS16 */
          isspace ((u_char)*pch))
             pch++;

   if (*pch == '\000') 
      return (1);
   if (strncmp (pch, SPC_REMOVE_VAR, strlen (SPC_REMOVE_VAR)))
      return (1);

   /*
    * Skip the white space after the keyword and move to the
    * beginning of the variable.
    */
   pch = pch + strlen (SPC_REMOVE_VAR);
   while (
#ifdef NLS16
          ((mblen (pch, MB_CUR_MAX) == 1)) &&
#endif	/* NLS16 */
          isspace ((u_char)*pch))
             pch++;

   if (*pch == '\000') 
      return (1);

   /*
    * pch should now point to the variable to be removed.
    *
    * tmp_var will be equal to the variable with a trailing '='.
    * This is added so the future comparison will not match
    * against variables that start with 'tmp_var' but then
    * have something else.
    */
   tmp_var = malloc ((strlen (pch) + 2) * sizeof (char));
   (void) sprintf (tmp_var, "%s=", pch);
   tmp_len = strlen (tmp_var);

   /*
    * Scan 'environ' for 'tmp_var'
    */
   for (ppch = environ; *ppch; *ppch++) {
      if (!strncmp (tmp_var, *ppch, tmp_len)) {
	 /*
	  * Found the variable so remove it by moving all
	  * variables after *ppch up.
	  */
	 for (ppch2 = ppch; *ppch2; *ppch2++) {
	    *ppch++;
	    *ppch2 = *ppch;
	 }
	 break;
      }
   }

   free ((char *) tmp_var);
   return (0);
}


/**************************************************************************
 *
 * resolve_variable_reference ()
 *
 * This function takes a string of the format:
 *
 *    var_name=some_value
 *  
 * and if 'some_value' contains a reference to an environment
 * variable, the reference is replaced with the value of the
 * variable.
 *
 *    For example, if 'string' is
 *
 *            FOO=$VFA_TOP/foo
 *
 *    and $VFA_TOP is '/some_tree', then 'string' will be changed to:
 *
 *            FOO=/some_tree/foo
 *
 * Notes:
 *
 *    o 'putenv' will be invoked with argument 'string', even if 'string' 
 *      was not modified.
 *
 *    o A valid variable name consists of alphanumerics and underscore
 *
 * Parameters:
 *
 *    char	**string 	MODIFIED - the environment variable 
 *				definition to be parsed
 *
 * Return Value: 
 *
 *    void
 *
 **************************************************************************/

static void
resolve_variable_reference(
  	char		**string )		/* MODIFIED */
{
   char		*string_end;
   char		*var_start;
   char		*pch;
   char		*value;
   int		n;			/* number of bytes in a string */
   int		len;
   int		found_brace;
   char		variable[256];
   char		*buf = NULL;
   char		*pbeg = *string;
   char		*string_start = *string;

   if (!remove_variable (*string))
      return;

   pch = pbeg;
   
   buf = malloc(2048);

   while (*pbeg) {

      /*
       * Look for '$' - the beginning of a variable reference.
       * If a '$' character is not found, exit the loop
       */
#ifdef NLS16
      while (*pch) {

	 if (((len = mblen (pch, MB_CUR_MAX)) == 1) && (*pch == '$'))
	    break;

	 /*
	  * Move past this char
	  */
	 if (len > 0)
	    pch += len;
	 else
	    /*
	     * pch is null or it points to an invalid mulit-byte
	     * character.
	     */
	    break;
      }
#else
      pch = strchr (pch, '$');
#endif /* NLS16 */

      if (pch == NULL || *pch == '\000')
	 /*
	  * The string doesn't contain any (more) variables
	  */
	 break;

      string_start = *string;

      /*
       * Found a '$' - the beginning of an environment variable so
       * skip it and check the next char for '{' and if it is found
       * skip it and move to the real beginning of an env variable.
       */
      string_end = pch;
      found_brace = 0;
      pch++;

#ifdef NLS16
      if ((mblen (pch, MB_CUR_MAX) == 1) && (*pch == '{')) {
#else
      if (*pch == '{') {
#endif /* NLS16 */
	 pch++;				/* input = ${ */
         found_brace = 1;
      }
      if (*pch == '\0')			/* input = $\0 or ${\0 */
	 break;

      /*
       * Find the end of the variable name - it is assumed to 
       * be the first character that is not an alpha-numeric
       * or '_'.
       */
      var_start = pch;
      n = 0;

      while (*pch) {
#ifdef NLS16
	 if ((mblen (pch, MB_CUR_MAX) > 1) || 
	     ((mblen (pch, MB_CUR_MAX) == 1) && 
	     ((*pch == '_') || (isalnum (*pch))))) {
#else
	 if (isalnum (*pch) || *pch == '_') {
#endif /* NLS16 */
	 
#ifdef NLS16
            len = mblen (pch, MB_CUR_MAX);
#else
            len = 1;
#endif /* NLS16 */
	    n += len;
	    pch += len;
	    continue;
	 }

#ifdef NLS16
         if (found_brace && (mblen (pch, MB_CUR_MAX) == 1) && (*pch == '}'))
#else
	 if (found_brace && *pch == '}')
#endif /* NLS16 */
	    /*
	     * Move past the closing brace
	     */
            pch++;
	 break;
      }

      if (n == 0) {
	 /*
	  * Nothing 'recognizable' follows the $ or ${
	  */
	 pbeg = pch;
	 continue;
      }

      /*
       * Stuff the environment variable name in 'variable' and then
       * get its value.  If the variable doesn't exist, leave its
       * name in the string.
       */
      (void) strncpy (variable, var_start, n);
      variable[n] = '\0';

      if ((value = getenv (variable)) == NULL) {
	 /*
	  * Leave what looks like an environment variable in place.
	  */
	 pbeg = pch;
	 continue;
      }

      if (strlen (value) == 0) {
	 pbeg = pch;
	 continue;
      }

      /*
       * Need to replace the variable definition with the string
       * pointed to by 'value'.  So create a string that contains the 
       * characters before the environment variable, then the contents 
       * of 'value' and finally, the characters after the environment 
       * variable.
       */
      if (string_end == string_start)
	 /*
	  * There is nothing to prepend before 'value'.
	  */
	 buf[0] = '\0';
      else {
	 (void) strncpy (buf, string_start, (string_end - string_start));
	 buf[(string_end - string_start)] = '\0';
      }
      (void) strcat (buf, value);
      len = strlen (buf);
      if (*pch != '\0')
	 (void) strcat (buf, pch);
       
      /*
       * Now put 'buf' into 'string'.
       */
      *string = realloc (*string, strlen (buf) + 1);
      (void) strcpy (*string, buf);
      pch = *string + len;
      pbeg = pch;
   }

   /*
    * Even if no substitutions were made, the variable must
    * be put in the environment.
    */
   (void) putenv (*string);
   if (buf) free(buf);
}
