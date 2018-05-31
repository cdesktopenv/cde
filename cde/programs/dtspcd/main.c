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
/*
 * File:         main.c $TOG: main.c /main/8 1999/09/30 15:31:41 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <bms/sbport.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>			/* ctime() */
#include <pwd.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>			/* initgroups() */
#include <string.h>
#include <libgen.h>			/* basename() */
#include <sys/param.h>			/* MAXPATHLEN */

#include <bms/bms.h>
#include <bms/MemoryMgr.h>
#include <bms/XeUserMsg.h>
#include <bms/usersig.h>
#include <bms/spc.h>

#include <SPC/spcP.h>		
#include <SPC/spc-proto.h>
#include <XlationSvc.h>
#include <LocaleXlate.h>

#include <Tt/tt_c.h>

#include "spcd_event.h"

static char *MOUNT_POINT = "DTMOUNTPOINT";

/* Forward declarations */
/** ------------------ **/
static void Merge_Lang_Var(SPC_Channel_Ptr channel);

void SPCD_Handle_Client_Data(void *channel, int *source, SbInputId *id );
int SPCD_Initialize(void);
void SPCD_Exit(int exitval);
void SPCD_Handle_Application_Data(void *chn, XeString text, int size, int conn_type );
void SPCD_Termination_Handler(SPC_Channel_Ptr channel, int pid, int type, int cause, void *data );
int Client_Abort(protocol_request_ptr prot);
int Client_Register(protocol_request_ptr prot);
int Client_Unregister(protocol_request_ptr prot);
int Client_Channel_Open(protocol_request_ptr prot);
int Client_Channel_Close(protocol_request_ptr prot);
int Client_Channel_Reset(protocol_request_ptr prot);
int Client_Channel_Attach(protocol_request_ptr prot);
int Client_Application_Spawn(protocol_request_ptr prot);
int Client_Application_Signal(protocol_request_ptr prot);
int Client_Application_Data(protocol_request_ptr prot);
int Client_Server_Debug(protocol_request_ptr prot);
int Client_Environ_Reset(protocol_request_ptr prot);
int Client_Reply_Devices(protocol_request_ptr prot);
int Client_Reply_Logfile(protocol_request_ptr prot);
int Client_Delete_Logfile(protocol_request_ptr prot);
int Client_Reset_Termio(protocol_request_ptr prot);
int Client_Reset_Termios(protocol_request_ptr prot);
int Client_Protocol_Version(protocol_request_ptr prot);
int Client_Reply_Protocol(protocol_request_ptr prot);
int Client_Reply_Hostinfo(protocol_request_ptr prot);

/* New B.00 methods */

int Client_Send_EOF(protocol_request_ptr prot);
int Client_Channel_Termios(protocol_request_ptr prot);
int Client_Enhanced_Spawn(protocol_request_ptr prot);

#if defined(_AIX) || defined(__linux__)
# define SA_HANDLER_INT_ARG
#endif /* _AIX */

void conditional_putenv(XeString env_str);
void SPCD_Reply(SPC_Connection_Ptr connection, protocol_request_ptr prot, int retval, int errval );
#ifdef SA_HANDLER_INT_ARG
void SPCD_Alarm_Handler (int);
#else
void SPCD_Alarm_Handler (void);
#endif /* SA_HANDLER_INT_ARG */

/* Global data */

protocol_request_handler spcd_protocol_dispatch_table[NREQS]={
  Client_Application_Data,   /* APPLICATION_DATA */
  NULL,                      /* APPLICATION_STDOUT */
  NULL,                      /* APPLICATION_STDERR */
  Client_Abort,              /* ABORT */
  Client_Register,           /* REGISTER */
  Client_Unregister,         /* UNREGISTER */
  Client_Channel_Open,       /* CHANNEL_OPEN */
  Client_Channel_Close,      /* CHANNEL_CLOSE */
  Client_Channel_Reset,      /* CHANNEL_RESET */
  Client_Channel_Attach,     /* CHANNEL_ATTACH */
  Client_Application_Spawn,  /* APPLICATION_SPAWN */
  Client_Application_Signal, /* APPLICATION_SIGNAL */
  NULL,                      /* APPLICATION_DIED */
  NULL,                      /* SERVER_ERROR */
  NULL,                      /* REPLY */
  Client_Server_Debug,       /* SERVER_DEBUG */
  Client_Environ_Reset,      /* ENVIRON_RESET */
  Client_Reply_Devices,      /* QUERY_DEVICES */
  NULL,                      /* DEVICE_REPLY */
  Client_Reply_Logfile,      /* QUERY_LOGFILE */
  NULL,                      /* LOGFILE_REPLY */
  Client_Delete_Logfile,     /* DELETE_LOGFILE */
  Client_Reset_Termio,       /* RESET_TERMIO (obsolete) */
  Client_Reset_Termios,      /* RESET_TERMIOS */
  Client_Send_EOF,           /* CHANNEL_SEND_EOF */
  Client_Channel_Termios,    /* CHANNEL_TERMIOS */
  Client_Enhanced_Spawn,     /* APP_B00_SPAWN */
};

