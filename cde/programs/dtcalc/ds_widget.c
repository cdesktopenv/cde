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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ds_widget.c /main/5 1996/09/23 11:20:59 mustafa $ */
/*									*
 *  ds_widet.c                                                          *
 *   Contains some common functions which create some Xm widget which   *
 *   are used throughout the Desktop Calculator.                        *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "ds_widget.h"

#ifndef  LINT_CAST
#ifdef   lint
#define  LINT_CAST(arg)    (arg ? 0 : 0)
#else
#define  LINT_CAST(arg)    (arg)
#endif /*lint*/
#endif /*LINT_CAST*/

struct tW_struct *
make_textW(Widget owner, char *label)
{
  struct tW_struct *w ;
 
  w = (struct tW_struct *) LINT_CAST(calloc(1, sizeof(struct tW_struct))) ;
 
  w->manager = XtVaCreateManagedWidget("manager",
                                       xmRowColumnWidgetClass,
                                       owner,
                                       XmNorientation,  XmHORIZONTAL,
                                       XmNnumColumns,   1,
                                       NULL) ;
  w->label = XtVaCreateManagedWidget(label,
                                     xmLabelWidgetClass,
                                     w->manager,
                                     XmNalignment, XmALIGNMENT_BEGINNING,
                                     NULL) ;
  w->textfield = XtVaCreateManagedWidget("textfield",
                                         xmTextFieldWidgetClass,
                                         w->manager,
                                         XmNmaxLength, INT_MAX,
                                         NULL) ;
  return(w) ;
}

void
set_text_str(struct tW_struct *w, enum text_type ttype, char *str)
{
  XmString cstr ;

  switch (ttype)
    {
      case T_LABEL : cstr = XmStringCreateLocalized(str) ;
                     XtVaSetValues(w->label, XmNlabelString, cstr, NULL) ;
                     XmStringFree(cstr) ;
                     break ;
      case T_VALUE : XmTextFieldSetString(w->textfield, str) ;
                     break ;
      default : break ;
    }
}
