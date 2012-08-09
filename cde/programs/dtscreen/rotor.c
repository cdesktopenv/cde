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
/* $XConsortium: rotor.c /main/3 1995/11/02 16:08:34 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * rotor.c - A swirly rotor for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 11-Nov-90: put into dtscreen (by Steve Zellers, zellers@sun.com)
 * 16-Oct-90: Received from Tom Lawrence (tcl@cs.brown.edu: 'flight' simulator)
 */

/*
 * A 'batchcount' of 3 or 4 works best!
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "dtscreen.h"

#define SAVE 		100	/* this is a good constant to tweak */
#define REPS  		50

#define MAXANGLE	10000.0	/* irrectangular */
#define DEFAULTCOUNT	3

typedef unsigned char Boolean;

#define IDENT(X)	X
#if defined (__STDC__) || defined (AIXV3)
#define CAT(X,Y)	X##Y
#else
#define CAT(X,Y)	IDENT(X)Y
#endif

struct elem {
    float       angle;
    float       radius;
    float       start_radius;
    float       end_radius;
    float       radius_drift_max;
    float       radius_drift_now;

    float       ratio;
    float       start_ratio;
    float       end_ratio;
    float       ratio_drift_max;
    float       ratio_drift_now;
};

typedef struct flightstruct {
    struct elem *elements;
    int         pix;
    int         lastx,
                lasty;
    int         num,
                rotor,
                prev;
    int         savex[SAVE],
                savey[SAVE];
    float       angle;
    int         centerx,
                centery;
    Boolean     firsttime;
    Boolean     smallscreen;	/* for iconified view */
    Boolean     forward;
    Boolean     unused;
}           flightstruct;


void
initrotor(pwin)
    perwindow *pwin;
{
    flightstruct *fs;
    XWindowAttributes xgwa;
    int         x;
    struct elem *pelem;
    Boolean     wassmall;

    if (pwin->data) free(pwin->data);
    pwin->data = (void *)malloc(sizeof(flightstruct));
    memset(pwin->data, 0, sizeof(flightstruct));
    fs = (flightstruct *)pwin->data;
    XGetWindowAttributes(dsp, pwin->w, &xgwa);
    fs->centerx = xgwa.width / 2;
    fs->centery = xgwa.height / 2;

    /*
     * sometimes, you go into small view, only to see a really whizzy pattern
     * that you would like to look more closely at. Normally, clicking in the
     * icon reinitializes everything - but I don't, cuz I'm that kind of guy.
     * HENCE, the wassmall stuff you see here.
     */

    wassmall = fs->smallscreen;
    fs->smallscreen = (xgwa.width < 100);

    if (wassmall && !fs->smallscreen)
	fs->firsttime = True;
    else {
	if (batchcount > 12)
	    batchcount = DEFAULTCOUNT;
	fs->num = batchcount;

        if ((fs->elements = (struct elem *)
              malloc(sizeof(struct elem) * fs->num)) == 0) {
		perror("malloc");
		exit(1);
	}
	memset(fs->savex, 0, sizeof(fs->savex));

	pelem = fs->elements;

	for (x = fs->num; --x >= 0; pelem++) {
	    pelem->radius_drift_max = 1.0;
	    pelem->radius_drift_now = 1.0;

	    pelem->end_radius = 100.0;

	    pelem->ratio_drift_max = 1.0;
	    pelem->ratio_drift_now = 1.0;
	    pelem->end_ratio = 10.0;
	}

	fs->rotor = 0;
	fs->prev = 1;
	fs->lastx = fs->centerx;
	fs->lasty = fs->centery;
	fs->angle = (random() % (long) MAXANGLE) / 3;
	fs->forward = fs->firsttime = True;
    }
    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));
    XFillRectangle(dsp, pwin->w, pwin->gc, 0, 0, xgwa.width, xgwa.height);
}

