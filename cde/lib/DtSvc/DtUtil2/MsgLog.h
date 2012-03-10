/* 
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * MsgLog.h - Public header file for the Message Logging Service
 *
 * $XConsortium: MsgLog.h /main/5 1995/07/14 13:22:57 drk $
 *
 */

#ifndef _Dt_MsgLog_h
#define _Dt_MsgLog_h

#include <stdio.h>		/* needed for FILE */

#ifdef __cplusplus
extern "C" {
#endif

# include <stdarg.h>

/*
 * Type declarations
 */
typedef enum {
	DtMsgLogInformation,
	DtMsgLogStderr,
	DtMsgLogDebug,
	DtMsgLogWarning,
	DtMsgLogError
} DtMsgLogType;

typedef void (*DtMsgLogHandler) (
	const char 		* program_name,
	DtMsgLogType		msg_type,
	const char 		* format,
	va_list			args );

/*
 * Function declarations
 */
extern void DtMsgLogMessage (
	const char 		* program_name,
	DtMsgLogType		msg_type,
	const char 		* format,
	... );

extern DtMsgLogHandler DtMsgLogSetHandler (
	DtMsgLogHandler 	handler );

extern FILE * DtMsgLogOpenFile (
	const char		* type,
	char			** filename_return);	/* MODIFIED */

#ifdef __cplusplus
}
#endif

#endif /* _Dt_MsgLog_h */
