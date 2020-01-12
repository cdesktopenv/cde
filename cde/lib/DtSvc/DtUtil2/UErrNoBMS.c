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
/* $TOG: UErrNoBMS.c /main/9 1998/04/09 17:50:11 mgreess $ */
/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1987, 1988, 1989, 1990, 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         UErrNoBMS.c
 **
 **   Project:	    DT
 **
 **   Description:  This file contains the CDE 1.0 message logging functions
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <Dt/MsgLog.h>
#include <Dt/MsgLogI.h>
#include <Dt/UserMsg.h>

#define MESSAGE_BUFFER		2048


static char * errno_string( 
                        unsigned int errn, char *buff) ;
static void log_message( 
                        char *progName,
                        char *help,
                        char *message,
                        DtSeverity severity,
                        int errnoset);


static char * 
errno_string(
        unsigned int errn, char *buff)
{
    char * s;
    
    switch (errn) {
	
    /* These are all in POSIX 1003.1 and/or X/Open XPG3     */
    /* ---------------------------------------------------- */
    case EPERM		: s = "EPERM"; 	break;
    case ENOENT		: s = "ENOENT"; break;
    case ESRCH		: s = "ESRCH"; 	break;
    case EINTR		: s = "EINTR"; 	break;
    case EIO		: s = "EIO"; 	break;
    case ENXIO		: s = "ENXIO"; 	break;
    case E2BIG		: s = "E2BIG"; 	break;
    case ENOEXEC	: s = "ENOEXEC";break;
    case EBADF		: s = "EBADF"; 	break;
    case ECHILD		: s = "ECHILD"; break;
    case EAGAIN		: s = "EAGAIN"; break;
    case ENOMEM		: s = "ENOMEM"; break;
    case EACCES		: s = "EACCES"; break;
    case EFAULT		: s = "EFAULT"; break;
    case ENOTBLK	: s = "ENOTBLK";break;
    case EBUSY		: s = "EBUSY"; 	break;
    case EEXIST		: s = "EEXIST"; break;
    case EXDEV		: s = "EXDEV"; 	break;
    case ENODEV		: s = "ENODEV"; break;
    case ENOTDIR	: s = "ENOTDIR";break;
    case EISDIR		: s = "EISDIR"; break;
    case EINVAL		: s = "EINVAL"; break;
    case ENFILE		: s = "ENFILE"; break;
    case EMFILE		: s = "EMFILE"; break;
    case ENOTTY		: s = "ENOTTY"; break;
    case ETXTBSY	: s = "ETXTBSY";break;
    case EFBIG		: s = "EFBIG"; 	break;
    case ENOSPC		: s = "ENOSPC"; break;
    case ESPIPE		: s = "ESPIPE"; break;
    case EROFS		: s = "EROFS"; 	break;
    case EMLINK		: s = "EMLINK"; break;
    case EPIPE		: s = "EPIPE"; 	break;
    case ENOMSG		: s = "ENOMSG"; break;
    case EIDRM		: s = "EIDRM"; 	break;
    case EDEADLK	: s = "EDEADLK";break;
    case ENOLCK		: s = "ENOLCK"; break;
#ifndef _AIX
    case ENOTEMPTY	: s = "ENOTEMPTY"; break;
#endif
    case ENAMETOOLONG	: s = "ENAMETOOLONG"; break;
    case ENOSYS		: s = "ENOSYS"; break;
    default             : s = NULL;     break;
    }
    
    if (s)
	sprintf(buff, "%s (%d)",s,errn);
    else
	sprintf(buff, "(%d)",errn);
    
    return (buff);
}


/*
 * This function calls the CDE message logging service.
 */
static void 
log_message(
        char *progName,
        char *help,
        char *message,
        DtSeverity severity,
        int errnoset )
{
   char			* errmsg = NULL;
   char			* errname = NULL;
   char			format[25];
   DtMsgLogType 	msg_type;
   char                 buff[40];
   
   (void) strcpy (format, "%s");

   if (help)
      (void) strcat (format, "\n  %s");

   if (errnoset) {
      unsigned int	errn;

      if (errnoset == TRUE)		/* Use "errno" from <errno.h> ? */
	 errn = errno;			/* --- yep.			*/
      else
	 errn = errnoset;		/* No, not the magic value, use parm */

      errname = errno_string(errn, buff);

      if (!(errmsg = strerror(errn)))
 	 errmsg = "unknown";
   } 

   /*
    * Must map the old message types to the new
    */
   switch (severity) {
      case DtError:
	   DtFatalError:
	   DtInternalError: 
		msg_type = DtMsgLogError; break;
      case DtIgnore:
	   DtInformation:
		msg_type = DtMsgLogInformation; break;
      case DtWarning:
		msg_type = DtMsgLogWarning; break;
      default: 
		msg_type = DtMsgLogError;
   }

   if (errmsg)
      (void) strcat (format, "\n  [%s] %s");

   if (help) {
      if (errmsg)
	 DtMsgLogMessage (progName, msg_type, format, message, help, 
			  errname, errmsg);
      else
	 DtMsgLogMessage (progName, msg_type, format, message, help);
   } else {
      if (errmsg)
	 DtMsgLogMessage (progName, msg_type, format, message, errname, errmsg);
      else
         DtMsgLogMessage (progName, msg_type, format, message);
   }
}


void 
_DtSimpleError(
        char *progName,
        DtSeverity severity,
        char *help,
        char *format,
        ... )
{
   va_list         args;
   char            *message = (char*) malloc(MESSAGE_BUFFER);

   if (NULL == message) return;

   Va_start(args, format);
   (void) vsnprintf(message, MESSAGE_BUFFER, format, args);
   va_end(args);

   log_message(progName, help, message, severity, FALSE);
   if (message) free(message);
}

void 
_DtSimpleErrnoError(
        char *progName,
        DtSeverity severity,
        char *help,
        char *format,
        ... )
{
   va_list         args;
   char            *message = (char*) malloc(MESSAGE_BUFFER);

   if (NULL == message) return;

   Va_start(args, format);
   (void) vsnprintf(message, MESSAGE_BUFFER, format, args);
   va_end(args);

   log_message(progName, help, message, severity, TRUE);
   if (message) free(message);
}
