/*******************************************************************************
**
**  monthglance.h
**
**  static char sccsid[] = "@(#)yearglance.h 1.2 93/08/10 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: monthglance.h /main/4 1996/04/12 14:58:21 rswiston $
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

#ifndef _MONTHGLANCE_H
#define _MONTHGLANCE_H

#include <csa.h>
#include "ansi_c.h"

typedef struct month {
	Widget	*hot_button;
	Widget	month_label;
	XPoint	*button_loc;
	int	ndays;
	int	dayname_height;
} Month;

extern void 	prepare_to_paint_monthview	P((Calendar*, XRectangle*));
extern void	get_time_str		P((Dtcm_appointment*, char*));
extern void	month_button		P((Widget, XtPointer, XtPointer));
extern void	month_event		P((XEvent*));
extern void	paint_grid		P((Calendar*, XRectangle*));
extern void	print_month_range	P((Calendar*, Tick, Tick));
extern void	cleanup_after_monthview P((Calendar*));


#endif
