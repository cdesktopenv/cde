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
 * $XConsortium: TermFunctionKey.h /main/1 1996/04/21 19:15:56 drk $
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermFunctionKey_h
#define _Dt_TermFunctionKey_h

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */


/* _DtTermFunctionKeyExecute(): This action is invoked via an action when
 * a function key F1..F32 is depressed...
 */
extern void _DtTermFunctionKeyExecute(
	Widget		  w,
	short		  keyNumber,
	Boolean		  shift
);

extern void
_DtTermFunctionKeyUserKeyExecute(
    Widget                w,
    short                 keyno,
    Boolean               shift
);

extern void
_DtTermFunctionKeyStringStore(       /* store the label for the given key number
 */
     Widget w,
     int  keyno,
     char *string
);


extern void
_DtTermFunctionKeyClear(
    Widget   w
);


extern void _DtTermFunctionKeyInit(
   Widget w
);

#ifdef	__cplusplus
} /* close scope of 'extern "C"'.... */
#endif	/* __cplusplus */

/* DONT'T ADD ANYTHING AFTER THIS #endif... */
#endif	/* _Dt_TermFunctionKey_h */
