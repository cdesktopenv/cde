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
/* $XConsortium: cmnutils.h /main/5 1995/11/01 16:12:06 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  cmnutils.h                                                             */
/*                                                                         */
/***************************************************************************/

#ifndef _CMNUTILS_H_INCLUDED
#define _CMNUTILS_H_INCLUDED


#include <Xm/Xm.h>

/********************************************************************************/
/*                                                                              */
/*  Function Declarations                                                       */
/*                                                                              */
/********************************************************************************/


XmStringTable TextStringsToXmStrings (char **);
char *XmStringToText (XmString);
char **XmStringsToTextStrings (void);
int countItems (char **);
void ffree_string_array (void);
Widget findOptionMenuButtonID (void);
void free_string_array (void);
char *getLabelString (void);
Widget getOptionMenuHistory (void);
void setOptionMenuHistoryByText (void);
void setOptionMenuHistoryByWidget (void);
void show_warning_dialog (void);
Widget show_working_dialog (void);
void show_info_dialog (void);
void free_XmStringTable (XmStringTable table);
void clear_text (Widget textfield);
void clear_text_field (Widget textfield);
void delete_all_list_items (Widget list);

#endif /* _CMNUTILS_H_INCLUDED */
