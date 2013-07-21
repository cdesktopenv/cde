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
 * $TOG: spc-error.c /main/10 1998/04/10 08:27:30 mgreess $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#include <Dt/UserMsg.h>

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */
#include <errno.h>
#include <stdarg.h>

#include <SPC/spcP.h>
#include <bms/MemoryMgr.h> /* NOTE: sbport.h must be the first include. */

#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include "DtSvcLock.h"

/*
 * Log file information (routines near bottom of file)
 */

#define TEMPLATE_EXTENSION	  (XeString)".XXXXXX" /* For mktemp(3c) */

XeChar spc_logfile[MAXPATHLEN+1];
XeChar spc_logging = FALSE;
/* FILE *spc_logF = NULL; -- to bmsglob.c */

int spc_logfd = 0;

/* This is the SPC error number variable */
/* ------------------------------------- */
/* int XeSPCErrorNumber = NULL;  --- now in bmsglob.c */

/*------------------------------------------------------------------------+*/
static XeString
err_mnemonic(unsigned int errn, char *buff)
/*------------------------------------------------------------------------+*/
{
    /* Since there is currently no standard way to get an err number    */
    /* mnenonic from the system, we do it the hard way.  We can't even  */
    /* use a table as the actual numeric values may differ on machines. */
    /* Another potential problem is if one of these gets passed across  */
    /* a network connection from a different type of machine that has   */
    /* values that differ than the machine this routine runs on, we     */
    /* could have problems.  I don't think anycode does that now.       */
    /* ---------------------------------------------------------------- */

    XeString s;
    
    switch (errn) {
	
    /* These are all in POSIX 1003.1 and/or X/Open XPG3     */
    /* ---------------------------------------------------- */
    case EPERM		: s = (XeString)"EPERM"; 	break;
    case ENOENT		: s = (XeString)"ENOENT"; 	break;
    case ESRCH		: s = (XeString)"ESRCH"; 	break;
    case EINTR		: s = (XeString)"EINTR"; 	break;
    case EIO		: s = (XeString)"EIO"; 		break;
    case ENXIO		: s = (XeString)"ENXIO"; 	break;
    case E2BIG		: s = (XeString)"E2BIG"; 	break;
    case ENOEXEC	: s = (XeString)"ENOEXEC"; 	break;
    case EBADF		: s = (XeString)"EBADF"; 	break;
    case ECHILD		: s = (XeString)"ECHILD"; 	break;
    case EAGAIN		: s = (XeString)"EAGAIN"; 	break;
    case ENOMEM		: s = (XeString)"ENOMEM"; 	break;
    case EACCES		: s = (XeString)"EACCES"; 	break;
    case EFAULT		: s = (XeString)"EFAULT"; 	break;
    case ENOTBLK	: s = (XeString)"ENOTBLK"; 	break;
    case EBUSY		: s = (XeString)"EBUSY"; 	break;
    case EEXIST		: s = (XeString)"EEXIST"; 	break;
    case EXDEV		: s = (XeString)"EXDEV"; 	break;
    case ENODEV		: s = (XeString)"ENODEV"; 	break;
    case ENOTDIR	: s = (XeString)"ENOTDIR"; 	break;
    case EISDIR		: s = (XeString)"EISDIR"; 	break;
    case EINVAL		: s = (XeString)"EINVAL"; 	break;
    case ENFILE		: s = (XeString)"ENFILE"; 	break;
    case EMFILE		: s = (XeString)"EMFILE"; 	break;
    case ENOTTY		: s = (XeString)"ENOTTY"; 	break;
    case ETXTBSY	: s = (XeString)"ETXTBSY"; 	break;
    case EFBIG		: s = (XeString)"EFBIG"; 	break;
    case ENOSPC		: s = (XeString)"ENOSPC"; 	break;
    case ESPIPE		: s = (XeString)"ESPIPE"; 	break;
    case EROFS		: s = (XeString)"EROFS"; 	break;
    case EMLINK		: s = (XeString)"EMLINK"; 	break;
    case EPIPE		: s = (XeString)"EPIPE"; 	break;
    case ENOMSG		: s = (XeString)"ENOMSG"; 	break;
    case EIDRM		: s = (XeString)"EIDRM"; 	break;
    case EDEADLK	: s = (XeString)"EDEADLK"; 	break;
    case ENOLCK		: s = (XeString)"ENOLCK"; 	break;
#ifndef __aix
    case ENOTEMPTY	: s = (XeString)"ENOTEMPTY"; 	break;
#endif
    case ENAMETOOLONG	: s = (XeString)"ENAMETOOLONG"; break;
    case ENOSYS		: s = (XeString)"ENOSYS"; 	break;

    /* You could include machine specific stuff here ...    */
    /* ---------------------------------------------------- */
	

    /* ---------------------------------------------------- */
	default         : s = XeString_NULL;            break;
    }
    

    if (s)
	sprintf(buff, "%s (%d)",s,errn);
    else
	sprintf(buff, "(%d)",errn);
    
    return buff;
}

