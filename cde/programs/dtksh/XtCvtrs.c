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
/* $XConsortium: XtCvtrs.c /main/3 1995/07/14 13:25:38 drk $ */
/*LINTLIBRARY*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* Conversion.c - implementations of resource type conversion procs */

#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>
#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"stdio.h"
#include	<X11/keysym.h>
#include	<X11/Xlocale.h>
#include        "msgs.h"
#include        "XtCvtrs.h"


#define	done(type, value) \
	{							\
	    if (toVal->addr != NULL) {				\
		if (toVal->size < sizeof(type)) {		\
		    toVal->size = sizeof(type);			\
		    return False;				\
		}						\
		*(type*)(toVal->addr) = (value);		\
	    }							\
	    else {						\
		static type static_val;				\
		static_val = (value);				\
		toVal->addr = (XPointer)&static_val;		\
	    }							\
	    toVal->size = sizeof(type);				\
	    return True;					\
	}


static void FreePixel(
                        XtAppContext app,
                        XrmValuePtr toVal,
                        XtPointer closure,
                        XrmValuePtr args,
                        Cardinal *num_args) ;




static XtConvertArgRec xtColorConvertArgs[] = {
    {XtWidgetBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.screen),
     sizeof(Screen *)},
    {XtWidgetBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.colormap),
     sizeof(Colormap)}
};


Boolean
DtkshCvtStringToPixel(
        Display *dpy,
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValuePtr fromVal,
        XrmValuePtr toVal,
        XtPointer *closure_ret )
{
    String	    str = (String)fromVal->addr;
    XColor	    screenColor;
    XColor	    exactColor;
    Screen	    *screen;
    Colormap	    colormap;
    Status	    status;
    char            *errmsg;

    if (*num_args != 2) 
        return False;

    screen = *((Screen **) args[0].addr);
    colormap = *((Colormap *) args[1].addr);

    if (DtCompareISOLatin1(str, XtDefaultBackground)) {
        *closure_ret = False;
        done(Pixel, WhitePixelOfScreen(screen));
    }
    if (DtCompareISOLatin1(str, XtDefaultForeground)) {
        *closure_ret = False;
        done(Pixel, BlackPixelOfScreen(screen));
    }

    status = XAllocNamedColor(DisplayOfScreen(screen), colormap,
			      (char*)str, &screenColor, &exactColor);
    if (status == 0) {
	String type;
	/* Server returns a specific error code but Xlib discards it.  Ugh */
	if (XLookupColor(DisplayOfScreen(screen), colormap, (char*)str,
			 &exactColor, &screenColor)) {
	    *closure_ret = (char*)True;
            done(Pixel, screenColor.pixel);
	}
	else {
            /* One last chance; see if it is a pixel number */
            char * p;
            Pixel pixelValue;

            pixelValue = strtoul(str, &p, 0);
            if (p == str)
            {
               char * errbuf;

	       errmsg = GETMESSAGE(1, 1, 
                        "DtkshCvtStringToPixel: The color '%s' is not defined");
               errbuf = XtMalloc(strlen(errmsg) + strlen(str) + 10);
               sprintf(errbuf, errmsg, str);
	       XtWarning(errbuf);
               XtFree(errbuf);
	       *closure_ret = False;
	       return False;
            }
            else
            {
	       *closure_ret = False;
               done(Pixel, pixelValue);
            }
	}
    } else {
	*closure_ret = (char*)True;
        done(Pixel, screenColor.pixel);
    }
}

/* ARGSUSED */
static void
FreePixel(
        XtAppContext app,
        XrmValuePtr toVal,
        XtPointer closure,
        XrmValuePtr args,
        Cardinal *num_args )
{
    Screen	    *screen;
    Colormap	    colormap;

    if (*num_args != 2)
        return;

    screen = *((Screen **) args[0].addr);
    colormap = *((Colormap *) args[1].addr);

    if (closure) {
	XFreeColors( DisplayOfScreen(screen), colormap,
		     (unsigned long*)toVal->addr, 1, (unsigned long)0
		    );
    }
}

Boolean
DtCompareISOLatin1(
        char *first,
        char *second )
{
    unsigned char *ap, *bp;

    for (ap = (unsigned char *) first, bp = (unsigned char *) second;
	 *ap && *bp; ap++, bp++) {
	unsigned char a, b;

	if ((a = *ap) != (b = *bp)) {
	    /* try lowercasing and try again */

	    if ((a >= XK_A) && (a <= XK_Z))
	      a += (XK_a - XK_A);
	    else if ((a >= XK_Agrave) && (a <= XK_Odiaeresis))
	      a += (XK_agrave - XK_Agrave);
	    else if ((a >= XK_Ooblique) && (a <= XK_Thorn))
	      a += (XK_oslash - XK_Ooblique);

	    if ((b >= XK_A) && (b <= XK_Z))
	      b += (XK_a - XK_A);
	    else if ((b >= XK_Agrave) && (b <= XK_Odiaeresis))
	      b += (XK_agrave - XK_Agrave);
	    else if ((b >= XK_Ooblique) && (b <= XK_Thorn))
	      b += (XK_oslash - XK_Ooblique);

	    if (a != b) break;
	}
    }
    return ((((int) *bp) - ((int) *ap)) == 0);
}

void
RegisterXtOverrideConverters( void )
{
        XtSetTypeConverter(XtRString, XtRPixel,
                (XtTypeConverter)DtkshCvtStringToPixel, 
                xtColorConvertArgs, XtNumber(xtColorConvertArgs),
                XtCacheNone, FreePixel);
}
