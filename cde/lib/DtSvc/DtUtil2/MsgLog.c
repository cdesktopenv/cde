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
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * MsgLog.c - public interfaces for the Message Logging Service
 *
 * NOTE: the cpp define MSGLOG_CLIENT_ONLY is not defined when this
 *       file is compiled for the DtSvc library.  MSGLOG_CLIENT_ONLY
 *       is only defined when an application intends to use these
 *       routines directly because the application does not to build
 *       in a dependecy to the DtSvc library (e.g. dtexec).
 *
 * $TOG: MsgLog.c /main/21 1998/10/26 17:23:21 mgreess $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define X_INCLUDE_PWD_H
#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <sys/param.h>
#include <limits.h>

#include <Dt/DtPStrings.h>
#ifndef MSGLOG_CLIENT_ONLY
# include <Dt/UserMsg.h>
# include <DtSvcLock.h>
#endif /* MSGLOG_CLIENT_ONLY */
#include <Dt/DtGetMessageP.h>
#include <Dt/MsgLog.h>
#include <Dt/MsgLogI.h>

#define MAX_DATE_TIME_STRING 	256

/* 
 * Static variables
 */
static char 		* information_string = NULL;
static char 		* stderr_string = NULL;
static char 		* debug_string = NULL;
static char 		* warning_string = NULL;
static char 		* error_string = NULL;
static char 		* unknown_string = NULL;
#ifndef MSGLOG_CLIENT_ONLY
static DtMsgLogHandler 	saved_msglog_handler 	= NULL;
#endif /* MSGLOG_CLIENT_ONLY */


/*
 * Static constants
 */
static const char	* LOGFILE_NAME		= DtERRORLOG_FILE;
static const char	* TMP_DIR		= "/tmp";
static const char	* OPEN_FLAG		= "a+";
static const int 	SET_NUM 		= 50;

#ifdef CDE_LOGFILES_TOP
static const char	* CDE_VAR_TMP_DIR	= CDE_LOGFILES_TOP ;
#else
static const char	* CDE_VAR_TMP_DIR	= "/var/dt/tmp";
#endif


/*
 * Static function forward declarations
 */
static char * get_file_name (
	const char	* type,
	FILE		** fp,
	const char	* format,
	... );
static char * check_possible_files (
	const char	* type,
	FILE		** fp );
static void initialize_message_strings (void);


/*
 * initialize_message_string - 
 *
 * Modified: initializes the static message string variables
 *   
 */
static void initialize_message_strings (void)
{
      information_string = strdup (Dt11GETMESSAGE (SET_NUM, 1, "INFORMATION"));
      stderr_string      = strdup (Dt11GETMESSAGE (SET_NUM, 2, "STDERR"));
      debug_string       = strdup (Dt11GETMESSAGE (SET_NUM, 3, "DEBUG"));
      warning_string     = strdup (Dt11GETMESSAGE (SET_NUM, 4, "WARNING"));
      error_string       = strdup (Dt11GETMESSAGE (SET_NUM, 5, "ERROR"));
      unknown_string     = strdup (Dt11GETMESSAGE (SET_NUM, 6, "UNKNOWN"));
}


/*
 * get_file_name - given a sprintf-like format and a variable
 *    list of args, create a filename and open the file.  
 *
 * Modified:
 *     
 *      fp		- set to the opened file or NULL if the open 
 *			  fails
 *
 * Returns: a filename or NULL if the filename cannot be opened
 *    with mode 'type'.
 */
static char * get_file_name (
	const char	* type,
	FILE		** fp,		/* MODIFIED */
	const char	* format,
	... )
{
	char		*file, *rtn;
	va_list		args;

	file = malloc(MAXPATHLEN+1);
	if (! file) return NULL;

	Va_start (args, format);

	(void) vsprintf (file, format, args);
	va_end (args);

	if ((*fp = fopen (file, type)) == NULL)
		return (NULL);
	       
	rtn = strdup (file);
	free(file);
	return rtn;
}


