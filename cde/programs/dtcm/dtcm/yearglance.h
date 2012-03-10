/*******************************************************************************
**
**  yearglance.h
**
**  static char sccsid[] = "@(#)yearglance.h 1.13 94/12/22 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: yearglance.h /main/4 1996/04/12 14:59:19 rswiston $
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

#ifndef _YEARGLANCE_H
#define _YEARGLANCE_H

extern int month_row_col[12][2];

#define ROW 0
#define COL 1

typedef struct year {
	Widget	form;
	Widget	label_form;
        Widget  *month_panels;
	Widget  year_label;
	int	year_shown;
} Year;

extern void	cleanup_after_yearview  P((Calendar *));
extern void	year_button		P((Widget, XtPointer, XtPointer));
extern CSA_return_code paint_year	P((Calendar*));
extern void     print_std_year_range    P((int, int));

#endif
