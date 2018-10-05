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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include <X11/X.h>
#include "TermP.h"
#include "TermColor.h"
#include "TermBuffer.h"

#define	ourColorPair	(values[(int) enhFgColor])
#define	ourBgColor	(values[(int) enhBgColor])
#define	ourFont		(values[(int) enhFont])
#define	ourVideo	(values[(int) enhVideo])

void
_DtTermEnhProc(Widget w, enhValues values, TermEnhInfo info)
{

    DtTermWidget tw = (DtTermWidget) w;
    DtTermData td = tw->vt.td;

    /* initialize the color pair if we need to... */
    if (!td->colorPairs[ourColorPair].initialized) {
	(void) _DtTermColorInitializeColorPair(w,
		&td->colorPairs[ourColorPair]);
    }
    if (!td->colorPairs[ourBgColor].initialized) {
	(void) _DtTermColorInitializeColorPair(w,
		&td->colorPairs[ourBgColor]);
    }

    /* take care of video enhancements...
     */
    /* half bright (picks fg color) ... */
    if (IS_HALF_BRIGHT(ourVideo) && td->colorPairs[ourColorPair].hbValid) {
	info->fg = td->colorPairs[ourColorPair].hb.pixel;
    } else {
	info->fg = td->colorPairs[ourColorPair].fg.pixel;
    }

    /* background is always background... */
    info->bg = td->colorPairs[ourBgColor].bg.pixel;

    /* if inverse video, swap fg and bg... */
    if (IS_INVERSE(ourVideo)) {
	Pixel tmp;

	tmp = info->fg;
	info->fg = info->bg;
	info->bg = tmp;
    }

    info->flags = (unsigned long) 0;
    if (IS_SECURE(ourVideo)) {
	info->flags |= TermENH_SECURE;
    }
    if (IS_UNDERLINE(ourVideo)) {
	info->flags |= TermENH_UNDERLINE;
    }

    info->font = td->renderFonts[RENDER_FONT_NORMAL].termFont;
    if (IS_BOLD(ourVideo)) {
	if (ourFont == FONT_NORMAL) {
	    if (td->renderFonts[RENDER_FONT_BOLD].termFont) {
		/* valid bold font -- use it... */
		info->font = td->renderFonts[RENDER_FONT_BOLD].termFont;
	    } else {
		/* embolden via overstrike... */
		info->flags |= TermENH_OVERSTRIKE;
	    }
	} else {
	    /* DKS: can't do bold with the linedraw font... */
	    info->font = td->renderFonts[RENDER_FONT_LINEDRAW].termFont;
	}
    } else {
	/* not bold -- default is FONT_NORMAL... */
	if (ourFont == FONT_LINEDRAW) {
	    info->font = td->renderFonts[RENDER_FONT_LINEDRAW].termFont;
	}
    }
		
    return;
}
