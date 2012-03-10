/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmGraphics.h /main/4 1995/11/01 11:39:06 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */


extern RList *AllocateRList (unsigned int amt);
extern void BevelDepressedRectangle (RList *prTop, RList *prBot, int x, 
				     int y, unsigned int width, 
				     unsigned int height, unsigned int top_wid,
 				     unsigned int right_wid, 
				     unsigned int bot_wid, 
				     unsigned int left_wid, 
				     unsigned int in_wid);
extern void BevelRectangle (RList *prTop, RList *prBot, int x, int y, 
			    unsigned int width, unsigned int height, 
			    unsigned int top_wid, unsigned int right_wid, 
			    unsigned int bot_wid, unsigned int left_wid);
extern void DrawStringInBox (Display *dpy, Window win, GC gc, 
			     XFontStruct *pfs, XRectangle *pbox, String str);
extern Boolean ExtendRList (RList *prl, unsigned int amt);
extern void FreeRList (RList *prl);
extern void StretcherCorner (RList *prTop, RList *prBot, int x, int y, 
			     int cnum, unsigned int swidth, 
			     unsigned int cwidth, unsigned int cheight);
extern void WmDrawString (Display *dpy, Drawable d, GC gc, int x, int y, 
			  char *string, unsigned int length);
#ifdef WSM
extern void WmDrawXmString (Display *dpy, Window w, XmFontList xmfontlist, 
			    XmString xmstring, GC gc, Position x, Position y, 
			    Dimension width, XRectangle *pbox,
			    Boolean bCenter);
#else /* WSM */
extern void WmDrawXmString (Display *dpy, Window w, XmFontList xmfontlist, 
			    XmString xmstring, GC gc, Position x, Position y, 
			    Dimension width, XRectangle *pbox);
#endif /* WSM */

extern GC WmGetGC (WmScreenData *pSD, unsigned long gc_mask, XGCValues *pGcv);


