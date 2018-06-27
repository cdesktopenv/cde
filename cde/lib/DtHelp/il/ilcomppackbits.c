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
/* $XConsortium: ilcomppackbits.c /main/3 1995/10/23 15:42:43 rswiston $ */
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
#include "ilerrors.h"


/* ========================================================================
   Compression private data structure definition
   ======================================================================== */

typedef struct {
   int                  stripHeight;
   int                  height;
   int                  width;
   int                  widthbytes;
   int                  beingRecalled;
   ilBool               firstStrip;
} ilCompPBPrivRec, *ilCompPBPrivPtr;



/* ========================================================================
             
       -------------------- ilComppackbitsInit -------------------

   ======================================================================== */

static ilError ilComppackbitsInit(
    ilCompPBPrivPtr    pPrivate,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )
{
    pPrivate->firstStrip = TRUE;
    return IL_OK;
}
/* End ilComppackbitsInit() */                               
                               


/* ========================================================================

     -------------------- ilComppackbitsExecute() -------------------

   ======================================================================== */

static ilError ilComppackbitsExecute(
    ilExecuteData      *pData,
    unsigned long       dstLine,
    unsigned long      *pNLines
    )
{


#define                 PB_MAX_BUFFER_WRITE 140    /* max bytes written after check for room in compressed buffer   */

    unsigned long       nBytes     = 0;         /* Number of source image bytes to unpack for current strip      */  
    unsigned long       nLines;                 /* Number of lines per source image strip                        */  
    ilPtr      pSrcByte;               /* Pointer to source image data first byte                       */
    ilPtr               pSrcLine;               /* Pointer to source image data first byte of line               */  
    ilPtr      pDstByte;               /* Pointer to destination image data                             */
    ilPtr               pDstBufferEnd;          /* Ptr to byte "PB_MAX_BUFFER_WRITE" from end of compressed buffer  */
    unsigned long       srcNBytes;              /* Number of source image bytes per row                          */  
    ilCompPBPrivPtr       pPriv;                  /* Pointer to private image data                                 */  
    ilImagePlaneInfo    *pSrcPlane, *pDstPlane;

    long                    offset;                 /* difference between current dst image address pointer and the
                                                   start address of the dst image buffer                         */
    long                    length;                 /* number of bytes written to dst image                      */
    int            count; 	                /* run count value                   */
    int            start, j;               /* indices into source data          */
    int            nBytesM1;               /* bytes per row of data minus one   */

/* ========================================================================
   Set up for execution of compression algorithm code
   ======================================================================== */

    pPriv             =  (ilCompPBPrivPtr) pData->pPrivate;

    nLines            = *pNLines;
    if (nLines <= 0)    return IL_OK; 

    pSrcPlane         = &pData->pSrcImage->plane[0];
    srcNBytes         = pSrcPlane->nBytesPerRow;
    pSrcLine          = pSrcPlane->pPixels + pData->srcLine * srcNBytes; 

    /* allocate destination buffer space */
    if (pPriv->firstStrip) {
        if (!_ilReallocCompressedBuffer (pData->pDstImage, 0, PB_MAX_BUFFER_WRITE)) 
            return IL_ERROR_MALLOC;
        pPriv->firstStrip = FALSE;
        }

    pDstPlane         = &pData->pDstImage->plane[0];
    pDstBufferEnd     = pDstPlane->pPixels + (pDstPlane->bufferSize - PB_MAX_BUFFER_WRITE);
                                             
    /* exit.  Pointer pPixels must not be NULL */
    if (!pDstPlane->pPixels) return IL_ERROR_NULL_COMPRESSED_IMAGE;

    pDstByte          = pDstPlane->pPixels + *pData->compressed.pDstOffset;

    /* write out image, one strip at a time */
    nBytesM1 = pPriv->widthbytes - 1;

    while ( nLines--  > 0 )  {

        pSrcByte    = pSrcLine;

        start = 0;
        j = 0;

        while (j <= nBytesM1) {
                
             start = j;

             if ( pSrcByte[j] == pSrcByte[j+1] )  {    /*   repeat run  */

                      while ((pSrcByte[j] == pSrcByte[j+1]) && (j < nBytesM1) && ((j-start) < 127) )  j++;
                          
                      /*    output buffer size handling code */
                      if (pDstByte >= pDstBufferEnd) {
                        offset    = pDstByte - pDstPlane->pPixels;
                        if (!_ilReallocCompressedBuffer (pData->pDstImage, 0, offset + PB_MAX_BUFFER_WRITE)) 
                            return IL_ERROR_MALLOC;
                        pDstByte  = pDstPlane->pPixels + offset;
                        pDstBufferEnd = pDstPlane->pPixels + (pDstPlane->bufferSize - PB_MAX_BUFFER_WRITE);
                      }

                      j++;
                      count        =  1 - (j-start);
                      *pDstByte++  =  count;
                      *pDstByte++  =  pSrcByte[j-1];

             }
             else  {              /*  literal run  */
                     
                      while ( (pSrcByte[j] != pSrcByte[j+1]) && (j <= nBytesM1) && ((j-start) < 128) )  j++;
                       
                      /*    output buffer size handling code */
                      if (pDstByte >= pDstBufferEnd) {
                        offset    = pDstByte - pDstPlane->pPixels;
                        if (!_ilReallocCompressedBuffer (pData->pDstImage, 0, offset + PB_MAX_BUFFER_WRITE)) 
                            return IL_ERROR_MALLOC;
                        pDstByte  = pDstPlane->pPixels + offset;
                        pDstBufferEnd = pDstPlane->pPixels + (pDstPlane->bufferSize - PB_MAX_BUFFER_WRITE);
                      }

                      count   =  j-start - 1;
                      *pDstByte++  =  count;
                      do {
                          *pDstByte++  = pSrcByte[start++];
                          } while (--count >= 0);

             }
        }  /* end row loop */

        pSrcLine   += srcNBytes; 
    }  /* end while */


    /*  Return the number of bytes written, = dst ptr - beginning of dst buffer */
    *pData->compressed.pNBytesWritten = pDstByte - (pDstPlane->pPixels + *pData->compressed.pDstOffset);
    return IL_OK;
} 



