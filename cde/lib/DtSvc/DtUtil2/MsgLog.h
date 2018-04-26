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
