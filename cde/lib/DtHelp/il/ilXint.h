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
