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
/* $XConsortium: ilcrop.c /main/5 1996/06/19 12:24:05 ageorge $ */
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

/* =============================================================================================================================

     /ilc/ilcrop.c : Image Library crop routines.   

   ============================================================================================================================= */


#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"



/*  =========================== Compressed Crop Code ================================= */
/*  This code crops compressed pipe images by ignoring strips that are outside of
    the crop rectangle.
*/

    /*  Private data for compressed crop filter. */
typedef struct {
    long                topStrip;       /* index (from 0) of first strip to output */
    long                bottomStrip;    /* last strip to output */
    long                stripIndex;     /* Init(): current strip index */
    } ilCropCompPrivRec, *ilCropCompPrivPtr;


    /*  ------------------------------ ilCropCompInit ----------------------------- */
    /*  Init() function for when cropping compressed images.
    */
static ilError ilCropCompInit (
    ilCropCompPrivPtr   pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->stripIndex = 0;          /* init current strip to first strip */
    return IL_OK;
}

    /*  ------------------------------ ilCropCompExecute ----------------------------- */
    /*  Execute() function for when cropping compressed images.
    */
static ilError ilCropCompExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilCropCompPrivPtr  pPriv;

        /*  If this strip is out of range of strips to be written, return 0 for
            # lines written (crops the strip); otherwise pass strip on to next filter.
        */
    pPriv = (ilCropCompPrivPtr)pData->pPrivate;
    if ((pPriv->stripIndex < pPriv->topStrip) || (pPriv->stripIndex > pPriv->bottomStrip)) {
        *pNLines = 0;                       /* skip this strip */
        *pData->compressed.pDstOffset = 0;  /* next filter should not be called anyway */
        *pData->compressed.pNBytesWritten = 0;
        }
    else {                                  /* in range, pass this strip down pipe */
        *pData->compressed.pDstOffset = pData->compressed.srcOffset;
        *pData->compressed.pNBytesWritten = pData->compressed.nBytesToRead;
        }
    pPriv->stripIndex++;                    /* inc current strip count */
    return IL_OK;
}


    /*  ------------------------------ ilCropCompressed ----------------------------- */
    /*  Called by ilCrop() when the pipe image is compressed.  "pRect" must point to
        a non-null rectangle which is the crop rectangle intersect with the image bounds.
       "pInfo" must point to the current pipe info.
    */
static ilBool ilCropCompressed (
    ilPipe              pipe,
    ilRect             *pRect,
    ilPipeInfo         *pInfo
    )
{
ilRect                  rect;
long           stripHeight, bottom;
long                    topStrip, bottomStrip, topLine;
ilDstElementData        dstData;
ilCropCompPrivPtr       pPriv;

    rect = *pRect;                              /* don't modify caller's copy */

        /*  Skip strips above and below the strips that intersect the crop rect.
            Can only do that if strips are constant, because we must declare the size
            of the image coming out of the filter added here.  Set "top/bottomStrip" to
            the index (from 0) of the top/bottom (first/last) strip within crop rect.
        */
    stripHeight = pInfo->stripHeight;
    if (pInfo->constantStrip && (stripHeight > 0)) {
        topStrip = rect.y / stripHeight;
        bottom = rect.y + rect.height;
        bottomStrip = (bottom - 1) / stripHeight;

            /*  Add a "no dst" filter to crop only if some strips would be skipped */
        if ((topStrip > 0) || (bottomStrip < ((pInfo->height - 1) / stripHeight))) {
            dstData.producerObject = (ilObject)NULL;
            dstData.pDes = (ilImageDes *)NULL;
            dstData.pFormat = (ilImageFormat *)NULL;
            dstData.width = pInfo->width;           /* width is not cropped */
            topLine = topStrip * stripHeight;       /* first line output from filter */
            dstData.height = (bottomStrip + 1) * stripHeight - topLine;
            dstData.stripHeight = stripHeight;
            dstData.constantStrip = TRUE;           /* checked above */
            dstData.pPalette = pInfo->pPalette;
            dstData.pCompData = pInfo->pCompData;

            pPriv = (ilCropCompPrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                sizeof (ilCropCompPrivRec), IL_ADD_PIPE_NO_DST, (ilSrcElementData *)NULL,
                &dstData, ilCropCompInit, IL_NPF, IL_NPF, ilCropCompExecute, NULL,  0);
            if (!pPriv)
                return FALSE;
            pPriv->topStrip = topStrip;
            pPriv->bottomStrip = bottomStrip;

                /*  Adjust the top of new crop rect by lines skipped by this filter. */
            rect.y -= topLine;
            }
        }

        /*  Decompress the pipe image and call ilCrop() recursively to crop the bits
            that remain after this code has (possibly) skipped unnecessary strips.
        */
    ilGetPipeInfo (pipe, TRUE, (ilPipeInfo *)NULL, (ilImageDes *)NULL, (ilImageFormat *)NULL);
    return ilCrop (pipe, &rect);
}


