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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ilycbcr.c /main/5 1996/06/19 12:20:22 ageorge $ */
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

        /*  /ilc/ilycbcr.c : Code for handling IL_YCBCR images, including
            conversions to/from RGB, and subsampling - upsampling is in /ilc/ilupsample.c
        */
#include <stdlib.h>

#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"

    /*  ========================== Sub/Upsample Code ================================== */

#define NSUBSAMPLES         3               /* hardwired to 3 samples: Y, Cb, Cr */

    /*  Sub/Upsubsample factors as shifts (1=0, 2=1, 4=2) - others not supported. */
typedef struct {
    unsigned int        horiz, vert;
    } ilSubsampleShiftRec, *ilSubsampleShiftPtr;

    /*  Private for sub/upsampling filters. */
typedef struct {
    int                 nSamples;           /* # of samples (components) to process */
    ilSubsampleShiftRec shift[NSUBSAMPLES]; /* upsample mul as shift value */
    } ilSubsamplePrivRec, *ilSubsamplePrivPtr;


    /*  ========================== Subsample Code =================================== */

    /*  -------------------------- ilExecuteSubsample ------------------------------- */
    /*  Execute(): subsample as necessary "pPriv->nSamples" planes of the source image.
        The source image must be planar, byte/pixel.
    */
static ilError ilExecuteSubsample (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
register ilSubsamplePrivPtr pPriv;
ilImagePlaneInfo           *pSrcPlane, *pDstPlane;
int                         nSamples;
ilPtr                       pSrcLine, pDstLine;
long                        height, width;
long                        srcRowInc, dstRowBytes;
long                        nDstLinesM1, nDstPixelsM1Init;
ilSubsampleShiftPtr          pShift;
register long               nDstPixelsM1, srcRowBytes, i;
register ilPtr              pSrc, pDst;
register unsigned int       pixel;

        /*  Get width and height of _upsampled_ image; exit if zero. */
    pPriv = (ilSubsamplePrivPtr)pData->pPrivate;
    height = *pNLines;
    if (height <= 0)
        return IL_OK;
    width = pData->pSrcImage->width;
    if (width <= 0)
        return IL_OK;

        /*  Loop on samples (components), upsample/translate each plane separately.
            Note that "srcLine" is shifted - indexing into plane based on vert subsample.
        */
    for (pSrcPlane = pData->pSrcImage->plane, pDstPlane = pData->pDstImage->plane,
      pShift = pPriv->shift, nSamples = pPriv->nSamples; 
      nSamples-- > 0; 
      pSrcPlane++, pDstPlane++, pShift++) {

            /*  Get # dst lines/pixels across - 1; skip plane if none */
        nDstLinesM1 = height >> pShift->vert;
        if (nDstLinesM1 <= 0)
            continue;
        nDstLinesM1--;
        nDstPixelsM1Init = width >> pShift->horiz;
        if (nDstPixelsM1Init <= 0)
            continue;
        nDstPixelsM1Init--;

            /*  Point to src/dst; note dstLine scaled down by vert subsample shift.
                Set srcRowInc to srcRowBytes scaled up by vertical shift.
            */
        srcRowBytes = pSrcPlane->nBytesPerRow;
        pSrcLine = pSrcPlane->pPixels + pData->srcLine * srcRowBytes;
        srcRowInc = srcRowBytes << pShift->vert;
        dstRowBytes = pDstPlane->nBytesPerRow;
        pDstLine = pDstPlane->pPixels + (dstLine >> pShift->vert) * dstRowBytes;

        switch (pShift->horiz) {            /* NOTE: vert shift must be the same! */

            /*  0 = no subsampling; just copy plane info */
          case 0:
            pSrc = pSrcLine;
            pDst = pDstLine;
            do {
                bcopy ((char *)pSrc, (char *)pDst, width);
                pSrc += srcRowBytes;
                pDst += dstRowBytes;
                } while (--nDstLinesM1 >= 0);
            break;

            /*  1 = 1/4 of pixels (half in each dimension). Take the average of 4
                pixels: (x,y), (x+1,y), (x,y+1), (x+1,y+1), for each even x,y.
            */
          case 1:
            do {
                pSrc = pSrcLine;
                pSrcLine += srcRowInc;
                pDst = pDstLine;
                pDstLine += dstRowBytes;
                nDstPixelsM1 = nDstPixelsM1Init;
                do {
                    pixel = *pSrc;
                    pixel += *(pSrc + 1);
                    pixel += *(pSrc + srcRowBytes);
                    pixel += *(pSrc + srcRowBytes + 1);
                    pSrc += 2;                  /* two pixels to the right */
                    *pDst++ = pixel >> 2;       /* store 1/4 sum of 4 pixels */
                    } while (--nDstPixelsM1 >= 0);
                } while (--nDstLinesM1 >= 0);
            break;

            /*  2 = 1/16 of pixels (1/4 in each dimension). Take the average of 16 pixels,
                a block below and to the right of (x,y), x,y = multiple of 4.
            */
          case 2:
            do {
                pSrc = pSrcLine;
                pSrcLine += srcRowInc;
                pDst = pDstLine;
                pDstLine += dstRowBytes;
                nDstPixelsM1 = nDstPixelsM1Init;
                do {
                    pixel = 0;
                    i = 3;
                    do {
                        pixel += *pSrc;
                        pixel += *(pSrc + 1);
                        pixel += *(pSrc + 2);
                        pixel += *(pSrc + 3);
                        pSrc += srcRowBytes;
                        } while (--i >= 0);
                    *pDst++ = pixel >> 4;       /* store 1/16 sum of 16 pixels */
                    pSrc += -srcRowInc + 4;     /* back to top line, 4 to right */
                    } while (--nDstPixelsM1 >= 0);
                } while (--nDstLinesM1 >= 0);
            break;

            }   /* END switch subsample shift */
        }       /* END each plane */

    return IL_OK;
}


    /*  ---------------------------- ilSubsampleYCbCr ----------------------------- */
    /*  Subsample the pipe image which must be a fully upsampled YCbCr image.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format.  *pDes to the _destination_ des, i.e. has subsample values.
    */
