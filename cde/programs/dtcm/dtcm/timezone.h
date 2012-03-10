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
