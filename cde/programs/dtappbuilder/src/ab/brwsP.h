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
 *	$XConsortium: brwsP.h /main/3 1995/11/06 17:20:46 rswiston $
 *
 * @(#)brwsP.h	1.15 03 Oct 1994
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * brwsP.h
 * Private declarations for browser
 */
#ifndef _BRWSP_H
#define _BRWSP_H

#include <ab_private/brws.h>
#include "brws_ui.h"

extern Widget		brws_draw_area(
                            Vwr	v
                        );

void			brwsP_create_find_box(
                            ABBrowser	b
			);

void			brwsP_destroy_find_box(
			    Vwr	v
			);

void			brwsP_show_find_box(
			    Vwr	v
			);

void			brwsP_hide_find_box(
			    Vwr	v
			);

int			brwsP_select_fn(
			    VNode       vnode
			);

void			brwsP_sync_views(
			    ABBrowser	ab,
			    short	select_at_least_one
			);

void			browser_show_view_elements(
			    Viewer		*b,
			    unsigned long	mask,
			    Widget		widget,
			    char		*set_str,
			    char		*unset_str
			);

void			recompute_viewer(
			    Viewer	*v
			);

int			brwsP_node_is_collapsed(
			    VNode       vnode
			);

int			brwsP_node_is_visible(
			    VNode       vnode
			);

void			brwsP_make_drawarea_snap(
			    Vwr		v,
			    Widget	draw_area
			);

void			brwsP_collapse_selected(
			    ABBrowser	ab
			);

void			brwsP_expand_selected(
			    ABBrowser	ab
			);

void			brwsP_expand_collapsed(
			    ABBrowser	ab
			);

void			brwsP_tear_off_selected(
			    ABBrowser	ab
			);

#endif /* _BRWSP_H */
