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
/* $XConsortium: ilscalesample.c /main/5 1996/06/19 12:20:45 ageorge $ */
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
#include "ilscaleint.h"
#include "ilerrors.h"


#define IL_FILTER_SIZE         3                   /* size of one side of filter square */
#define MIN_DESIRED_STRIP     16                   /* arbitrary: desired size of strips */


typedef struct {
    unsigned long   adjacentFracTable[256];     /* Area sample fraction tables */                
    unsigned long   centerFracTable[256];       /* Area sample fraction tables */
    unsigned long   topbelowFracTable[256];     /* Area sample fraction tables */                 
    unsigned long   cornerFracTable[256];       /* Area sample fraction tables */               
    long            srcWidth;                   /* width of src (pipe) image */
    long            dstWidth, dstHeight;        /* size of dst (output) image */
    long            widthDiff, heightDiff;      /* src - dst of width/height */
    long            srcBufferHeight;            /* height of perm src image, else 0 */
    long            lineAcc;                    /* line accumulator for vert scaling */
    } ilScaleSampleRec, *ilScaleSamplePtr;


 /* These routines support scaling down for rgb & byte images by averaging the values
    of neighboring pixels.  A 3x3 region is sampled with the center at the pixel to be written.
    The amount or fraction that each neighboring pixel contributes to the total sum is based on 
    the ratio between the src and dest height & width.  Example: if the scale reduces the image
    by 3:1 or more than the 3x3 region will constructed so that each neighboring pixel contributes
    1/9th to the overall sum.  Tables are precomputed to store the values for the center pixel fraction,
    fraction for adjacent pixels, fraction for pixels above and below, and the fraction for pixels
    at the corners of the 3x3 region sampled.                                                            */


/* =============================================================================================================================

      ilScaleInitTables    -  Init fraction tables used to perform area sampling 
   ============================================================================================================================= */

static void ilScaleInitTables (
    long            srcWidth,
    long            srcHeight,
    long            dstWidth,
    long            dstHeight,
    unsigned long   *centerFracTable,
    unsigned long   *adjacentFracTable,
    unsigned long   *topbelowFracTable,
    unsigned long   *cornerFracTable
    )
{

  double   widthscale, heightscale, totalarea;
  double   centerfraction, adjacentfraction, topbelowfraction, cornerfraction;
    long   index;

 
    /* The fraction of each pixel selected is based on a square of size "srcWidth/dstWidth" 
       by "srcHeight/dstHeight", mapped over the source pixels, centered on pixel being written.
       If the scale factor is greater than 3.0 in either direction, make it 3.0 (all pixels
       contribute equally in that direction.                                                 */

    widthscale = (double)srcWidth / (double)dstWidth;
    if (widthscale > 3.0)    widthscale = 3.0;

    heightscale = (double)srcHeight / (double)dstHeight;
    if (heightscale > 3.0)   heightscale = 3.0;

    totalarea = widthscale * heightscale;

    /* compute fractions for the center, adjacent, top/below, and corner pixels */
    centerfraction  = 1.0 / totalarea;
    adjacentfraction  = (widthscale - 1.0)  / (2.0 * totalarea);
    topbelowfraction = (heightscale - 1.0) / (2.0 *totalarea);
    cornerfraction   = ((widthscale - 1.0) * (heightscale - 1.0)) / (4.0 * totalarea);
       

    /* Build the area sample fraction tables */

    for ( index = 0;  index < 256 ; index++ )  {
         centerFracTable[index]   =  256.0 * (centerfraction * (double) index + 0.1);
         adjacentFracTable[index] =  256.0 * (adjacentfraction * (double) index + 0.1);
         topbelowFracTable[index] =  256.0 * (topbelowfraction * (double) index + 0.1);
         cornerFracTable[index]   =  256.0 * (cornerfraction * (double) index + 0.1);
    }


}


/* =============================================================================================================================

      ilScaleSampleInit    -  Init routine for private data
   ============================================================================================================================= */

