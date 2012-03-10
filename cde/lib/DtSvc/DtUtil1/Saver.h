/* $XConsortium: Saver.h /main/3 1995/10/26 15:10:29 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

#ifndef _dtsaver_h
#define _dtsaver_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Functions
 */

extern Boolean DtSaverGetWindows(
                 Display *display,
                 Window **window,
                 int *count);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /*_dtsaver_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
