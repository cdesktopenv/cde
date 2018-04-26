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
