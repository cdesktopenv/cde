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
/* $XConsortium: ActionFind.h /main/4 1995/10/26 14:59:41 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ActionFind.h
 **
 **   Project:     DT
 **
 **   Description: Public include file for the ActionFind functions.
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dt_ActionFind_h
#define _Dt_ActionFind_h

#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include <Dt/ActionP.h>
#include <Dt/DtsDb.h>
#include <Dt/DtsMM.h>


#  ifdef __cplusplus
extern "C" {
#  endif

extern void _DtSortActionDb(void);
extern ActionPtr
_DtActionFindDBEntry(	ActionRequest *reqp,
			DtShmBoson actQuark );

#  ifdef __cplusplus
}
#  endif


#endif /* _Dt_ActionFind_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */

