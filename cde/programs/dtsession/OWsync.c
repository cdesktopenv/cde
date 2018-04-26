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
/* $TOG: OWsync.c /main/4 1998/07/23 18:11:12 mgreess $ */
/*******************************************************************************
**
**  OWsync.c
**
**  Implements functions to synchronize CDE and OpenWindows resources.
**
**  Copyright 1995 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <Dt/DtP.h>

/*
 * Include the appropriate header for the ColorSet structure
 */
#if defined(dtsession)
#include "Srv.h"
#elif defined(dtstyle)
#include "ColorMain.h"
#endif

/*
 * Sync OW Color resources with CDE color object values
 */
void	
OWsyncColorResources(
	Display *	display,
	int 		monitorType, 
	ColorSet * 	colors)
{
	char 		colorRes[512];
	int		winChip, dataChip, wsChip;

	switch (monitorType) {
	case XmCO_HIGH_COLOR:
		winChip = 4; dataChip = 3; wsChip = 7;
		break;
	case XmCO_MEDIUM_COLOR:
		winChip = 1; dataChip = 3; wsChip = 1;
		break;
	case XmCO_LOW_COLOR:
	case XmCO_BLACK_WHITE:
		winChip = 1; dataChip = 1; wsChip = 1;
		break;
	default:
		return;
	}

	sprintf(colorRes,
"*background:\t#%04X%04X%04X\n\
*foreground:\t#%04X%04X%04X\n\
OpenWindows.WindowColor:\t#%04X%04X%04X\n\
OpenWindows.WindowForeground:\t#%04X%04X%04X\n\
OpenWindows.WorkspaceColor:\t#%04X%04X%04X\n\
OpenWindows.DataBackground:\t#%04X%04X%04X\n\
OpenWindows.DataForeground:\t#%04X%04X%04X\n\
Window.Color.Background:\t#%04X%04X%04X\n\
Window.Color.Foreground:\t#%04X%04X%04X\n",
	colors[winChip].bg.red, 
	colors[winChip].bg.green,
	colors[winChip].bg.blue,
	colors[winChip].fg.red,
	colors[winChip].fg.green,
	colors[winChip].fg.blue,
	colors[winChip].bg.red, 
	colors[winChip].bg.green,
	colors[winChip].bg.blue,
	colors[winChip].fg.red,
	colors[winChip].fg.green,
	colors[winChip].fg.blue,
	colors[wsChip].bg.red,
	colors[wsChip].bg.green,
	colors[wsChip].bg.blue,
	colors[dataChip].bg.red,
	colors[dataChip].bg.green,
	colors[dataChip].bg.blue,
	colors[dataChip].fg.red,
	colors[dataChip].fg.green,
	colors[dataChip].fg.blue,
	colors[dataChip].bg.red,
	colors[dataChip].bg.green,
	colors[dataChip].bg.blue,
	colors[dataChip].fg.red,
	colors[dataChip].fg.green,
	colors[dataChip].fg.blue);

#ifdef DEBUG
fprintf(stderr,"OWsyncColorResource: setting\n%s\n",colorRes);
#endif

	_DtAddToResource(display, colorRes);
}

/*
 * Sync OW locale resources with CDE's use of the LANG environment variable
 */
void
OWsyncLocaleResources(
	Display *	display)
{
	char	*localeRes;
	char	*lang;

	lang = getenv("LANG");
	if (lang == NULL || *lang == 0)
		lang = "C";

	localeRes = XtMalloc(BUFSIZ);

	sprintf(localeRes,
"OpenWindows.BasicLocale:\t%s\n\
OpenWindows.InputLang:\t%s\n\
OpenWindows.DisplayLang:\t%s\n\
OpenWindows.TimeFormat:\t%s\n\
OpenWindows.NumericFormat:\t%s\n",
		lang, lang, lang, lang, lang);

#ifdef DEBUG
fprintf(stderr,"OWsyncLocaleResource: setting\n%s\n",localeRes);
#endif

	_DtAddToResource(display, localeRes);

	XtFree(localeRes);
}