/* ========================================================================

        -------------------- ilCompressPackbits --------------------

   ======================================================================== */

IL_PRIVATE ilBool _ilCompressPackbits (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData
    )
{
    ilDstElementData    dstdata;
    ilCompPBPrivPtr       pPriv;
    ilImageDes          des;
    ilBool              bitPerPixel;

        /*  Validate that image is bit/pixel, or if not bit, convert to byte if fits. */
    if (pimdes->nSamplesPerPixel != 1)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);
    if (pimformat->nBitsPerSample[0] == 1)
        bitPerPixel = TRUE;
    else {
        bitPerPixel = FALSE;
        if (pimformat->nBitsPerSample[0] != 8)
            if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_BYTE, 0, (ilPtr)NULL))
                return FALSE;
        }

    /* dstdata describes strips being output to next pipe element */
    dstdata.producerObject  =  (ilObject) NULL;
    des                     = *pimdes;
    des.compression         =  IL_PACKBITS;
    dstdata.pDes            =  &des;
    dstdata.pFormat         =  (bitPerPixel) ? IL_FORMAT_BIT : IL_FORMAT_BYTE;
    dstdata.width           =  pinfo->width;
    dstdata.height          =  pinfo->height;
    dstdata.stripHeight     =  pSrcData->stripHeight;
    dstdata.constantStrip   =  pSrcData->constantStrip;
    dstdata.pPalette        =  pinfo->pPalette;
    dstdata.pCompData       =  (ilPtr)NULL;


/* ========================================================================
   Add the filter to the pipeline
   ======================================================================== */

    pPriv = (ilCompPBPrivPtr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilCompPBPrivRec), 0, pSrcData, &dstdata,
                                             ilComppackbitsInit, IL_NPF, IL_NPF, ilComppackbitsExecute, 0);

    if (!pPriv) return FALSE; /* EXIT */

    /* save private date */
    pPriv->stripHeight   = dstdata.stripHeight;
    pPriv->height        = pinfo->height;
    pPriv->width         = pinfo->width;
    pPriv->widthbytes    = (bitPerPixel) ? (pPriv->width + 7) / 8 : pPriv->width;
    pPriv->beingRecalled = FALSE;
    return TRUE;

}

