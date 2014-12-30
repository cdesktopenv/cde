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
/* $TOG: dsdm.c /main/6 1997/06/18 17:27:35 samborn $ */
/*	Copyright (c) 1990, 1991, 1992, 1993 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1985, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF     	*/
/*	UNIX System Laboratories, Inc.                     	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *      Copyright (C) 1986,1992  Sun Microsystems, Inc
 *                      All rights reserved.
 *              Notice of copyright on this source code
 *              product does not indicate publication.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by
 * the U.S. Government is subject to restrictions as set forth
 * in subparagraph (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software Clause at DFARS 252.227-7013 (Oct. 1988)
 * and FAR 52.227-19 (c) (June 1987).
 *
 *      Sun Microsystems, Inc., 2550 Garcia Avenue,
 *      Mountain View, California 94043.
 *
 */

/*
 * @(#)dsdm.c	1.5 91/02/13
 * Drop Site Database Manager for drag'n'drop.
 *
 * Master algorithm:
 *
 * Start with visible region as whole screen.
 * For each top-level window, do
 * (0) flatten its interest rectangles
 * (1) intersect interest rects with the top-level window
 * (2) intersect them with the visible region
 * (3) append them to the master list
 * (4) subtract this top-level frame from the visible region
 *
 * USL changes are #ifdef'd with "oldcode" or commented with *USL* - Sam Chang
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(SVR4) || defined(SYSV)
#include <string.h>			/*USL*/
#else	/* SVR4 or SYSV */
#include <strings.h>
#endif	/* SVR4 or SYSV */

#if !defined(__STDC__) && !defined(__cplusplus) && !defined(c_plusplus) /*USL*/
#define void	char
#endif

#define XFreeDefn	char *

#include <X11/Xproto.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

/*
 * Use DPRINTF to write debugging messages.  Place all arguments in an
 * extra pair of parentheses, like so:
 *	DPRINTF(("%s: some error occurred (code=%d)\n", ProgramName, err));
 */

#ifdef    DEBUG
#define DPRINTF(args) (void) printf args
#else  /* DEBUG */
#define DPRINTF(a)
#endif /* DEBUG */

#define INTEREST_MAX 100000000L /*(1024L*1024L)*/

#define FIND_CONTINUE ((Window) 0L)
#define FIND_STOP     ((Window) 1L)

#define PROXY 1
#ifdef PROXY
extern Window GetAtomWindow();
extern Atom ATOM_MOTIF_RECEIVER_INFO;
Window proxy_win;
#endif

char *ProgramName;
Atom ATOM_DRAGDROP_DSDM;
Atom ATOM_DRAGDROP_INTEREST;
Atom ATOM_WM_STATE;
Atom ATOM_SITE_RECTS;


int (*DefaultErrorHandler)();

typedef struct _site {
    int screen;
    unsigned long site_id;
    Window window_id;
    unsigned long flags;
    Region region;
    struct _site *next;
} drop_site_t;

drop_site_t *MasterSiteList = NULL;
drop_site_t **NextSite;
int SitesFound = 0;


Bool SearchChildren();

/*
 * Region stuff.  Stolen from region.h.
 */

typedef struct _box {
    short x1, x2, y1, y2;
} BOX;

typedef struct {
    long size;
    long numRects;
    BOX *rects;
    BOX extents;
} REGION;


#define REGION_NUMRECTS(r) (((REGION *)(r))->numRects)



#ifdef DEBUG	/*USL*/
/*
 * Flash the visible region.  Useful for debugging.
 */
