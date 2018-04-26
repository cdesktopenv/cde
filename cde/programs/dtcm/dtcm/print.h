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
**  print.h
**
**  static char sccsid[] = "@(#)print.h 1.2 94/06/01 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: print.h /main/8 1996/11/25 10:28:34 rswiston $
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

#ifndef _PRINT_H
#define _PRINT_H

#include "calendar.h"
#include <Dt/Print.h>

/*
 * IMPORTANT
 *
 * These indicate the type of report to be printed
 * they are used in callback for option menu in print.c
 * and stored in print data structure to record the
 * option that's selected.
 */
#define	PR_DAY_VIEW	0
#define	PR_WEEK_VIEW	1
#define	PR_MONTH_VIEW	2
#define	PR_YEAR_VIEW	3
#define	PR_APPT_LIST	4
#define	PR_TODO_LIST	5

/*
 *  Moved from ps_graphics.h 04/10/96
 */
#define PUBLIC          0 
#define SEMIPRIVATE     1 
#define PRIVATE         2 

#define PORTRAIT	TRUE
#define LANDSCAPE	FALSE

/* bit masks for checking privacy for printing */
#define PRINT_PUBLIC		0x1
#define PRINT_SEMIPRIVATE 	0x2
#define PRINT_PRIVATE		0x4

extern void post_print_dialog(Calendar *c);
extern void create_print_dialog(Calendar *c);
extern char *pd_get_printer_name(Calendar *c);
extern int pd_get_copies(Calendar *c);
extern Tick pd_get_start_date(Calendar *c);
extern Tick pd_get_end_date(Calendar *c);
extern char *pd_get_print_options(Calendar *c);
extern Boolean pd_print_to_file(Calendar *c);
extern char *pd_get_file_name(Calendar *c);
extern int pd_get_report_type(Calendar *c);
extern void print_report(Calendar *c);
extern Widget pd_get_print_shell(Calendar *c);
extern Display *pd_get_print_display(Calendar *c);
extern void pd_set_bad_alloc_error(Calendar *c, Boolean errorOn);
extern Boolean pd_get_bad_alloc_error(Calendar *c);

# ifdef GR_DEBUG
extern Boolean inDebugMode(Calendar *c);
# endif

#endif