/*
 * HomeDir will be set to "HOME=pwent->pw_dir".
 *
 * ShellDir will be set to "SHELL=pwent->pw_shell".
 */
XeChar HomeDir[MAXPATHLEN + 6];
XeChar ShellDir[MAXPATHLEN + 7];
XeString *default_environment;

int client_validated=0;
int SPCD_Abort_Okay = FALSE;

SPC_Connection_Ptr client_connection;

SPC_Channel client_channel;

/*
 * This variable contains the number of minutes in the daemon's
 * exit timer.  If the daemon has no activity within this period 
 * of time and there are no sub-processes running, the daemon
 * will exit.
 */
static int exit_timeout = SPCD_DEFAULT_TIMEOUT;

/*
 * The following variable is used by the timer code to indicate
 * if a request is currently being serviced.  This is necessary
 * because of the following scenario:
 *
 *   t0 - a request is made
 *   t1 - the request is serviced
 *   t2 - the timer expires
 *   t3 - the timer handler sees no sub-processes running so it exits
 *   t4 - request is done, reply to client
 *
 * This variable is set to SPCD_REQUEST_PENDING at t0 and to
 * SPCD_NO_REQUST_PENDING at t4.  If at t3 the variable is set
 * to SPCD_REQUEST_PENDING, the alarm will be reset and the
 * daemon will continue.
 */
static int request_pending = SPCD_NO_REQUEST_PENDING;

/*----------------------------------------------------------------------+*/
int main(int argc, XeString *argv)
/*----------------------------------------------------------------------+*/
{
  /* Parse the command line and set globals accordingly. */
  XeString log_path = NULL;
  Boolean terminate_flag = FALSE;
  int i;
  struct sigaction alarm_vector;
  char tmp[200];
  char *pch;
  
  /*
   * The SPC library needs to know this is a SPC 'daemon' process
   * to ensure 'SPC_Initialize()' installs a SIGCLD handler.
   */
  SPC_who_am_i = SPC_I_AM_A_DAEMON;
  SPCD_Authentication_Dir = NULL;

  /* set up log file path */
  log_path = XeSBTempPath((XeString)"DTSPCD.log");

  if(NULL == freopen("/dev/null", "w", stderr)) {
    printf("Unable to open /dev/null\n");
    exit(EXIT_FAILURE);
  }
  
  /* Process arguments and set flags.  */
  for (i=1; i < argc; i++) {

    if (!strcmp ("-log", argv[i])) {
      /* Log mode.  Print information to a log file */
      /* Open an error log with whatever name the library wants to use */
      SPC_Open_Log(log_path, FALSE);
    }

    else if (!strcmp ("-debug", argv[i])) {
      /* Debug mode.  Print protocol information to a log file */
      /* Open an error log with whatever name the library wants to use */
      SPC_Open_Log(log_path, FALSE);
      SPC_Print_Protocol = spc_logF;
      if(NULL == freopen(log_path, "a", stderr)) {
        printf("Unable to reopen '%s' as stderr\n", log_path);
        exit(EXIT_FAILURE);
      }
      setbuf(stderr, NULL);
    }

    else if (!strcmp ("-auth_dir", argv[i])) {
      /* 
       * Used to override the default directory for authentication file 
       */
      i++;
      if (i != argc) {
	SPC_Format_Log((XeString)"Authentication directory set to '%s'.", 
		       argv[i]);
        SPCD_Authentication_Dir = strdup (argv[i]);
      }
    }

    else if (!strcmp ("-mount_point", argv[i])) {
      /* 
       * Mount point for the filename mapping system.
       */
      i++;
      if (i != argc) {
	(void) snprintf (tmp, sizeof(tmp), "%s=%s", MOUNT_POINT, argv[i]);
	if (putenv (tmp) == 0) {
	  SPC_Format_Log((XeString)"Mount point set to '%s'.", argv[i]);
	  SPC_mount_point_env_var = (char *) malloc (strlen (tmp) + 1);
	  (void) strcpy (SPC_mount_point_env_var, tmp);
	}
	else
	  SPC_Format_Log((XeString)"Failed to add the mount point '%s' to the environment.", tmp);
      }
    }

    else if (!strcmp ("-timeout", argv[i])) {
      /* 
       * The timeout is specified, so use it instead of the default.
       */
      i++;
      if (i != argc) {
	exit_timeout = atoi(argv[i]);
	SPC_Format_Log((XeString)"Setting the exit timer to '%s' minutes.", 
		       argv[i]);
      }
    }

    else {
      /* 
       * Unknown command option 
       */
      SPC_Format_Log((XeString)"Command line option '%s' unrecognized.", 
		     argv[i]);
    }
  }

  /* free strings allocated for path */
  XeFree(log_path);

  /*
   * Initialize the i/o function pointers.
   */
  SbAddInput_hookfn        = SPCD_AddInput;
  SbAddException_hookfn    = SPCD_AddException;
  SbRemoveInput_hookfn     = SPCD_RemoveInput;
  SbRemoveException_hookfn = SPCD_RemoveException;
  SbMainLoopUntil_hookfn   = SPCD_MainLoopUntil;
  SbBreakMainLoop_hookfn   = SPCD_BreakMainLoop;

  /* Initialization Commands */
  if (SPCD_Initialize()==SPC_ERROR)
    SPCD_Exit(1);

  client_connection=SPC_Start_Daemon(FALSE);
  if(client_connection==SPC_ERROR)
    SPCD_Exit(1);

  client_channel.connection = client_connection;
  
  SPC_XtAddInput(&client_channel,
		 &client_connection->termination_id,
		 client_connection->sid,
		 SPCD_Handle_Client_Data,
		 SPC_Client);
  
  if (exit_timeout != SPCD_NO_TIMER) {
    memset(&alarm_vector, 0, sizeof(struct sigaction));
    alarm_vector.sa_handler = SPCD_Alarm_Handler;
    alarm_vector.sa_flags = 0;
    (void) sigaction (SIGALRM, &alarm_vector, (struct sigaction *)NULL);
    (void) alarm (exit_timeout * 60);
  }

  /*
   * The daemon's mount point environment variable needs to be 
   * saved.  It will be used to override the client's mount point
   * setting or the mount point specified in any of the environment 
   * files.
   */
  if (SPC_mount_point_env_var == NULL)
    if ((pch = getenv (MOUNT_POINT)) != NULL) {
      SPC_mount_point_env_var = (char *) malloc (strlen (pch) + 
						 strlen (MOUNT_POINT) + 2);
      (void) sprintf (SPC_mount_point_env_var, "%s=%s", MOUNT_POINT, pch);
    }

  XeCall_SbMainLoopUntil(&terminate_flag);
  SPCD_Exit(0);
}

