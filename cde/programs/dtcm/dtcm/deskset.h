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
**  deskset.h
**
**  static char sccsid[] = "@(#)deskset.h 1.6 94/12/22 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: deskset.h /main/3 1995/11/03 10:22:50 rswiston $
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

#ifndef _DESKSET_H
#define _DESKSET_H
 
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "ansi_c.h"

/*
**  Location ops for ds_position_popup().
*/
typedef enum {
        DS_POPUP_RIGHT,       /* Place popup to right of baseframe */
        DS_POPUP_LEFT,        /* Place popup to left of baseframe */
        DS_POPUP_ABOVE,       /* Place popup above baseframe */
        DS_POPUP_BELOW,       /* Place popup below baseframe */
        DS_POPUP_LOR,         /* Place popup to right or left of baseframe */
        DS_POPUP_AOB,         /* Place popup above or below baseframe */
        DS_POPUP_CENTERED     /* Center popup within baseframe */
} ds_location_op;

/*
**  Function declarations.
*/
extern int		ds_force_popup_on_screen P((Widget, int*, int*)) ;
extern void		ds_get_screen_size	P((Widget, int*, int*)) ;
extern int		ds_position_popup	P((Widget, Widget,
							ds_location_op)) ;
extern char		*ds_relname		P(());

#endif