void
FlashRegion(Display *dpy, int s, GC gc)
{
    XEvent e;
    Bool done = False;
    Window root = RootWindow(dpy, s);

    if (GrabSuccess != XGrabPointer(dpy, root, False,
	ButtonPressMask, GrabModeAsync, GrabModeAsync,
	None, None, CurrentTime)) {
	    fputs("can't grab the pointer\n", stderr);
	    return;
    }

    do {
	XFillRectangle(dpy, root, gc, 0, 0,
		       DisplayWidth(dpy, s),
		       DisplayHeight(dpy, s));
	XFlush(dpy);

	XMaskEvent(dpy, ButtonPressMask, &e);
	if (e.xbutton.button == Button3)
	    done = True;

	XFillRectangle(dpy, root, gc, 0, 0,
		       DisplayWidth(dpy, s),
		       DisplayHeight(dpy, s));
	XFlush(dpy);
    } while (!done);
    XUngrabPointer(dpy, CurrentTime);
} /* end of FlashRegion */


void
FlashDropSites(Display *dpy)
{
    drop_site_t *site = MasterSiteList;
    XGCValues gcv;
    GC *gcs;
    int s;
    GC gc;

    gcv.function = GXinvert;
    gcv.subwindow_mode = IncludeInferiors;

    gcs = (GC *) malloc(sizeof(GC)*ScreenCount(dpy));
    for (s=0; s<ScreenCount(dpy); ++s)
	gcs[s] = XCreateGC(dpy, RootWindow(dpy, s),
			   GCFunction|GCSubwindowMode, &gcv);

    while (site != NULL) {
	printf("sid %ld   wid 0x%lx   flags 0x%lx\n",
	       site->site_id, site->window_id, site->flags);
	gc = gcs[site->screen];
	XSetRegion(dpy, gc, site->region);
	FlashRegion(dpy, site->screen, gc);
	site = site->next;
    }

    for (s=0; s<ScreenCount(dpy); ++s)
	XFreeGC(dpy, gcs[s]);
    free(gcs);
} /* end of FlashDropSites */

#endif /* DEBUG */


/*
 * Get the interest property from this window.  If a valid interest property
 * was found, a pointer to the data is returned.  This data must be freed with
 * XFree().  If no valid property is found, NULL is returned.
 */
#ifdef oldcode
void *
GetInterestProperty(Display *dpy, Window win, int *nitems)
#else
unsigned char *
GetInterestProperty(Display *dpy, Window win, unsigned long *nitems)
#endif
{
    Status s;
    Atom acttype;
#ifdef oldcode
    int actfmt, remain;
    void *data;
#else
    	int		actfmt;
	unsigned long	remain;
	unsigned char *	data;
#endif

    s = XGetWindowProperty(dpy, win, ATOM_DRAGDROP_INTEREST, 0L, INTEREST_MAX,
			   False, ATOM_DRAGDROP_INTEREST, &acttype, &actfmt,
			   nitems, &remain, &data);

    if (s != Success)
	return NULL;

    if (acttype == None)
	/* property does not exist */
	return NULL;

    if (acttype != ATOM_DRAGDROP_INTEREST) {
	fputs("dsdm: interest property has wrong type\n", stderr);
	return NULL;
    }

    if (actfmt != 32) {
	fputs("dsdm: interest property has wrong format\n", stderr);
#ifdef oldcode
	XFree(data);
#else
	XFree((XFreeDefn) data);
#endif
	return NULL;
    }

    if (remain > 0) {
	/* XXX didn't read it all, punt */
	fputs("dsdm: interest property too long\n", stderr);
#ifdef oldcode
	XFree(data);
#else
	XFree((XFreeDefn) data);
#endif
	return NULL;
    }
    return data;
} /* end of GetInterestProperty */


/*
 * Check to see if window win is a top-level window, that is, if it is a
 * viewable, InputOutput window that has a drop interest or WM_STATE property
 * on it.  If either property is found, return True.  Additionally, if True is
 * returned, psite will be set to point to the drop interest property data if 
 * that property is found, or NULL if not.  If neither property is found, all 
 * children of this window are searched.
 */