/*----------------------------------------------------------------------+*/
void SPCD_Handle_Client_Data(void      *channel,
			     int       *UNUSED_PARM(source),
			     SbInputId *UNUSED_PARM(id))
/*----------------------------------------------------------------------+*/
{
  protocol_request_ptr prot;
  int retval;
  SPC_Connection_Ptr connection=((SPC_Channel_Ptr)channel)->connection;

  request_pending = SPCD_REQUEST_PENDING;

  prot=SPC_Read_Protocol(connection);
  if(!prot)
    SPCD_Exit(0);

  /* Check for valid client (in other words, that we have registered
     this client). */

  if((!client_validated) && (prot->request_type != REGISTER)) {
    SPC_Error(SPC_Client_Not_Valid);
    if (exit_timeout != SPCD_NO_TIMER)
      (void) alarm (exit_timeout * 60);
    request_pending = SPCD_REQUEST_PENDING;
    SPC_Free_Protocol_Ptr(prot);
    return;
  }

  SPCD_Abort_Okay = TRUE;
  retval=SPC_Dispatch_Protocol(prot, spcd_protocol_dispatch_table);
  SPCD_Abort_Okay = FALSE;
  if(REPLY_EXPECTED(prot->request_type, TRUE) != NO_REPLY_VAL)
    SPCD_Reply(connection, prot, retval, errno);
  SPC_Free_Protocol_Ptr(prot);

  /* 
   * Reset the alarm and go back to select.
   */
  if (exit_timeout != SPCD_NO_TIMER)
    (void) alarm (exit_timeout * 60);

  request_pending = SPCD_NO_REQUEST_PENDING;
}

/*----------------------------------------------------------------------+*/
int SPCD_Initialize(void)
/*----------------------------------------------------------------------+*/
{
  XeString sys_env_file = NULL;
  
  /* Do initialization for SPC */
  if(SPC_Initialize()==SPC_ERROR)
    return(SPC_ERROR);

  /* Do Daemon specific initialization */

  /* Error handling */

  XeProgName=SPCD_PROG_NAME;

  default_environment=
    (XeString *) malloc(DEFAULT_ENVP_SIZE * sizeof(XeString));
  default_environment[0]=NULL;

  /*
   * First add the installed environment file.
   */
  sys_env_file = (XeString) malloc (strlen(SPCD_ENV_INSTALL_DIRECTORY) +
				    strlen(SPCD_ENV_FILE) + 3);
  (void) sprintf (sys_env_file, "%s/%s", 
		  SPCD_ENV_INSTALL_DIRECTORY, 
		  SPCD_ENV_FILE);
  default_environment=SPC_Add_Env_File(sys_env_file,default_environment);

  /*
   * Now add the configured environment file.
   */
  sys_env_file = (XeString) realloc (sys_env_file,
				     strlen(SPCD_ENV_CONFIG_DIRECTORY) +
				     strlen(SPCD_ENV_FILE) + 3);
  (void) sprintf (sys_env_file, "%s/%s", 
		  SPCD_ENV_CONFIG_DIRECTORY, 
		  SPCD_ENV_FILE);

  default_environment=SPC_Add_Env_File(sys_env_file,default_environment);

  free (sys_env_file);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
void SPCD_Exit(int exitval)
/*----------------------------------------------------------------------+*/
{
  int i;

  if (SPC_logfile_list != NULL)
    for (i = 0; SPC_logfile_list[i] != NULL; i++)
      (void) unlink (SPC_logfile_list[i]);

  SPC_Format_Log((XeString)"Exiting server.  Retval: %d", exitval);
  SPC_Close_Log();

  exit(exitval);
}

/*----------------------------------------------------------------------+*/
void SPCD_Handle_Application_Data(void * chn,
                                  XeString text,
                                  int size,
                                  int conn_type )
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel= (SPC_Channel_Ptr) chn;
  SPC_Write_Protocol_Request(client_connection, channel,
			     CONNECTOR_TO_PROT(conn_type), text, size);
}


