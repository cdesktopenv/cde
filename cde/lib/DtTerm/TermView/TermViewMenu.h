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
/*
 * $XConsortium: TermViewMenu.h /main/1 1996/04/21 19:20:32 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */


#ifndef	_Dt_TermViewMenu_h
#define	_Dt_TermViewMenu_h

extern Widget
_DtTermViewCreatePulldownMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg			  menuArglist[],
    int			  menuArgcount
);

extern Widget
_DtTermViewCreatePopupMenu
(
    Widget		  termView,
    Widget		  parent,
    Arg			  menuArglist[],
    int			  menuArgcount
);

extern Widget
_DtTermViewCreateCascadeButton
(
    Widget		  parent,
    Widget		  subMenuId,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreatePushButton
(
    Widget		  parent,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreateToggleButton
(
    Widget		  parent,
    char		 *label,
    KeySym		  mnemonic,
    char		 *accelerator,
    char		 *acceleratorText,
    XtCallbackProc	  callback,
    XtPointer		  clientData
);

extern Widget
_DtTermViewCreateSeparator
(
    Widget		  parent,
    char		 *label
);

extern void
_HptermViewMenuToggleMenuBar
(
    Widget		  w
);

extern int
_DtTermViewGetUserFontListIndex
(
    Widget		  w
);

extern void
_DtTermViewSetUserFontListIndex
(
    Widget		  w,
    long		  i
);

#endif	/* _Dt_TermViewMenu_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
