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
/* $XConsortium: pyro.c /main/3 1995/11/02 16:07:59 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * pyro.c - Fireworks for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 16-Mar-91: Written. (received from David Brooks, brooks@osf.org).
 */

/* The physics of the rockets is a little bogus, but it looks OK.  Each is
 * given an initial velocity impetus.  They decelerate slightly (gravity
 * overcomes the rocket's impulse) and explode as the rocket's main fuse
 * gives out (we could add a ballistic stage, maybe).  The individual
 * stars fan out from the rocket, and they decelerate less quickly.
 * That's called bouyancy, but really it's again a visual preference.
 */

#include "dtscreen.h"
#include <math.h>
#include <stdlib.h>

#define TWOPI 6.2831853

/* Define this >1 to get small rectangles instead of points */
#ifndef STARSIZE
#define STARSIZE 2
#endif

#define SILENT 0
#define REDGLARE 1
#define BURSTINGINAIR 2

#define CLOUD 0
#define DOUBLECLOUD 1
/* Clearly other types and other fascinating visual effects could be added...*/

/* P_xxx parameters represent the reciprocal of the probability... */
#define P_IGNITE 5000		/* ...of ignition per cycle */
#define P_DOUBLECLOUD 10	/* ...of an ignition being double */
#define P_MULTI 75		/* ...of an ignition being several @ once */
#define P_FUSILLADE 250		/* ...of an ignition starting a fusillade */

#define ROCKETW 2		/* Dimensions of rocket */
#define ROCKETH 4
#define XVELFACTOR 0.0025	/* Max horizontal velocity / screen width */
#define MINYVELFACTOR 0.016	/* Min vertical velocity / screen height */
#define MAXYVELFACTOR 0.018
#define GRAVFACTOR 0.0002	/* delta v / screen height */
#define MINFUSE 50		/* range of fuse lengths for rocket */
#define MAXFUSE 100

#define FUSILFACTOR 10		/* Generate fusillade by reducing P_IGNITE */
#define FUSILLEN 100		/* Length of fusillade, in ignitions */

#define SVELFACTOR 0.1		/* Max star velocity / yvel */
#define BOUYANCY 0.2		/* Reduction in grav deceleration for stars */
#define MAXSTARS 75		/* Number of stars issued from a shell */
#define MINSTARS 50
#define MINSFUSE 50		/* Range of fuse lengths for stars */
#define MAXSFUSE 100

#define INTRAND(min,max) (random()%((max+1)-(min))+(min))
#define FLOATRAND(min,max) ((min)+(random()/MAXRAND)*((max)-(min)))

static void ignite();
static void animate();
static void shootup();
static void burst();

typedef struct {
    int         state;
    int         shelltype;
    int         color1, color2;
    int         fuse;
    float       xvel, yvel;
    float       x, y;
    int         nstars;
#if STARSIZE > 1
    XRectangle  Xpoints[MAXSTARS];
    XRectangle  Xpoints2[MAXSTARS];
#else
    XPoint      Xpoints[MAXSTARS];
    XPoint      Xpoints2[MAXSTARS];
#endif
    float       sx[MAXSTARS], sy[MAXSTARS];	/* Distance from notional
						 * center  */
    float       sxvel[MAXSTARS], syvel[MAXSTARS];	/* Relative to notional
							 * center */
}           rocket;

typedef struct {
    Screen     *scr;
    Colormap    cmap;
    int         p_ignite;
    unsigned long bgpixel;
    unsigned long fgpixel;
    unsigned long rockpixel;
    GC          bgGC;
    int         nflying;
    int         fusilcount;
    int         width, lmargin, rmargin, height;
    float       minvelx, maxvelx;
    float       minvely, maxvely;
    float       maxsvel;
    float       rockdecel, stardecel;
    rocket     *rockq;
}           pyrostruct;

static int  orig_p_ignite;
static int  just_started = True;/* Greet the user right away */

