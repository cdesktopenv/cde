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
/* $XConsortium: ds_popup.h /main/3 1995/11/01 12:40:17 rswiston $ */
/*                                                                      *
 *  ds_popup.h                                                          *
 *   Contains the NO_PROTO defines for functions is ds_popup.h.         *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _ds_popup_h
#define _ds_popup_h

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else  /* ! __STDC__ */
#define P(args)  ()
#endif  /* STDC */

/* Location ops for ds_position_popup(). */

enum ds_location_op {
        DS_POPUP_RIGHT,       /* Place popup to right of baseframe */
        DS_POPUP_LEFT,        /* Place popup to left of baseframe */
        DS_POPUP_ABOVE,       /* Place popup above baseframe */
        DS_POPUP_BELOW,       /* Place popup below baseframe */
        DS_POPUP_LOR,         /* Place popup to right or left of baseframe */
        DS_POPUP_AOB,         /* Place popup above or below baseframe */
        DS_POPUP_CENTERED     /* Center popup within baseframe */
} ;

int ds_position_popup         P((Widget, Widget, enum ds_location_op)) ;
int ds_force_popup_on_screen  P((Widget, int *, int *)) ;

void ds_get_screen_size       P((Widget, int *, int *)) ;

void _DtGenericMapWindow      P((Widget shell, Widget parent ));
void _DtChildPosition         P((Widget w, Widget parent, Position *newX, Position *newY));

#endif /*!_ds_popup_h*/