/*----------------------------------------------------------------------+*/
XeString SPC_copy_string(XeString str)
/*----------------------------------------------------------------------+*/
{
  int len;
  XeString tmp_str;

  if(!str)
    return(XeString_NULL);
  
  len=strlen(str);

  tmp_str=(XeString)XeMalloc(len+1);
  strcpy(tmp_str, str);
  return(tmp_str);
}

/*----------------------------------------------------------------------+*/
void SPC_Error (int error, ...)
/*----------------------------------------------------------------------+*/
{
  va_list ap;
  SPCError *err;
  XeChar *buffer;
  XeString arg1;
  long arg2;
  XeChar buff[40];
  
  err=SPC_Lookup_Error(error);

  if(!err)
    return;

  if(err->text) {
    free(err->text);
    err->text = NULL;
  }

  va_start(ap, error);                 /** Start varargs **/
  arg1=va_arg(ap, XeString);

  if (arg1)
  {
  /* The argument on the stack may be holding an int or a char pointer. */
  /* Always popping off a long into the value of arg2 works fine */
  /* because the subsequent call to sprintf does the proper conversion via */
  /* "err->format". */
      arg2=va_arg(ap, long);
  }      
  else
  {
      arg1 = XeString_Empty;
      arg2 = 0;
  }

  buffer = (XeChar*) malloc(sizeof(XeChar) * SPC_BUFSIZ);
  if (buffer)
  {
      sprintf(buffer, err->format, arg1, arg2);
      err->text=SPC_copy_string(buffer);
      free(buffer);
  }
  va_end(ap);                   /** End varargs **/

  _DtSvcProcessLock();
  XeSPCErrorNumber=error;
  if (SPC_who_am_i == SPC_I_AM_A_DAEMON){

     SPC_Format_Log((XeString)"DTSPCD error (%d): %s", 
		    XeSPCErrorNumber, err->text);
     if (err->use_errno){
	XeString        errname;
	XeString        errmsg;
	unsigned int    errn = errno;

	errname = err_mnemonic(errn, buff);

	if (!(errmsg = strerror(errn)))
	   errmsg = (XeString) "unknown";

	SPC_Format_Log((XeString)"  [%s] %s", errname, errmsg);
     }
     if(err->severity == XeFatalError || err->severity == XeInternalError) {
	SPC_Format_Log((XeString)"Exiting server ...");
	SPC_Close_Log();
	_DtSvcProcessUnlock();
        exit (3);
     }
  }
  else
     _DtSimpleError(XeProgName, XeError, XeString_NULL, err->text);

  _DtSvcProcessUnlock();
  return;
}

/*
 * Log file routines
 *
 * Note: Current restriction of only one log file open at a time.
 */

/*----------------------------------------------------------------------+*/
int
SPC_Make_Log_Filename(XeString name,
		      int unique)      	/* When TRUE, make name unique */
/*----------------------------------------------------------------------+*/
{
  /* Make a log filename based on the passed name (and perhaps process id) */
  XeString cp;
  XeString log_file_path = NULL;

  _DtSvcProcessLock();
  /* first build the log file path */
  if (!name || !*name) {
    log_file_path = XeSBTempPath((XeString)"DTSPCD_log");
    strcpy(spc_logfile, log_file_path);
  }
  else {
    if (strlen(name) > MAXPATHLEN)
	_DtSimpleError(XeProgName, XeInternalError, XeString_NULL, 
		      (XeString)"String too long in DTSPCD_Make_Log_Filename()");
    
    strcpy(spc_logfile, name);
  }

  if (unique) {
    /* Add the extension.  No strlen checking is done */
    strcat(spc_logfile, TEMPLATE_EXTENSION);

    cp = (XeString) mktemp(spc_logfile);
    if (!cp || !*cp) {
      /* Sorry, but this is the best we can do */
      strcpy(spc_logfile, (log_file_path) ? log_file_path : name);
    }
  }

  /* free the strings allocated for the path */
  if (log_file_path) XeFree(log_file_path);
  _DtSvcProcessUnlock();
  return TRUE;
}

