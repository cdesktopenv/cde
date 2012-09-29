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
/* $XConsortium: cmncbs.h /main/4 1995/11/01 16:10:49 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  cmncbs.h                                                               */
/*                                                                         */
/***************************************************************************/

#ifndef _CMNCBS_H_INCLUDED
#define _CMNCBS_H_INCLUDED

#ifndef STORAGECLASS
#ifdef  NOEXTERN
#define STORAGECLASS
#else
#define STORAGECLASS extern
#endif
#endif

/***************************************************************************/
/*                                                                         */
/*  Prototypes for functions                                               */
/*                                                                         */
/***************************************************************************/

void activateCB_open_FindSet (Widget,XtPointer,XmFileSelectionBoxCallbackStruct *);
void activateCB_edit_icon (Widget,XtPointer,XmPushButtonCallbackStruct *);
void DisplayHelpDialog (Widget, XtPointer, XtPointer);
void helpCB_general (Widget, XtPointer, XtPointer);
void createCB_IconGadget(Widget wid, Boolean bActionIcons, enum icon_size_range IconSize);



#endif /* _CMNCBS_H_INCLUDED */
