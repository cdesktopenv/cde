/*******************************************************************************
**
**  dayglance.h
**
**  static char sccsid[] = "@(#)dayglance.h 1.11 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: dayglance.h /main/5 1996/04/12 14:58:09 rswiston $
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

#ifndef _DAYGLANCE_H
#define _DAYGLANCE_H

#include <csa.h>
#include "ansi_c.h"
#include "props.h"

#define MOBOX_AREA_WIDTH	(int)(c->view->winw*.4)
#define APPT_AREA_WIDTH		(c->view->winw-MOBOX_AREA_WIDTH-2)
#define HRBOX_MARGIN		30

typedef struct dayglance {
	int mobox_height1;
	int mobox_height2;
	int mobox_height3;
	int month1; 		/* tick in 1st mo */ 
	int month1_y;
	int month2; 		/* tick in 2nd mo */
	int month2_y;
	int month3; 		/* tick in 3rd mo */
	int month3_y;
	int col_w;  		/* width of month box col */
	int row_h;  		/* height of month box row */
	int mobox_width;  	/* width of month box row */
	int day_selected;
	int day_selected_x;
	int day_selected_y;
	int day_selected_x2;
	int day_selected_y2;
	Widget panel_form;
	Widget *month_panels;
} Day;

extern void	init_dayview		P((Calendar*));
extern void	day_button		P((Widget, XtPointer, XtPointer));
extern void	init_mo			P((Calendar*));
extern void	monthbox_datetoxy	P((Calendar*));
extern void	monthbox_xytodate	P((Calendar*, int x, int y));
extern void	paint_dayview		P((Calendar*, Boolean,
					   XRectangle*, Boolean));
extern void	paint_day		P((Calendar*));
extern void	print_day_range		P((Calendar*, Tick, Tick));
extern void	day_event		P((XEvent*));
extern int	morning			P((int));
extern void 	paint_day_header	P((Calendar *, Tick, void *));
extern void 	cleanup_after_dayview	P((Calendar *));
#endif