/*----------------------------------------------------------------------+*/
void SPCD_Termination_Handler(SPC_Channel_Ptr channel,
                              int UNUSED_PARM(pid),
                              int UNUSED_PARM(type),
                              int UNUSED_PARM(cause),
			      void * UNUSED_PARM(data))
/*----------------------------------------------------------------------+*/
{
  /* Write a termination protocol request */

  SPC_Write_Protocol_Request(client_connection, channel,
			     APPLICATION_DIED, channel->status);

  /* That's all, folks! */
}

/*
 ***
 *** Protocol request handlers
 ***
*/

/*----------------------------------------------------------------------+*/
int Client_Abort(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  return(print_protocol_request((XeString)"--> ABORT", prot));
}

#define FREE_USER_PASS(a, b) free(a); free(b);

/*----------------------------------------------------------------------+*/
int Client_Register(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  XeString username;
  XeString passwd;
  XeString proto_ver;
  XeString hostinfo;
  struct passwd *pwent;
  XeString tmpfile, tmppath;
  XeChar  buffer[MAXPATHLEN * 2];
  struct stat buf;
  XeString netfile;
  int free_netfile = 0;
  char *spc_prefix = "/.SPC_";
  char *spc_suffix;
  char tmpnam_buf[L_tmpnam + 1];
  size_t buffsize;

  print_protocol_request((XeString)"--> REGISTER", prot);
  prot->channel=0;

  READ_REGISTER(prot->dataptr, username, passwd, proto_ver, hostinfo);
  
  if(strcmp(username, "") != 0) {
    SPC_Format_Log ( "+++> Starting authentication for user '%s'\n     from host '%s'.", 
	    username, hostinfo);
    /*  
     * We have a username so generate a temp filename and send it 
     * back to the client after creating the proper path for the file.
     */
    if(SPCD_Authentication_Dir)
      /*
       * Use the directory specified on the command line.
       */
      tmppath = strdup(SPCD_Authentication_Dir);
    else {
      /*
       * Use the $HOME directory if it can be retrieved from the
       * password file.
       */
      if(!(pwent=getpwnam(username))) {
	SPC_Format_Log ("+++> FAILURE: username '%s' is unknown.", username);
	SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			           FAILED_FILE_NAME, NULL, NULL);
	FREE_USER_PASS(username, passwd);
	SPC_Error(SPC_Bad_Username);
	free(hostinfo);
	return(SPC_ERROR);
      }
      else {
	tmppath = (XeString)(pwent->pw_dir);
	   
        if ((lstat (tmppath, &buf) != 0) || 
	    (!S_ISDIR(buf.st_mode))     || 
	    (!(buf.st_mode & S_IRUSR)))
	  /*
	   * Use the default directory.
	   */
	  tmppath = XeSBTempPath(XeString_NULL);
      }
    }

    /*
     * tempnam(3) has side effects caused by permissions of the directory
     * given or the TMPDIR envirnoment variable.  Because of these side
     * effects, the function may return "/tmp/.SPC_xxxxxx" and ignore
     * tmppath.  The protocol will fail when this occurs.  The fix is
     * to construct the tmpfile name.
     */
    tmpnam(tmpnam_buf);
    spc_suffix = basename(tmpnam_buf); /* Don't free result - not alloc'd! */

    /* Allocate space for tmppath, spc_prefix, and spc_suffix. */
    buffsize = strlen(tmppath) + strlen(spc_prefix) + strlen(spc_suffix) + 1;
    tmpfile = (char *)malloc(buffsize);
    if(tmpfile) {
        snprintf(tmpfile, buffsize, "%s%s%s", tmppath, spc_prefix, spc_suffix);
    }
  }
  else {
#if 0
    /*
     * No username was supplied (this could happen with pre-CDE
     * clients) so generate a temp filename.
     */
    if(SPCD_Authentication_Dir)
      tmppath = strdup(SPCD_Authentication_Dir);
    else
      tmppath = XeSBTempPath(XeString_NULL);
    tmpfile = tempnam(tmppath, ".SPC_" );
    XeFree(tmppath);
#endif

    SPC_Format_Log ("+++> FAILURE: NULL username.");
    SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			       FAILED_FILE_NAME, NULL, NULL);
    FREE_USER_PASS(username, passwd);
    SPC_Error(SPC_Bad_Username);
    free(hostinfo);
    return(SPC_ERROR);

  }

  if(!tmpfile) {
    SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			       FAILED_FILE_NAME, NULL, NULL);
    SPC_Format_Log("+++> FAILURE: cannot malloc.");
    SPC_Error(SPC_Out_Of_Memory);
    free(hostinfo);
    return(SPC_ERROR);
  }

  /* 
   * Store info about the client (protocol version & host type) 
   * For Pre A.01, this will be defaulted to (hpux 7.0 s300) 
   */
  client_connection->hostinfo = hostinfo;

  if (proto_ver) {
      (void) sscanf(proto_ver, "%d", &client_connection->protocol_version);
      SPC_client_version_number = client_connection->protocol_version;
      XeFree(proto_ver);
  }
  else
      /* The client didn't send a protocol_version so set it to "1". */
      SPC_client_version_number = 1;

  SPC_Format_Log("     Client protocol version is '%d'.", 
		 SPC_client_version_number);
  SPC_Format_Log("+++> Authentication file is '%s'.", tmpfile);

  /*
   * For non-CDE clients, the clients expect the daemon to send
   * a "real" file name.  Beginning with 'SPC_PROTOCOL_VERSION_CDE_BASE'
   * the clients are expecting a "netfile" name.
   */
  netfile = tmpfile;

  if (SPC_client_version_number >= SPC_PROTOCOL_VERSION_CDE_BASE) {
     netfile = tt_file_netfile (tmpfile);

     if (tt_ptr_error (netfile) != TT_OK) {
       SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
				  FAILED_FILE_NAME, NULL, NULL);
       SPC_Format_Log("+++> FAILURE: cannot create a cannonical file name for the authentication file.\n     (%s)", 
		      tt_status_message(tt_pointer_error(netfile)));
       XeFree(tmpfile);
       SPC_Error(SPC_Bad_Authentication);
       return(SPC_ERROR);
     }
     free_netfile = 1;
     SPC_Format_Log("     Authentication 'netfile' is '%s'.", netfile);
  }

  /* As we send the next packet, include protocol and host info about */
  /* us (the server).  If going to a pre A.02 client, it will be      */
  /* ignored by it.                                                   */

  SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY, 
			     netfile, SPC_PROTOCOL_VERSION_STR, 
		             SPC_LocalHostinfo());

  prot=SPC_Filter_Connection(client_connection, NULL, REGISTER, TRUE);
  if(prot==SPC_ERROR) {
    XeFree(tmpfile);
    return(SPC_ERROR);
  }
  sprintf(buffer, (XeString)"--> REGISTER (%s)", netfile);
  print_protocol_request(buffer, prot);
  SPC_Free_Protocol_Ptr(prot);

  /* Was the client able to create the authentication */
  /* file in the temp directory?		        */

  if(lstat(tmpfile, &buf)==ERROR) {
    int terrno = errno;
    SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			       FAILED_FILE_NAME, NULL, NULL);
    SPC_Format_Log("+++> FAILURE: lstat authentication file '%s'.", tmpfile);
    SPC_Format_Log("+++> FAILURE: lstat() returned error '%s'\n", 
                   strerror(terrno));
    if (free_netfile)
      tt_free(netfile);
    XeFree(tmpfile);
    SPC_Error(SPC_Bad_Authentication);
    return(SPC_ERROR);
  }

  if (S_ISLNK(buf.st_mode))
    {				/* somebody is jerkin us around */
      SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
                                 FAILED_FILE_NAME, NULL, NULL);
      SPC_Format_Log("+++> FAILURE: lstat authentication file '%s' is a symlink! Possible compromise attempt.", tmpfile);
      if (free_netfile)
        tt_free(netfile);
      XeFree(tmpfile);
      SPC_Error(SPC_Bad_Authentication);
      return(SPC_ERROR);
    }

  /*
   * If the file does not have the setuid bit set then return failure.
   *
   * Note that if the protocol_version is < 2, this bit will
   * not be set and the client will not be able to connect.
   */
  if(!(buf.st_mode & S_ISUID)) {
    SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			       FAILED_FILE_NAME, NULL, NULL);
    SPC_Format_Log("+++> FAILURE: authentication file '%s' does not have the setuid bit set.",
		   tmpfile);
    if (free_netfile)
      tt_free(netfile);
    XeFree(tmpfile);
    SPC_Error(SPC_Bad_Permission);
    return(SPC_ERROR);
  }
    
  unlink(tmpfile);
  XeFree(tmpfile);
    
  /* 
   * Is uid associated with the authentication file created by the 
   * client present in our password file?    
   */
  if(!(pwent=getpwuid(buf.st_uid))) {
    SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			       FAILED_FILE_NAME, NULL, NULL);
    SPC_Format_Log("+++> FAILURE: the authentication file created by the client has a uid '%d'\n     and this uid is not in the password file.",
		     buf.st_uid);
    SPC_Error(SPC_Bad_Username);
    if (free_netfile)
      tt_free(netfile);
    return(SPC_ERROR);
  }

  /*
   * Comapre the user name in the request with the user name in 
   * the passwd file.  They must be the same to continue.
   */
  if (strcmp (pwent->pw_name, username) != 0) {
      SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
				 FAILED_FILE_NAME, NULL, NULL);
      SPC_Format_Log("+++> FAILURE: the request is for username '%s' with uid '%d'\n     but this uid has name '%s' in the password file.",
		     username,
		     buf.st_uid,
		     pwent->pw_name);
      SPC_Error(SPC_Bad_Password);
      if (free_netfile)
	tt_free(netfile);
      return(SPC_ERROR);
  }

  client_validated=TRUE;

  /*
   * Need to initialize the group access list if a username
   * was supplied in the request.
   */
  if ((initgroups(username, pwent->pw_gid)) == -1) {
        SPC_Format_Log("+++> FAILURE: initgroups ('%s', '%s')",
		       username, 
		       pwent->pw_gid);
  }

  /* We have authenticated ourselves.  Set the process identifiers of
     this process to the looked up ones. */
  setgid(pwent->pw_gid);
  setuid(pwent->pw_uid);

  Xechdir(pwent->pw_dir);
  sprintf(HomeDir,  (XeString)"HOME=%s",  pwent->pw_dir);
  sprintf(ShellDir, (XeString)"SHELL=%s", pwent->pw_shell);
  putenv(HomeDir);
  putenv(ShellDir);

  spc_user_environment_file=(XeString)XeMalloc(MAXPATHLEN);
  sprintf(spc_user_environment_file, (XeString)"%s/%s/%s", pwent->pw_dir, 
	  SPCD_ENV_HOME_DIRECTORY, SPCD_ENV_FILE);
  default_environment=
    SPC_Add_Env_File(spc_user_environment_file, default_environment);

  SPC_Write_Protocol_Request(client_connection, NULL, LOGFILE_REPLY,
			     PASSED_FILE_NAME, NULL, NULL);

  return(prot->seqno);
}