/*  =========================== Uncompressed Crop Code =============================== */
/*  This code crops uncompressed pipe images by copying only the relevant pixels.
*/


#define LONGSZ        4
#define WORDPOS       32


 typedef struct {
        int    illinecount;           /* running line count as pipe strips are executed */
        int    ilCropDstheight;       /* destination height value saved to avoid strip sizes  */
        int    ilCropSrcheight;       /* src height value saved to avoid strip sizes  */
        int    ilCropXoff;            /* crop X origin   */
        int    ilCropYoff;            /* crop Y origin   */
 }  ilCropPriv,  *ilCropPrivptr;


/* =============================================================================================================================
   ============================================================================================================================= */

static ilError ilCropInit(
    ilCropPrivptr   pPrivate,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )

{
   /* Initialize counters */
   pPrivate->illinecount =  1;

   return IL_OK;
}




/* =============================================================================================================================

      ilCropBitonalExecute  -  Crop processing for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilCropBitonalExecute (
    ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
CARD32            *psrc, *pdst, *psrcline, *pdstline, srca, srcb;
long              srcnwords, dstnwords, nlines;
unsigned long     dstwidth;
long              nlongs, firstword;
ilImagePlaneInfo           *pplane;
unsigned long     local_noDstLine, Lwordoff, Rwordoff;
int               x, y, lastcount, yextent;
ilCropPrivptr     pPriv;



   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   psrcline                =  (CARD32 *) (pplane->pPixels)  +  pData->srcLine * srcnwords; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   pdstline                =  (CARD32 *) (pplane->pPixels)  +  dstLine * dstnwords; 
   pPriv                   =  (ilCropPrivptr) pData->pPrivate;


   dstwidth  = pData->pDstImage->width;
   yextent   = pPriv->ilCropYoff + pPriv->ilCropDstheight;

   firstword = (pPriv->ilCropXoff)/WORDPOS;
   nlongs    = (dstwidth - 1)/WORDPOS + 1;
   Lwordoff  = pPriv->ilCropXoff % WORDPOS;
   Rwordoff  = WORDPOS - Lwordoff;


   local_noDstLine = 0;
   lastcount = pPriv->illinecount - 1;


   for ( y = lastcount;  y < (lastcount + *pNLines);  y++, pPriv->illinecount++) {

       if ((y >= pPriv->ilCropYoff) && (y < yextent)) {     /* process */
              psrc = psrcline;
              pdst = pdstline;

              for( x = 0; x < firstword;  x++, psrc++)  { }  /* skip long words up to the offset */
                                 
  
              if ( Lwordoff == 0 ) {
                 for( x = 0; x < nlongs;  x++ )  *pdst++ = *psrc++;
              }
              else {

                 srca = *psrc++;
                 for( x = 0; x < nlongs;  x++ ) { 

                   srcb     =  *psrc++;
                   *pdst++  =  (srcb >> Rwordoff) | (srca << Lwordoff);
                   srca     = srcb;
                 }
              }

             pdstline += dstnwords;
             local_noDstLine++;
       }

       psrcline += srcnwords;

   }  /* end loop */


   *pNLines = local_noDstLine;
   return IL_OK;

}