void
drawrotor(pwin)
    perwindow *pwin;
{
    register flightstruct *fs;
    register struct elem *pelem;
    int         thisx,
                thisy;
    int         i,
                rp;
    int         x1,
                y1,
                x2,
                y2;

#define SCALE(W,N)	CAT(W,N)/=12; CAT(W,N)+=(CAT(fs->center,W)-2)
#define SCALEIFSMALL()    if (fs->smallscreen) { 	\
                              SCALE(x,1); SCALE(x,2);	\
			      SCALE(y,1); SCALE(y,2);	\
			  }

    fs = (flightstruct *)pwin->data;
    for (rp = 0; rp < REPS; rp++) {
	thisx = fs->centerx;
	thisy = fs->centery;

	for (i = fs->num, pelem = fs->elements; --i >= 0; pelem++) {
	    if (pelem->radius_drift_max <= pelem->radius_drift_now) {
		pelem->start_radius = pelem->end_radius;
		pelem->end_radius =
		    (float) (random() % 40000) / 100.0 - 200.0;
		pelem->radius_drift_max =
		    (float) (random() % 100000) + 10000.0;
		pelem->radius_drift_now = 0.0;
	    }
	    if (pelem->ratio_drift_max <= pelem->ratio_drift_now) {
		pelem->start_ratio = pelem->end_ratio;
		pelem->end_ratio =
		    (float) (random() % 2000) / 100.0 - 10.0;
		pelem->ratio_drift_max =
		    (float) (random() % 100000) + 10000.0;
		pelem->ratio_drift_now = 0.0;
	    }
	    pelem->ratio = pelem->start_ratio +
		(pelem->end_ratio - pelem->start_ratio) /
		pelem->ratio_drift_max * pelem->ratio_drift_now;
	    pelem->angle = fs->angle * pelem->ratio;
	    pelem->radius = pelem->start_radius +
		(pelem->end_radius - pelem->start_radius) /
		pelem->radius_drift_max * pelem->radius_drift_now;

	    thisx += (int) (cos(pelem->angle) * pelem->radius);
	    thisy += (int) (sin(pelem->angle) * pelem->radius);

	    pelem->ratio_drift_now += 1.0;
	    pelem->radius_drift_now += 1.0;
	}

	if (fs->firsttime)
	    fs->firsttime = False;
	else {
	    XSetForeground(dsp, pwin->gc, BlackPixelOfScreen(pwin->perscreen->screen));

	    x1 = (int) fs->savex[fs->rotor];
	    y1 = (int) fs->savey[fs->rotor];
	    x2 = (int) fs->savex[fs->prev];
	    y2 = (int) fs->savey[fs->prev];

	    SCALEIFSMALL()

	    XDrawLine(dsp, pwin->w, pwin->gc, x1, y1, x2, y2);

	    if (!mono && pwin->perscreen->npixels > 2) {
		XSetForeground(dsp, pwin->gc,
			       pwin->perscreen->pixels[fs->pix]);
		if (++fs->pix >= pwin->perscreen->npixels)
		    fs->pix = 0;
	    } else
		XSetForeground(dsp, pwin->gc, 
                  WhitePixelOfScreen(pwin->perscreen->screen));

	    x1 = fs->lastx;
	    y1 = fs->lasty;
	    x2 = thisx;
	    y2 = thisy;

	    SCALEIFSMALL()

	    XDrawLine(dsp, pwin->w, pwin->gc, x1, y1, x2, y2);
	}
	fs->savex[fs->rotor] = fs->lastx = thisx;
	fs->savey[fs->rotor] = fs->lasty = thisy;

	++fs->rotor;
	fs->rotor %= SAVE;
	++fs->prev;
	fs->prev %= SAVE;
	if (fs->forward) {
	    fs->angle += 0.01;
	    if (fs->angle >= MAXANGLE) {
		fs->angle = MAXANGLE;
		fs->forward = False;
	    }
	} else {
	    fs->angle -= 0.1;
	    if (fs->angle <= 0) {
		fs->angle = 0.0;
		fs->forward = True;
	    }
	}
    }
}