/*----------------------------------------------------------------------+*/
int Client_Unregister(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  return(print_protocol_request((XeString)"--> UNREGISTER", prot));
}

/*----------------------------------------------------------------------+*/
int Client_Channel_Open(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  int iomode;
  SPC_Channel_Ptr channel;

  print_protocol_request((XeString)"--> CHANNEL_OPEN", prot);
  READ_OPEN(prot->dataptr, iomode);

  /* Don't ever wanna wait, do line-oriented reads,
     use the toolkit, or execute as a system command */

  /* We don't do system commands because the client side has
     already converted the channel into the proper form for us */

  iomode &= ~(SPCIO_WAIT | SPCIO_LINEORIENTED |
	      SPCIO_USE_XTOOLKIT | SPCIO_SYSTEM);

  /* However, we do always want our termination to be synchronous */

  iomode |= SPCIO_SYNC_TERMINATOR;

  channel=XeSPCOpen(NULL, iomode);
  if(channel==SPC_ERROR)
    return(SPC_ERROR);

  XeSPCAddInput(channel, SPCD_Handle_Application_Data, channel);
  XeSPCRegisterTerminator(channel, SPCD_Termination_Handler, channel);

  return((int) (intptr_t) channel);
}

/*----------------------------------------------------------------------+*/
int Client_Channel_Close(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> CHANNEL_CLOSE", prot);

  if(IS_ACTIVE(channel)) {

    /*
      Uh-oh.  We have received a close request, but the channel is
      active.  We kill the process, and explicitly wait for the
      process to terminate.
      */

    XeSPCSignalProcess(channel, SIGKILL);
    SPC_Wait_For_Termination(channel);
  }
  
  return(XeSPCClose(channel));
}