/* =============================================================================================================================

      ilCrop3ByteExecute  -  Crop processing for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilCrop3ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes, nlines;
unsigned long     dstwidth;
ilImagePlaneInfo           *pplane;
unsigned long     local_noDstLine;
int               x, y, lastcount, yextent, xextent;
ilCropPrivptr     pPriv;


   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels)  +  pData->srcLine * srcnbytes; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes; 
   pPriv                   =  (ilCropPrivptr) pData->pPrivate;


   dstwidth  = pData->pDstImage->width;
   yextent   = pPriv->ilCropYoff + pPriv->ilCropDstheight;
   xextent   = pPriv->ilCropXoff + dstwidth;

   local_noDstLine = 0;
   lastcount = pPriv->illinecount - 1;

   for ( y = lastcount;  y < (lastcount + *pNLines);  y++, pPriv->illinecount++) {

       if ((y >= pPriv->ilCropYoff) && (y < yextent)) {    /* process */
              psrc = psrcline;
              pdst = pdstline;

              for( x = 0; x < xextent;  x++)  {
                   if  ( x < pPriv->ilCropXoff )  psrc += 3;
                   else {
                       *pdst++ = *psrc++;
                       *pdst++ = *psrc++;
                       *pdst++ = *psrc++;
                   }
              }
              pdstline += dstnbytes;
              local_noDstLine++;
       }
       psrcline += srcnbytes;

   }  /* end loop */


   *pNLines = local_noDstLine;
   return IL_OK;

}




/* =============================================================================================================================

      ilCropByteExecute  -  Crop processing for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilCropByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes, nlines;
unsigned long     dstwidth;
ilImagePlaneInfo           *pplane;
unsigned long     local_noDstLine;
int               x, y, lastcount, yextent, xextent;
ilCropPrivptr     pPriv;


   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels)  +  pData->srcLine * srcnbytes; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes; 
   pPriv                   =  (ilCropPrivptr) pData->pPrivate;


   dstwidth  = pData->pDstImage->width;
   yextent   = pPriv->ilCropYoff + pPriv->ilCropDstheight;
   xextent   = pPriv->ilCropXoff + dstwidth;

   local_noDstLine = 0;
   lastcount = pPriv->illinecount - 1;

   for ( y = lastcount;  y < (lastcount + *pNLines);  y++, pPriv->illinecount++) {

       if ((y >= pPriv->ilCropYoff) && (y < yextent)) {    /* process */
              psrc = psrcline;
              pdst = pdstline;

              for( x = 0; x < xextent;  x++)  {
                   if  ( x < pPriv->ilCropXoff )  psrc++;
                   else  *pdst++ = *psrc++;
              }
              pdstline += dstnbytes;
              local_noDstLine++;
       }
       psrcline += srcnbytes;

   }  /* end loop */


   *pNLines = local_noDstLine;
   return IL_OK;

}



/* =============================================================================================================================

      ilCrop   -  Public function.  Insert a pipe element which crops to the given rectangle. 
                   Check for format types and do an explicit conversion if necessary.

   ============================================================================================================================= */