IL_PRIVATE ilBool _ilSubsampleYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    )
{
    register ilSubsamplePrivPtr pSubPriv;
    ilSubsamplePrivRec       priv;
    ilSubsampleShiftRec     *pShift;
    ilYCbCrSampleInfo      *pSample;
    int                     sample;
    ilDstElementData        dstData;
    ilSrcElementData        srcData;

        /*  Only 8 bit planar YCbCr current handled; cvt if not that format. */
    if ((pFormat->sampleOrder != IL_SAMPLE_PLANES)
     || (pFormat->nBitsPerSample[0] != 8)
     || (pFormat->nBitsPerSample[1] != 8)
     || (pFormat->nBitsPerSample[2] != 8)) {
        *pFormat = *IL_FORMAT_3BYTE_PLANE;
        if (!ilConvert (pipe, (ilImageDes *)NULL, pFormat, 0, NULL))
            return FALSE;
        }

        /*  Determine subsample values: only 1 (no subsampling), 2 (1/4 = 1/2 in each 
            dimension) or 4 (1/16) allowed; same both vertical/horizontal.
        */
    priv.nSamples = NSUBSAMPLES;
    pSample = pDes->typeInfo.YCbCr.sample;
    pShift = priv.shift;
    for (sample = 0; sample < NSUBSAMPLES; sample++, pShift++, pSample++) {
        if (pSample->subsampleHoriz != pSample->subsampleVert)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_SUBSAMPLE);
        switch ((unsigned int)pSample->subsampleHoriz) {
          case 1: pShift->horiz = 0; break;
          case 2: pShift->horiz = 1; break;
          case 4: pShift->horiz = 2; break;
          default: return ilDeclarePipeInvalid (pipe, IL_ERROR_SUBSAMPLE);
            }
        pShift->vert = pShift->horiz;       /* same subsampling both dimensions */
        }

        /*  Init dstData, add subsample filter. */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
 
        /*  Demand constant strips, a multiple of maximum subsample (4). */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = 8;                /* Arbitrary, but must be multiple of 4 */
    srcData.constantStrip = TRUE;
    srcData.minBufferHeight = 0;

    pSubPriv = (ilSubsamplePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                 sizeof (ilSubsamplePrivRec), 0, &srcData,
                 &dstData, IL_NPF, IL_NPF, IL_NPF, ilExecuteSubsample, NULL, 0);
    if (!pSubPriv)
        return FALSE;
    *pSubPriv = priv;                   /* copy priv to priv ptr */
    return TRUE;
}



    /*  ======================= YCbCr to RGB Code =================================== */

    /*  Lookup tables for YCbCr to RGB conversion.  Each table is indexed by Y, Cb or Cr,
        and contains two values; one the upper short (1st) the other in the lower (2nd);
        the upper short values are used to compute green.
            Y   G cvt: Y factor = (1 - Lb - Lr)/Lg      scale for refBlack/White
            Cb  G cvt: Cb factor = -Lb*(2 - 2*Lb)/Lg    Cb scale * (2 - 2 * Lr)
            Cr  G cvt: Cr factor = -Lr*(2 - 2*Lr)/Lg    Cr scale * (2 - 2 * Lb)
    */
typedef struct {
    long        YTrans[256];        /* values indexed by Y, as above */
    long        CbTrans[256];       /* values indexed by Cb, as above */
    long        CrTrans[256];       /* values indexed by Cr, as above */
    } ilRGBTransRec, *ilRGBTransPtr;

    /*  Private for RGB to YCbCr filter. */
typedef struct {
    ilRGBTransPtr   pTrans;         /* ptr to all the translate tables */
    long            y;              /* for dither only: current Y */
    } ilYCbCrToRGBPrivRec, *ilYCbCrToRGBPrivPtr;

    /*  Pointer to "std" translation table: works for "std" YCbCr values -
        those in IL_DES_YCBCR.
    */
static ilRGBTransPtr _ilStdYCbCrTransPtr = (ilRGBTransPtr)NULL;


    /*  --------------------------- ilDestroyTransTable --------------------- */
    /*  Destroy function: used only if pPriv->pTrans points to locally malloc'd
        translate table, i.e. not using std values and table.
    */
static ilError ilDestroyTransTable (
    ilYCbCrToRGBPrivPtr pPriv
    )
{
    IL_FREE (pPriv->pTrans);
    return IL_OK;
}

    /*  --------------------- ilExecutePlanarYCbCrToRGB ----------------------------- */
    /*  Execute(): convert _planar_ src YCbCr to _pixel_ dst RGB.
        NOTE: the YCbCr image must have been upsampled, i.e. thru ilExecuteUpsample().
    */