/*----------------------------------------------------------------------+*/
int Client_Channel_Reset(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> CHANNEL_RESET", prot);

  return(XeSPCReset(channel));
}

/*----------------------------------------------------------------------+*/
int Client_Channel_Attach(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;
  int pid;

  print_protocol_request((XeString)"--> ATTACH", prot);

  READ_ATTACH(prot->dataptr, pid);
  return(XeSPCAttach(channel, pid));
}

static void
Merge_Lang_Var(SPC_Channel_Ptr channel)
{
  XeString stdLang;

  if ((stdLang = SPC_Getenv((XeString)"LANG", channel->envp))
      != (XeString)NULL)
  {
    _DtXlateDb db = NULL;
    char platform[_DtPLATFORM_MAX_LEN];
    XeChar *langBuf;
    int execVer;
    int compVer;
    XeString myLang;

    if (_DtLcxOpenAllDbs(&db) == 0)
    {
      if ((_DtXlateGetXlateEnv(db, platform, &execVer, &compVer) == 0) &&
	  (_DtLcxXlateStdToOp(db, platform, compVer, DtLCX_OPER_SETLOCALE,
			      stdLang, NULL, NULL, NULL, &myLang) == 0))
      {
	if ((langBuf = (XeChar *)malloc((strlen(myLang) + 6)
					* sizeof(XeChar)))
	    != (XeChar *)NULL)
	{
	  sprintf(langBuf, "LANG=%s", myLang);
	  channel->envp = SPC_Putenv(langBuf, channel->envp);
	  free(langBuf);
	}

	free(myLang);
      }

      _DtLcxCloseDb(&db);
    }
  }
}

/*----------------------------------------------------------------------+*/
int Client_Application_Spawn(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;
  int retval;

  print_protocol_request((XeString)"--> APPLICATION_SPAWN", prot);
  READ_APPLICATION_SPAWN(prot->dataptr,
			 channel->path,
			 channel->context_dir,
			 channel->argv, channel->envp);

  Merge_Lang_Var(channel);

  channel->IOMode |= SPCIO_DEALLOC_ARGV;

  channel->envp=SPC_Merge_Envp(channel->envp, default_environment);

  retval=XeSPCExecuteProcess(channel);
  if(retval==SPC_ERROR)
    return(SPC_ERROR);

  return(channel->pid);
}