Bool
FindRecursively(Display *dpy, Window root, Window win, Window *pwin, void **psite, unsigned long *plen, int *px, int *py)
{
    XWindowAttributes attr;
    Window junk;
    Atom acttype;
    int actfmt;
    unsigned long nitems;
    unsigned long remain;
#ifdef oldcode
    void *data;
#else
	unsigned char *	data;
#endif
    Status s;
    
    if (XGetWindowAttributes(dpy, win, &attr) == 0) {
	fprintf(stderr, "%s: XGetWindowAttributes failed for window 0x%lx\n",
		ProgramName, win);
	return False;
    }

    if (attr.depth == 0 || attr.class == InputOnly ||
	    attr.map_state != IsViewable) {
	return False;
    }

    data = GetInterestProperty(dpy, win, &nitems);
    if (data != NULL) {
	if (!XTranslateCoordinates(dpy, win, root, 0, 0, px, py, &junk)) {
	    fprintf(stderr, "%s: window 0x%lx isn't on the same root!\n",
		    ProgramName, win);
#ifdef oldcode
	    XFree(data);
#else
	    XFree((XFreeDefn) data);
#endif
	    return False;
	}
	*psite = (void *) data;
	*plen = nitems;
	*pwin = win;
	DPRINTF(("%s: found top-level window 0x%lx with an interest\n",
		 ProgramName, win));
	return True;
    }
	
    s = XGetWindowProperty(dpy, win, ATOM_WM_STATE, 0L, 1,
			   False, ATOM_WM_STATE, &acttype, &actfmt,
			   &nitems, &remain, &data);

    if (s != Success)	/* couldn't find the window */
	return False;

    if (acttype == ATOM_WM_STATE) {
	/* found it! */
	DPRINTF(("%s: found top-level window 0x%lx with no interest\n",
		 ProgramName, win));
#ifdef oldcode
	XFree(data);
#else
	XFree((XFreeDefn) data);
#endif
	*psite = NULL;
	*plen = 0;

#ifdef PROXY
#define DRAGDROP_VERSION	0
#define INTEREST_RECT		0
#define DATA_LEN		11
#define MOTIF_RECEIVER_FLAG	0x80000000
	/* only do the following if the window doesnot 
	   have SUN_DND_INTEREST advertised */
	if (GetAtomWindow(dpy, win, ATOM_MOTIF_RECEIVER_INFO)) {
	    /* got a motif receiver, create a drop site */
	    CARD32 *data = (CARD32 *) malloc(sizeof(CARD32) * DATA_LEN);

	    data[0] = DRAGDROP_VERSION;
	    data[1] = 1;	/* site count */
	    data[2] = proxy_win;	/* event_window, xview needs this */
	    data[3] = win;	/* site_id */
	    data[4] = MOTIF_RECEIVER_FLAG; /*flag to indicate motif receiver */
	    data[5] = INTEREST_RECT; /* drop site type */
	    data[6] = 1;	/* drop site count */
	    data[7] = 0;	/* drop window x */
	    data[8] = 0;	/* drop window y */
	    data[9] = attr.width;	/* drop window width */
	    data[10] = attr.height;	/* drop window height */

	    *pwin = win;
	    *psite = (void *) data;
	    *plen = DATA_LEN;
	    XTranslateCoordinates(dpy, win, root, 0, 0, px, py, &junk);

	    /* XView needs this */
	    XChangeProperty(dpy,
		    win,
		    ATOM_DRAGDROP_INTEREST, /* property */
		    ATOM_DRAGDROP_INTEREST, /* type */
		    32,               /* format */
		    PropModeReplace, /* mode */
		    (unsigned char *) data, /* data */
		    DATA_LEN  /* number of elements */
		    );

	    /* dsdm needs this */
	    data[2] = win;
	}
#undef DATA_LEN
#endif
	return True;
    }

#ifdef oldcode
    return(SearchChildren(dpy, root, win, pwin, psite, plen, px, py));
#else
    return(SearchChildren(dpy, root, win, pwin, psite, plen, px, py, True));
#endif
} /* end of FindRecursively */



/*
 * Look through all the children of window win for a top-level window.
 */
