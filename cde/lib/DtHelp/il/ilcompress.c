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
/* $XConsortium: ilcompress.c /main/3 1995/10/23 15:42:52 rswiston $ */
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
#include "ilcompress.h"
#include "ilcodec.h"
#include "ilerrors.h"


    /*  --------------------------- ilCompress ----------------------------------- */
    /*  Public function; see spec.
        Forces pipe data to be compressed with the given "compression", in constant
        strips. If "dstStripHeight" is zero (0), then any stripHeight is allowable,
        but if compression is done, it is done with the default strip height.
    */
ilBool ilCompress (
    ilPipe              pipe,
    unsigned int        compression,
    ilPtr               pCompData,
    long                dstStripHeight,
    unsigned long       mustBeZero
    )
{
    unsigned int        state;
    ilPipeInfo          info;                              
    ilImageDes          des;
    ilImageFormat       format;
    ilSrcElementData    srcData;
    ilBool              correct;
     

    if (ilGetPipeInfo (pipe, FALSE, &info, &des, &format) != IL_PIPE_FORMING)
        if (!pipe->context->error)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);

    if (mustBeZero != 0)
        return ilDeclarePipeInvalid(pipe, IL_ERROR_PAR_NOT_ZERO);

        /*  Force requested strip height to image height if greater. */
    if (dstStripHeight > info.height)
        dstStripHeight = info.height;

        /*  Check current pipe image and force decompression if wrong compression, or 
            inconstant strip height, or not requested (non-zero) strip height.
        */
    if ((des.compression != compression)
     || (dstStripHeight && (info.stripHeight != dstStripHeight))
     || !info.constantStrip)
        ilGetPipeInfo (pipe, TRUE, &info, &des, &format);       /* decompress */

        /*  Specify the input (and therefore the output) strip height: if dstStripHeight
            is zero, take default, otherwise requested, always  constant strips.
        */
    if (dstStripHeight <= 0)
        dstStripHeight = ilRecommendedStripHeight (&des, &format, info.width, info.height);
    srcData.consumerImage = (ilObject)NULL;
    srcData.minBufferHeight = 0;
    srcData.stripHeight = dstStripHeight;
    srcData.constantStrip = TRUE;
 
        /*  Pipe image is now correct compression or is uncompressed.  For compression
            types which have no variants, exit if already requested compression; for
            types with variant, call compression-specific code to check variant.
        */
    switch (compression) {

      case IL_UNCOMPRESSED:                     /* image now uncompressed; skip */
        break;

      case IL_G3:                               /* check G3 variants */
        return _ilCompressG3 (pipe, &info, &des, &format, &srcData, pCompData);
        break;

      case IL_JPEG:                             /* check JPEG variants */
        return _ilCompressJPEG (pipe, &info, &des, &format, &srcData, pCompData);
        break;

      case IL_G4:                               /* check G4 variants */
        return _ilCompressG4 (pipe, &info, &des, &format, &srcData, pCompData);
        break;

      case IL_LZW:
        if (des.compression != IL_LZW)
            return _ilCompressLZW (pipe, &info, &des, &format, &srcData);
        break;

      case IL_PACKBITS:   
        if (des.compression != IL_PACKBITS)
            return _ilCompressPackbits (pipe, &info, &des, &format, &srcData);
        break;

      default:
        return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
    }

        /*  No (re)compression needed; exit with success */
    pipe->context->error = IL_OK;
    return TRUE;
}
