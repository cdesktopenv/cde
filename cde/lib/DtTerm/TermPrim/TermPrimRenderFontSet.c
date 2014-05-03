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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimRenderFontSet.c /main/1 1996/04/21 19:18:53 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "TermPrimP.h"
#include "TermPrimDebug.h"
#include "TermPrimRenderP.h"
#include "TermPrimRenderFontSet.h"

typedef struct _TermFontSetRec {
    XFontSet fontSet;
    int ascent;
    int height;
    int width;
} TermFontSetRec, *TermFontSet;

static void
FontSetRenderFunction(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *string,
    int			  len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    XGCValues values;
    unsigned long valueMask;
    TermFontSet termFontSet = (TermFontSet) font->fontInfo;
    int escapement;
    XRectangle extents;
    Boolean fixExtents;

    /* set the renderGC... */
    valueMask = (unsigned long) 0;

    /* set background... */
    if (tpd->renderGC.background != bg) {
	tpd->renderGC.background = bg;
	values.background = bg;
	valueMask |= GCBackground;
    }

    /* since Xlib will be mucking with the GC's font under us, we need to
     * make sure we trash the cached value...
     */
    tpd->renderGC.fid = (Font) 0;

    escapement = (tpd->mbCurMax == 1) ?
        XmbTextExtents(termFontSet->fontSet, (char *) string, len,
		       NULL, &extents) :
        XwcTextExtents(termFontSet->fontSet, (wchar_t*) string, len,
		       NULL, &extents);

    /* Text height may be smaller than cellHeight (happens when using font sets).
       In this case we need to fill background manually and then use X*DrawString
       instead of X*DrawImageString */
    fixExtents = extents.height < tpd->cellHeight;

    if (fixExtents) {
      /* set background color as foreground if needed*/
      if (tpd->renderGC.foreground != bg) {
	tpd->renderGC.foreground = bg;
	valueMask|= GCForeground ;
	values.foreground = bg;
      }
      if (valueMask) {
	(void) XChangeGC(XtDisplay(w), tpd->renderGC.gc, valueMask, &values);
	valueMask= (unsigned long) 0;
      }
      /* paint background manually */
      (void) XFillRectangle(XtDisplay(w),
			    XtWindow(w),
			    tpd->renderGC.gc,
			    x,
			    y,
			    escapement,
			    tpd->cellHeight);
    }

    /* set the foreground... */
    if (TermIS_SECURE(flags)) {
	if (tpd->renderGC.foreground != bg) {
	    tpd->renderGC.foreground = bg;
	    values.foreground = bg;
	    valueMask |= GCForeground;
	}
    } else {
	if (tpd->renderGC.foreground != fg) {
	    tpd->renderGC.foreground = fg;
	    values.foreground = fg;
	    valueMask |= GCForeground;
	}
    }
    if (valueMask) {
	(void) XChangeGC(XtDisplay(w), tpd->renderGC.gc, valueMask,
		&values);
    }

    /* draw image string a line of text... */
    if (isDebugFSet('t', 1)) {
        /* we need to clear background after the debug draw and delay,
           so this will cause  X*DrawImageString to be always used
           in debug mode */
        fixExtents = 0;
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	/* Fill in the text area so we can see what is going to
	 * be displayed...
	 */
	(void) XFillRectangle(XtDisplay(w),
		XtWindow(w),
		tpd->renderGC.gc,
		x,
		y,
		escapement,
		extents.height);
	  (void) XSync(XtDisplay(w), False);
	  (void) shortSleep(100000);
    }


    if (tpd->mbCurMax == 1)
    {
        /* select right function, we do not want text background be drawn twice */
        (void) (fixExtents ? XmbDrawString: XmbDrawImageString)
                 (XtDisplay(w),                 /* Display		*/
		 XtWindow(w),			/* Drawable		*/
		 termFontSet->fontSet,		/* XFontSet		*/
		 tpd->renderGC.gc,		/* GC			*/
		 x,				/* x			*/
		 y + termFontSet->ascent,	/* y			*/
                 (char *)string,       		/* string		*/
		 len);				/* length		*/

        /* handle overstrike... */
        if (TermIS_OVERSTRIKE(flags)) {
	    (void) XmbDrawString(XtDisplay(w),	/* Display		*/
		     XtWindow(w),		/* Drawable		*/
		     termFontSet->fontSet,	/* XFontSet		*/
		     tpd->renderGC.gc,		/* GC			*/
		     x + 1,			/* x			*/
		     y + termFontSet->ascent,	/* y			*/
		     (char *)string,           	/* string		*/
		     len);			/* length		*/
	 }
    }
    else
    {
        /* select right function, we do not want text background be drawn twice */
        (void)  (fixExtents ? XwcDrawString: XwcDrawImageString)
	        (XtDisplay(w),	                /* Displa*/
	        XtWindow(w),			/* Drawable		*/
	        termFontSet->fontSet,		/* XFontSet		*/
                tpd->renderGC.gc,		/* GC			*/
	        x,				/* x			*/
	        y + termFontSet->ascent,	/* y			*/
	        (wchar_t *) string,		/* string		*/
	        len);				/* length		*/

        /* handle overstrike... */
        if (TermIS_OVERSTRIKE(flags)) {
	    (void) XwcDrawString(XtDisplay(w),	/* Display		*/
		    XtWindow(w),		/* Drawable		*/
		    termFontSet->fontSet,	/* XFontSet		*/
		    tpd->renderGC.gc,		/* GC			*/
		    x + 1,			/* x			*/
		    y + termFontSet->ascent,	/* y			*/
		    (wchar_t *) string,		/* string		*/
		    len);			/* length		*/
        }
    }
    /* handle the underline enhancement... */
    /* draw the underline... */
    if (TermIS_UNDERLINE(flags)) {
        XDrawLine(XtDisplay(w),             /* Display          */
                  XtWindow(w),              /* Window           */
                  tpd->renderGC.gc,         /* GC               */
                  x,                        /* X1               */
                  y + tpd->cellHeight - 1,  /* Y1               */
                  x - 1 + escapement,       /* X2               */
                  y + tpd->cellHeight - 1); /* Y2               */
    }
}