/*----------------------------------------------------------------------+*/
int Client_Application_Signal(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  XeString signame;
  int sig = 0;
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> APPLICATION_SIGNAL", prot);
  READ_STRING_NO_COPY(prot->dataptr, signame);

  if (client_connection->protocol_version >= 2) {
      if ( (sig = XeNameToSignal( signame )) == XE_SIG_NOT_IN_TABLE ) {
	  SPC_Error(SPC_Bad_Signal_Name, signame);
	  return (SPC_ERROR);
      }
  } else {
      /* Must be the old 1.0, 1.1 protocol, assume its a number.       */
      /* Note however, that this is not portable as the signal numbers */
      /* differ from system to system.  This should really be an error */

      /* 1.0 code allowed signal 0 (check if process alive), but its     */
      /* part of XPG3 so we don't honor it .. nobody ever used it anyway */

      int ok = sscanf(signame, "%x", &sig);  /* WRITE_INT (%x) was used */
      if ( (ok == EOF) || (sig == 0) ) {
	  SPC_Error(SPC_Bad_Signal_Format, signame);
	  return (SPC_ERROR);
      }
  }

  return(XeSPCSignalProcess(channel, sig));
}

/*----------------------------------------------------------------------+*/
int Client_Application_Data(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;
  buffered_data_ptr pdata=prot->dataptr;

  print_protocol_request((XeString)"--> APPLICATION_DATA", prot);

  return(XeSPCWrite(channel,
		    pdata->data+REQUEST_HEADER_LENGTH,
		    pdata->len));
}

/*----------------------------------------------------------------------+*/
int Client_Server_Debug(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;
  buffered_data_ptr pdata=prot->dataptr;
  XeChar filename[MAXPATHLEN];
  time_t timeval;

  print_protocol_request((XeString)"--> SERVER_DEBUG", prot);
  READ_DEBUG(pdata, filename);

  if(SPC_Print_Protocol)
    fclose(SPC_Print_Protocol);

  SPC_Print_Protocol=fopen(filename, (XeString)"a+");

  if(!SPC_Print_Protocol)
    return(ERROR);

  setbuf(SPC_Print_Protocol, NULL);

  time(&timeval);
  fprintf(SPC_Print_Protocol, (XeString)"Begin protocol filedump: %s", ctime(&timeval));
  return(TRUE);
}

/*
 ***
 *** The purpose of this routine is to filter out environment variables
 *** which should not get put into the environment.
 ***
*/

/*----------------------------------------------------------------------+*/
void conditional_putenv(XeString env_str)
/*----------------------------------------------------------------------+*/
{
  if(strncmp(env_str, (XeString)"HOME=", 5) && /* HOME may be different */
     strncmp(env_str, (XeString)"PWD=", 4)     /* PWD set by chdir, may change */
                                               /* Should SHELL be on this list? */
     )
    {
      putenv(env_str);
    }
}

