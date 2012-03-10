/* 
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.2
*/ 
/*   $XConsortium: WmImage.h /main/4 1995/11/01 11:42:39 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern char  *BitmapPathName (char *string);
#ifdef WSM
extern int    GetBitmapIndex (WmScreenData *pSD, 
				char *name, 
				Boolean bReportError);
#else /* WSM */
extern int    GetBitmapIndex (WmScreenData *pSD, char *name);
#endif /* WSM */
extern Pixmap MakeCachedIconPixmap (ClientData *pCD, int bitmapIndex, Pixmap mask);
extern Pixmap MakeCachedLabelPixmap (WmScreenData *pSD, Widget menuW, int bitmapIndex);
extern Pixmap MakeClientIconPixmap (ClientData *pCD, Pixmap iconBitmap, Pixmap iconMask);
extern Pixmap MakeIconPixmap (ClientData *pCD, Pixmap bitmap, Pixmap mask, unsigned int width, unsigned int height, unsigned int depth);
extern Pixmap MakeNamedIconPixmap (ClientData *pCD, String iconName);