void
initpyro(perwindow *pwin)
{
    pyrostruct *pp;
    rocket     *rp;
    XWindowAttributes xwa;
    XGCValues   xgcv;
    int         rockn, starn, bsize;

    if (pwin->data) free(pwin->data);
    pwin->data = (pyrostruct *)malloc(sizeof(pyrostruct));
    memset(pwin->data, '\0', sizeof(pyrostruct));
    pp = (pyrostruct *)pwin->data;
    XGetWindowAttributes(dsp, pwin->w, &xwa);

    orig_p_ignite = P_IGNITE / batchcount;
    if (orig_p_ignite <= 0)
	orig_p_ignite = 1;
    pp->p_ignite = orig_p_ignite;

    pp->rockq = (rocket *) malloc(batchcount * sizeof(rocket));

    pp->nflying = pp->fusilcount = 0;

    bsize = (xwa.height <= 64) ? 1 : STARSIZE;
    for (rockn = 0, rp = pp->rockq; rockn < batchcount; rockn++, rp++) {
	rp->state = SILENT;
#if STARSIZE > 1
	for (starn = 0; starn < MAXSTARS; starn++) {
	    rp->Xpoints[starn].width = rp->Xpoints[starn].height =
		rp->Xpoints2[starn].width = rp->Xpoints2[starn].height = bsize;
	}
#endif
    }

    pp->width = xwa.width;
    pp->lmargin = xwa.width / 16;
    pp->rmargin = xwa.width - pp->lmargin;
    pp->height = xwa.height;
    pp->scr = pwin->perscreen->screen;
    pp->cmap = DefaultColormapOfScreen(pp->scr);

    pp->fgpixel = WhitePixelOfScreen(pp->scr);
    pp->bgpixel = BlackPixelOfScreen(pp->scr);
    if (!mono && pwin->perscreen->npixels > 3)
	pp->rockpixel = pwin->perscreen->pixels[3];/* Just the right shade of
						 * orange */
    else
	pp->rockpixel = pp->fgpixel;

    xgcv.foreground = pp->bgpixel;
    pp->bgGC = XCreateGC(dsp, pwin->w, GCForeground, &xgcv);
    
/* Geometry-dependent physical data: */
    pp->maxvelx = (float) (xwa.width) * XVELFACTOR;
    pp->minvelx = -pp->maxvelx;
    pp->minvely = -(float) (xwa.height) * MINYVELFACTOR;
    pp->maxvely = -(float) (xwa.height) * MAXYVELFACTOR;
    pp->maxsvel = pp->minvely * SVELFACTOR;
    pp->rockdecel = (float) (pp->height) * GRAVFACTOR;
    pp->stardecel = pp->rockdecel * BOUYANCY;

    XFillRectangle(dsp, pwin->w, pp->bgGC, 0, 0, xwa.width, xwa.height);
}

/*ARGSUSED*/
void
drawpyro(perwindow *pwin)
{
    pyrostruct *pp;
    rocket     *rp;
    int         rockn;

    pp = (pyrostruct *)pwin->data;
    if (just_started || (random() % pp->p_ignite == 0)) {
	just_started = False;
	if (random() % P_FUSILLADE == 0) {
	    pp->p_ignite = orig_p_ignite / FUSILFACTOR;
	    pp->fusilcount = INTRAND(FUSILLEN * 9 / 10, FUSILLEN * 11 / 10);
	}
	ignite(pwin, pp);
	if (pp->fusilcount > 0) {
	    if (--pp->fusilcount == 0)
		pp->p_ignite = orig_p_ignite;
	}
    }
    for (rockn = pp->nflying, rp = pp->rockq; rockn > 0; rp++) {
	if (rp->state != SILENT) {
	    animate(pwin, pp, rp);
	    rockn--;
	}
    }
}

static void
ignite(perwindow *pwin, pyrostruct *pp)
{
    rocket     *rp;
    int         multi, shelltype, nstars, fuse, npix, pix, color1, color2;
    float       xvel, yvel, x;

    x = random() % pp->width;
    xvel = FLOATRAND(-pp->maxvelx, pp->maxvelx);
/* All this to stop too many rockets going offscreen: */
    if ((x < pp->lmargin && xvel < 0.0) || (x > pp->rmargin && xvel > 0.0))
	xvel = -xvel;
    yvel = FLOATRAND(pp->minvely, pp->maxvely);
    fuse = INTRAND(MINFUSE, MAXFUSE);
    nstars = INTRAND(MINSTARS, MAXSTARS);
    if (!mono && (npix = pwin->perscreen->npixels) > 2) {
	color1 = pwin->perscreen->pixels[pix = random() % npix];
	color2 = pwin->perscreen->pixels[(pix + (npix / 2)) % npix];
    } else {
	color1 = color2 = WhitePixelOfScreen(pwin->perscreen->screen);
    }

    multi = 1;
    if (random() % P_DOUBLECLOUD == 0)
	shelltype = DOUBLECLOUD;
    else {
	shelltype = CLOUD;
	if (random() % P_MULTI == 0)
	    multi = INTRAND(5, 15);
    }

    rp = pp->rockq;
    while (multi--) {
	if (pp->nflying >= batchcount)
	    return;
	while (rp->state != SILENT)
	    rp++;
	pp->nflying++;
	rp->shelltype = shelltype;
	rp->state = REDGLARE;
	rp->color1 = color1;
	rp->color2 = color2;
	rp->xvel = xvel;
	rp->yvel = FLOATRAND(yvel * 0.97, yvel * 1.03);
	rp->fuse = INTRAND((fuse * 90) / 100, (fuse * 110) / 100);
	rp->x = x + FLOATRAND(multi * 7.6, multi * 8.4);
	rp->y = pp->height - 1;
	rp->nstars = nstars;
    }
}

