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
/*******************************************************************************
**
**  timezone.h
**
**  static char sccsid[] = "@(#)timezone.h 1.4 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $TOG: timezone.h /main/4 1999/07/01 16:37:08 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _TIMEZONE_H
#define _TIMEZONE_H

typedef enum {mytime, customtime} Timezonetype;
typedef struct {
	Widget          	frame;
	Widget          	form;
	Widget			timezone_rc_mgr;
	Widget			mytime;
	Widget			customtime;
	Widget			gmtlabel;
	Widget			gmtcombo;
	Widget			hourlabel;
	Widget			cancelbutton;
	Widget			helpbutton;
	Widget			okbutton;
	Widget			applybutton;
	Widget			timezone_message;
	Timezonetype		timezone_type;
	Timezonetype		edit_timezone_type;
	char			mytimezone[BUFSIZ];
	char			gmttimezone[BUFSIZ];
}Timezone;

extern caddr_t	make_timezone	P((Calendar*));
extern void	refresh_timezone P((Timezone*));

#endif
