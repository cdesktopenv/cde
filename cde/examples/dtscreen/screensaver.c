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
/* $XConsortium: screensaver.c /main/3 1995/10/27 10:40:15 rswiston $ */
/************************************/
/** Sample CDE Screen Saver Client **/
/************************************/

#include <locale.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Dt/Saver.h>

#define SS_MAX_COLORS 6

typedef struct {
    Window	id;
    int		width;
    int		height;
    unsigned long ssPixels[SS_MAX_COLORS];
    Colormap	cmap;
    GC		gc;
} WinDataStruct;

int
usleep(usec)
    unsigned long usec;
{
    poll((struct poll *) 0, (size_t) 0, usec / 1000);   /* milliseconds */
    return 0;
}

int main(int argc, char *argv[])
{
Display *dpy;			/* Display connection      */
Window *winprop = NULL;		/* dtsession cover windows */
int nWindows;			/* number of windows	   */
WinDataStruct *perWindow;	/* per-window information  */
int i, j;			/* index variable	   */
XColor actual, exact;		/* color structs	   */
int colorRotate = 0;		/* color rotation counter  */

    setlocale(LC_ALL, "");

    /*******************************/
    /** open a display connection **/
    /*******************************/
    if (!(dpy = XOpenDisplay(NULL)) ) {
	fprintf(stderr, "Unable to open the Display.\n");
	exit(1);
    }

    /***********************************************************/
    /** Get the list of screen saver windows from the desktop **/
    /***********************************************************/
    if (!DtSaverGetWindows(dpy, &winprop, &nWindows)) {
	fprintf(stderr, "Unable to get screen saver info.\n");
	XCloseDisplay(dpy);
	exit(1);
    }

    /******************************************************/
    /** allocate an array to hold per window information **/
    /******************************************************/
    if ( (perWindow = (WinDataStruct *)malloc(nWindows * sizeof(WinDataStruct)))
	== NULL) {
	fprintf(stderr, "Out of memory.\n");
	XCloseDisplay(dpy);
	exit(1);
    }

    /*****************************************************/
    /** get things set up for each window we were given **/
    /*****************************************************/
    for (i = 0; i < nWindows; i++) {
	XWindowAttributes attr;

	perWindow[i].id = winprop[i];
	/*************************************/
	/** get information for each window **/
	/*************************************/
	if (! XGetWindowAttributes(dpy, perWindow[i].id, &attr)) {
	    fprintf(stderr, "Unable to get window %d attributes.\n",
                    perWindow[i].id);
            free((void *)perWindow);
            XCloseDisplay(dpy);
            exit(1);
	}

	/***************************************/
	/** save the window info we will need **/
	/***************************************/
	perWindow[i].width = attr.width;
	perWindow[i].height = attr.height;
	perWindow[i].cmap = DefaultColormapOfScreen(attr.screen);
	perWindow[i].gc = DefaultGCOfScreen(attr.screen);

	/***********************************************/
	/** Allocate the colors we will use, fallback **/
	/** to black and white if necessary           **/
	/***********************************************/
	if (XAllocNamedColor(dpy,perWindow[i].cmap,"red",&actual,&exact)){
	    perWindow[i].ssPixels[0] = actual.pixel;
	} else {
	    perWindow[i].ssPixels[0] = BlackPixelOfScreen(attr.screen);
	}
        if (XAllocNamedColor(dpy,perWindow[i].cmap,"orange",&actual,&exact)){
            perWindow[i].ssPixels[1] = actual.pixel;
        } else {
            perWindow[i].ssPixels[1] = WhitePixelOfScreen(attr.screen);
        }
        if (XAllocNamedColor(dpy,perWindow[i].cmap,"yellow",&actual,&exact)){
            perWindow[i].ssPixels[2] = actual.pixel;
        } else {
            perWindow[i].ssPixels[2] = BlackPixelOfScreen(attr.screen);
        }
	if (XAllocNamedColor(dpy,perWindow[i].cmap,"green",&actual,&exact)){
                perWindow[i].ssPixels[3] = actual.pixel;
        } else {
                perWindow[i].ssPixels[3] = WhitePixelOfScreen(attr.screen);
        }
	if (XAllocNamedColor(dpy,perWindow[i].cmap,"blue",&actual,&exact)){
                perWindow[i].ssPixels[4] = actual.pixel;
        } else {
            	perWindow[i].ssPixels[4] = BlackPixelOfScreen(attr.screen);;
        }
        if (XAllocNamedColor(dpy,perWindow[i].cmap,"purple",&actual,&exact)){
            perWindow[i].ssPixels[5] = actual.pixel;
        } else {
            perWindow[i].ssPixels[5] = WhitePixelOfScreen(attr.screen);
        }
    }

    /************************************/
    /** OK, now enter our drawing loop **/
    /************************************/
    while (1) {
	/************************/
	/** update each window **/
	/************************/
	for (i = 0; i < nWindows; i++) {
	    /***************************/
	    /** for each color we use **/
	    /***************************/
	    for (j = 0; j < SS_MAX_COLORS; j++) {
		int x, y, width, height;
		unsigned long curColor;

		curColor = perWindow[i].ssPixels[
				(int)fmod((j + colorRotate),SS_MAX_COLORS)];
		XSetBackground(dpy, perWindow[i].gc, curColor);
		XSetForeground(dpy, perWindow[i].gc, curColor);
		x = j * ((perWindow[i].width/2) / (SS_MAX_COLORS));
		y = j * ((perWindow[i].height/2) / (SS_MAX_COLORS));
		width = perWindow[i].width - (2 * x);
		height = perWindow[i].height - (2 * y);
		XFillRectangle(dpy, perWindow[i].id, perWindow[i].gc,
			       x, y, width, height);
	    }
	}
	/**XFlush(dpy); **/
	XSync(dpy, False);
	colorRotate = (int) fmod ((colorRotate + 1), SS_MAX_COLORS);

	usleep(200000);
    }
}
