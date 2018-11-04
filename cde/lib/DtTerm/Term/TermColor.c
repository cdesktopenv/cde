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
/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include <X11/X.h>
#include "TermP.h"
#include "TermColor.h"
#include "TermPrimDebug.h"

#ifndef	BBA
#define InitColor(p, r, g, b)   (p)->red = r ? 0xffff : 0; \
				(p)->green = g ? 0xffff : 0; \
				(p)->blue = b ? 0xffff : 0
#else	/* BBA */
static void
InitColor
(
    XColor	 *p,
    int		  r,
    int		  g,
    int		  b
)
{
    p->red = r ? 0xffff : 0;
    p->green = g ? 0xffff : 0;
    p->blue = b ? 0xffff : 0;
}

#endif	/* BBA */

static Boolean debugColors = False;
static int debugColorsAvailable = 0;

#define	DebugIsColorAvailable()	(!debugColors || (debugColors && (debugColorsAvailable > 0)))

void
_DtTermColorInit(Widget w)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int i;

    if (isDebugFSet('C', 0)) {
#ifdef	BBA
#pragma	BBA_IGNORE
#endif	/*BBA*/
        _DtTermProcessLock();
	debugColors = True;
	if (isDebugFSet('C', 1)) {
	    debugColorsAvailable = 5;
	} else if (isDebugFSet('C', 2)) {
	    debugColorsAvailable = 6;
	} else if (isDebugFSet('C', 3)) {
	    debugColorsAvailable = 7;
	} else if (isDebugFSet('C', 4)) {
	    debugColorsAvailable = 8;
	}
        _DtTermProcessUnlock();
    }

    /* set up color pairs... */
    td->colorPairs[0].fg.pixel = tw->primitive.foreground;
    td->colorPairs[0].bg.pixel = tw->core.background_pixel;

    /* Assume that we can't free the foreground and background colors.
     * this will keep us from messing up any hidden widget stuff that
     * either depends on them (since the toolkit allocated them for us
     * to begin with), or mucks with and/or owns them (like the VUE
     * color object)...
     */
    td->colorPairs[0].fgCommon = True;
    td->colorPairs[0].bgCommon = True;
    /* initialize the color... */
    (void) _DtTermColorInitializeColorPair(w, &td->colorPairs[0]);

    /* set the default colors for colorpairs 1-7...
     */
    InitColor(&td->colorPairs[1].fg, 0, 0, 0);	/* 1: fg=black*/
    InitColor(&td->colorPairs[2].fg, 1, 0, 0);	/* 2: fg=red		*/
    InitColor(&td->colorPairs[3].fg, 0, 1, 0);	/* 2: fg=green		*/
    InitColor(&td->colorPairs[4].fg, 1, 1, 0);	/* 3: fg=yellow		*/
    InitColor(&td->colorPairs[5].fg, 0, 0, 1);	/* 4: fg=blue		*/
    InitColor(&td->colorPairs[6].fg, 1, 0, 1);	/* 5: fg=magenta	*/
    InitColor(&td->colorPairs[7].fg, 0, 1, 1);	/* 6: fg=cyan		*/
    InitColor(&td->colorPairs[8].fg, 1, 1, 1);	/* 7: fg=white		*/
    InitColor(&td->colorPairs[1].bg, 0, 0, 0);	/* 1: bg=black          */
    InitColor(&td->colorPairs[2].bg, 1, 0, 0);	/* 2: bg=red		*/
    InitColor(&td->colorPairs[3].bg, 0, 1, 0);	/* 3: bg=green		*/
    InitColor(&td->colorPairs[4].bg, 1, 1, 0);	/* 4: bg=yellow		*/
    InitColor(&td->colorPairs[5].bg, 0, 0, 1);	/* 5: bg=blue		*/
    InitColor(&td->colorPairs[6].bg, 1, 0, 1);	/* 6: bg=magenta	*/
    InitColor(&td->colorPairs[7].bg, 0, 1, 1);	/* 7: bg=cyan		*/
    InitColor(&td->colorPairs[8].bg, 1, 1, 1);	/* 8: bg=white		*/
    return;
}