/*
 * check_possible_files - generates possible filenames to use for
 *    the message logging.
 *
 *    The first one of the following files that is append'able is
 *    returned:
 *
 *       o $HOME/DtPERSONAL_CONFIG_DIRECTORY/LOGFILE_NAME
 *
 *       o CDE_VAR_TMP_DIR/$DTUSERSESSION/LOGFILE_NAME
 *
 *       o TMP_DIR/<login_name_from_passwd_file>/LOGFILE_NAME
 *
 * Note: #2 is only checked if $DTUSERSESSION is defined
 *
 * Modified:
 *
 *	fp		- set to the opened file or NULL if the open 
 *			  fails
 *
 * Returns: a filename if one if found that is append'able or NULL
 *    if such a file cannot be determined.
 */
static char * check_possible_files (
	const char	* type,
	FILE		** fp )		/* MODIFIED */
{
	char		* file;
	char		* env;
	_Xgetpwparams	pwd_buf;
	struct passwd	* pwd_ret;

	if ((file = get_file_name (type,
				   fp,
				   "%s/%s/%s",
				   getenv("HOME"),
				   DtPERSONAL_CONFIG_DIRECTORY,
				   LOGFILE_NAME)) != NULL)
		return (file);

	if ((env = getenv ("DTUSERSESSION")) != NULL) {
		if ((file = get_file_name (type,
					   fp,
					   "%s/%s/%s",
					   CDE_VAR_TMP_DIR,
					   env,
					   LOGFILE_NAME)) != NULL)
			return (file);
	}

	if ((env = getenv ("LOGNAME")) != NULL) {
		if ((file = get_file_name (type,
					   fp,
					   "%s/%s.%s",
					   TMP_DIR,
					   env,
					   LOGFILE_NAME)) != NULL)
			return (file);
	}

	if ((env = getenv ("USER")) != NULL) {
		if ((file = get_file_name (type,
					   fp,
					   "%s/%s.%s",
					   TMP_DIR,
					   env,
					   LOGFILE_NAME)) != NULL)
			return (file);
	}

	if ((pwd_ret = _XGetpwuid (getuid(), pwd_buf)) != NULL) {
		if ((file = get_file_name (type,
					   fp,
					   "%s/%s.%s",
					   TMP_DIR,
					   pwd_ret->pw_name,
					   LOGFILE_NAME)) != NULL)
			return (file);
	}

	return (NULL);
}

/*
 * DtMsgLogMessage -
 *
 * Returns: 0 if the message is successfully logged or 1 if an
 *    error occurs and the message is not logged.
 */
