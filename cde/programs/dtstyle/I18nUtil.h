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
/* $XConsortium: I18nUtil.h /main/1 1996/03/25 00:52:10 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        I18nUtil.h
 **
 **
 **  This file contains function definitions for the corresponding .c
 **  file
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _i18nUtil_h
#define _i18nUtil_h

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>


	/* string */

#define  COMMENT_CHAR            '!'
#define  COMMENT_CHAR2           '#'

#define	is_comment_char(c)	((c) == COMMENT_CHAR || (c) == COMMENT_CHAR2)
#define	is_white(c)		((c) == ' ' || (c) == '\t' || (c) == '\n')
#define	skip_white(p)		while (is_white(*(p)))	(p)++
#define	cut_field(p)	\
	for ((p)++; *(p) && !is_white(*(p)); (p)++) ; *(p)++ = 0; skip_white(p)


#endif /* _i18nUtil_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