static ilError ilScaleSampleInit (
    ilScaleSamplePtr   pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{

    pPriv->lineAcc = pPriv->heightDiff;
    return IL_OK;

}


/* =============================================================================================================================

      ilScaleSample3ByteExecute -  Scale processing for images with 24 bits per pixel format .

   ============================================================================================================================= */

static ilError ilScaleSample3ByteExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilScaleSamplePtr  pPriv;
ilPtr             pSrc, pDst, pSrcTemp;
long              pixel, i, j, right;
unsigned long    *pCornerTable, *pTopBelowTable;
long              colAcc, dstWidth;
long                       nDstPixels, widthDiff;
ilPtr                      psrcline, pdstline, pSrcEnd;
long                       srcnbytes, dstnbytes;
long                       bottomLine, line;
long                       nlines;
ilImagePlaneInfo           *pplane;
long                       nlineswritten;
long                       srcBytesAbove, srcBytesBelow;


   nlines             =  *pNLines;
   if (nlines <= 0)   return IL_OK;

   pplane             =  &pData->pSrcImage->plane[0];
   srcnbytes          =  pplane->nBytesPerRow;
   psrcline           = (unsigned char *) (pplane->pPixels) + pData->srcLine * srcnbytes;

   pplane             =  &pData->pDstImage->plane[0];
   dstnbytes          =  pplane->nBytesPerRow;
   pdstline           = (unsigned char *) (pplane->pPixels) + dstLine * dstnbytes;

   pPriv              =  (ilScaleSamplePtr) pData->pPrivate;

   /*  Point pSrcLine to srcLine: = middle line of 3x3 matrix.
       Set bottomLine to last available line in the buffer.     */


   if (pPriv->srcBufferHeight) bottomLine = pPriv->srcBufferHeight - 1;
   else                        bottomLine = pData->srcLine + nlines - 1;

   line = pData->srcLine;  
   nlineswritten = 0;
   dstWidth = pPriv->dstWidth;

   pCornerTable = pPriv->cornerFracTable;
   pTopBelowTable = pPriv->topbelowFracTable;


   /*  Loop over number of lines  */
   while (TRUE) {

       while (pPriv->lineAcc > 0) {
          if (nlines-- <= 0) 
             break;
          pPriv->lineAcc -= pPriv->dstHeight;
          psrcline += srcnbytes;
          line++;
          }
       if (nlines-- <= 0)
          break;
       pPriv->lineAcc += pPriv->heightDiff;

       pDst = pdstline;
       pSrc = psrcline;

       srcBytesAbove = (line <= pData->srcLine)  ? 0 : srcnbytes;
       srcBytesBelow = ((line + 1) > bottomLine) ? 0 : srcnbytes;

       widthDiff = pPriv->widthDiff;
       colAcc = widthDiff;
       nDstPixels = dstWidth;

            /*  If colAcc (widthDiff, = src - dst width) is 0, then no hori scaling
                is being done; just copy pixels and continue.
            */
       if (colAcc == 0) {
           while (nDstPixels-- > 0) {
               *pDst++ = *pSrc++;
               *pDst++ = *pSrc++;
               *pDst++ = *pSrc++;
               }
           }
       else {

            /*  For each dst pixel, average, based on weight tables, using a 3x3 square,
                centered around "pSrc+1" (+1 to make indexing easier).  Note that pSrc 
                cannot point to left edge, because "colAcc" is > 0 (checked above), which
                will cause pSrc to be inc'd below.  Set "right" to 1 if at right edge
                (reuse middle pixel), else to 2 to use pixel to the right.
                Pre-fetch all memory values, to avoid RISC register interlock.
            */
        pSrc -= 3;                                  /* point pSrc one to left */
        pSrcEnd = pSrc + 3 * pPriv->srcWidth - 3;   /* pSrc at right edge when here */
        while (nDstPixels-- > 0) {
            while (colAcc > 0) {
                colAcc -= dstWidth;
                pSrc += 3;
                }
            colAcc += widthDiff;
            right = (pSrc >= pSrcEnd) ? 3 : 6;      /* reuse mid pixel if at right edge */

                /*  Do RED */
            pSrcTemp = pSrc - srcBytesAbove;        /* line above */
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel = pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            i = pSrc[0];                            /* middle line */
            j = pSrc[3];
            i = pPriv->adjacentFracTable[i];
            j = pPriv->centerFracTable[j];
            pixel += i;
            i = pSrc[right];
            pixel += j;
            i = pPriv->adjacentFracTable[i];

            pSrcTemp = pSrc + srcBytesBelow;        /* line below */
            pixel += i;
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel += pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            *pDst++ = pixel >> 8;                   /* store pixel, move src to right */
            pSrc++;

                /*  Do GREEN */
            pSrcTemp = pSrc - srcBytesAbove;        /* line above */
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel = pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            i = pSrc[0];                            /* middle line */
            j = pSrc[3];
            i = pPriv->adjacentFracTable[i];
            j = pPriv->centerFracTable[j];
            pixel += i;
            i = pSrc[right];
            pixel += j;
            i = pPriv->adjacentFracTable[i];

            pSrcTemp = pSrc + srcBytesBelow;        /* line below */
            pixel += i;
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel += pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            *pDst++ = pixel >> 8;                   /* store pixel, move src to right */
            pSrc++;

                /*  Do BLUE */
            pSrcTemp = pSrc - srcBytesAbove;        /* line above */
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel = pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            i = pSrc[0];                            /* middle line */
            j = pSrc[3];
            i = pPriv->adjacentFracTable[i];
            j = pPriv->centerFracTable[j];
            pixel += i;
            i = pSrc[right];
            pixel += j;
            i = pPriv->adjacentFracTable[i];

            pSrcTemp = pSrc + srcBytesBelow;        /* line below */
            pixel += i;
            i = pSrcTemp[0];
            j = pSrcTemp[3];
            pixel += pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            *pDst++ = pixel >> 8;                   /* store pixel, move src to right */
            pSrc++;
            }
        }       /* END hori scaling */

       nlineswritten++;
       psrcline += srcnbytes;
       pdstline += dstnbytes;
       line++;

   }  /* end  while loop */


   *pNLines = nlineswritten;
   return IL_OK;
}