/*----------------------------------------------------------------------+*/
int
SPC_Open_Log(XeString filename,
	     int unique)		/* When TRUE, make filename unique */
/*----------------------------------------------------------------------+*/
{
  /* Open the SPC log file */

  /* Use the filename if one was passed to make a log filename */
  SPC_Make_Log_Filename(filename, unique);

  /* Open the logfile */
  _DtSvcProcessLock();
  spc_logF = fopen(spc_logfile, "a+");
  if (!spc_logF) {
    _DtSvcProcessUnlock();
    return(SPC_ERROR);
  }
  spc_logging = TRUE;
  
  SPC_Format_Log((XeString)"*** DTSPCD logging started, file: `%s'", spc_logfile);
  
  _DtSvcProcessUnlock();
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
SPC_Close_Log(void)
/*----------------------------------------------------------------------+*/
{
  /* Close the current log file */

  SPC_Format_Log((XeString)"*** DTSPCD logging stopped");
  
  _DtSvcProcessLock();
  if (spc_logF) {
    fclose(spc_logF);
  }
  spc_logfd = 0;
  spc_logF = NULL;
  spc_logging = FALSE;

  _DtSvcProcessUnlock();
  return(TRUE);
}

/*----------------------------------------------------------------------+*/
int
SPC_Write_Log(XeString str)
/*----------------------------------------------------------------------+*/
{
  /* Write the passed message to the log file */
  time_t t;
  _Xctimeparams ctime_buf;
  char *result;

  _DtSvcProcessLock();
  if (spc_logging && spc_logF) {
    t = time(NULL);
    result = _XCtime(&t, ctime_buf);
    fprintf(spc_logF, "%s: %s", str, result);
    fflush(spc_logF);
  }

  _DtSvcProcessUnlock();
  return(TRUE);
}


/*----------------------------------------------------------------------+*/
int SPC_Format_Log (XeString format, ...)
/*----------------------------------------------------------------------+*/
{
  /* Format the passed message to the log file */
  va_list args;
  time_t t;
  _Xctimeparams ctime_buf;
  char *result;

  _DtSvcProcessLock();
  if (spc_logging && spc_logF) {
    /* First the message */
    va_start(args, format);
    vfprintf(spc_logF, format, args);
    va_end(args);

    /* Now a time stamp */
    t = time(NULL);
    result = _XCtime(&t, ctime_buf);
    fprintf(spc_logF, ": %s", result);
    fflush(spc_logF);
  }

  _DtSvcProcessUnlock();
  return(TRUE);
}

/*
 **
 ** This next routine used to be such a nice little guy...  Once upon a
 ** time I had all the error messages in a very
 ** compact representation.  It was a vector of SPCError structures.
 ** When I wanted to go from the integer representation of an error to
 ** its textual form, I simply did a table lookup.  Unfortunately,
 ** that method did not work at all well with the NLS scheme cooked up
 ** by the Excalibur team.  This scheme was nice for programs which
 ** just had the strings in the text.  It basically searched the
 ** source program for a string with a funny symbol and replaced it
 ** with a function call.  Well, to make a long story short, I decided
 ** to go with that scheme.  Thus this function.
 **
*/ 

SPCError spc_error_struct;

/*----------------------------------------------------------------------+*/
SPCError *SPC_Lookup_Error(int errornum)
/*----------------------------------------------------------------------+*/
{
  _DtSvcProcessLock();
  switch (errornum) {
    
  case SPC_Out_Of_Memory:
    spc_error_struct.format    = (XeString) "><Unable to allocate memory for internal SPC operation\n Perhaps you need to add more swap space to the system";
    spc_error_struct.severity  = XeFatalError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Argument:
    spc_error_struct.format    = (XeString) "><Bad argument to DTSPCD call";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;
    
  case SPC_Active_Channel:
    spc_error_struct.format    = (XeString) "><Channel already active";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Inactive_Channel:
    spc_error_struct.format    = (XeString) "><Channel is not active";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Internal_Error:
    spc_error_struct.format    = (XeString) "><Internal SPC Error";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Cannot_Fork:
    spc_error_struct.format    = (XeString) "><Cannot fork";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Cannot_Exec:
    spc_error_struct.format    = (XeString) "><Cannot exec file %s.\nPerhaps your PATH variable is incorrect.\nUse the following errno value to further diagnose the problem.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_No_Pipe:
    spc_error_struct.format    = (XeString) "><Cannot get pipe";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_No_Pty:
    spc_error_struct.format    = (XeString) "><Unable to allocate pty for a DTSPCD channel.\nTry cleaning up some currently running processes to release their ptys,\nor reconfigure your kernel to increase the pty limit.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Connector:
    spc_error_struct.format    = (XeString) "><Bad connector";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Reading:
    spc_error_struct.format    = (XeString) "><Unexpected error reading data on connection to host %s.\nUse the following errno value to correct the problem.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Writing:
    spc_error_struct.format    = (XeString) "><Unexpected error writing data on channel";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Service:
    spc_error_struct.format    = (XeString) "><Unknown internet service %s/%s.\nMake an entry in your /etc/services file";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Unknown_Host:
    spc_error_struct.format    = (XeString) "><Unable to find a host entry for %s.\nTry adding an entry in /etc/hosts for it.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Socket:
    spc_error_struct.format    = (XeString) "><Socket failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Connect:
    spc_error_struct.format    = (XeString) "><Connect call failed to remote host %s\nPerhaps the desktop is not installed on the remote host,\nor the remote inetd program needs to be restarted (via 'inetd -c'),\nor the remote file /etc/inetd.conf does not have an entry for the dtspcd process.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Bind:
    spc_error_struct.format    = (XeString) "><Bind failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Accept:
    spc_error_struct.format    = (XeString) "><Accept failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Reuse:
    spc_error_struct.format    = (XeString) "><Reuse socket option failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Open:
    spc_error_struct.format    = (XeString) "><Cannot open file";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Connection_EOF:
    if (SPC_who_am_i == SPC_I_AM_A_DAEMON)
      spc_error_struct.format    = (XeString) "><Client has disconneted (received EOF).";
    else
      spc_error_struct.format    = (XeString) "><The dtspcd process on host '%s' has terminated.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Timeout:
    spc_error_struct.format    = (XeString) "><Internal timeout expired";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Protocol:
    spc_error_struct.format    = (XeString) "><Illegal protocol request";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Unexpected_Reply:
    spc_error_struct.format    = (XeString) "><Protocol error: unexpected reply";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_No_Channel:
    spc_error_struct.format    = (XeString) "><Cannot initialize channel";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Illegal_Iomode:
    spc_error_struct.format    = (XeString) "><Inconsistent iomode value specified";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_No_Signal_Handler:
    spc_error_struct.format    = (XeString) "><Cannot set SIGCLD handler";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Operation:
    spc_error_struct.format    = (XeString) "><Illegal operation";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Fd:
    spc_error_struct.format    = (XeString) "><Bad file descriptor";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Ioctl:
    spc_error_struct.format    = (XeString) "><ioctl call failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Select:
    spc_error_struct.format    = (XeString) "><select call failed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bind_Timeout:
    spc_error_struct.format    = (XeString) "><Timeout on bind";
    spc_error_struct.severity  = XeWarning;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Arg_Too_Long:
    spc_error_struct.format    = (XeString) "><Argument %.50s... to DTSPCD system call is too long, max. length is %d";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Write_Prot:
    spc_error_struct.format    = (XeString) "><Error writing protocol request to host %s.\nPerhaps the remote server has crashed.\nUse the following errno value to diagnose the problem.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Username:
    spc_error_struct.format    = (XeString) "><Incorrect user name";
    spc_error_struct.severity  = XeFatalError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Password:
    spc_error_struct.format    = (XeString) "><Incorrect password";
    spc_error_struct.severity  = XeFatalError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Client_Not_Valid:
    spc_error_struct.format    = (XeString) "><Client not valid";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Cannot_Open_Slave:
    spc_error_struct.format    = (XeString) "><Unable to open slave pty %s.\nUse the following errno value to correct the problem";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Protocol_Abort:
    spc_error_struct.format    = (XeString) "><Received ABORT protocol request on connection to %s.";
    spc_error_struct.severity  = XeFatalError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Env_Too_Big:
    spc_error_struct.format    = (XeString) "><Environment variable %.50s... too big,\nmaximum size is %d\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Unlink_Logfile:
    spc_error_struct.format    = (XeString) "><Cannot unlink logfile";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Closed_Channel:
    spc_error_struct.format    = (XeString) "><Channel already closed";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Authentication:
    spc_error_struct.format    = (XeString) "><Cannot open user authentication file";
    spc_error_struct.severity  = XeFatalError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Cannot_Open_Log:
    spc_error_struct.format    = (XeString) "><Unable to open log file %s\nUse the following errno value to correct the problem";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Connection_Reset:
    spc_error_struct.format    = (XeString) "><Remote data connection to %s reset by peer\nRemote host may not have an entry for the local host in /usr/adm/inetd.sec.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Register_Username:
    spc_error_struct.format    = (XeString) "><Cannot register user --\nImproper password or uid for user '%s' on remote host '%s'.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Register_Netrc:
    spc_error_struct.format    = (XeString) "><Cannot register user --\nUnable to create a pathname to the authentication file '%s' on host '%s'.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Register_Open:
    spc_error_struct.format    = (XeString) "><Cannot register user --\nUnable to open authentication file '%s' on host '%s'.\nUse the following errno value to diagnose the problem.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Register_Handshake:
    spc_error_struct.format    = (XeString) "><Cannot register user --\nPerhaps user '%s' does not have the same uid on host '%s'.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

 case SPC_Bad_Termios_Mode :
    spc_error_struct.format    = (XeString) "><An error has been detected in the TERMIOS_REQUEST data.\nThe item '%s' is not recognized as a valid item for the Mode flags.\nThe item has been ignored.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

 case SPC_Bad_Termios_Speed :
    spc_error_struct.format    = (XeString) "><An error has been detected in the TERMIOS_REQUEST data.\nThe item '%s' is not recognized as a valid item for a speed setting.\nThe item has been ignored.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

 case SPC_Bad_Termios_CC :
    spc_error_struct.format    = (XeString) "><An error has been detected in the TERMIOS_REQUEST data.\nThe item '%s' is not recognized as a valid item for a Control Character name/value pair.\nThe item has been ignored.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

 case SPC_Bad_Termios_Proto :
    spc_error_struct.format    = (XeString) "><An error has been detected in the TERMIOS_REQUEST data.\nThe string does not have the correct number of fields -- %s.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Signal_Name :
    spc_error_struct.format    = (XeString) "><The signal '%s' is not supported on this machine.\nThe DTSPCD signal request has been ignored.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Signal_Value :
    spc_error_struct.format    = (XeString) "><The signal %d is not supported by the XeSignalToName() routine.\nIt can not be sent via DTSPCD to a remote machine.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_Signal_Format :
    spc_error_struct.format    = (XeString) "><The APPLICATION_SIGNAL DTSPCD data '%s' is not recognized.\nIt is expected to be a signal name or a signal number.\n";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

  case SPC_Bad_tc_Call :
    spc_error_struct.format    = (XeString) "><The terminal control call to '%s' failed.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_cannot_Chdir :
    spc_error_struct.format    = (XeString) "><Cannot cd to directory '%s'.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;

  case SPC_Bad_Permission :
    spc_error_struct.format    = (XeString) "><Incorrect permission on DTSPCD Authentication file.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = TRUE;
    break;
    
  case SPC_Cannot_Create_Netfilename :
    spc_error_struct.format    = (XeString) "><Cannot create a pathname to the current working\ndirectory '%s' from host '%s'.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;
    
  case SPC_Protocol_Version_Error:
    spc_error_struct.format    = (XeString) "><SPC protocol version mismatch.  The local version is %d, but the version of the SPC Daemon is %d.  This operation requires equivalent protocol versions.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;

    /* JET - buffer overflow attempt */
    /* VU#172583 */
  case SPC_Buffer_Overflow:
    spc_error_struct.format    = (XeString) "><Attempted Buffer Overflow from host %s.\nConnection dropped.";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;


  default:
    spc_error_struct.format    = (XeString) "><Unknown error code";
    spc_error_struct.severity  = XeError;
    spc_error_struct.use_errno = FALSE;
    break;
  }
    
  _DtSvcProcessUnlock();
  return(&spc_error_struct);
}

