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
/* $XConsortium: ds_xlib.h /main/3 1995/11/01 12:40:54 rswiston $ */
/*                                                                      *
 *  ds_xlib.h                                                           *
 *   Contains the NO_PROTO defines for functions is ds_xlib.h.          *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _ds_xlib_h
#define _ds_xlib_h

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/* Function declarations. */

char *ds_get_resource(XrmDatabase, char *, char *);

int ds_get_choice_value(Widget);
int ds_get_strwidth(XFontStruct *, char *);
int ds_save_resources(XrmDatabase, char *);

void ds_add_help(Widget, char *);
void ds_beep(Display *);
void ds_get_geometry_size(char *, int *, int *);
void ds_get_frame_size(Widget, int *, int *, int *, int *);
void ds_put_resource(XrmDatabase *, char *, char *, char *);
void ds_save_cmdline(Display *, Window, int, char **);
void ds_set_choice_value(Widget, int);
void ds_set_frame_size(Widget, int, int, int, int);

XrmDatabase ds_load_resources(Display *);

#endif /*_ds_xlib_h*/
