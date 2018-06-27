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
/* $XConsortium: ilreadimage.c /main/3 1995/10/23 15:59:31 rswiston $ */
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

#include "ilint.h"
#include "ilpipelem.h"
#include "ilpipeint.h"
#include "ilimage.h"
#include "ilerrors.h"


        /*  Private structure for ilRCIExecute(). (RCI = "Read Compressed Image") */
typedef struct {
    ilImagePtr          pImage;             /* ptr to src image */
    long                strip;              /* Init(): index of next strip to write */
    long                nLinesWritten;      /* Init(): # of lines written so far */
    } ilRCIPrivRec, *ilRCIPrivPtr;


    /*  ------------------------- ilRCIInit ----------------------------------- */
    /*  Init() function for ilRCI().
    */
static ilError ilRCIInit (
    ilRCIPrivPtr        pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->strip = 0;                       /* init strip index, for first strip */
    pPriv->nLinesWritten = 0;
    return IL_OK;
}

    /*  --------------------- ilRCIExecute -------------------------- */
    /*  Execute() for ilReadImage() producer when the image is compressed.
    */
static ilError ilRCIExecute (
    ilExecuteData  *pData,
    long                    dstLine,            /* ignored */
    long                   *pNLines             /* ignored on input */
    )
{
ilRCIPrivPtr       pPriv;
ilImagePtr         pImage;
long                        offset;

    pPriv = (ilRCIPrivPtr)pData->pPrivate;
    pImage = pPriv->pImage;

        /*  Return error if image has never been written to. */
    if (!pImage->pStripOffsets || !pImage->i.plane[0].pPixels)
        return IL_ERROR_NULL_COMPRESSED_IMAGE;

        /*  "write" current strip by patching next filter's offset to offset for
            current strip, and setting its # bytes to size of this strip.
        */
    offset = pImage->pStripOffsets[pPriv->strip];
    *pData->compressed.pDstOffset = offset;
    pPriv->strip++;
    *pData->compressed.pNBytesWritten = pImage->pStripOffsets[pPriv->strip] - offset;

        /*  If last strip, # of lines is remaining, else is strip height */
    if ((pPriv->nLinesWritten + pImage->stripHeight) > pImage->i.height) {
        *pNLines = pImage->i.height - pPriv->nLinesWritten;
        return IL_ERROR_LAST_STRIP;
        }
    pPriv->nLinesWritten += pImage->stripHeight;
    *pNLines = pImage->stripHeight;
    return IL_OK;
}


        /*  ------------------------ ilReadImage ---------------------------------- */
        /*  Public function: see spec.
            Adds the given image as a producer of the pipe.
        */
ilBool ilReadImage (
    ilPipe              pipe,
    ilObject            image
    )
{
ilImagePtr     pImage;
ilRCIPrivPtr   pPriv;

        /*  Validate that pipe and image are such, have same context and pipe empty */
    pImage = (ilImagePtr)image;
    if (pipe->objectType != IL_PIPE) {
        pipe->context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

    if ((pImage->o.p.objectType != IL_INTERNAL_IMAGE) 
     && (pImage->o.p.objectType != IL_CLIENT_IMAGE))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_OBJECT_TYPE);

    if (pImage->o.p.context != pipe->context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

    if (ilGetPipeInfo (pipe, FALSE, (ilPipeInfo *)NULL, (ilImageDes *)NULL, 
                                    (ilImageFormat *)NULL) != IL_PIPE_EMPTY) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

        /*  If uncompressed, add producer image and exit.  stripHeight = image height, 
            need producer throttle filter before next element is added.
        */
    if (pImage->des.compression == IL_UNCOMPRESSED)
        return _ilAddProducerImage (pipe, pImage, IL_PIPE_IMAGE, pImage->i.height, 
                                   pImage->i.height, TRUE, TRUE);

        /*  Image is compressed: if no stripHeight yet, allocate strip offsets array using
            default strip height.  Add producer image: constant strips the size of the 
            image's strip height.  Add producer throttle to feed each strip.
        */
    if (!pImage->pStripOffsets)
        if (!_ilAllocStripOffsets (pImage, 0))
            return FALSE;

    if (!_ilAddProducerImage (pipe, pImage, IL_PIPE_IMAGE, pImage->i.height, 
                             pImage->stripHeight, TRUE, FALSE))
        return FALSE;

    pPriv = (ilRCIPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilRCIPrivRec), 
        IL_ADD_PIPE_NO_DST, (ilSrcElementData *)NULL, (ilDstElementData *)NULL, 
        ilRCIInit, IL_NPF, IL_NPF, ilRCIExecute, 0);
    if (!pPriv)
        return FALSE;
    pPriv->pImage = pImage;

    return TRUE;
}

