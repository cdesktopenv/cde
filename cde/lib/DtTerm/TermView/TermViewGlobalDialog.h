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
/*
 * $XConsortium: TermViewGlobalDialog.h /main/1 1996/04/21 19:20:23 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermViewGlobalDialog_h
#define	_Dt_TermViewGlobalDialog_h

extern Widget _DtTermViewCreateGlobalOptionsDialog(Widget parent);

typedef struct _lineupList {
    Widget		  left;
    Dimension		  widthLeft;
    Dimension		  marginLeft;
    Dimension		  marginRight;
    Dimension		  marginWidth;
    Dimension		  correction;
    Widget		  right;
    struct _lineupList	 *next;
} _DtTermViewLineupList;


#define	CORRECTION_LABEL	7
#define	CORRECTION_OPTION_MENU	0
#define	CORRECTION_TEXT_OFFSET	-1

/* private function declarations... */
extern _DtTermViewLineupList * _DtTermViewLineupListCreate();
extern void _DtTermViewLineupListAddItem(
    _DtTermViewLineupList *list,
    Widget		  left,
    Widget		  right,
    Dimension		  correction);
extern void _DtTermViewLineupListLineup(
    _DtTermViewLineupList *list);

extern void _DtTermViewLineupListFree(
    _DtTermViewLineupList *list);
extern Widget _DtTermViewCreatePulldown(
    Widget		  parent,
    char		 *name);
extern Widget _DtTermViewCreateOptionMenu(
    Widget		  parent,
    Widget		  submenu,
    char		 *label,
    KeySym		  mnemonic,
    Arg			  al[],
    int			  ac);
extern void _DtTermViewEqualizeHeights(
    Widget		  label,
    Widget		  text);

#endif	/* _Dt_TermViewGlobalDialog_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
