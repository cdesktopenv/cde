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
/* $XConsortium: syncx.h /main/2 1996/06/25 20:04:52 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include "_falutil.h"

extern XrmQuark falrmStringToQuark(
    const char* 	/* string */
);
extern XrmString falrmQuarkToString(
    XrmQuark 		/* quark */
);

extern char *falGetAtomName(
    Display*		/* display */,
    Atom		/* atom */
);

extern char **falListFonts(
    Display*		/* display */,
    const char*		/* pattern */,
    int			/* maxnames */,
    int*		/* actual_count_return */
);

extern char **falListFontsWithInfo(
    Display*		/* display */,
    const char*		/* pattern */,
    int			/* maxnames */,
    int*		/* count_return */,
    XFontStruct**	/* info_return */
);

extern XFontStruct *falLoadQueryFont(
    Display*		/* display */,
    const char*		/* name */
);

