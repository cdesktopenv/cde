/* $XConsortium: blank.c /main/3 1995/11/02 16:06:17 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * blank.c - blank screen for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 31-Aug-90: Written.
 */

#include "dtscreen.h"

/*ARGSUSED*/
void
drawblank(pwin)
    perwindow  *pwin;
{
}

void
initblank(pwin)
    perwindow  *pwin;
{
    XClearWindow(dsp, pwin->w);
}
