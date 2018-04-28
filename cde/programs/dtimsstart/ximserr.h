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
/* @(#)$XConsortium: ximserr.h /main/3 1996/05/07 14:02:24 drk $ */

#ifndef	_XIMSERR_H_
#define	_XIMSERR_H_	1

#include	<errno.h>

    /* Error Codes */
#define	NO_ERROR		0
#define	ERROR_BASE		-200

typedef enum {
    NoError = 0,
    ErrSyntax = ERROR_BASE,	/* -200 */
    ErrNoHome,			/* -199 */
    ErrNoLocale,		/* -198 */
    ErrNoCDELocale,             /* -197 */
    ErrNoDisplay,		/* -196 */
    ErrDisabled,		/* -195 */
    ErrSignaled,		/* -194 */

	/* File I/O */
    ErrFileOpen,		/* -193 */
    ErrFileCreate,		/* -192 */
    ErrDirCreate,		/* -191 */
    ErrMissEntry,		/* -190 */

	/* Selection */
    ErrAnotherProg,		/* -189 */
    ErrNoSelectionFile,		/* -188 */
    ErrSaveSelection,		/* -187 */
    ErrNoSelection,		/* -186 */
    ErrNoLocaleConf,		/* -185 */
    ErrNoImsEntry,		/* -184 */
    ErrNoImsConf,		/* -183 */
    ErrNotRegistered,		/* -182 */
    ErrNoExecutable,		/* -181 */

	/* Start */
    ErrImsRunning,		/* -180 */
    ErrImsExecution,		/* -179 */
    ErrImsAborted,		/* -178 */
    ErrImsTimeout,		/* -177 */

	/* Remote */
    ErrUnknownHost,		/* -176 */
    ErrRemoteAction,		/* -175 */
    ErrRemoteData,		/* -174 */
    ErrRemoteNoIms,		/* -173 */
    ErrRemoteMissIms,		/* -172 */
    ErrNoImsstart,		/* -171 */
    ErrRemoteIms,		/* -170 */

	/* X */
    ErrOpenDpy,			/* -169 */
    ErrOpenResource,		/* -168 */

	/* Misc */
    ErrMemory,			/* -167 */

	/* Internal */
    ErrIsNone,			/* -166 */
    ErrNotRun,			/* -165 */
    ErrImsWaiting,		/* -164 */
    ErrImsWaitDone,		/* -163 */
    ErrImsConnecting,		/* -162 */
    ErrImsConnectDone,		/* -161 */
    ErrInvState,		/* -160 */
    ErrInternal,		/* -159 */

    LastErrorCode		/* -158 */
} ximsError;


    /* Global Variabls */
# ifdef	_EXTERN_DEFINE_
#define	Extern
# else
#define	Extern	extern
# endif	/* _EXTERN_DEFINE_ */
Extern ximsError	OpErrCode;
Extern void		*ximsErrArgs[3];
Extern char		errFilePath[MAXPATHLEN];
Extern char		*errFuncName;

    /* Macros for setting parameters of error messages */
#define	_setErrArg(n, arg)	ximsErrArgs[n] = (void *)(arg)
#define	setErrArg1(arg)		_setErrArg(0, arg)
#define	setErrArg2(arg)		_setErrArg(1, arg)
#define	setErrArg3(arg)		_setErrArg(2, arg)
#define	setErrFile(path)	snprintf(errFilePath, sizeof(errFilePath), "%s", (path))
#define	setErrFunc(name)	errFuncName = (name)

#undef	Extern

#endif	/* _XIMSERR_H_ */