void DtMsgLogMessage (
	const char 		* program_name,
	DtMsgLogType		msg_type,
	const char 		* format,
	... )
{
	va_list			args;
	FILE 			* fp = NULL;
	char			* file = NULL;
	time_t 			now;
	char			* msg_string;  /* temp msg type string */
	int			num_bytes;
	char 			buf[MAX_DATE_TIME_STRING];
#ifdef NLS16
	char			* tmp_format;
#endif
	_Xctimeparams		ctime_buf;
	char			* result;
	_Xltimeparams		localtime_buf;
	struct tm		* current_time;

	Va_start (args, format);

#ifndef MSGLOG_CLIENT_ONLY
	_DtSvcProcessLock();
	if (saved_msglog_handler != NULL) {

		(*saved_msglog_handler) (program_name ? program_name : 
					 DtProgName, 
					 msg_type,
					 format,
					 args);
		_DtSvcProcessUnlock();
		va_end (args);

		return;
	}
	_DtSvcProcessUnlock();
#endif /* MSGLOG_CLIENT_ONLY */

	if (!information_string) {
#ifndef MSGLOG_CLIENT_ONLY
		_DtSvcProcessLock();
#endif /* MSGLOG_CLIENT_ONLY */
		if (!information_string)
			initialize_message_strings ();
#ifndef MSGLOG_CLIENT_ONLY
		_DtSvcProcessUnlock();
#endif /* MSGLOG_CLIENT_ONLY */
	}

	/*
	 * Need to get a copy of the string in case another 
	 * thread calls catgets and puts a different
	 * string in catgets's static buffer before 
	 * msg_string is output
	 */
	switch (msg_type) {
		case	DtMsgLogInformation:
			msg_string = information_string;
			break;
		case	DtMsgLogStderr:
			msg_string = stderr_string;
			break;
		case	DtMsgLogDebug:
			msg_string = debug_string;
			break;
		case	DtMsgLogWarning:
			msg_string = warning_string;
			break;
		case	DtMsgLogError:
			msg_string = error_string;
			break;
		default:
			msg_string = unknown_string;
			break;
	}

	now = time ((time_t)0);

	/*
	 * Write to stderr if a log file cannot be determined
	 * or if it isn't writeable.
	 */
	if ((fp = DtMsgLogOpenFile (OPEN_FLAG, &file)) == NULL)
		fp = stderr;
#ifdef NLS16

	current_time = _XLocaltime(&now, localtime_buf);
	/*
	 * Need to save format because the next call to catgets
	 * may overwrite it on some platforms (if format itself 
	 * is the result of a call to catgets).
	 */
	tmp_format = strdup ((char *) format);

	(void) strftime (buf, 
			 MAX_DATE_TIME_STRING, 
			 Dt11GETMESSAGE (48, 1, "%a %b %d %H:%M:%S %Y\n"), 
			 current_time);

	num_bytes = fprintf (fp, "*** %s(%d): %s: PID %d: %s", 
			msg_string, msg_type, 
#ifndef MSGLOG_CLIENT_ONLY
			program_name ? program_name : DtProgName, 
#else
			program_name ? program_name : "", 
#endif /* MSGLOG_CLIENT_ONLY */
			getpid(), buf);
#else
	result = _XCtime(&now, ctime_buf);
	num_bytes = fprintf (fp, "*** %s(%d): %s: PID %ld: %s", 
			msg_string, msg_type, 
#ifndef MSGLOG_CLIENT_ONLY
			program_name ? program_name : DtProgName, 
#else
			program_name ? program_name : "", 
#endif /* MSGLOG_CLIENT_ONLY */
			(long)getpid(), result);
#endif

#ifdef NLS16
	num_bytes += vfprintf (fp, tmp_format, args);
	free (tmp_format);
#else
	num_bytes += vfprintf (fp, format, args);
#endif
	va_end (args);

	fprintf (fp, "\n*** [%d]\n\n", num_bytes);

	if (fp != stderr) {
		(void) fflush (fp);
		(void) fclose(fp);
	}

  	if (file)
		free (file);
}


#ifndef MSGLOG_CLIENT_ONLY
/*
 * DtMsgLogSetHandler - caches an alternate message logging
 *    handler
 *
 * Modified:
 *
 *    saved_msglog_handler - set to the given handler
 *
 * Returns: if handler is NULL, the default handler is restored;
 *    returns a pointer to the previous handler
 *
 */
DtMsgLogHandler DtMsgLogSetHandler (
	DtMsgLogHandler		handler )
{
	DtMsgLogHandler 	previous_handler;

	_DtSvcProcessLock();
	if (handler == NULL) {
		if (saved_msglog_handler) {
			previous_handler = saved_msglog_handler;
			saved_msglog_handler = NULL;
			return (previous_handler);
		}
		else {
			saved_msglog_handler = NULL;
			return  (DtMsgLogHandler) DtMsgLogMessage;
		}
	}

	if (saved_msglog_handler)
		previous_handler = saved_msglog_handler;
	else
		previous_handler = (DtMsgLogHandler) DtMsgLogMessage;

	saved_msglog_handler = handler;
	_DtSvcProcessUnlock();
	return (previous_handler);
}
#endif /* MSGLOG_CLIENT_ONLY */


/*
 * DtMsgLogOpenFile - opens the logfile 
 *
 * Returns: returns a pointer to the opened logfile; if a logfile
 *    cannot be opened, stderr is returned
 *
 * Modified:  
 *
 *    fp - is set to the opened file
 *
 *    filename_return - will be set to the filename if it
 *       if it is not NULL and a file is opened.  If filename_return
 *       is not NULL and and a file is opened, the calling function
 *       should free the space allocated for the filename.
 */
FILE * DtMsgLogOpenFile (
	const char		* type,
	char			** filename_return)	/* MODIFIED */
{
	FILE		* fp = NULL;
	char		* pch;

	pch = check_possible_files (type, &fp);

	if (filename_return)
		*filename_return = pch;
	else if (pch)
		free (pch);

	if (!fp)
		fp = stderr;

	return (fp);	
}
