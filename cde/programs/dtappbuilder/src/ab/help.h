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
 *	$XConsortium: help.h /main/3 1995/11/06 17:30:51 rswiston $
 *
 * @(#)help.h	1.5 14 Jul 1994
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/* 
 * File: help.h - Public header file for App Builder Help Editor functions.
 * 		  See help_ed_stubs.c for more details.
 */

#ifndef _HELP_H_
#define _HELP_H_

/* 
** External method to forcibly set the current help object retained
** privately within the Help Editor.
*/
extern void	ab_set_help_obj(ABObj obj);

/* 
** Routines to enable & disable help functionality when entering and leaving
** test mode (respectively).
*/
extern void 	help_test_enable(ABObj obj);
extern void 	help_test_disable(ABObj obj);

/* 
** Function used to provide on-item help support for App Builder, and which
** should be called by the "On Item..." item in the App Builder main Help
** menu.
*/
extern Boolean	help_do_onitem_help();

/*
** Method to bring up the Help Editor, used as a callback for the "Help Text..."
** button on various prop sheets.
*/
extern void	ab_popup_help(
    		    Widget	widget,
    		    XtPointer	client_data,
    		    XtPointer	call_data
		);

#endif /* _HELP_H_ */
