/* $XConsortium: JpegUtilsI.h /main/3 1996/10/06 19:37:31 rws $ */
#ifndef _DtJpegUtilsI_h
#define _DtJpegUtilsI_h

/* Include files */

#include <X11/Xlib.h>
#include "GraphicsP.h"

/* Function prototypes */

/* Creates a pixmap from a jpeg stream */
enum _DtGrLoadStatus jpeg_to_ximage (
    _DtGrStream           *stream,
    Screen                *screen,
    Visual                *visual,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    XImage                **ximage,
    XColor                **xcolors,
    int                    *ncolors,
    int                    *xres
);

#endif /* _DtJpegUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