/* =============================================================================================================================

      ilScaleSampleByteExecute -  Scale processing for images with byte per pixel format .

   ============================================================================================================================= */

static ilError ilScaleSampleByteExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilScaleSamplePtr  pPriv;
ilPtr             pSrc, pDst, pSrcTemp;
long              pixel, i, j, right;
unsigned long    *pCornerTable, *pTopBelowTable;
long              colAcc, dstWidth;
long                       nDstPixels, widthDiff;
ilPtr                      psrcline, pdstline, pSrcEnd;
long                       srcnbytes, dstnbytes;
long                       bottomLine, line;
long                       nlines;
ilImagePlaneInfo           *pplane;
long                       nlineswritten;
long                       srcBytesAbove, srcBytesBelow;


   nlines             =  *pNLines;
   if (nlines <= 0)   return IL_OK;

   pplane             =  &pData->pSrcImage->plane[0];
   srcnbytes          =  pplane->nBytesPerRow;
   psrcline           = (unsigned char *) (pplane->pPixels) + pData->srcLine * srcnbytes;

   pplane             =  &pData->pDstImage->plane[0];
   dstnbytes          =  pplane->nBytesPerRow;
   pdstline           = (unsigned char *) (pplane->pPixels) + dstLine * dstnbytes;

   pPriv              =  (ilScaleSamplePtr) pData->pPrivate;

   /*  Point pSrcLine to srcLine: = middle line of 3x3 matrix.
       Set bottomLine to last available line in the buffer.     */


   if (pPriv->srcBufferHeight) bottomLine = pPriv->srcBufferHeight - 1;
   else                        bottomLine = pData->srcLine + nlines - 1;

   line = pData->srcLine;  
   nlineswritten = 0;
   dstWidth = pPriv->dstWidth;

   pCornerTable = pPriv->cornerFracTable;
   pTopBelowTable = pPriv->topbelowFracTable;

   /*  Loop over number of lines  */
   while (TRUE) {

        while (pPriv->lineAcc > 0) {
            if (nlines-- <= 0) 
                break;
            pPriv->lineAcc -= pPriv->dstHeight;
            psrcline += srcnbytes;
            line++;
            }
        if (nlines-- <= 0)
            break;
       pPriv->lineAcc += pPriv->heightDiff;

       pDst = pdstline;
       pSrc = psrcline;

       srcBytesAbove = (line <= pData->srcLine)  ? 0 : srcnbytes;
       srcBytesBelow = ((line + 1) > bottomLine) ? 0 : srcnbytes;

       widthDiff = pPriv->widthDiff;
       colAcc = widthDiff;
       nDstPixels = dstWidth;

            /*  If colAcc (widthDiff, = src - dst width) is 0, then no hori scaling
                is being done; just copy pixels and continue.
            */
       if (colAcc == 0) {
           while (nDstPixels-- > 0)
               *pDst++ = *pSrc++;
           }
       else {

            /*  For each dst pixel, average, based on weight tables, using a 3x3 square,
                centered around "pSrc+1" (+1 to make indexing easier).  Note that pSrc 
                cannot point to left edge, because "colAcc" is > 0 (checked above), which
                will cause pSrc to be inc'd below.  Set "right" to 1 if at right edge
                (reuse middle pixel), else to 2 to use pixel to the right.
                Pre-fetch all memory values, to avoid RISC register interlock.
            */
        pSrc--;                                     /* point pSrc one to left */
        pSrcEnd = pSrc + pPriv->srcWidth - 1;       /* pSrc at right edge when here */
        while (nDstPixels-- > 0) {
            while (colAcc > 0) {
                colAcc -= dstWidth;
                pSrc++;
                }
            colAcc += widthDiff;
            right = (pSrc >= pSrcEnd) ? 1 : 2;      /* reuse mid pixel if at right edge */

            pSrcTemp = pSrc - srcBytesAbove;        /* line above */
            i = pSrcTemp[0];
            j = pSrcTemp[1];
            pixel = pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            i = pSrc[0];                            /* middle line */
            j = pSrc[1];
            i = pPriv->adjacentFracTable[i];
            j = pPriv->centerFracTable[j];
            pixel += i;
            i = pSrc[right];
            pixel += j;
            i = pPriv->adjacentFracTable[i];

            pSrcTemp = pSrc + srcBytesBelow;        /* line below */
            pixel += i;
            i = pSrcTemp[0];
            j = pSrcTemp[1];
            pixel += pCornerTable[i];
            i = pSrcTemp[right];
            j = pTopBelowTable[j];
            i = pCornerTable[i];
            pixel += j;
            pixel += i;

            *pDst++ = pixel >> 8;                   /* store pixel, move src to right */
            pSrc++;
            }
        }       /* END hori scaling */

       nlineswritten++;
       psrcline += srcnbytes;
       pdstline += dstnbytes;
       line++;

   }  /* end  while nlines loop */


   *pNLines = nlineswritten;
   return IL_OK;
}


