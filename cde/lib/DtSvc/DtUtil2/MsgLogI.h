/* 
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * MsgLogI.h - Private header file for the Message Logging Service
 *
 * $XConsortium: MsgLogI.h /main/4 1995/07/14 13:23:02 drk $
 *
 */

#ifndef _Dt_MsgLogI_h
#define _Dt_MsgLogI_h

# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)

/*
 * Account for the various macros on different systems which indicate that
 * stdarg.h has been included.	Code in this file only checks for 
 * _STDARG_INCLUDED.  If a given system defines another macro that means the
 * same thing -- then define _STDARG_INCLUDED here.
 *
 *	System		Macro Indicating stdarg.h has been included
 *     --------        ---------------------------------------------
 *	HPUX		_STDARG_INCLUDED
 *	AIX		_H_STDARG
 *	SOLARIS		_STDARG_H
 */

#ifdef _H_STDARG
#define	_STDARG_INCLUDED
#endif

#ifdef  _STDARG_H
#define	_STDARG_INCLUDED
#endif

#endif /* _Dt_MsgLogI_h */