Bool
#ifdef oldcode
SearchChildren(Display *dpy, Window root, Window win, Window *pwin, void **psite, unsigned long *plen, int *px, int *py)
#else
SearchChildren(Display *dpy, Window root, Window win, Window *pwin, void **psite, unsigned long *plen, int *px, int *py, Bool from_FindRec)
#endif
{
    Window junk;
    Window *children;
#ifdef oldcode
    int nchildren;
#else
    unsigned int nchildren;
#endif
    int i;

    if (XQueryTree(dpy, win, &junk, &junk, &children, &nchildren) == 0)
	return False;

    for (i=0; i<nchildren; ++i) {
	if (FindRecursively(dpy, root, children[i], pwin, psite, plen, px, py))
#ifdef oldcode
	    return True;
#else
	{
		XFree((XFreeDefn)children);
	    return True;
	}
#endif
    }
#ifndef oldcode
	if (from_FindRec == False && nchildren)
		XFree((XFreeDefn)children);
#endif
    return False;
} /* end of SearchChildren */


/*
 * Create and return a region that contains a given rectangle.
 */
Region
MakeRegionFromRect(int x, int y, unsigned int w, unsigned int h)
{
    XRectangle r;
    Region reg;

    r.x = x;
    r.y = y;
    r.width = w;
    r.height = h;
    reg = XCreateRegion();
    XUnionRectWithRegion(&r, reg, reg);
    return reg;
} /* end of MakeRegionFromRect */


/*
 * Create and return a region that contains the geometry of the window.
 * The region returned must be destroyed with XDestroyRegion().  The offset 
 * parameter indicates whether the window's geometry should be offset by its
 * (x,y) location w.r.t. its parent.  If it is false, the region's upper left 
 * corner is at (0,0).
 */
Region
GetWindowRegion(Display *dpy, Window win, Bool offset)
{
    Window wjunk;
    int x, y;
    unsigned int width, height, junk;
    Region winrgn;

#ifdef oldcode
    winrgn = XCreateRegion();
#endif
    if (0 == XGetGeometry(dpy, win, &wjunk, &x, &y, &width, &height,
			  &junk, &junk)) {
	fprintf(stderr, "%s: XGetGeometry failed on window 0x%lx\n",
		ProgramName, win);
#ifndef oldcode
	    winrgn = XCreateRegion();
#endif
	return winrgn;
    }
    return MakeRegionFromRect(offset ? x : 0, offset ? y: 0, width, height);
} /* end of GetWindowRegion */


/*
 * Subtract the area of a window from the current visible region.
 */
void
SubtractWindowFromVisibleRegion(Display *dpy, Window win, Region visrgn)
{
    Region winrgn = GetWindowRegion(dpy, win, True);
    XSubtractRegion(visrgn, winrgn, visrgn);
    XDestroyRegion(winrgn);
} /* end of SubtractWindowFromVisibleRegion */


#define DRAGDROP_VERSION	0
#define INTEREST_RECT		0
#define INTEREST_WINDOW		1

#define NEXTWORD(dest) do {						\
	    if (++cur >= datalen) {					\
		fprintf(stderr,						\
			"%s: drop interest data too short on 0x%lx\n",	\
			ProgramName, win);				\
		if (region != NULL)					\
		    XDestroyRegion(region);				\
		if (toprgn != NULL)					\
		    XDestroyRegion(toprgn);				\
		return;							\
	    }								\
	    (dest) = array[cur];					\
	} while (0)


void
ProcessInterestProperty(dpy, win, screen, data, datalen, visrgn, xoff, yoff)
#ifndef oldcode
	Display *	dpy;