void
_DtTermColorDestroy(Widget w)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    int i;
    int j;
    Pixel pixels[3];

    /* run through the color pairs and free up all the pixels that
     * we allocated.  This function will mark things as invalid/
     * uninitialized so that it will not kill things if it is
     * called more than once on destroy...
     */
    for (i = 0; i < 8; i++) {
	if (td->colorPairs[i].initialized) {
	    j = 0;
	    if (!td->colorPairs[i].fgCommon) {
		pixels[j++] = td->colorPairs[i].fg.pixel;
	    }
	    if (!td->colorPairs[i].bgCommon) {
		pixels[j++] = td->colorPairs[i].bg.pixel;
	    }
	    if (td->colorPairs[i].hbValid) {
		pixels[j++] = td->colorPairs[i].hb.pixel;
		td->colorPairs[i].hbValid = False;
	    }
	    if (j > 0) {
		(void) XFreeColors(XtDisplay(w), w->core.colormap, pixels, j,
			0);
		_DtTermProcessLock();
		debugColorsAvailable += j;
		_DtTermProcessUnlock();
	    }
	    td->colorPairs[i].initialized = False;
	}
    }
    return;
}

void
_DtTermColorInitializeColorPair(Widget w, VtColorPair colorPair)
{
    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;
    Boolean fgValid = colorPair->fgCommon;
    Boolean bgValid = colorPair->bgCommon;
    Pixel pixels[2];
    int i;

    if (colorPair->initialized) {
	/* already initialized... */
	return;
    }

    _DtTermProcessLock();
    /* initialize foreground... */
    if (!colorPair->fgCommon) {
	if (DebugIsColorAvailable() &&
		XAllocColor(XtDisplay(w), w->core.colormap, &colorPair->fg)) {
	    fgValid = True;
	    (void) debugColorsAvailable--;
	}
    }
    /* initialize background... */
    if (!colorPair->bgCommon) {
	if (DebugIsColorAvailable() &&
		XAllocColor(XtDisplay(w), w->core.colormap, &colorPair->bg)) {
	    bgValid = True;
	    (void) debugColorsAvailable--;
	}
    }

    /* did we have a failure?... */
    if (!fgValid || !bgValid) {
	/* we were unable to allocate a foreground/background pair.  Let's
	 * use the base colorpair pair[0]...
	 */
	i = 0;

	/* free up any allocated color cells... */
	if (fgValid && !colorPair->fgCommon) {
	    pixels[i++] = colorPair->fg.pixel;
	}
	if (bgValid && !colorPair->bgCommon) {
	    pixels[i++] = colorPair->bg.pixel;
	}
	if (i > 0) {
	    (void) XFreeColors(XtDisplay(w), w->core.colormap, pixels, i, 0);
	    debugColorsAvailable += i;
	}

	/* use the base color (colorPair 0)... */
	(void) memcpy(&colorPair->fg, &td->colorPairs[0].fg,
		sizeof(td->colorPairs[0].fg));
	colorPair->fgCommon = td->colorPairs[0].fgCommon;

	(void) memcpy(&colorPair->bg, &td->colorPairs[0].bg,
		sizeof(td->colorPairs[0].bg));
	colorPair->bgCommon = td->colorPairs[0].bgCommon;

	/* since this is the base pair which is always common (since it is
	 * owned either by Motif or CDE), we don't need to re-alloc the pixels
	 * to maintain the correct usage count...
	 */
    }

    /* for common colors, query the server to get the current values before
     * we generate the half bright...
     */
    if (colorPair->fgCommon) {
	(void) XQueryColor(XtDisplay(w), w->core.colormap, &colorPair->fg);
    }
    if (colorPair->bgCommon) {
	(void) XQueryColor(XtDisplay(w), w->core.colormap, &colorPair->bg);
    }
    /* make the "half bright" 3/4 the intensity of the foreground color... */
    colorPair->hb.red = ((int) colorPair->fg.red) * 3 / 4;
    colorPair->hb.green = ((int) colorPair->fg.green) * 3 / 4;
    colorPair->hb.blue = ((int) colorPair->fg.blue) * 3 / 4;

    /* special case out black -- make it 1/4 brighter...
     */
    if ((0 == colorPair->hb.red) &&
	    (0 == colorPair->hb.green) &&
	    (0 == colorPair->hb.blue)) {
	colorPair->hb.red = 0xffff / 4;
	colorPair->hb.green = 0xffff / 4;
	colorPair->hb.blue = 0xffff / 4;
    }
    colorPair->hb.flags = colorPair->fg.flags;

    /* allocate the halfbright color... */
    if (DebugIsColorAvailable() &&
	    XAllocColor(XtDisplay(w), w->core.colormap, &colorPair->hb)) {
	/* success... */
	colorPair->hbValid = True;
	(void) debugColorsAvailable--;
    } else {
	colorPair->hbValid = False;
    }
    _DtTermProcessUnlock();

    colorPair->initialized = True;
}