static void
animate(perwindow *pwin, pyrostruct *pp, rocket *rp)
{
    int         starn;
    float       r, theta;

    if (rp->state == REDGLARE) {
	shootup(pwin, pp, rp);

/* Handle setup for explosion */
	if (rp->state == BURSTINGINAIR) {
	    for (starn = 0; starn < rp->nstars; starn++) {
		rp->sx[starn] = rp->sy[starn] = 0.0;
		rp->Xpoints[starn].x = (int) rp->x;
		rp->Xpoints[starn].y = (int) rp->y;
		if (rp->shelltype == DOUBLECLOUD) {
		    rp->Xpoints2[starn].x = (int) rp->x;
		    rp->Xpoints2[starn].y = (int) rp->y;
		}
/* This isn't accurate solid geometry, but it looks OK. */

		r = FLOATRAND(0.0, pp->maxsvel);
		theta = FLOATRAND(0.0, TWOPI);
		rp->sxvel[starn] = r * cos(theta);
		rp->syvel[starn] = r * sin(theta);
	    }
	    rp->fuse = INTRAND(MINSFUSE, MAXSFUSE);
	}
    }
    if (rp->state == BURSTINGINAIR) {
	burst(pwin, pp, rp);
    }
}

static void
shootup(perwindow *pwin, pyrostruct *pp, rocket *rp)
{
    XFillRectangle(dsp, pwin->w, pp->bgGC, (int) (rp->x), (int) (rp->y),
		   ROCKETW, ROCKETH + 3);

    if (rp->fuse-- <= 0) {
	rp->state = BURSTINGINAIR;
	return;
    }
    rp->x += rp->xvel;
    rp->y += rp->yvel;
    rp->yvel += pp->rockdecel;
    XSetForeground(dsp, pwin->gc, pp->rockpixel);
    XFillRectangle(dsp, pwin->w, pwin->gc, (int) (rp->x), (int) (rp->y),
		   ROCKETW, ROCKETH + random() % 4);
}

static void
burst(perwindow *pwin, pyrostruct *pp, rocket *rp)
{
    register int starn;
    register int nstars, stype;
    register float rx, ry, sd;	/* Help compiler optimize :-) */
    register float sx, sy;

    nstars = rp->nstars;
    stype = rp->shelltype;

#if STARSIZE > 1
    XFillRectangles(dsp, pwin->w, pp->bgGC, rp->Xpoints, nstars);
    if (stype == DOUBLECLOUD)
	XFillRectangles(dsp, pwin->w, pp->bgGC, rp->Xpoints2, nstars);
#else
    XDrawPoints(dsp, pwin->w, pp->bgGC, rp->Xpoints, nstars, CoordModeOrigin);
    if (stype == DOUBLECLOUD)
	XDrawPoints(dsp, pwin->w, pp->bgGC, rp->Xpoints2, nstars, CoordModeOrigin);
#endif

    if (rp->fuse-- <= 0) {
	rp->state = SILENT;
	pp->nflying--;
	return;
    }
/* Stagger the stars' decay */
    if (rp->fuse <= 7) {
	if ((rp->nstars = nstars = nstars * 90 / 100) == 0)
	    return;
    }
    rx = rp->x;
    ry = rp->y;
    sd = pp->stardecel;
    for (starn = 0; starn < nstars; starn++) {
	sx = rp->sx[starn] += rp->sxvel[starn];
	sy = rp->sy[starn] += rp->syvel[starn];
	rp->syvel[starn] += sd;
	rp->Xpoints[starn].x = (int) (rx + sx);
	rp->Xpoints[starn].y = (int) (ry + sy);
	if (stype == DOUBLECLOUD) {
	    rp->Xpoints2[starn].x = (int) (rx + 1.7 * sx);
	    rp->Xpoints2[starn].y = (int) (ry + 1.7 * sy);
	}
    }
    rp->x = rx + rp->xvel;
    rp->y = ry + rp->yvel;
    rp->yvel += sd;

    XSetForeground(dsp, pwin->gc, rp->color1);
#if STARSIZE > 1
    XFillRectangles(dsp, pwin->w, pwin->gc, rp->Xpoints, nstars);
    if (stype == DOUBLECLOUD) {
	XSetForeground(dsp, pwin->gc, rp->color2);
	XFillRectangles(dsp, pwin->w, pwin->gc, rp->Xpoints2, nstars);
    }
#else
    XDrawPoints(dsp, pwin->w, pwin->gc, rp->Xpoints, nstars, CoordModeOrigin);
    if (stype == DOUBLECLOUD) {
	XSetForeground(dsp, pwin->gc, rp->color2);
	XDrawPoints(dsp, pwin->w, pwin->gc, rp->Xpoints2, nstars,
		    CoordModeOrigin);
    }
#endif
}
