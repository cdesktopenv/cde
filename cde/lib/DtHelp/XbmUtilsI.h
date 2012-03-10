/* $XConsortium: XbmUtilsI.h /main/2 1996/05/09 03:45:01 drk $ */
#ifndef _DtXbmUtilsI_h
#define _DtXbmUtilsI_h

/* Include files */

#include <X11/Xlib.h>
#include "GraphicsP.h"

/* Function prototypes */

/* Reads an XBM stream into a data buffer */
int _DtGrReadBitmapStreamData (
    _DtGrStream *fstream,
    unsigned int *width,                /* RETURNED */
    unsigned int *height,               /* RETURNED */
    unsigned char **data,               /* RETURNED */
    int *x_hot,                         /* RETURNED */
    int *y_hot                          /* RETURNED */
);

/* Converts an XBM stream into an X pixmap */ 
int _DtGrReadBitmapStream (
    Display *display,
    Drawable d,
    _DtGrStream *stream,
    unsigned int *width,                /* RETURNED */
    unsigned int *height,               /* RETURNED */
    Pixmap *pixmap,                     /* RETURNED */
    int *x_hot,                         /* RETURNED */
    int *y_hot                          /* RETURNED */
);

#endif /* _DtXbmUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
