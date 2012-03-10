/*******************************************************************************
**
**  misc.h
**
**  static char sccsid[] = "@(#)misc.h 1.27 95/02/09 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: misc.h /main/4 1995/11/03 10:29:44 rswiston $
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

#ifndef _MISC_H
#define _MISC_H

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <csa.h>
#include "ansi_c.h"
#include "calendar.h"
#include "cm_tty.h"
#include "datefield.h"

/*
**  Variable "types" to be passed to the dialog_popup function (that
**  function accepts a variable argument list with these attribute, value
**  pairs).
**
**  The BUTTON_IDENT attribute can be used to identify 1 or more buttons on
**  the dialog.  The int id value will be returned if that button is
**  pressed and the char* name value will be the button label.
*/
typedef enum {
	DIALOG_TITLE = 1,	/*  Followed by char*  */
	DIALOG_TEXT,		/*  Followed by char* */
	BUTTON_IDENT,		/*  Followed by int, char*  */
	DIALOG_IMAGE,		/*  Followed by pixmap from Props_pu */
	BUTTON_HELP,		/*  Followed by char*  */
	BUTTON_INSENSITIVE	/*  Followed by int, char*  */
} Dialog_create_op;

/*
**  External function declarations
*/
extern char	*cm_mbchar		P((char*));
extern int	cm_mbstrlen		P((char*));
extern Widget	create_for_menu		P((Widget));
extern Widget	create_privacy_menu	P((Widget));
extern Widget	create_repeat_menu	P((Widget, XtCallbackProc, XtPointer));
extern Widget	create_repeat_scope_menu P((Widget, XmString, XtCallbackProc,
					    XtPointer));
extern Widget	create_start_stop_time_menu P((Widget, XmString, XtCallbackProc,
					       XtPointer, Props*, Boolean, 
						Boolean, Widget**, int*));
extern Widget	create_time_scope_menu	P((Widget, XmString, XtCallbackProc,
					   XtPointer));
#ifndef _AIX
extern int	dialog_popup		P((Widget, ...));
#endif /* _AIX */
extern void	editor_err_msg		P((Widget, char*, Validate_op, Pixmap));
extern void	backend_err_msg		P((Widget, char*, CSA_return_code, Pixmap));
extern void	get_range		P((Glance, time_t, time_t*, time_t*));
extern boolean_t query_user		P((void*));
extern void	set_message		P((Widget, char*));
extern void	setup_quit_handler	P((Widget, XtCallbackProc, caddr_t));
extern int	ds_is_double_click      P((XEvent*, XEvent*));
extern int	cm_select_text		P((Widget, Time));
extern void	set_time_submenu	P((Widget, Widget, Props*, 
					   XtCallbackProc, XtPointer, 
					   Boolean, Boolean, Widget**, int*));
extern void 	ManageChildren		P((Widget));
#endif
