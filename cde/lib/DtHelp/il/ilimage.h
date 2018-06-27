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
/* $XConsortium: ilimage.h /main/3 1995/10/23 15:49:13 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1991 Hewlett-Packard Co.
***    
***                             RESTRICTED RIGHTS LEGEND
***    Use, duplication, or disclosure by the U.S. Government is subject to
***    restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
***    Technical Data and Computer Software clause in DFARS 252.227-7013.
***                             Hewlett-Packard Company
***                             3000 Hanover Street
***                             Palo Alto, CA 94304 U.S.A.
***    Rights for non-DOD U.S. Government Departments and Agencies are as set
***    forth in FAR 52.227-19(c)(1,2).
***
***-------------------------------------------------------------------*/

#ifndef ILIMAGE_H
#define ILIMAGE_H

    /*  Defines what an image (client or internal) looks like internally.
        NOTE: this file should NOT be included by most IL code.
        In particular, pipe elements should not include this file -
        their view of images is completely determined by /ilinc/ilpipelem.h .
    */

#ifndef ILINT_H
#include "ilint.h"
#endif
#ifndef ILPIPELEM_H
#include "ilpipelem.h"
#endif

        /*  Returns the number of occurrences of "plane" for an image. 
        */
#define IL_NPLANES(_pImage) \
    (((_pImage)->format.sampleOrder == IL_SAMPLE_PIXELS) ? \
                 1 : (_pImage)->des.nSamplesPerPixel)

    /*  Definition of IL internal and client images.  Notes:

            o               object header - MUST BE FIRST.
                            Not meaningful for temporary pipe images, which are owned
                            by /ilc/ilimage.c and have an object type of IL_NULL_OBJECT.

            des             descriptor for this image.

            format          format of this image.

        Next three fields used for compressed images only:

            nStrips         # of strips in the image

            stripHeight     height in scanlines of each strip

            pStripOffsets   ptr to an array of byte offsets to each strip within the
                            compressed pixels.  The length of each strip "i" is
                            pStripOffsets[i+1] - pStripOffsets[i].  There are nStrips+1
                            elements in array so this works for the last strip.

            i               view of image to pipe elements - see /ilinc/ilpipelem.h .

    */

typedef struct {
    ilObjectRec         o;
    ilImageDes          des;
    ilImageFormat       format;
    long                nStrips;
    long                stripHeight;
    long               *pStripOffsets;
    ilImageInfo         i;
    } ilImageRec, *ilImagePtr;

        /*  Typecast the given ptr to an ilImagePtr.
        */
#define IL_IMAGE_PTR(_pObject) ((ilImagePtr)_pObject)


        /*  Creates a buffer for the pixels of image "*pImage", based on pImage->des,
            pImage->format, and pImage->height.  Set ownPixels to true.
                NOTE: handles uncompressed images only!
            Returns: zero if success, else error code.
        */
IL_EXTERN ilError _ilMallocImagePixels (
    ilImagePtr              pImage
    );

        /*  Destroy() function for client and internal image objects.
            Also used to discard pixels when writing images or loading client images.
            Free the pixels of the image only if the IL owns them.
                NOTE: handles uncompressed images only!
        */
IL_EXTERN void _ilFreeImagePixels (
    ilImagePtr              pImage
    );

    /*  For compressed images only: allocate strip offsets array (pStripOffsets)
        using the given "stripHeight", or use the default strip height if "stripHeight"
        is zero (0).  Locks image to that strip height.
    */
IL_EXTERN ilBool _ilAllocStripOffsets (
    ilImagePtr pImage,
    long                stripHeight
    );

#endif