/* =============================================================================================================================

      ilScaleSample        - pipe constructor function for scaling down using area sampling method.

   ============================================================================================================================= */

IL_PRIVATE void _ilScaleSample ( 
    ilPipe              pipe,
    int                 Ncomponents,
    unsigned long       dstWidth,
    unsigned long       dstHeight,
    unsigned long       nLevels,
    ilBool              blackIsZero,
    ilPipeInfo         *pInfo
    )
{

ilScaleSamplePtr pPriv = NULL;
ilDstElementData          dstdata;



    /*  Add a filter to do the scale, then init *pPriv.  */

    dstdata.producerObject = (ilObject)NULL;
    dstdata.pDes           = (ilImageDes *) NULL;
    dstdata.pFormat        = (ilImageFormat *) NULL;
    dstdata.width          = dstWidth;
    dstdata.height         = dstHeight;
    dstdata.pPalette       = pInfo->pPalette;;

    dstdata.stripHeight = ((dstHeight * pInfo->recommendedStripHeight) / pInfo->height) + 1;
    dstdata.constantStrip = FALSE;

    switch  (Ncomponents) {

       case  3:
                 pPriv = (ilScaleSamplePtr) ilAddPipeElement (pipe, IL_FILTER, sizeof (ilScaleSampleRec), 0, (ilSrcElementData *)NULL,
                                                              &dstdata, ilScaleSampleInit, IL_NPF, IL_NPF, ilScaleSample3ByteExecute, NULL, 0);
                 break;
       case  1:
                 pPriv = (ilScaleSamplePtr) ilAddPipeElement (pipe, IL_FILTER, sizeof (ilScaleSampleRec), 0, (ilSrcElementData *)NULL,
                                                              &dstdata, ilScaleSampleInit, IL_NPF, IL_NPF, ilScaleSampleByteExecute, NULL, 0);
    }


    if (!pPriv) return;

    pPriv->srcWidth   =  pInfo->width;
    pPriv->dstWidth   =  dstWidth;
    pPriv->dstHeight  =  dstHeight;
    pPriv->widthDiff  =  pInfo->width - dstWidth;
    pPriv->heightDiff =  pInfo->height - dstHeight;

    /*  Pipe element added, init pPriv.  If not a tempImage, reading directly from
        a permanent image: set srcHeight to image height; else set to 0.     */
    if (!pInfo->tempImage)
         pPriv->srcBufferHeight = pInfo->height;
    else pPriv->srcBufferHeight = 0;

    /*  Init area sample fraction tables in private based on scale factors and # levels.   */

    ilScaleInitTables ( pPriv->srcWidth, pInfo->height, pPriv->dstWidth, pPriv->dstHeight,
                        pPriv->centerFracTable, pPriv->adjacentFracTable,
                        pPriv->topbelowFracTable, pPriv->cornerFracTable);

    pipe->context->error = IL_OK;
}