#endif
    Window win;
    int screen;
    void *data;
    unsigned long datalen;
    Region visrgn;
    int xoff, yoff;
{
    unsigned long *array = (unsigned long *)data;
    int cur = 0;
    int i, j, nsites;
    Window wid;
    Window wjunk;
    Window areawin;
    unsigned long sid;
    int areatype;
    int nrects;
    unsigned long flags;
    Region region = NULL;
    Region toprgn = NULL;
    XRectangle rect;
    drop_site_t *site;
    int x, y;
    unsigned int width, height, junk, border;
#ifndef oldcode
	int	ignore;
#endif

    if (array[cur] != DRAGDROP_VERSION) {
	fprintf(stderr,
		"%s: unknown drop interest property version (%ld) on 0x%lx\n",
		ProgramName, array[cur], win);
	return;
    }

    toprgn = GetWindowRegion(dpy, win, False);

    NEXTWORD(nsites);
    for (i=0; i<nsites; ++i) {
	NEXTWORD(wid);
	NEXTWORD(sid);
	NEXTWORD(flags);
#ifdef PROXY
	if (flags & MOTIF_RECEIVER_FLAG)
	    wid = sid;    /* replace proxy window id with receiver window id */
#endif
	NEXTWORD(areatype);
	switch (areatype) {
	case INTEREST_RECT:
	    region = XCreateRegion();
	    NEXTWORD(nrects);
	    for (j=0; j<nrects; ++j) {
		NEXTWORD(rect.x);
		NEXTWORD(rect.y);
		NEXTWORD(rect.width);
		NEXTWORD(rect.height);
		XUnionRectWithRegion(&rect, region, region);
	    }
	    break;
	case INTEREST_WINDOW:
	    region = XCreateRegion();
	    NEXTWORD(nrects);
	    for (j=0; j<nrects; ++j) {
		NEXTWORD(areawin);
		/* REMIND need to make sure areawin isn't bogus */
#ifdef oldcode
		if (0 == XGetGeometry(dpy, areawin, &wjunk, &junk, &junk,
#else
		if (0 == XGetGeometry(dpy, areawin, &wjunk, &ignore, &ignore,
#endif
				      &width, &height, &border, &junk)) {
		    fprintf(stderr,
			    "%s: XGetGeometry failed on window 0x%lx\n",
			    ProgramName, win);
		    return;
		}
		(void) XTranslateCoordinates(dpy, areawin, win, 0, 0,
					     &x, &y, &wjunk);
		rect.x = x - border;
		rect.y = y - border;
		rect.width = width + border;
		rect.height = height + border;
		XUnionRectWithRegion(&rect, region, region);
	    }
	    break;
	default:
	    fprintf(stderr,
		    "%s: unknown site area type on window 0x%lx\n",
		    ProgramName, win);
	    return;
	}
	XIntersectRegion(region, toprgn, region);
	XOffsetRegion(region, xoff, yoff);
	XIntersectRegion(region, visrgn, region);
	site = (drop_site_t *) malloc(sizeof(drop_site_t));
	/* XXX check for site == NULL */
	site->screen = screen;
	site->site_id = sid;
	site->window_id = wid;
	site->flags = flags;
	site->region = region;
	site->next = NULL;
	(*NextSite) = site;
	NextSite = &site->next;
	++SitesFound;
	region = NULL;
    }
#ifndef oldcode
    XDestroyRegion(toprgn);
#endif
} /* end of ProcessInterestProperty */


/*
 * For the root window of each screen, get the list of children.  For each 
 * child, get its drop forwarding information and find the top-level window 
 * underneath that child, and get the top-level window's drop site 
 * information.  Add the top-level window's site information and the site 
 * forwarding information to the site database.
 */
void
FindDropSites(Display *dpy)
{
#ifdef oldcode
    int s, i, nchildren;
#else
    	int		s, i;
	unsigned int	nchildren;
#endif
    Window root, junk, *children, topwin;
    void *sitedata;
    Region visrgn, framergn, toprgn;
    XWindowAttributes attr;
    unsigned long datalen;
    int xoff, yoff;
    void *fwdsitedata;
    unsigned long fwdlen;
    Bool foundtoplevel;

    for (s=0; s<ScreenCount(dpy); ++s) {

	/* Find the virtual root here, if necessary. */
	root = RootWindow(dpy, s);
	visrgn = GetWindowRegion(dpy, root, False);

	if (XQueryTree(dpy, root, &junk, &junk, &children, &nchildren) == 0) {
	    fprintf(stderr, "%s: XQueryTree failed on root window 0x%lx\n",
		    ProgramName, root);
	    continue;
	}

	/*
	 * For each mapped, InputOutput, child-of-root window, look for a drop
	 * interest property.  This will be a forwarding interest property
	 * placed by the window manager.  Then, find the top-level window
	 * underneath this window and process its interest property.
	 */

	for (i=nchildren-1; i>=0; --i) {
	    if (XGetWindowAttributes(dpy, children[i], &attr) == 0) {
		fprintf(stderr,
			"%s: XGetWindowAttributes failed for window 0x%lx\n",
			ProgramName, children[i]);
		continue;
	    }

	    if (attr.depth == 0 || attr.class == InputOnly ||
		attr.map_state != IsViewable) {
		    continue;
	    }

	    fwdsitedata = GetInterestProperty(dpy, children[i], &fwdlen);

#ifdef oldcode
	    foundtoplevel = SearchChildren(dpy, root, children[i], &topwin,
					   &sitedata, &datalen, &xoff, &yoff);
#else
	    foundtoplevel = SearchChildren(dpy, root, children[i], &topwin,
				   &sitedata, &datalen, &xoff, &yoff, False);
#endif
	    if (foundtoplevel && sitedata != NULL) {
		/* we found a valid drop interest */
		ProcessInterestProperty(dpy, topwin, s, sitedata,
					datalen, visrgn, xoff, yoff);
#ifdef oldcode
		XFree(sitedata);
#else
		XFree((XFreeDefn)sitedata);
#endif
		if (fwdsitedata != NULL) {
		    framergn = MakeRegionFromRect(attr.x, attr.y,
						  attr.width, attr.height);
		    XIntersectRegion(framergn, visrgn, framergn);
		    toprgn = GetWindowRegion(dpy, topwin, False);
		    XOffsetRegion(toprgn, xoff, yoff);
		    XSubtractRegion(framergn, toprgn, framergn);
		    ProcessInterestProperty(dpy, children[i], s, fwdsitedata,
					    fwdlen, framergn, attr.x, attr.y);
		    XDestroyRegion(framergn);
		    XDestroyRegion(toprgn);
#ifdef oldcode
		    XFree(fwdsitedata);
#else
		    XFree((XFreeDefn)fwdsitedata);
#endif
		}
	    } else {
		if (fwdsitedata != NULL) {
		    ProcessInterestProperty(dpy, children[i], s, fwdsitedata,
					    fwdlen, visrgn, attr.x, attr.y);
#ifdef oldcode
		    XFree(fwdsitedata);
#else
		    XFree((XFreeDefn)fwdsitedata);
#endif
		}
	    }

	    SubtractWindowFromVisibleRegion(dpy, children[i], visrgn);
	}
	XDestroyRegion(visrgn);
#ifndef oldcode
		if (nchildren)
			XFree((XFreeDefn)children);
#endif
    }
} /* end of FindDropSites */


void
FreeDropSites(void)
{
    drop_site_t *next, *temp;

    next = MasterSiteList;
    while (next != NULL) {
	temp = next->next;
#ifdef oldcode
	free(next);
#else
	if (next->region)
		XDestroyRegion(next->region);
	XFree((XFreeDefn)next);
#endif
	next = temp;
    }
    MasterSiteList = NULL;
    SitesFound = 0;
    NextSite = &MasterSiteList;
} /* end of FreeDropSites */


/*
 * Write a property containing site rectangle information.  The format 
 * consists of zero or more blocks of 8 words, as follows:
 *	8k+0	screen number
 *	8k+1	site id
 *	8k+2	window id
 *	8k+3	x
 *	8k+4	y
 *	8k+5	width
 *	8k+6	height
 *	8k+7	flags
 */
void
WriteSiteRectList(Display *dpy, Window win, Atom prop)
{
    unsigned long *cur;
    unsigned long *array;
    drop_site_t *site;
    int numrects = 0;
    REGION *region;
    BOX *box, *last;

    site = MasterSiteList;
    while (site != NULL) {
	numrects += REGION_NUMRECTS(site->region);
	site = site->next;
    }

    /* XXX beware of malloc(0) */
#ifdef oldcode
    array = (unsigned long *) malloc(8*numrects*sizeof(int));
#else
    array = (unsigned long *) malloc(8*numrects*sizeof(unsigned long));
#endif
    cur = array;
    site = MasterSiteList;
    while (site != NULL) {
	region = (REGION *) site->region;
	box = region->rects;
	last = box + region->numRects;
	for ( ; box < last ; ++box) {
	    *cur++ = site->screen;
	    *cur++ = site->site_id;
#ifdef PROXY
	    /* if the receiver is motif then let event window be proxy win */
	    if (site->flags & MOTIF_RECEIVER_FLAG)
		*cur++ = proxy_win;
	    else
		*cur++ = site->window_id;
#else
	    *cur++ = site->window_id;
#endif
	    *cur++ = box->x1;
	    *cur++ = box->y1;
	    *cur++ = box->x2 - box->x1;
	    *cur++ = box->y2 - box->y1;
	    *cur++ = site->flags;
	}
	site = site->next;
    }

    XChangeProperty(dpy, win, prop, XA_INTEGER, 32, PropModeReplace,
#ifdef oldcode
		    (char *)array, cur - array);
    free(array);
#else
		    (unsigned char *)array, cur - array);
    XFree((XFreeDefn)array);
#endif
} /* end of WriteSiteRectList */


/*
 * Ignore BadWindow and BadDrawable errors on a variety of requests.  These
 * errors often occur if the requester window goes away after requesting the
 * site database.  REMIND: more robust error handling is called for.
 */
int
ErrorHandler(Display *dpy, XErrorEvent *error)
{
    if (    (error->error_code == BadWindow ||
	     error->error_code == BadDrawable) &&
	    (error->request_code == X_GetWindowAttributes ||
	     error->request_code == X_ChangeWindowAttributes ||
	     error->request_code == X_GetGeometry ||
	     error->request_code == X_QueryTree ||
	     error->request_code == X_ChangeProperty ||
	     error->request_code == X_GetProperty ||
	     error->request_code == X_SendEvent)) {
	DPRINTF(("ignored BadWindow error on request %d\n",
		 error->request_code));
	return 0;
    }

    fputs("dsdm: ", stderr);
    (*DefaultErrorHandler)(dpy, error);
    exit(1);
    /*NOTREACHED*/
} /* end of ErrorHandler */


int
main(int argc, char **argv)
{
    enum { XA_SUN_DRAGDROP_DSDM, XA_SUN_DRAGDROP_INTEREST,
	   XA_SUN_DRAGDROP_SITE_RECTS, XAWM_STATE, NUM_ATOMS };
    static char *atom_names[] = {
      "_SUN_DRAGDROP_DSDM", "_SUN_DRAGDROP_INTEREST",
      "_SUN_DRAGDROP_SITE_RECTS", "WM_STATE" };

    Display *dpy;
    Window selwin, win;
    XSetWindowAttributes attr;
    XEvent e;
    XEvent reply;
    int xflag = 0;
    char *slash = NULL;
    char *cp;
    char call_proxy_main;
    Atom atoms[NUM_ATOMS];

    cp = argv[0];
    while (*cp != '\0') {
	if (*cp == '/')
	    slash = cp;
	++cp;
    }
    if (slash != NULL)
	ProgramName = slash + 1;
    else
	ProgramName = argv[0];	

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	fprintf(stderr, "%s: can't open display %s\n",
		ProgramName, XDisplayName(NULL));
	exit(1);
    }

    DefaultErrorHandler = XSetErrorHandler(ErrorHandler);

    XInternAtoms(dpy, atom_names, NUM_ATOMS, False, atoms);
    ATOM_DRAGDROP_DSDM = atoms[XA_SUN_DRAGDROP_DSDM];
    ATOM_DRAGDROP_INTEREST = atoms[XA_SUN_DRAGDROP_INTEREST];
    ATOM_SITE_RECTS = atoms[XA_SUN_DRAGDROP_SITE_RECTS];
    ATOM_WM_STATE = atoms[XAWM_STATE];

    attr.event_mask = PropertyChangeMask;
    selwin = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1, 1, 0, 0,
			   InputOnly, CopyFromParent,
			   CWEventMask, &attr);

    if (argc > 1 && 0 == strcmp(argv[1], "-x"))
	xflag = 1;

    XGrabServer(dpy);
    win = XGetSelectionOwner(dpy, ATOM_DRAGDROP_DSDM);

    if (xflag) {
	if (win == None) {
	    fprintf(stderr, "%s: no DSDM is running\n", ProgramName);
	    exit(1);
	}
	/* Clear any DSDM selection to force the running DSDM to exit */
	XSetSelectionOwner(dpy, ATOM_DRAGDROP_DSDM, None, CurrentTime);
	XFlush(dpy);
	exit(0);
    }

    if (win != None) {
	fprintf(stderr,
		"%s: another DSDM is already running\n",
		ProgramName);
	exit(1);
    }