/*----------------------------------------------------------------------+*/
int Client_Environ_Reset(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  int num_vars;
  XeString envp[100];
  char **ret;
  int outlen, i;

  ret = NULL;
  READ_ENVIRON_RESET(prot->dataptr, num_vars);
  if(num_vars < 100)
    ret = envp;

  ret = SPC_Get_Multi_Packet(client_connection, prot,
			     ret, &outlen,
			     ENVIRON_RESET, "--> ENVIRON_RESET");

  if(ret == NULL)
    return(SPC_ERROR);

  for(i=0; i<outlen; i++)
    if(ret[i] && *ret[i])
      conditional_putenv(ret[i]);

  if(ret != envp)
    free((char *)ret);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int Client_Reply_Devices(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> QUERY_DEVICES", prot);

  SPC_Write_Protocol_Request(client_connection, channel, DEVICE_REPLY,
			     channel->wires[STDIN]->master_name,
			     channel->wires[STDIN]->slave_name,
			     channel->wires[STDOUT]->master_name,
			     channel->wires[STDOUT]->slave_name,
			     channel->wires[STDERR]->master_name,
			     channel->wires[STDERR]->slave_name);
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int Client_Reply_Logfile(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  char *netfile;
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> QUERY_LOGFILE", prot);

  if (SPC_client_version_number >= SPC_PROTOCOL_VERSION_CDE_BASE && 
      IS_SPCIO_USE_LOGFILE(channel->IOMode)) {
    netfile = tt_file_netfile (channel->logfile);

    if (tt_ptr_error (netfile) != TT_OK) {
      SPC_Format_Log("+++> FAILURE: cannot create a 'netfile' name for the logfile.\n     (%s)", 
	             tt_status_message(tt_pointer_error(netfile)));
      return(SPC_ERROR);
    }

    SPC_Write_Protocol_Request(client_connection, channel, LOGFILE_REPLY,
			       netfile, NULL, NULL);
    tt_free (netfile);
  }
  else
    SPC_Write_Protocol_Request(client_connection, channel, LOGFILE_REPLY,
			       channel->logfile, NULL, NULL);

  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int Client_Delete_Logfile(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> DELETE_LOGFILE", prot);

  return(XeSPCRemoveLogfile(channel));
}

/*----------------------------------------------------------------------+*/
int Client_Reset_Termio(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  /* This handles old 1.0 versions of the SPC code.  We used to send */
  /* an hp-ux version of the termio struct in a non-portable manner  */
  /* We need to be able to "eat" such an request if we get one.      */

  print_protocol_request((XeString)"--> RESET_TERMIO", prot);

  return(SPC_Get_Termio(prot));
}

/*----------------------------------------------------------------------+*/
int Client_Reset_Termios(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  print_protocol_request((XeString)"--> RESET_TERMIOS", prot);

  return(SPC_Get_Termios(prot));
}

/* New B.00 methods */

/*----------------------------------------------------------------------+*/
int Client_Send_EOF(protocol_request_ptr prot)
/*----------------------------------------------------------------------+*/
{
  SPC_Channel_Ptr channel=prot->channel;

  print_protocol_request((XeString)"--> SEND_EOF", prot);

  return(XeSPCSendEOF(channel));
}

int Client_Channel_Termios(protocol_request_ptr prot)
{
  SPC_Channel_Ptr channel=prot->channel;

  int connection, side;
  char buffer[1024];
  struct termios t;

  print_protocol_request((XeString)"--> CHANNEL_TERMIOS", prot);

  READ_TERMIOS(prot->dataptr, connection, side, buffer);
  SPC_Encode_Termios(buffer, &t);
  
  
  return(XeSPCSetTermio(channel, connection, side, &t));
}

int Client_Enhanced_Spawn(protocol_request_ptr prot)
{
  SPC_Channel_Ptr channel = prot->channel;
  int num_vars;
  XeString buf[100];
  char **ret;
  int numenv, numarg;
  int return_value, outlen, i;

  ret = NULL;
  READ_ENVIRON_RESET(prot->dataptr, num_vars);
  if(num_vars < 100)
    ret = buf;

  ret = SPC_Get_Multi_Packet(client_connection, prot,
			     ret, &outlen,
			     APP_B00_SPAWN, "--> APP_B00_SPAWN");

  if(ret == NULL)
    return(SPC_ERROR);

  channel->path = strdup(ret[0]);
  channel->context_dir  = strdup(ret[1]);
  
  numarg = atoi(ret[2]);
  numenv = atoi(ret[3]);

  if(numarg == 0)
    channel->argv = NULL;
  else {
    channel->argv = &ret[4];
    ret[numarg+3] = NULL;
  }

  if(numenv == 0)
    channel->envp = NULL;
  else {
    channel->envp = (char **)XeMalloc((numenv+1)*sizeof(char *));
    channel->envp[0] = NULL;
    channel->envp = SPC_Merge_Envp(channel->envp, &ret[numarg+4]);

    Merge_Lang_Var(channel);

    channel->envp = SPC_Merge_Envp(channel->envp, default_environment);
  }
  
  return_value = XeSPCExecuteProcess(channel);

  /* Make the world safe for freeing the channel */
  
  channel->argv = NULL;
  if(channel->envp == &ret[numarg+4])
    channel->envp = NULL;
  
  for(i=0; i<outlen; i++)
    if(ret[i])
      free(ret[i]);
  
  if(ret != buf)
    free((char *)ret);

  if(return_value==SPC_ERROR)
    return(SPC_ERROR);
  
  return(channel->pid);
}

/*----------------------------------------------------------------------+*/
void SPCD_Reply(SPC_Connection_Ptr connection,
                 protocol_request_ptr prot,
                 int retval,
                 int errval)
/*----------------------------------------------------------------------+*/
{
  if(retval==SPC_ERROR)
    retval= (-XeSPCErrorNumber);

  SPC_Write_Reply(connection, prot, retval, errval);
}

/*----------------------------------------------------------------------+*/
  /*
   * This function is invoked when the exit timer expires.
   * If sub-processes are running, return to select; 
   * otherwise exit.
   */
#ifdef SA_HANDLER_INT_ARG
void SPCD_Alarm_Handler(int not_used)
#else
void SPCD_Alarm_Handler()
#endif /* SA_HANDLER_INT_ARG */
/*----------------------------------------------------------------------+*/
{
  int i;

  if (exit_timeout == SPCD_NO_TIMER) 
    return;

  if (SPC_pid_list != NULL) 
    for (i=0; SPC_pid_list[i] != 0; i++) {
      if (SPC_pid_list[i] != SPCD_DEAD_PROCESS) {
	/*
	 * Have at least one sub- process running so reset the 
	 * alarm and return to select.
	 */
        (void) alarm (exit_timeout * 60);
	return;
      }
    }

  /*
   * There are no sub-processes running.  Exit if a request is not pending
   */
  if (request_pending != SPCD_REQUEST_PENDING) {
    SPC_Format_Log((XeString)
	           "Exit timer expired after '%d' minutes of no activity.", 
		   exit_timeout);
    SPCD_Exit (0);
  }
}
