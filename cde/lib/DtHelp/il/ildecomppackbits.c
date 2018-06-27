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
/* $XConsortium: ildecomppackbits.c /main/5 1996/06/19 12:23:29 ageorge $ */
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
#include "ildecomp.h"
#include "ilerrors.h"


/* ========================================================================
   Decompression private data structure definition
   ======================================================================== */

typedef struct {
   long                 height;
   long                 width;
   long                 nDstLineBytes;
} ilDecompPBPrivRec, *ilDecompPBPrivPtr;



/* =============================================================================================================================    

    -------------------- ilDecompPackbitsExecute() -------------------
    Routine defined in ilDecompPackbits for executing Packbits 
    decompression when the pipe gets executed.

   ============================================================================================================================= */

static ilError ilDecompPackbitsExecute(
    ilExecuteData        *pData,
    unsigned long        dstLine,
    unsigned long        *pNLines
    )
{

    unsigned long           dstNBytes;              /* Number of destination image bytes per row                     */                    
    long                    nLines;                 /* Number of lines per source image strip                        */  
    long           nBytesToGo;             /* Number of source image bytes left to unpack for current strip */
    long           nDstLineBytesToGo;      /* Number of bytes left to write to this line of destination     */
    ilPtr          pSrcByte;               /* Pointer to source image data                                  */
    ilPtr          pDstByte;               /* Pointer to destination image data first byte                  */
    ilPtr                   pSrcLine;               /* Pointer to source image data first byte of each scanline      */  
    ilPtr                   pDstLine;               /* Pointer to destination image data first byte of each scanline */  
    ilDecompPBPrivPtr       pPriv;                  /* Pointer to private image data                                 */  
    ilImagePlaneInfo        *pPlane;
    int            count;                  /* run-length code value     */
    ilByte         repeatbyte;             /* repeated data             */


/* ========================================================================
   ilDecompPackbitsExecute() set up for decompression algorithm code           
   ======================================================================== */

    pPriv            =  (ilDecompPBPrivPtr) pData->pPrivate;
    nLines           = *pNLines;
    if (nLines <= 0)    return IL_OK; 

    nBytesToGo       =  pData->compressed.nBytesToRead;   
    if (nBytesToGo <= 0)    return IL_OK; 

    pPlane           = &pData->pSrcImage->plane[0];
    if (!pPlane->pPixels) return IL_ERROR_NULL_COMPRESSED_IMAGE;
    pSrcLine         =  pPlane->pPixels + pData->compressed.srcOffset; 

    pPlane           = &pData->pDstImage->plane[0];
    if (!pPlane->pPixels) return IL_ERROR_NULL_COMPRESSED_IMAGE;
    dstNBytes        =  pPlane->nBytesPerRow;         
    pDstLine         =  pPlane->pPixels + dstLine * dstNBytes;   

    pSrcByte    =  pSrcLine;

        
    /* decode each line of compressed data into the destination image */
    while (nLines-- > 0) {
            pDstByte    =  pDstLine;
            nDstLineBytesToGo = pPriv->nDstLineBytes;

            while (nDstLineBytesToGo > 0) {
             
                if (--nBytesToGo < 0)
                    return IL_ERROR_COMPRESSED_DATA;
                count = (signed char)*pSrcByte++;

                if (count >= 0) {
                        count++;
                        nDstLineBytesToGo -= count;
                        nBytesToGo -= count;
                        if ((nDstLineBytesToGo < 0) || (nBytesToGo < 0))
                            return IL_ERROR_COMPRESSED_DATA;
                        count--;
                        do {
                            *pDstByte++ = *pSrcByte++;
                            } while (--count >= 0);
                }
                else if (count != 128) {            /* 128 is a noop for Packbits */
                        count       =  -count;      /* count is now -1 of real count */
                        nDstLineBytesToGo -= (count + 1);
                        nBytesToGo--;
                        if ((nDstLineBytesToGo < 0) || (nBytesToGo < 0))
                            return IL_ERROR_COMPRESSED_DATA;
                        repeatbyte  =  *pSrcByte++;
                        do {
                            *pDstByte++ = repeatbyte;
                            } while (--count >= 0);
                }
            }  /* end row */
           
            pDstLine +=  dstNBytes;

    }    /* end strip */

    return IL_OK;
}



/* =============================================================================================================================

        -------------------- ilDecompPackbits() --------------------
    Main body of code for Packbits decompression.  This includes image descriptor parameter error checking
    and function calls for: strip handler initialization, adding the filter element to the pipe, pipe initialization
    and execution, decompression algorithm.....

   ============================================================================================================================= */

IL_PRIVATE ilBool _ilDecompPackbits (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat
    )
{

    ilDstElementData    dstdata;
    ilDecompPBPrivPtr   pPriv;
    ilImageDes          des;
    ilBool              bitPerPixel;


        /*  If image has more than one sample/pixel or is not bit or byte/pixel, error */
    bitPerPixel = (pimformat->nBitsPerSample[0] == 1);
    if ((pimdes->nSamplesPerPixel != 1)
     || (!bitPerPixel && (pimformat->nBitsPerSample[0] != 8)))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);

    dstdata.producerObject  =  (ilObject) NULL;
    des                     = *pimdes;
    des.compression         =  IL_UNCOMPRESSED;
    dstdata.pDes            =  &des;
    dstdata.width           =  pinfo->width;
    dstdata.height          =  pinfo->height;
    dstdata.pPalette        =  pinfo->pPalette;
    dstdata.pCompData       =  (ilPtr)NULL;
    dstdata.stripHeight     =  pinfo->stripHeight;
    dstdata.constantStrip   =  pinfo->constantStrip;
    dstdata.pFormat = (bitPerPixel) ? IL_FORMAT_BIT : IL_FORMAT_BYTE;

    pPriv = (ilDecompPBPrivPtr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilDecompPBPrivRec), 0, 
        (ilSrcElementData *)NULL, &dstdata, 
        IL_NPF, IL_NPF, IL_NPF, ilDecompPackbitsExecute, NULL, 0);
    if (!pPriv) 
        return FALSE;

    pPriv->height        = pinfo->height;
    pPriv->width         = pinfo->width;
    pPriv->nDstLineBytes = (bitPerPixel) ? (pPriv->width + 7) / 8 : pPriv->width;

    return TRUE;
}