#ifdef PROXY
    ProxyInit(dpy, selwin);
#endif

    XSetSelectionOwner(dpy, ATOM_DRAGDROP_DSDM, selwin, CurrentTime);
    /* no need to get owner per ICCCM, because we have the server grabbed */
    XUngrabServer(dpy);
    XFlush(dpy);

/* An attempt to minimize contention at startup:  in the olinitrc file, DO
 * NOT start the window manager or the file manager in the background.  This
 * way, the initialization code for the two applications is run sequentially.
 */
    if (argc == 1)
        if (fork() != 0)
            exit(0); /* on parent success or failure */

    for (;;) {
	call_proxy_main = True;
	XNextEvent(dpy, &e);
	switch (e.type) {
	case SelectionRequest:
	    if (e.xselectionrequest.selection != ATOM_DRAGDROP_DSDM) {
#ifndef PROXY
		DPRINTF(("%s: got SelectionRequest on wrong selection?\n",
			 ProgramName));
#endif
		break;
	    }
	    if (e.xselectionrequest.owner != selwin) {
		fprintf(stderr, "%s: got SelectionRequest on wrong window?\n",
			ProgramName);
		break;
	    }
		
	    reply.xselection.display = e.xselectionrequest.display;
	    reply.xselection.type = SelectionNotify;
	    reply.xselection.requestor = e.xselectionrequest.requestor;
	    reply.xselection.selection = ATOM_DRAGDROP_DSDM;
	    reply.xselection.target = e.xselectionrequest.target;
	    reply.xselection.time = e.xselectionrequest.time;

	    if (e.xselectionrequest.target != ATOM_SITE_RECTS) {
		fprintf(stderr,
			"%s: got SelectionRequest for unknown target\n",
			ProgramName);
		reply.xselection.property = None;
	    } else {
		DPRINTF(("%s: got SelectionRequest event OK\n", ProgramName));
		FreeDropSites();
		FindDropSites(dpy);
		WriteSiteRectList(dpy, e.xselectionrequest.requestor,
				  e.xselectionrequest.property);
		reply.xselection.property =
		    e.xselectionrequest.property;
	    }

	    (void) XSendEvent(dpy, reply.xselection.requestor, False, 0,
			      &reply);
	    call_proxy_main = False;
	    break;
	case SelectionClear:
	    exit(0);
	}
#ifdef PROXY
	if (call_proxy_main)
		ProxyMain(dpy, &e);
#endif
    }
} /* end of main */

