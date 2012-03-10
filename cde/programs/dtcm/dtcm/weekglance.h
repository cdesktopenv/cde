/*******************************************************************************
**
**  weekglance.h
**
**  static char sccsid[] = "@(#)weekglance.h 1.14 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: weekglance.h /main/4 1996/04/12 14:58:56 rswiston $
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

#ifndef _WEEKGLANCE_H
#define _WEEKGLANCE_H

#include <csa.h>
#include "ansi_c.h"

typedef struct week {
        int     start_date;
        Dimension     canvas_w;
        Dimension     canvas_h;
        int     width;
        int     height;
        int     label_height;
        int     day_height;
        int     day_width;
        int     x;
        int     y;
        int     begin_hour;
        int     end_hour;
        int     chart_width;
        int     chart_height;
        int     chart_hour_height;
        int     chart_day_width;
        int     chart_x;
        int     chart_y;
        Cal_Font *font;
        Cal_Font *small_font;
        Cal_Font *small_bold_font;
        char    *time_array;
        int     segs_in_array;
        caddr_t current_selection;
        int	add_pixels;
	Widget	*hot_button;
} Week;

typedef enum {okay, lower, upper} Boundary;

extern void 	paint_weekview	P((Calendar*, XRectangle*));
extern void	print_week_range	P((Calendar*, Tick, Tick));
extern int	count_multi_appts	P((CSA_entry_handle*, int, Calendar*));
extern void	week_button		P((Widget, XtPointer, XtPointer));
extern void	week_event		P((XEvent*));
extern void	cleanup_after_weekview	P((Calendar*));
extern void	resize_weekview P((Calendar *, Boundary *));

#endif