ilBool ilCrop (
    ilPipe              pipe,
    ilRect             *pRect
    )
{
unsigned int            state;
ilPipeInfo              info;
ilCropPrivptr  pPriv;
ilDstElementData        dstdata;
ilImageDes              imdes;
ilImageFormat           imformat;
ilBool                  convert;
ilBool                  bitonal;
ilRect                  Srcrect, Dstrect;


       /* Get ptr to pipe info and check state: don't decompress! */
       state = ilGetPipeInfo(pipe, FALSE, &info, &imdes, &imformat);
       if(state != IL_PIPE_FORMING) {
         if (!pipe->context->error)
             ilDeclarePipeInvalid(pipe, IL_ERROR_PIPE_STATE);
         return FALSE;
       }

      if (!pRect)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_NULL_RECT_PTR);

       /* Clip the specified crop rectangle within the Src rectangle */
       Srcrect.x      =  0;
       Srcrect.y      =  0;
       Srcrect.width  =  info.width;
       Srcrect.height =  info.height;
       Dstrect        =  *pRect;        /* bug #0067: don't modify caller's copy */
       _ilIntersectRect ( &Srcrect, &Dstrect );
    
       /* If crop rect = image rect; a noop - return.  If null crop rect, error. */
       if ( (Srcrect.height == Dstrect.height) && (Srcrect.width == Dstrect.width) &&
            (Dstrect.x == 0) && (Dstrect.y == 0) )     return TRUE;
       if ( (Dstrect.width <= 0) || (Dstrect.height <= 0) )
            return ilDeclarePipeInvalid (pipe, IL_ERROR_ZERO_SIZE_IMAGE);

       /* If the pipe image is compressed, handle separately.  Code will crop
          compressed data by ignoring strips outside of the range of the crop rect.
          It will then force decompression of the pipe image and call ilCrop()
          to crop within the strips.
       */
       if (imdes.compression != IL_UNCOMPRESSED)
           return ilCropCompressed (pipe, &Dstrect, &info);

       /* Check for valid Formats */

       bitonal = FALSE;
       convert = FALSE;

       switch (imdes.nSamplesPerPixel)  {
            case  3: /* RGB or YUV */  
                     if(imformat.sampleOrder != IL_SAMPLE_PIXELS)  { 
                           imformat.sampleOrder = IL_SAMPLE_PIXELS;
                           convert = TRUE;
                     }

                     if((imformat.nBitsPerSample[0] != 8) ||
                        (imformat.nBitsPerSample[1] != 8) ||
                        (imformat.nBitsPerSample[2] != 8))  {

                          imformat.nBitsPerSample[0] = 8;
                          imformat.nBitsPerSample[1] = 8;
                          imformat.nBitsPerSample[2] = 8;
                          convert = TRUE;
                     }
                     break;

            case  1:
                     switch (imformat.nBitsPerSample[0]) {
                             case 8:  /* Byte per pixel */
                                      break;

                             case 1:  /* Bitonal */
                                      bitonal = TRUE; 
                                      if (imformat.rowBitAlign != 32) {
                                         imformat.rowBitAlign = 32;
                                         convert = TRUE;
                                      }
                                      break;

                             default: /* something other than 1 - try 8 */
                                      imformat.nBitsPerSample[0] = 8;
                                      convert = TRUE;
                     }  
                     break;

            default:
                      return ilDeclarePipeInvalid(pipe, IL_ERROR_NOT_IMPLEMENTED);
       }


       if(convert && !ilConvert(pipe, &imdes, &imformat, 0, NULL))
           return FALSE;

       dstdata.producerObject = (ilObject) NULL;
       dstdata.pDes           = (ilImageDes *) NULL;
       dstdata.pFormat        = (ilImageFormat *) NULL;
       dstdata.width          = Dstrect.width;
       dstdata.height         = Dstrect.height;

       /* set output strip height */
       dstdata.stripHeight    = info.recommendedStripHeight;
       dstdata.constantStrip  = FALSE;


       switch (imdes.nSamplesPerPixel) {
            case 3:
                            pPriv = (ilCropPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilCropPriv), 0, (ilSrcElementData *) NULL,
                                                                     &dstdata, ilCropInit, IL_NPF, IL_NPF, ilCrop3ByteExecute, NULL, 0);
                     break;

            case 1:  
                     if(bitonal) 
                            pPriv = (ilCropPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilCropPriv), 0, (ilSrcElementData *) NULL,
                                                                     &dstdata, ilCropInit, IL_NPF, IL_NPF, ilCropBitonalExecute, NULL, 0);
                     else 
                            pPriv = (ilCropPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilCropPriv), 0, (ilSrcElementData *) NULL,
                                                                     &dstdata, ilCropInit, IL_NPF, IL_NPF, ilCropByteExecute, NULL, 0);
       }

       if(!pPriv)
           return FALSE;

       /* Save away true heights and offsets */
       pPriv->ilCropSrcheight       = info.height;
       pPriv->ilCropDstheight       = Dstrect.height;
       pPriv->ilCropXoff            = Dstrect.x;
       pPriv->ilCropYoff            = Dstrect.y;


       pipe->context->error = IL_OK;
       return TRUE;
}