static ilError ilExecutePlanarYCbCrToRGB (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilPtr                       pYLine, pCbLine, pCrLine, pDstLine;
register ilRGBTransPtr      pTrans;
register ilPtr              pY, pCb, pCr, pDst;
register long               YTrans, CbTrans, CrTrans, pixel, nPixelsM1;
register int                Y;
long                        nPixelsM1Init, nLinesM1;
long                        YRowBytes, CbRowBytes, CrRowBytes, dstRowBytes;
ilImagePlaneInfo           *pPlane;


        /*  Set nPixels/LinesM1 to # pixels / lines - 1; exit if either 0. */
    nPixelsM1Init = pData->pSrcImage->width;
    if (nPixelsM1Init <= 0)
        return;
    nPixelsM1Init--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return;
    nLinesM1--;

        /*  Point pY/Cb/CrLine to 1st line in src planes; pDstLine to 1st line in dst.
            Get bytes/row for each of them.
        */
    pPlane = pData->pSrcImage->plane;
    YRowBytes = pPlane->nBytesPerRow;
    pYLine = pPlane->pPixels + pData->srcLine * YRowBytes;
    pPlane++;
    CbRowBytes = pPlane->nBytesPerRow;
    pCbLine = pPlane->pPixels + pData->srcLine * CbRowBytes;
    pPlane++;
    CrRowBytes = pPlane->nBytesPerRow;
    pCrLine = pPlane->pPixels + pData->srcLine * CrRowBytes;

    pPlane = pData->pDstImage->plane;
    dstRowBytes = pPlane->nBytesPerRow;
    pDstLine = pPlane->pPixels + dstLine * dstRowBytes;

        /*  Convert planar YCbCr to pixel RGB. */
    pTrans = ((ilYCbCrToRGBPrivPtr)pData->pPrivate)->pTrans;
    do {
        pY = pYLine;
        pYLine += YRowBytes;
        pCb = pCbLine;
        pCbLine += CbRowBytes;
        pCr = pCrLine;
        pCrLine += CrRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;

        nPixelsM1 = nPixelsM1Init;
        do {
            YTrans = pTrans->YTrans[*pY++];
            CrTrans = pTrans->CrTrans[*pCr++];
            Y = ((short)YTrans);
            CbTrans = pTrans->CbTrans[*pCb++];
            pixel = Y + ((short)CrTrans);
            if (pixel >> 8) goto plClipR;
plRClipped: *pDst++ = pixel;                /* store red */
            pixel = YTrans >> 16;
            pixel += CbTrans >> 16;
            pixel += CrTrans >> 16;
            if (pixel >> 8) goto plClipG;
plGClipped: *pDst++ = pixel;                /* store green */
            pixel = Y + ((short)CbTrans);
            if (pixel >> 8) goto plClipB;
plBClipped: *pDst++ = pixel;                /* store blue */
            } while (--nPixelsM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;

    /*  goto point for when R, G or B out of range: clamp pixel and go back and continue.
        This is done to avoid taking a branch in the most common case: not out of range.
    */
plClipR: if (pixel > 255) pixel = 255; else pixel = 0;
        goto plRClipped;
plClipG: if (pixel > 255) pixel = 255; else pixel = 0;
        goto plGClipped;
plClipB: if (pixel > 255) pixel = 255; else pixel = 0;
        goto plBClipped;
}


    /*  --------------------- ilExecutePixelYCbCrToRGB ----------------------------- */
    /*  Execute(): convert _pixel_ src YCbCr to _pixel_ dst RGB.
        NOTE: the YCbCr image must have been upsampled, i.e. thru ilExecuteUpsample().
    */
static ilError ilExecutePixelYCbCrToRGB (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilPtr                       pSrcLine, pDstLine;
register ilRGBTransPtr      pTrans;
register ilPtr              pSrc, pDst;
register long               YTrans, CbTrans, CrTrans, pixel, nPixelsM1;
register int                Y;
long                        nPixelsM1Init, nLinesM1;
long                        srcRowBytes, dstRowBytes;
ilImagePlaneInfo           *pPlane;


        /*  Set nPixels/LinesM1 to # pixels / lines - 1; exit if either 0. */
    nPixelsM1Init = pData->pSrcImage->width;
    if (nPixelsM1Init <= 0)
        return;
    nPixelsM1Init--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return;
    nLinesM1--;

        /*  Point pY/Cb/CrLine to 1st line in src planes; pDstLine to 1st line in dst.
            Get bytes/row for each of them.
        */
    pPlane = pData->pSrcImage->plane;
    srcRowBytes = pPlane->nBytesPerRow;
    pSrcLine = pPlane->pPixels + pData->srcLine * srcRowBytes;

    pPlane = pData->pDstImage->plane;
    dstRowBytes = pPlane->nBytesPerRow;
    pDstLine = pPlane->pPixels + dstLine * dstRowBytes;

        /*  Convert pixel YCbCr to pixel RGB. */
    pTrans = ((ilYCbCrToRGBPrivPtr)pData->pPrivate)->pTrans;
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;

        nPixelsM1 = nPixelsM1Init;
        do {
            YTrans = pTrans->YTrans[*pSrc++];
            CbTrans = pTrans->CbTrans[*pSrc++];
            Y = ((short)YTrans);
            CrTrans = pTrans->CrTrans[*pSrc++];
            pixel = Y + ((short)CrTrans);
            if (pixel >> 8) goto piClipR;
piRClipped: *pDst++ = pixel;                /* store red */
            pixel = YTrans >> 16;
            pixel += CbTrans >> 16;
            pixel += CrTrans >> 16;
            if (pixel >> 8) goto piClipG;
piGClipped: *pDst++ = pixel;                /* store green */
            pixel = Y + ((short)CbTrans);
            if (pixel >> 8) goto piClipB;
piBClipped: *pDst++ = pixel;                /* store blue */
            } while (--nPixelsM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;

    /*  goto point for when R, G or B out of range: clamp pixel and go back and continue.
        This is done to avoid taking a branch in the most common case: not out of range.
    */
piClipR: if (pixel > 255) pixel = 255; else pixel = 0;
        goto piRClipped;
piClipG: if (pixel > 255) pixel = 255; else pixel = 0;
        goto piGClipped;
piClipB: if (pixel > 255) pixel = 255; else pixel = 0;
        goto piBClipped;
}


    /*  ------------------------- ilGetYCbCrConvertTable -------------------------- */
    /*  Returns a pointer to the YCbCr conversion table (ilRGBTransRec); either
        "_ilStdYCbCrTransPtr" or a ptr to a table that caller must destroy, or
        null if a malloc error occurs.
    */
static ilRGBTransPtr ilGetYCbCrConvertTable (
    ilImageDes             *pDes
    )
{
    register ilRGBTransPtr  pTrans;
    const ilYCbCrInfo      *pYCbCr, *pStdYCbCr;
    double                  Lr, Lg, Lb, scaledY;
    register double         factor, gFactor;
    register int            i, refBlack, upper, lower;
    ilBool                  isStd;
    struct {
        int                 refBlack;
        double              scale;
        } sample [3];

        /*  If luma and refBlack/White info same as std IL_DES_YCBCR, then reuse
            the table at _ilStdYCbCrTransPtr; create it here if does not exist yet.
        */
    pYCbCr = &pDes->typeInfo.YCbCr;
    pStdYCbCr = &IL_DES_YCBCR->typeInfo.YCbCr;
    if ((pYCbCr->sample[0].refBlack == pStdYCbCr->sample[0].refBlack)
     && (pYCbCr->sample[0].refWhite == pStdYCbCr->sample[0].refWhite)
     && (pYCbCr->sample[1].refBlack == pStdYCbCr->sample[1].refBlack)
     && (pYCbCr->sample[1].refWhite == pStdYCbCr->sample[1].refWhite)
     && (pYCbCr->sample[2].refBlack == pStdYCbCr->sample[2].refBlack)
     && (pYCbCr->sample[2].refWhite == pStdYCbCr->sample[2].refWhite)
     && (pYCbCr->lumaRed == pStdYCbCr->lumaRed)
     && (pYCbCr->lumaGreen == pStdYCbCr->lumaGreen)
     && (pYCbCr->lumaBlue == pStdYCbCr->lumaBlue)) {
        isStd = TRUE;
        pTrans = _ilStdYCbCrTransPtr;       /* alloc below if not there yet */
        }
    else {
        isStd = FALSE;
        pTrans = (ilRGBTransPtr)NULL;       /* allocate it below */
        }

        /*  Build translate tables.  First, from pg 3 of Appendix O of TIFF spec:
                R = Cr * (2 - 2 * Lr) + Y
                B = Cb * (2 - 2 * Lb) + Y
                G = (Y - Lb * B - Lr * R) / Lg
            Get green in terms of Cb and Cr:
                G = (Y - Lb*B - Lr*R) / Lg
                G = (Y - Lb*(Cb * (2 - 2*Lb) + Y) - Lr*(Cr * (2 - 2*Lr) + Y)) / Lg
                G = (Y - Lb*Cb*(2 - 2*Lb) - Lb*Y - Lr*Cr*(2 - 2*Lr) - Lr*Y) / Lg
                G = (Y - Lb*Y - Lr*Y - Cb*Lb*(2 - 2*Lb) - Cr*Lr*(2 - 2*Lr)) / Lg
                G = (Y * (1 - Lb - Lr) - Cb*Lb*(2 - 2*Lb) - Cr*Lr*(2 - 2*Lr)) / Lg
                G = Y * (1 - Lb - Lr)/Lg + Cb * -Lb*(2 - 2*Lb)/Lg + Cr * -Lr*(2 - 2*Lr)/Lg
        */
    if (!pTrans) {                      /* non-std or std table not build yet */
        pTrans = (ilRGBTransPtr)IL_MALLOC (sizeof (ilRGBTransRec));
        if (!pTrans)
            return pTrans;

            /*  The Y, Cb and Cr components must be scaled up based on refBlack/White.
                Factor that scaling in everywhere Y/Cb/Cr are looked up.
                First get the scale and refBlack for each sample.
            */
        for (i = 0; i < 3; i++) {
            sample[i].refBlack = pYCbCr->sample[i].refBlack;
            sample[i].scale = ((i == 0) ? 255 : 127) / 
                              (pYCbCr->sample[i].refWhite - pYCbCr->sample[i].refBlack);
            }

            /*  Calc the Cb/Cr lookup tables, factoring in their scale factors. */
        Lr = ((double)pYCbCr->lumaRed / (double)10000);
        Lg = ((double)pYCbCr->lumaGreen / (double)10000);
        Lb = ((double)pYCbCr->lumaBlue / (double)10000);

            /*  Y translation: lower is scaled Y, upper is value added to get green */
        gFactor = (1 - Lb - Lr) / Lg;
        for (i = 0; i < 256; i++) {
            scaledY = sample[0].scale * i - sample[0].refBlack;
            upper = scaledY * gFactor + 0.5;
            lower = scaledY;
            pTrans->YTrans[i] = (upper << 16) | (lower & 0xffff);
            }

            /*  Cb: lower is added to Y to get blue, upper added to get green */
        factor = (2 - 2 * Lb) * sample[1].scale;
        gFactor = -Lb * (2 - 2 * Lb) / Lg;
        refBlack = sample[1].refBlack;
        for (i = 0; i < 256; i++) {
            upper = (i - refBlack) * gFactor + 0.5;
            lower = (i - refBlack) * factor + 0.5;
            pTrans->CbTrans[i] = (upper << 16) | (lower & 0xffff);
            }

            /*  Cr: lower is added to Y to get red, upper added to get green */
        factor = (2 - 2 * Lr) * sample[2].scale;
        gFactor = -Lr * (2 - 2 * Lr) / Lg;
        refBlack = sample[2].refBlack;
        for (i = 0; i < 256; i++) {
            upper = (i - refBlack) * gFactor + 0.5;
            lower = (i - refBlack) * factor + 0.5;
            pTrans->CrTrans[i] = (upper << 16) | (lower & 0xffff);
            }

            /*  If creating "std" conversion table for first time, set ptr to it */
        if (isStd)
            _ilStdYCbCrTransPtr = pTrans;
        }   /* END build table */

    return pTrans;
}


    /*  ---------------------------- ilConvertYCbCrToRGB ----------------------------- */
    /*  Convert from source type (pDes->type) == IL_YCBCR to RBB.  
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format, *pDes to the dst descriptor.
    */

IL_PRIVATE ilBool _ilConvertYCbCrToRGB (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    )
{
    ilDstElementData        dstData;
    ilError                 (*executeFunction)(), (*destroyFunction)();
    ilYCbCrToRGBPrivPtr     pPriv;
    register ilRGBTransPtr  pTrans;

    if ((pFormat->nBitsPerSample[0] != 8)
     || (pFormat->nBitsPerSample[1] != 8)
     || (pFormat->nBitsPerSample[2] != 8))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);

        /*  Handle planar/pixel src format with different executeFunction, but in 
            either case output RGB in 3 byte pixel format.
        */
    if (pFormat->sampleOrder == IL_SAMPLE_PIXELS)
        executeFunction = ilExecutePixelYCbCrToRGB;
    else {
        executeFunction = ilExecutePlanarYCbCrToRGB;
        *pFormat = *IL_FORMAT_3BYTE_PIXEL;
        }

        /*  Get translation table or error; destroy it if not the "std" one */
    pTrans = ilGetYCbCrConvertTable (pDes);
    if (!pTrans)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
    destroyFunction = (pTrans == _ilStdYCbCrTransPtr) ? IL_NPF : ilDestroyTransTable;

        /*  Add element to the pipe, init private */
    dstData.producerObject = (ilObject)NULL;
    *pDes = *IL_DES_RGB;
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = (ilYCbCrToRGBPrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
            sizeof(ilYCbCrToRGBPrivRec), 0, (ilSrcElementData *)NULL, &dstData, 
            IL_NPF, IL_NPF, destroyFunction, executeFunction, NULL, 0);
    if (!pPriv) {
        if (destroyFunction)        /* local copy of pTrans; free it */
            IL_FREE (pTrans);
        return FALSE;
        }
    pPriv->pTrans = pTrans;

    return TRUE;
}


    /*  ======================= Dither YCbCr Code =================================== */

    /*  Defines for # bits of RGB to dither to. NOTE! however that these are not
        arbitrary; # bits of red must = # bits of green, because the pre-dither-mul'd
        value of "Y" is the same for both.
    */
#define YCBCR2_NR_BITS      2                   /* # of bits of R/G/B to dither to */
#define YCBCR2_NG_BITS      3
#define YCBCR2_NB_BITS      2
#define YCBCR2_MAX_R        ((1 << YCBCR2_NR_BITS) - 1)     /* max value for R/G/B */
#define YCBCR2_MAX_G        ((1 << YCBCR2_NG_BITS) - 1)
#define YCBCR2_MAX_B        ((1 << YCBCR2_NB_BITS) - 1)

    /*  Private for RGB to dithered pixel filter. */
typedef struct {
    ilRGBTransRec   trans;          /* YCbCr->dither-mul'd values tables */
    ilPtr           pTranslate;     /* ptr to ilMap() lookup table pixels */
    unsigned short *pPalette;       /* ptr to palette to destroy or null */
    long            y;              /* Init(): current Y within dst image */
    ilByte          translate [256];        /* identity image if no mapImage given */
    } ilYCbCr2DitherPrivRec, *ilYCbCr2DitherPrivPtr;

        /*  Init() function: init the counter of "y" within private */
static ilError ilYCbCr2DitherInit (
    ilYCbCr2DitherPrivPtr pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->y = 0;
    return IL_OK;
}

    /*  Destroy function: destroy pTrans if not std and pPalette if not null */
static ilError ilYCbCr2DitherDestroy (
    ilYCbCr2DitherPrivPtr pPriv
    )
{
    if (pPriv->pPalette)
        IL_FREE (pPriv->pPalette);
    return IL_OK;
}


        /*  ---------------------- ilYCbCr2DitherExecute ------------------------ */
        /*  Execute() function: dither from subsampled-by-2 YCbCr to 8 bit pixels.
            Dithering is done to RGB levels of 484 (232 in bits).
        */
static ilError ilYCbCr2DitherExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilYCbCr2DitherPrivPtr       pPriv;
ilImagePlaneInfo           *pPlane;
long                        nLinesDiv2, halfWidthM1, y;
long                        CbRowBytes, CrRowBytes;
ilPtr                       pYLine, pCbLine, pCrLine, pDstLine;
int                        *pKernelRowEnd;
long                        nPixelsDiv2M1;
ilPtr                       pCb, pCr;
register long               YRowBytes, dstRowBytes;
register long               Y, Cb, Cr, YTrans, CbTrans, CrTrans, comp, pixel;
register ilPtr              pY, pDst;
register ilRGBTransPtr      pTrans;
register ilPtr              pTranslate;
register int               *pKernel, kernel;
#define YCBCR2_KERNEL_SIZE  8                   /* size of dither kernel used */

        /*  This filter handles a pipe image of YCbCr subsampled by 2 in Cb/Cr only.
            The # of lines of Cb/Cr is therefore 1/2 the # of lines of Y.
            Note: can only handle images with even width/height; checked elsewhere.
        */
    pPlane = pData->pSrcImage->plane;
    YRowBytes = pPlane->nBytesPerRow;
    pYLine = pPlane->pPixels;
    pPlane++;
    CbRowBytes = pPlane->nBytesPerRow;
    pCbLine = pPlane->pPixels;
    pPlane++;
    CrRowBytes = pPlane->nBytesPerRow;
    pCrLine = pPlane->pPixels;
    if (pData->srcLine) {
        pYLine  += pData->srcLine * YRowBytes;
        pCbLine += (pData->srcLine >> 1) * CbRowBytes;
        pCrLine += (pData->srcLine >> 1) * CrRowBytes;
        }
    pPlane = pData->pDstImage->plane;
    dstRowBytes = pPlane->nBytesPerRow;
    pDstLine = pPlane->pPixels + dstLine * dstRowBytes;

    pPriv = (ilYCbCr2DitherPrivPtr)pData->pPrivate;
    halfWidthM1 = pData->pSrcImage->width >> 1; /* 1/2 # of pixels - 1 */
    nLinesDiv2 = *pNLines >> 1;
    if ((halfWidthM1 <= 0) || (nLinesDiv2 <= 0))
        return IL_OK;
    halfWidthM1 -= 1;
    y = pPriv->y;
    pPriv->y += *pNLines;

    pTrans = &pPriv->trans;
    pTranslate = pPriv->pTranslate;

        /*  Do 4 pixels at a time, using 4 Ys (below L, L, below R, R) for each Cb,Cr pair.
            For each Y, do one pixel: get "comp" (RGB) as done in other YCbCr->RGB
            filters, except that all values are pre-multiplied by what the dither tables
            would yield if the lookup to _ilMul4/8[] was done.  "pixel" is then used as
            an index into pTranslate table to get pixel to write to dst.
                See comments in ilGetYCbCrDitherTable() for details.
        */
    while (nLinesDiv2-- > 0) {
        pY = pYLine;
        pYLine += YRowBytes << 1;       /* skip 2 lines; read 2 lines each loop */
        pCb = pCbLine;
        pCbLine += CbRowBytes;
        pCr = pCrLine;
        pCrLine += CrRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes << 1;   /* skip 2 lines; write 2 lines each loop */
        nPixelsDiv2M1 = halfWidthM1;

            /*  Point to even row within kernel based on "y"; point to end of row */
        pKernel = (int *)&_ilDitherKernel[(y & 6) * YCBCR2_KERNEL_SIZE];
        y += 2;
        pKernelRowEnd = pKernel + 8;

            /*  Dither 4 pixels, using one set of Cb,Cr and 4 Y's. */
        do {
            CrTrans = pTrans->CrTrans[*pCr++];
            CbTrans = pTrans->CbTrans[*pCb++];
            Cr = ((short)CrTrans);
            CrTrans >>= 16;
            Cb = ((short)CbTrans);
            CbTrans >>= 16;
            CbTrans += CrTrans;                 /* add Cb, Cr contributions to green */

                /*  Do pixel below */
            YTrans = pTrans->YTrans[pY[YRowBytes]];
            kernel = pKernel[YCBCR2_KERNEL_SIZE];
            Y = ((short)YTrans);
            Y += kernel;                        /* pre-add kernel to Y */
            pixel = (Y + Cr) >> 8;
            if (pixel >> YCBCR2_NR_BITS) goto d0ClipR;
d0RClipped: comp = ((YTrans >> 16) + CbTrans + kernel) >> 8;
            if (comp >> YCBCR2_NG_BITS) goto d0ClipG;
d0GClipped: pixel = (pixel << YCBCR2_NG_BITS) + comp;
            comp = (Y + Cb) >> 8;
            if (comp >> YCBCR2_NB_BITS) goto d0ClipB;
d0BClipped: pixel = (pixel << YCBCR2_NB_BITS) + comp;
            pDst[dstRowBytes] = pTranslate[pixel];

                /*  Do pixel, move pKernel, pY, pDst one to the right */
            YTrans = pTrans->YTrans[*pY++];
            kernel = *pKernel++;
            Y = ((short)YTrans);
            Y += kernel;
            pixel = (Y + Cr) >> 8;
            if (pixel >> YCBCR2_NR_BITS) goto d1ClipR;
d1RClipped: comp = ((YTrans >> 16) + CbTrans + kernel) >> 8;
            if (comp >> YCBCR2_NG_BITS) goto d1ClipG;
d1GClipped: pixel = (pixel << YCBCR2_NG_BITS) + comp;
            comp = (Y + Cb) >> 8;
            if (comp >> YCBCR2_NB_BITS) goto d1ClipB;
d1BClipped: pixel = (pixel << YCBCR2_NB_BITS) + comp;
            *pDst++ = pTranslate[pixel];

                /*  Same as above two steps for one pixel to the right */
            YTrans = pTrans->YTrans[pY[YRowBytes]];
            kernel = pKernel[YCBCR2_KERNEL_SIZE];
            Y = ((short)YTrans);
            Y += kernel;                        /* pre-add kernel to Y */
            pixel = (Y + Cr) >> 8;
            if (pixel >> YCBCR2_NR_BITS) goto d2ClipR;
d2RClipped: comp = ((YTrans >> 16) + CbTrans + kernel) >> 8;
            if (comp >> YCBCR2_NG_BITS) goto d2ClipG;
d2GClipped: pixel = (pixel << YCBCR2_NG_BITS) + comp;
            comp = (Y + Cb) >> 8;
            if (comp >> YCBCR2_NB_BITS) goto d2ClipB;
d2BClipped: pixel = (pixel << YCBCR2_NB_BITS) + comp;
            pDst[dstRowBytes] = pTranslate[pixel];

                /*  Do pixel, move pKernel, pY, pDst one to the right */
            YTrans = pTrans->YTrans[*pY++];
            kernel = *pKernel++;
            Y = ((short)YTrans);
            Y += kernel;
            pixel = (Y + Cr) >> 8;
            if (pixel >> YCBCR2_NR_BITS) goto d3ClipR;
d3RClipped: comp = ((YTrans >> 16) + CbTrans + kernel) >> 8;
            if (comp >> YCBCR2_NG_BITS) goto d3ClipG;
d3GClipped: pixel = (pixel << YCBCR2_NG_BITS) + comp;
            comp = (Y + Cb) >> 8;
            if (comp >> YCBCR2_NB_BITS) goto d3ClipB;
d3BClipped: pixel = (pixel << YCBCR2_NB_BITS) + comp;
            *pDst++ = pTranslate[pixel];

                /*  If pKernel off end of row, reset back (-8) to beginning of row */
            if (pKernel >= pKernelRowEnd)
                pKernel -= YCBCR2_KERNEL_SIZE;
            } while (--nPixelsDiv2M1 >= 0);
        }

    return IL_OK;

    /*  goto point for when R, G or B out of range: clamp pixel and go back and continue.
        This is done to avoid taking a branch in the most common case: not out of range.
    */
d0ClipR: if (pixel > YCBCR2_MAX_R) pixel = YCBCR2_MAX_R; else pixel = 0;
        goto d0RClipped;
d0ClipG: if (comp > YCBCR2_MAX_G) comp = YCBCR2_MAX_G; else comp = 0;
        goto d0GClipped;
d0ClipB: if (comp > YCBCR2_MAX_B) comp = YCBCR2_MAX_B; else comp = 0;
        goto d0BClipped;
d1ClipR: if (pixel > YCBCR2_MAX_R) pixel = YCBCR2_MAX_R; else pixel = 0;
        goto d1RClipped;
d1ClipG: if (comp > YCBCR2_MAX_G) comp = YCBCR2_MAX_G; else comp = 0;
        goto d1GClipped;
d1ClipB: if (comp > YCBCR2_MAX_B) comp = YCBCR2_MAX_B; else comp = 0;
        goto d1BClipped;
d2ClipR: if (pixel > YCBCR2_MAX_R) pixel = YCBCR2_MAX_R; else pixel = 0;
        goto d2RClipped;
d2ClipG: if (comp > YCBCR2_MAX_G) comp = YCBCR2_MAX_G; else comp = 0;
        goto d2GClipped;
d2ClipB: if (comp > YCBCR2_MAX_B) comp = YCBCR2_MAX_B; else comp = 0;
        goto d2BClipped;
d3ClipR: if (pixel > YCBCR2_MAX_R) pixel = YCBCR2_MAX_R; else pixel = 0;
        goto d3RClipped;
d3ClipG: if (comp > YCBCR2_MAX_G) comp = YCBCR2_MAX_G; else comp = 0;
        goto d3GClipped;
d3ClipB: if (comp > YCBCR2_MAX_B) comp = YCBCR2_MAX_B; else comp = 0;
        goto d3BClipped;
}

    /*  ------------------------- ilSetupYCbCrDitherTable -------------------------- */
    /*  Setup the given YCbCr conversion table with values for converting YCbCr->RGB
        with the dither values pre-multiplied into them.
    */
static void ilSetupYCbCrDitherTable (
    register ilRGBTransPtr  pTrans,
    ilImageDes             *pDes
    )
{
    ilYCbCrInfo            *pYCbCr;
    double                  Lr, Lg, Lb, scaledY;
    register double         factor, gFactor, rbDither, gDither;
    register int            i, refBlack, upper, lower;
    struct {
        int                 refBlack;
        double              scale;
        } sample [3];

        /*  This function sets up the given translation table, similar to 
            ilGetYCbCrConvertTable(); see that function for more details.  The difference
            is that the multiplications done for dithering (i.e. by table lookup into
            _ilMul4/8[]) are done into the values stored in the table.
                For example, red is obtained from Y and Cr by:
                    red = (short)YTrans[Y] + (short)CrTrans[Cr]         (1)
                then red is dithered down to 4 levels by:
                    red = (pMul4[red] + kernel) >> 8;                   (2)
                where "pMul4[i]" = i * (nLevels(4) - 1) * 256 / 255"; see /ilc/ildither.c 
                But if the values in the low-order short of YTrans and CrTrans are
                premultiplied by "(nLevels - 1) * 256 / 255" then the lookup into pMul4[]
                can be skipped, and step (2) above can be skipped.
        */
    pYCbCr = &pDes->typeInfo.YCbCr;

    for (i = 0; i < 3; i++) {
        sample[i].refBlack = pYCbCr->sample[i].refBlack;
        sample[i].scale = ((i == 0) ? 255 : 127) / 
                          (pYCbCr->sample[i].refWhite - pYCbCr->sample[i].refBlack);
        }

        /*  Calc the Cb/Cr lookup tables, factoring in their scale factors. */
    Lr = ((double)pYCbCr->lumaRed / (double)10000);
    Lg = ((double)pYCbCr->lumaGreen / (double)10000);
    Lb = ((double)pYCbCr->lumaBlue / (double)10000);

        /*  Calc the RGB dither multiply factors; R and B must be the same! */
    rbDither = ((1 << YCBCR2_NR_BITS) - 1) * 256 / 255;
    gDither = ((1 << YCBCR2_NG_BITS) - 1) * 256 / 255;

        /*  Y translation: lower is scaled Y, upper is value added to get green */
    gFactor = (1 - Lb - Lr) / Lg;
    for (i = 0; i < 256; i++) {
        scaledY = sample[0].scale * i - sample[0].refBlack;
        upper = scaledY * gFactor * gDither + 0.5;
        lower = scaledY * rbDither;
        pTrans->YTrans[i] = (upper << 16) | (lower & 0xffff);
        }

        /*  Cb: lower is added to Y to get blue, upper added to get green */
    factor = (2 - 2 * Lb) * sample[1].scale;
    gFactor = -Lb * (2 - 2 * Lb) / Lg;
    refBlack = sample[1].refBlack;
    for (i = 0; i < 256; i++) {
        upper = (i - refBlack) * gFactor * gDither + 0.5;
        lower = (i - refBlack) * factor * rbDither + 0.5;
        pTrans->CbTrans[i] = (upper << 16) | (lower & 0xffff);
        }

        /*  Cr: lower is added to Y to get red, upper added to get green */
    factor = (2 - 2 * Lr) * sample[2].scale;
    gFactor = -Lr * (2 - 2 * Lr) / Lg;
    refBlack = sample[2].refBlack;
    for (i = 0; i < 256; i++) {
        upper = (i - refBlack) * gFactor * gDither + 0.5;
        lower = (i - refBlack) * factor * rbDither + 0.5;
        pTrans->CrTrans[i] = (upper << 16) | (lower & 0xffff);
        }

}


    /*  ---------------------------- _ilDitherYCbCr ----------------------------- */
    /*  Does conversions of some forms of YCbCr to dithered IL_PALETTE.
        Returns "true" if conversion handled, else "false" if not handled or if error.
        The pipe image must be an uncompressed YCbCr image.
    */
IL_PRIVATE ilBool _ilDitherYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    int                     option,
    ilConvertToPaletteInfo *pData
    )
{
    ilSrcElementData        srcData;
    ilDstElementData        dstData;
    ilPtr                   pTranslate;
    ilYCbCr2DitherPrivPtr   pPriv;
    unsigned short         *pPalette;
    ilImageDes              des;

        /*  Check for case handled here: 8x8 area dither, levels 484 specified; pipe
            image is 8 bit/comp YCbCr, subsampled by 2, even width and height.
        */
    if (((option != IL_CONVERT_TO_PALETTE) && (option != 0))
     || !pData
     || (pData->method != IL_AREA_DITHER)
     || (pData->levels[0] != 4)
     || (pData->levels[1] != 8) 
     || (pData->levels[0] != 4)
     || (pData->kernelSize != 8)
     || (pFormat->sampleOrder == IL_SAMPLE_PIXELS) 
     || (pFormat->nBitsPerSample[0] != 8)
     || (pFormat->nBitsPerSample[1] != 8)
     || (pFormat->nBitsPerSample[2] != 8)
     || (pDes->typeInfo.YCbCr.sample[0].subsampleHoriz != 1)
     || (pDes->typeInfo.YCbCr.sample[0].subsampleVert != 1)
     || (pDes->typeInfo.YCbCr.sample[1].subsampleHoriz != 2)
     || (pDes->typeInfo.YCbCr.sample[1].subsampleVert != 2)
     || (pDes->typeInfo.YCbCr.sample[2].subsampleHoriz != 2)
     || (pDes->typeInfo.YCbCr.sample[2].subsampleVert != 2)
     || ((pInfo->width | pInfo->height) & 1)) { /* width or height odd */
        pipe->context->error = 0;               /* can't handle; not an error */
        return FALSE;
        }

        /*  If mapImage given, validate it and point to its pixels.  If not given,
            pTranslate becomes null and will point to "identity" mapImage below.
        */
    pTranslate = (ilPtr)NULL;
    if (pData->mapImage) {
        ilImageInfo *pInfo;
        if (!ilQueryClientImage (pData->mapImage, &pInfo, 0)
         || (pInfo->width != 256) || (pInfo->height != 1) 
         || (pInfo->pDes->compression != IL_UNCOMPRESSED)
         || (pInfo->pDes->nSamplesPerPixel != 1)
         || (pInfo->pFormat->nBitsPerSample[0] != 8))
            return ilDeclarePipeInvalid (pipe, IL_ERROR_MAP_IMAGE);
        pTranslate = pInfo->plane[0].pPixels;
        }

        /*  alloc pPalette unless not palette dst; init with ramp if not choosing */
    pPalette = (unsigned short *)NULL;
    if (pData->dstType == IL_PALETTE) {
        register int            red, green, blue;
        register int            redLevel, greenLevel, blueLevel;
        register unsigned short *pPal;

        pPalette = (unsigned short *)IL_MALLOC_ZERO (3 * 256 * sizeof(unsigned short));
        if (!pPalette)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
        pPal = pPalette;
        for (red = 0; red < pData->levels[0]; red++) {
            redLevel = 65535 * red / (pData->levels[0] - 1);
            for (green = 0; green < pData->levels[1]; green++) {
                greenLevel = 65535 * green / (pData->levels[1] - 1);
                for (blue = 0; blue < pData->levels[2]; blue++) {
                    blueLevel = 65535 * blue / (pData->levels[2] - 1);
                    pPal [0]   = redLevel;
                    pPal [256] = greenLevel;
                    pPal [512] = blueLevel;
                    pPal++;
                    }
                }
            }
        }

        /*  Add element to the pipe, init private.  Accept strips as they are;
            presumably they will be even # lines because image is subsampled.
        */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = pInfo->stripHeight;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;

    dstData.producerObject = (ilObject)NULL;
    des = *IL_DES_GRAY;
    des.type = pData->dstType;
    if (des.type == IL_PALETTE) {
        des.flags = IL_DITHERED_PALETTE;
        des.typeInfo.palette.levels[0] = pData->levels[0];
        des.typeInfo.palette.levels[1] = pData->levels[1];
        des.typeInfo.palette.levels[2] = pData->levels[2];
        }
    dstData.pDes = &des;
    dstData.pFormat = IL_FORMAT_BYTE;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = pPalette;

    pPriv = (ilYCbCr2DitherPrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
            sizeof(ilYCbCr2DitherPrivRec), 0, &srcData, &dstData, 
            ilYCbCr2DitherInit, IL_NPF, ilYCbCr2DitherDestroy, ilYCbCr2DitherExecute, NULL, 0);
    if (!pPriv) {
        if (pPriv->pPalette)
            IL_FREE (pPriv->pPalette);
        return FALSE;
        }

        /*  Init pPriv; point pTranslate to identity image if no mapImage given */
    if (pTranslate)
        pPriv->pTranslate = pTranslate;
    else {
        int     i;
        pPriv->pTranslate = pPriv->translate;
        for (i = 0; i < 256; i++)
            pPriv->translate[i] = i;
        }
    pPriv->pPalette = pPalette;

        /*  Init tables for YCbCr->RGB conversion plus dithering */
    ilSetupYCbCrDitherTable (&pPriv->trans, pDes);

        /*  Update pipe info and return */
    ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
    return TRUE;
}



    /*  ======================= RGB To YCbCr Code =================================== */

    /*  # of bits of fixed point precision for interim values during conversion */
