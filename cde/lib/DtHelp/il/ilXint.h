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
/* $XConsortium: ilXint.h /main/3 1995/10/23 15:41:15 rswiston $ */
#ifndef ILXINT_H
#define ILXINT_H

     /* PRIVATE definitions shared between /ilc/ilX.c and /ilc/ilXycbcr.c .
        NO OTHER CODE SHOULD INCLUDE THIS FILE !
     */

        /*  Called by ilConvertForXWrite() to do fast dithered display of YCbCr.
            The pipe image must be planar, subsampled by 2 in Cb/Cr, 1 in Y.
            "pMapPixels" points to 256 bytes from the XWC map image, which are folded
            into the lookup table used for this function.
        */
IL_EXTERN ilBool _ilFastYCbCrDither (
    ilPipe                  pipe,
    ilImageDes             *pDes,
    ilPipeInfo             *pInfo,
    int                     nBitsRed,
    int                     nBitsGreen,
    int                     nBitsBlue,
    ilBool                  doDouble,
    ilPtr                   pMapPixels
    );

#endif