static void
FontSetDestroyFunction(
    Widget		  w,
    TermFont		  font
)
{
    (void) XtFree((char *) font->fontInfo);
    (void) XtFree((char *) font);
}

static void
FontSetExtentsFunction(
    Widget		  w,
    TermFont		  font,
    unsigned char	 *string,
    int			  len,
    int			 *widthReturn,
    int			 *heightReturn,
    int			 *ascentReturn
)
{
    TermFontSet termFontSet = (TermFontSet) font->fontInfo;

    if (widthReturn) {
	*widthReturn = len * termFontSet->width;
    }
    if (heightReturn) {
	*heightReturn = termFontSet->height;
    }
    if (ascentReturn) {
	*ascentReturn = termFontSet->ascent;
    }
    return;
}

TermFont
_DtTermPrimRenderFontSetCreate(
    Widget		  w,
    XFontSet		  fontSet
)
{
    TermFont termFont;
    TermFontSet termFontSet;
    XFontSetExtents *fontSetExtents;

    termFont = (TermFont) XtMalloc(sizeof(TermFontRec));
    termFont->renderFunction = FontSetRenderFunction;
    termFont->destroyFunction = FontSetDestroyFunction;
    termFont->extentsFunction = FontSetExtentsFunction;

    termFontSet = (TermFontSet) XtMalloc(sizeof(TermFontSetRec));
    termFontSet->fontSet = fontSet;
    fontSetExtents = XExtentsOfFontSet(fontSet);
    termFontSet->width = fontSetExtents->max_logical_extent.width;
    termFontSet->height = fontSetExtents->max_logical_extent.height;
    termFontSet->ascent = -fontSetExtents->max_logical_extent.y;
    termFont->fontInfo = (XtPointer) termFontSet;

    return(termFont);
}