#define PR2Y    16

    /*  Private for RGB to YCbCr filter. For each component to be calculated (Y/Cb/Cr),
        a table indexed by the src component (R/G/B) which gives the contribution
        for that src component, in fixed point: <32-PR2Y bits>.<PR2Y bits>
    */
typedef struct {
    long    YR[256], YG[256], YB[256];          /* component lookup tables */
    long    CbR[256], CbG[256], CbB[256];
    long    CrR[256], CrG[256], CrB[256];
    long    YRefBlack, CbRefBlack, CrRefBlack;  /* component reference black */
    } ilRGBToYCbCrPrivRec, *ilRGBToYCbCrPrivPtr;

    /*  Macro to do floor/ceiling of given value: makes it 0..255. 
        Shift right by 8; if not zero, is < 0 or > 255; handle those cases.
    */
#define BYTE_RANGE(_pixel) {          \
    if (_pixel & ~0xff)  \
        if ((_pixel) < 0) _pixel = 0; \
        else _pixel = 255;            \
    }


    /*  ----------------------- ilExecuteRGBToYCbCr -------------------------------- */
    /*  Execute(): convert _pixel_ src RGB to _planar_ dst YCbCr.
    */
static ilError ilExecuteRGBToYCbCr (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
register ilRGBToYCbCrPrivPtr pPriv;
ilPtr                       pSrcLine, pYLine, pCbLine, pCrLine;
register ilPtr              pSrc, pY, pCb, pCr;
register long               nPixelsM1;
register long               YRefBlack, CbRefBlack, CrRefBlack;
long                        nPixelsM1Init, nLinesM1;
long                        YRowBytes, CbRowBytes, CrRowBytes, srcRowBytes;
ilImagePlaneInfo           *pPlane;
register long               R, G, B, pixel;


        /*  Set nPixels/LinesM1 to # pixels / lines - 1; exit if either 0. */
    pPriv = (ilRGBToYCbCrPrivPtr)pData->pPrivate;
    nPixelsM1Init = pData->pSrcImage->width;
    if (nPixelsM1Init <= 0)
        return;
    nPixelsM1Init--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return;
    nLinesM1--;

        /*  Point pSrcLine to first line of src RGB (pixel-order) data; point 
            pY/Cb/CrLine to 1st line in dst YCbCr (planar-order) data planes.
            Get bytes/row for each of them.
        */
    pPlane = pData->pSrcImage->plane;
    srcRowBytes = pPlane->nBytesPerRow;
    pSrcLine = pPlane->pPixels + pData->srcLine * srcRowBytes;

    pPlane = pData->pDstImage->plane;
    YRowBytes = pPlane->nBytesPerRow;
    pYLine = pPlane->pPixels + dstLine * YRowBytes;
    pPlane++;
    CbRowBytes = pPlane->nBytesPerRow;
    pCbLine = pPlane->pPixels + dstLine * CbRowBytes;
    pPlane++;
    CrRowBytes = pPlane->nBytesPerRow;
    pCrLine = pPlane->pPixels + dstLine * CrRowBytes;

        /*  Load component refBlack values, plus 1/2 shift (precision) amt for rounding 
            into values, shifted up so it can be added before right shift.
        */
    YRefBlack = (pPriv->YRefBlack << PR2Y) + (1 << (PR2Y - 1));
    CbRefBlack = (pPriv->CbRefBlack << PR2Y) + (1 << (PR2Y - 1));
    CrRefBlack = (pPriv->CrRefBlack << PR2Y) + (1 << (PR2Y - 1));

        /*  Convert pixel RGB to planar YCbCr. */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pY = pYLine;
        pYLine += YRowBytes;
        pCb = pCbLine;
        pCbLine += CbRowBytes;
        pCr = pCrLine;
        pCrLine += CrRowBytes;

        nPixelsM1 = nPixelsM1Init;
        do {
            R = *pSrc++;
            G = *pSrc++;
            B = *pSrc++;

            pixel = (pPriv->YR[R] + pPriv->YG[G] + pPriv->YB[B] + YRefBlack) >> PR2Y;
            if (pixel >> 8) goto YClip;
YClipped:   *pY++ = pixel;

            pixel = (pPriv->CbR[R] + pPriv->CbG[G] + pPriv->CbB[B] + CbRefBlack) >> PR2Y;
            if (pixel >> 8) goto CbClip;
CbClipped:  *pCb++ = pixel;

            pixel = (pPriv->CrR[R] + pPriv->CrG[G] + pPriv->CrB[B] + CrRefBlack) >> PR2Y;
            if (pixel >> 8) goto CrClip;
CrClipped:  *pCr++ = pixel;

            } while (--nPixelsM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;

    /*  goto point for when Y/Cb/Cr out of range: clamp pixel and go back and continue.
        This is done to avoid taking a branch in the most common case: not out of range.
    */
YClip:  if (pixel > 255) pixel = 255; else pixel = 0;
        goto YClipped;
CbClip:  if (pixel > 255) pixel = 255; else pixel = 0;
        goto CbClipped;
CrClip:  if (pixel > 255) pixel = 255; else pixel = 0;
        goto CrClipped;
}

    /*  ---------------------------- ilConvertRGBToYCbCr ----------------------------- */
    /*  Convert from source type (pDes->type) == RGB to YCbCr.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format. pNewDes points to the dst dest; *pDes to the src (pipe) des;
        on return it is updated to the new descriptor.
    */
IL_PRIVATE ilBool _ilConvertRGBToYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    register ilImageDes    *pDes,
    register const ilImageDes *pNewDes,
    ilImageFormat          *pFormat
    )
{
    register ilRGBToYCbCrPrivPtr pPriv;
    register ilYCbCrInfo   *pYCbCr;
    ilDstElementData        dstData;
    double                  Lr, Lg, Lb, range;
    long                    YR, YG, YB;
    long                    CbR, CbG, CbB;
    long                    CrR, CrG, CrB;
    int                     i;

        /*  Get format = planar order, 8 bits / pixel, or error */
    if ((pFormat->nBitsPerSample[0] != 8)
     || (pFormat->nBitsPerSample[1] != 8)
     || (pFormat->nBitsPerSample[2] != 8)
     || (pFormat->sampleOrder != IL_SAMPLE_PIXELS)) {
        if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_3BYTE_PIXEL, 0, NULL))
            return FALSE;
        }

        /*  Add filter to write 3 planar YCbCr format image.  Update pDes: std YCbCr but
            with caller's sample refBlack/White and lumaRed/Green/Blue.
        */
    *pDes = *IL_DES_YCBCR;
    pDes->typeInfo.YCbCr.sample[0].refBlack = pNewDes->typeInfo.YCbCr.sample[0].refBlack;
    pDes->typeInfo.YCbCr.sample[0].refWhite = pNewDes->typeInfo.YCbCr.sample[0].refWhite;
    pDes->typeInfo.YCbCr.sample[1].refBlack = pNewDes->typeInfo.YCbCr.sample[1].refBlack;
    pDes->typeInfo.YCbCr.sample[1].refWhite = pNewDes->typeInfo.YCbCr.sample[1].refWhite;
    pDes->typeInfo.YCbCr.sample[2].refBlack = pNewDes->typeInfo.YCbCr.sample[2].refBlack;
    pDes->typeInfo.YCbCr.sample[2].refWhite = pNewDes->typeInfo.YCbCr.sample[2].refWhite;
    pDes->typeInfo.YCbCr.lumaRed   = pNewDes->typeInfo.YCbCr.lumaRed;
    pDes->typeInfo.YCbCr.lumaGreen = pNewDes->typeInfo.YCbCr.lumaGreen;
    pDes->typeInfo.YCbCr.lumaBlue  = pNewDes->typeInfo.YCbCr.lumaBlue;

    *pFormat = *IL_FORMAT_3BYTE_PLANE;
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = (ilRGBToYCbCrPrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
            sizeof(ilRGBToYCbCrPrivRec), 0, (ilSrcElementData *)NULL, &dstData, 
            IL_NPF, IL_NPF, IL_NPF, ilExecuteRGBToYCbCr, NULL, 0);
    if (!pPriv)
        return FALSE;

        /*  See Appendix O pg 3 of TIFF spec.  Use fixed point (16.16) for 
            floating point values.
                Y = Lr*R + Lg*G + Lb*B
                Cb = (B - Y) / (2-2*Lb)
                   = (B - Lr*R - Lg*G - Lb*B) / (2-2*Lb)
                   = R * -Lr/(2-2*Lb) + G * -Lg/(2-2*Lb) + B * (1-Lb)/(2-2*Lb)
                Cr = (R - Y) / (2-2*Lr)
                   = (R - Lr*R - Lg*G - Lb*B) / (2-2*Lr)
                   = R * (1-Lr)/(2-2*Lr) + G * -Lg/(2-2*Lr) + B * -Lb/(2-2*Lr)
            In each case, the scale up to account for the scaling range is done
            by multiplying the scaling range into each RGB factor.  The refBlack is
            then added (not shown above).  Each of the values multiplied times R/G/B
            is called "Y/Cb/Cr|R/G/B".
        */
    pYCbCr = &pDes->typeInfo.YCbCr;
    Lr = (double)pYCbCr->lumaRed / 10000;
    Lg = (double)pYCbCr->lumaGreen / 10000;
    Lb = (double)pYCbCr->lumaBlue / 10000;

        /*  Y R/G/B multiples, plus refBlack. */
    range = (1 << PR2Y) * (double)(pYCbCr->sample[0].refWhite - pYCbCr->sample[0].refBlack) / 255;
    YR = range * Lr + 0.5;
    YG = range * Lg + 0.5;
    YB = range * Lb + 0.5;
    pPriv->YRefBlack = pYCbCr->sample[0].refBlack;

        /*  Cb and Cr multiples, refBlack.  Note coding range for Cb/Cr == 127. */
    range = (1 << PR2Y) * (double)(pYCbCr->sample[1].refWhite - pYCbCr->sample[1].refBlack) / 127;
    CbR = range * -Lr/(2-2*Lb) + 0.5;
    CbG = range * -Lg/(2-2*Lb) + 0.5;
    CbB = range * (1-Lb)/(2-2*Lb) + 0.5;
    pPriv->CbRefBlack = pYCbCr->sample[1].refBlack;

    range = (1 << PR2Y) * (double)(pYCbCr->sample[2].refWhite - pYCbCr->sample[2].refBlack) / 127;
    CrR = range * (1-Lr)/(2-2*Lr) + 0.5;
    CrG = range * -Lg/(2-2*Lr) + 0.5;
    CrB = range * -Lb/(2-2*Lr) + 0.5;
    pPriv->CrRefBlack = pYCbCr->sample[2].refBlack;

        /*  Build lookup table which is the result of multiplying the above by 0..255,
            i.e. the source RGB values.  Using this lookup table is equivalent to multiply
            but a whole lot faster.
        */
    for (i = 0; i < 256; i++) {
        pPriv->YR[i] = YR * i;
        pPriv->YG[i] = YG * i;
        pPriv->YB[i] = YB * i;
        pPriv->CbR[i] = CbR * i;
        pPriv->CbG[i] = CbG * i;
        pPriv->CbB[i] = CbB * i;
        pPriv->CrR[i] = CrR * i;
        pPriv->CrG[i] = CrG * i;
        pPriv->CrB[i] = CrB * i;
        }

    return TRUE;
}


