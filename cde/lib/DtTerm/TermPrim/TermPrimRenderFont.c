#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimRenderFont.c /main/1 1996/04/21 19:18:47 drk $";
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
#include "TermPrimRenderFont.h"

static void
FontRenderFunction(
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
    XFontStruct *fontStruct = (XFontStruct *) font->fontInfo;

    /* set the renderGC... */
    valueMask = (unsigned long) 0;

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

    /* set background... */
    if (tpd->renderGC.background != bg) {
	tpd->renderGC.background = bg;
	values.background = bg;
	valueMask |= GCBackground;
    }

    /* set the font for renderGC if necessary */
    if (tpd->renderGC.fid != fontStruct->fid) {
	tpd->renderGC.fid = fontStruct->fid;
	values.font = fontStruct->fid;
	valueMask |= GCFont;
    }

    if (valueMask) {
	(void) XChangeGC(XtDisplay(w), tpd->renderGC.gc, valueMask,
		&values);
    }

    /* draw image string a line of text... */
    if (isDebugFSet('t', 1)) {
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
		tpd->cellWidth * len,
		tpd->cellHeight);
	(void) XSync(XtDisplay(w), False);
	(void) shortSleep(100000);
    }
			
    (void) XDrawImageString(XtDisplay(w),	/* Display		*/
	    XtWindow(w),			/* Drawable		*/
	    tpd->renderGC.gc,			/* GC			*/
	    x,					/* x			*/
	    y + fontStruct->ascent,		/* y			*/
	    (char *) string,			/* string		*/
	    len);				/* length		*/

    /* handle overstrike... */
    if (TermIS_OVERSTRIKE(flags)) {
	(void) XDrawString(XtDisplay(w),	/* Display		*/
		XtWindow(w),			/* Drawable		*/
		tpd->renderGC.gc,		/* GC			*/
		x + 1,				/* x			*/
		y + fontStruct->ascent,		/* y			*/
		(char *) string,		/* string		*/
		len);				/* length		*/
    }

    /* handle the underline enhancement... */
    /* draw the underline... */
    if (TermIS_UNDERLINE(flags)) {
	XDrawLine(XtDisplay(w),			/* Display		*/
		XtWindow(w),			/* Window		*/
		tpd->renderGC.gc,		/* GC			*/
		x,				/* X1			*/
		y + tpd->cellHeight - 1,		/* Y1			*/
		x + len * tpd->cellWidth,	/* X2			*/
		y + tpd->cellHeight - 1);	/* Y2			*/
    }
}

static void
FontDestroyFunction(
    Widget		  w,
    TermFont		  font
)
{
    (void) XtFree((char *) font);
}

static void
FontExtentsFunction(
    Widget		  w,
    TermFont		  font,
    unsigned char	 *string,
    int			  len,
    int			 *widthReturn,
    int			 *heightReturn,
    int			 *ascentReturn
)
{
    XFontStruct *fontStruct = (XFontStruct *) font->fontInfo;

    if (widthReturn) {
	*widthReturn = len * fontStruct->max_bounds.width;
    }
    if (heightReturn) {
	*heightReturn = fontStruct->ascent + fontStruct->descent;
    }
    if (ascentReturn) {
	*ascentReturn = fontStruct->ascent;
    }
    return;
}

TermFont
_DtTermPrimRenderFontCreate(
    Widget		  w,
    XFontStruct		 *fontStruct
)
{
    TermFont termFont;

    termFont = (TermFont) XtMalloc(sizeof(TermFontRec));
    termFont->renderFunction = FontRenderFunction;
    termFont->destroyFunction = FontDestroyFunction;
    termFont->extentsFunction = FontExtentsFunction;
    termFont->fontInfo = (XtPointer) fontStruct;

    return(termFont);
}
