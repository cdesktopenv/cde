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
/* $XConsortium: ilupsample.c /main/6 1996/06/19 12:20:32 ageorge $ */
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

        /*  /ilc/ilupsample.c : Code for doing upsampling, or scaling
            by 1, 2, 4x of planar images; each plane independent.
        */
#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"

    /*  ========================== Fast Code ================================= */
    /*  This code handles the upsampling cases where the vertical and horizontal subsample
        factors are equal (generally the case).  It is faster than the general case code.
    */

    /*  Private for fast upsampling filters. "subsampleShift" is the shift value (0,1,2)
        for how that sample was subsampled; "shift" is the shift for upsampling that
        sample.  They are the same unless "double"; shift[i]=subsampleShift[i]+1 then.
    */
typedef struct {
    unsigned int        nSamples;
    unsigned int        subsampleShift[IL_MAX_SAMPLES];
    unsigned int        shift[IL_MAX_SAMPLES];
    int                 scaleFactor;
    } ilUpFastPrivRec, *ilUpFastPrivPtr;


    /*  -------------------------- ilExecuteFastUpsample ------------------------------ */
    /*  Called by ilExecuteFastUpsample() to quadruple (scale up by 4) one plane.
    */
static void ilUpsampleQuadruple (
    long                    nSrcLines,
    long                    nSrcBytes,
    long                    srcRowBytes,
    ilPtr                   pSrcLine,
    long           dstRowBytes,
    ilPtr                   pDstLine
    )
{
int                temp0, temp1, delta;
unsigned long      aLong;
long                        nSrcBytesM1, dstOffset;
ilPtr              pSrc, pSrcBelow, pDst;
int                left, leftBelow, right, rightBelow;

#define FUPINTER(_start, _delta, _temp, _result) { \
    _result = (_start) << 8; \
    _temp += _delta;        \
    _result += _temp >> 2; \
    _temp += _delta;        \
    _result <<= 8;         \
    _result += _temp >> 2; \
    _temp += _delta;        \
    _result <<= 8;         \
    _result += _temp >> 2; \
    }

        /*  Quadrupling: similar to doubling except * 4.  Basically interpolate
            and write 4 values from src pixels "left" and "right", by setting
            "temp" = left * 4, then adding "right-left" each time to it and 
            writing that result div 4.  Do down for four lines.
        */
    dstOffset = 4 - 3 * dstRowBytes;    /* from 3 lines down to next long */
    while (nSrcLines-- > 0) {           /* double and interpolate next line */
        pSrc = pSrcLine;
        if (nSrcLines > 0)              /* else use last for below line */
            pSrcLine += srcRowBytes;
        pSrcBelow = pSrcLine;
        pDst = pDstLine;
        pDstLine += dstRowBytes << 2;   /* skip down 4 dst lines */

        nSrcBytesM1 = nSrcBytes - 1;
        if (nSrcBytesM1 > 0) {
            left = *pSrc++;
            leftBelow = *pSrcBelow++;
            }
        else {                          /* one pixel; use left as right */
            left = *pSrc;
            leftBelow = *pSrcBelow;
            }
        while (TRUE) {
            right = *pSrc++;
            temp0 = left << 2;
            delta = right - left;
            FUPINTER (left, delta, temp0, aLong)
            *((unsigned long *)pDst) = aLong;
            pDst += dstRowBytes;

            rightBelow = *pSrcBelow++;
            temp0 = (left << 1) + left;
            temp0 += leftBelow;
            temp1 = (right << 1) + right;
            temp1 += rightBelow;
            delta = (temp1 >> 2) - (temp0 >> 2);
            FUPINTER (temp0>>2, delta, temp0, aLong)
            *((unsigned long *)pDst) = aLong;
            pDst += dstRowBytes;

            temp0 = (left + leftBelow) >> 1;
            temp1 = (right + rightBelow) >> 1;
            delta = temp1 - temp0;
            temp0 <<= 2;
            FUPINTER (temp0>>2, delta, temp0, aLong)
            *((unsigned long *)pDst) = aLong;
            pDst += dstRowBytes;

            temp0 = (leftBelow << 1) + leftBelow;
            temp0 += left;
            temp1 = (rightBelow << 1) + rightBelow;
            temp1 += right;
            delta = (temp1 >> 2) - (temp0 >> 2);
            FUPINTER (temp0>>2, delta, temp0, aLong)
            *((unsigned long *)pDst) = aLong;
            pDst += dstOffset;          /* bump to next long of first line */

            left = right;
            leftBelow = rightBelow;
            if (--nSrcBytesM1 < 0)
                break;                  /* last pixel; done */
            if (nSrcBytesM1 == 0) {     /* next-to-last pixel; back up src */
                pSrc--;
                pSrcBelow--;
                }
            }   /* END while bytes across */
        }       /* END while lines */
}


    /*  -------------------------- ilExecuteFastUpsample ------------------------------ */
    /*  Execute(): upsample as necessary "pPriv->nSamples" planes of the source image,
        where the hori/vertical upsampling factors are equal for each plane.
    */
static ilError ilExecuteFastUpsample (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilUpFastPrivPtr             pPriv;
ilImagePlaneInfo           *pSrcPlane, *pDstPlane;
int                         sample, subsampleShift;
ilPtr                       pSrcLine, pDstLine;
long                        width, nLines, nSrcLines, nSrcBytes, srcRowBytes, nSrcBytesM2;
long               dstRowBytes;
ilPtr              pSrc, pSrcBelow, pDst;
int                left, leftBelow, right, rightBelow;

        /*  Get width and height of _upsampled_ image; exit if zero. */
    pPriv = (ilUpFastPrivPtr)pData->pPrivate;
    nLines = *pNLines;                          /* # of lines before subsampling */
    *pNLines = nLines << pPriv->scaleFactor;    /* # of dst lines after any scaling */
    if (nLines <= 0)
        return IL_OK;
    width = pData->pSrcImage->width;
    if (width <= 0)
        return IL_OK;

        /*  Loop on samples (components), upsample/translate each plane separately.
            Note that "srcLine" is shifted - indexing into plane based on vert subsample.
        */
    pSrcPlane = pData->pSrcImage->plane;
    pDstPlane = pData->pDstImage->plane;
    for (sample = 0; sample < pPriv->nSamples; sample++, pSrcPlane++, pDstPlane++) {
        subsampleShift =  pPriv->subsampleShift[sample];

        srcRowBytes = pSrcPlane->nBytesPerRow;
        pSrcLine = pSrcPlane->pPixels + (pData->srcLine >> subsampleShift) * srcRowBytes;
        dstRowBytes = pDstPlane->nBytesPerRow;
        pDstLine = pDstPlane->pPixels + dstLine * dstRowBytes;

            /*  Handle tiny image: if subsampling left nothing, fill with zeros. */
        nSrcLines = nLines >> subsampleShift;
        nSrcBytes = width >> subsampleShift;
        if ((nSrcLines <= 0) || (nSrcBytes <= 0)) {
            long i = nLines;
            while (i-- > 0) {
                bzero ((char *)pDstLine, width);
                pDstLine += dstRowBytes;
                }
            }
        else {
            switch (pPriv->shift[sample]) {

                /*  No upsampling/doubling: just copy this plane */
              case 0:
                pSrc = pSrcLine;
                pDst = pDstLine;
                if (srcRowBytes == dstRowBytes)
                    bcopy ((char *)pSrc, (char *)pDst, nSrcBytes * nSrcLines);
                else while (nSrcLines-- > 0) {
                    bcopy ((char *)pSrc, (char *)pDst, nSrcBytes);
                    pSrc += srcRowBytes;                
                    pDst += dstRowBytes;
                    }
                break;

                /*  Doubling: loop while there are a pair of src lines, replicating
                    and interpolating into dst.  For each pixel across: do left with
                    interpolated pixel between.  For each pixel pair, interpolate
                    between hori and vertically.  Replicate last pixel.  For last line,
                    point "pSrcBelow" to last src line: replication of last line occurs.
                */
              case 1:
                pSrc = pSrcLine;
                pDst = pDstLine;
                while (nSrcLines-- > 0) {           /* double and interpolate next line */
                    pSrc = pSrcLine;
                    if (nSrcLines > 0)              /* else use last for below line */
                        pSrcLine += srcRowBytes;
                    pSrcBelow = pSrcLine;
                    pDst = pDstLine;
                    pDstLine += dstRowBytes << 1;   /* skip down 2 dst lines */

                    leftBelow = *pSrcBelow++;       /* do first (left) byte */
                    left = *pSrc++;
                    pDst[dstRowBytes] = (left + leftBelow) >> 1;
                    *pDst++ = left; 
                    nSrcBytesM2 = nSrcBytes - 2;
                    if (nSrcBytesM2 >= 0) {         /* middle bytes with interpolation */
                        do {
                            rightBelow = *pSrcBelow++;
                            right = *pSrc++;
                            left = (left + right) >> 1;
                            pDst[dstRowBytes] = (left + ((leftBelow + rightBelow) >> 1)) >> 1;
                            *pDst++ = left;
                            pDst[dstRowBytes] = (right + rightBelow) >> 1;
                            *pDst++ = right;
                            left = right;
                            leftBelow = rightBelow;
                            } while (--nSrcBytesM2 >= 0);
                        }
                    *pDst = left;                   /* replicate last byte */
                    pDst[dstRowBytes] = (left + leftBelow) >> 1;
                    }   /* END while lines */
                break;  /* END doubling */

              case 2:
                ilUpsampleQuadruple (nSrcLines, nSrcBytes, srcRowBytes, pSrcLine,
                                     dstRowBytes, pDstLine);
                break;

                }   /* END switch sample's shift */
            }       /* END not tiny image */
        }           /* END for each sample/plane */

    return IL_OK;
}

    /*  ---------------------------- _ilFastUpsample ----------------------------- */
    /*  Attempt to upsample and scale based on "scaleFactor" (0 = no scaling; 1 = double;
        2 = 4x), or return false if it cannot be done with the given pipe image (in which
        case pipe->context->error == 0) or if error occurs (error != 0).  Pipe image
        must be decompressed before calling this function.
            Note: when scaling up by 2x or 4x, always yields even/ *4 width/height.
        Thus, *cannot* be used to upsample odd width/height YCbCr images.
    */
IL_PRIVATE ilBool _ilFastUpsample (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    int                     scaleFactor
    )
{
    ilUpFastPrivPtr pUpPriv;
    ilUpFastPrivRec         upPriv;
    int                     i, j;
    ilYCbCrSampleInfo      *pSample;
    ilDstElementData        dstData;
    ilSrcElementData        srcData;
    const ilImageDes       *pNewDes;

        /*  Check image type; return if not handled, else init upPrivRec */
    pipe->context->error = IL_OK;           /* assume no error */
    pNewDes = (ilImageDes *)NULL;           /* assume no descriptor change */
    upPriv.scaleFactor = scaleFactor;

    switch (pDes->type) {
      case IL_GRAY:
        if (!scaleFactor) return TRUE;      /* no scaling is noop; return */
        if (!ilConvert (pipe, IL_DES_GRAY, IL_FORMAT_BYTE, 0, NULL))
            return FALSE;
        upPriv.nSamples = 1;                /* double one plane */
        upPriv.subsampleShift[0] = 0;
        upPriv.shift[0] = scaleFactor;
        ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
        break;

      case IL_RGB:
        if (!scaleFactor) return TRUE;      /* no scaling is noop; return */
        if (!ilConvert (pipe, IL_DES_RGB, IL_FORMAT_3BYTE_PLANE, 0, NULL))
            return FALSE;
        upPriv.nSamples = 3;                /* double three planes */
        upPriv.subsampleShift[0] = upPriv.subsampleShift[1] = upPriv.subsampleShift[2] = 0;
        upPriv.shift[0] = upPriv.shift[1] = upPriv.shift[2] = scaleFactor;
        ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
        break;

        /*  YCbCr: hori/vert subsample each plane must be equal and must be planar */
      case IL_YCBCR:
        if (pFormat->sampleOrder != IL_SAMPLE_PLANES)
            return FALSE;
        upPriv.nSamples = 3;
        for (i = 0, pSample = pDes->typeInfo.YCbCr.sample; i < 3; i++, pSample++) {
            if (pSample->subsampleHoriz != pSample->subsampleVert)
                return FALSE;
            j = _ilSubsampleShift [pSample->subsampleHoriz];
            upPriv.subsampleShift[i] = j;   /* shift due to subsampling */
            j += scaleFactor;               /* plus that due to scaling */
            if (j > 2)                      /* more than 4x scale; can't do */
                return FALSE;
            upPriv.shift[i] = j;            /* shift due to subsampling and doubling */
            }
        pNewDes = IL_DES_YCBCR;             /* now YCbCr with no subsampling */
        break;

      default:
        return FALSE;                       /* can't handle image type */
        }

        /*  Add the pipe element: width / height scaled if double */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pNewDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width << scaleFactor;
    dstData.height = pInfo->height << scaleFactor;
    dstData.stripHeight = pInfo->stripHeight << scaleFactor;
    dstData.constantStrip = pInfo->constantStrip;
    dstData.pPalette = (unsigned short *)NULL;

        /*  Set format: 32 bit alignment required if any scales up by 4x.
            Don't mark as such if width multiple of 4, as other filters may check it.
        */
    if (dstData.width & 3) {                /* not long-aligned */
        for (i = 0; i < upPriv.nSamples; i++)
            if (upPriv.shift[i] == 2) {
                pFormat->rowBitAlign = 32;
                break;
                }
        }

        /*  Use current strip height rather than allowing image to be split into
            smaller strips; there is a copied rather than smoothed line at the end
            of each strip, so should do min number of strips.  Could force no strips
            for better quality, but performance penalty probably not worth it.
        */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = pInfo->stripHeight;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;
    pUpPriv = (ilUpFastPrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                 sizeof (ilUpFastPrivRec), 0, &srcData,
                 &dstData, IL_NPF, IL_NPF, IL_NPF, ilExecuteFastUpsample, NULL, 0);
    if (!pUpPriv)
        return FALSE;
    *pUpPriv = upPriv;

    ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
    return TRUE;
}


    /*  ========================== Slow General Code ================================= */
    /*  This code handles the upsampling cases where the vertical and horizontal subsample
        factors are not the same.
    */

    /*  Upsample factors as shifts (1=0, 2=1, 4=2) - others not supported. */
typedef struct {
    unsigned int        horiz, vert;
    } ilUpsampleShiftRec, *ilUpsampleShiftPtr;

    /*  Private for upsampling filters. */
typedef struct {
    int                 nSamples;               /* # of samples (components) to process */
    ilUpsampleShiftRec  shift[IL_MAX_SAMPLES];  /* upsample mul as shift value */
    } ilUpsamplePrivRec, *ilUpsamplePrivPtr;



    /*  -------------------------- ilUpsampleHorizontal ------------------------------ */
    /*  Do horizontal upsampling as necessary first, spreading the dst lines
        based on vertical upsampling.
        # of lines to do is "height" divided by vertical upsample factor.
    */
static void ilUpsampleHorizontal (
    unsigned int    shift,          /* 0 = no upsample; 1 = * 2 (doubling); 2 = * 4  */
    long            width,          /* width of _whole_ (un-upsampled) image */
    long            nLines,         /* # of src lines to upsample */
    long            srcRowBytes,    /* bytes / row of src (downsampled) image */
    ilPtr           pSrcLine,       /* ptr to first line of src image */
    long            dstRowInc,      /* byte offset between dst lines written */
    ilPtr           pDstLine        /* ptr to first line of dst image */
    )
{
long                        nMidPixels, nEndPixels;
long               nPixelsM1;
ilPtr              pSrc, pDst;
ilByte             pixel, prevPixel;

    if (nLines <= 0)
        return;

        /*  Upsample based on "shift", i.e. scale up by 2 ++ "shift". */
    switch (shift) {

        /*  0: no horizontal upsampling - just copy */
      case 0:
        pSrc = pSrcLine;
        pDst = pDstLine;
        while (nLines-- > 0) {
            bcopy ((char *)pSrc, (char *)pDst, width);
            pSrc += srcRowBytes;                
            pDst += dstRowInc;
            }
        break;

        /*  1: doubling; The image for this plane is 1/2 "width".  Copy the first pixel, 
            interpolate pixels between previous pixels and the last pixel, then replicate
            last pixel to fill out width.  Example: width == 9, source width = 4 (0..3):
                 A                    B                       C:
                <0> | <0+1/2> <1> <1+2/2> <2> <2+3/2> <3> | <3> <3> 
            A: write first src pixel (pixel <0>); set into "prevPixel"
            B: write (interpolated pixel, src pixel) pair "nMidPixels" times, leaving
               srcPixel in prevPixel
            C: replicate last pixel (prevPixel) "nEndPixels" times
        */
      case 1:
        nMidPixels = (width >> 1) - 1;
        nEndPixels = width - (nMidPixels << 1) - 1;
        while (nLines-- > 0) {
            pSrc = pSrcLine;
            pSrcLine += srcRowBytes;
            pDst = pDstLine;
            pDstLine += dstRowInc;
            prevPixel = *pSrc++;
            *pDst++ = prevPixel;        /* prevPixel = first src pixel; copy to dst */
            if (nMidPixels > 0) {       /* <interpolated>, <src> pairs */
                nPixelsM1 = nMidPixels - 1;
                do {
                    pixel = *pSrc++;
                    *pDst++ = (pixel + prevPixel) >> 1;
                    *pDst++ = pixel;
                    prevPixel = pixel;
                    } while (--nPixelsM1 >= 0);
                }
            if (nEndPixels > 0) {
                nPixelsM1 = nEndPixels - 1;
                do {
                    *pDst++ = prevPixel;
                    } while (--nPixelsM1 >= 0);
                }
            }   /* END while lines */
        break;

        /*  1: quadrupling; The image for this plane is 1/4 "width".  Copy the first pixel,
            interpolate pixels between previous pixels and the last pixel, then replicate
            last pixel to fill out width.  To interpolate: add "delta" = pixel-prevPixel
            to "temp" (which starts at prevPixel*4) then divide temp by 4 before storing.
            For example, if prevPixel = 3 and pixel = 6, delta = 3, pixels are:
                3 (prevPixel) | (12+3)/4=3   (15+3)/4=4   (18+3)/4=5   6 (src pixel) |
        */
      case 2: {
        int delta, temp;

        nMidPixels = (width >> 2) - 1;
        nEndPixels = width - (nMidPixels << 2) - 1;
        while (nLines-- > 0) {
            pSrc = pSrcLine;
            pSrcLine += srcRowBytes;
            pDst = pDstLine;
            pDstLine += dstRowInc;
            prevPixel = *pSrc++;
            *pDst++ = prevPixel;        /* prevPixel = first src pixel; copy to dst */
            if (nMidPixels > 0) {       /* <3 interpolated>, <src> 4tuples */
                nPixelsM1 = nMidPixels - 1;
                do {
                    pixel = *pSrc++;
                    delta = pixel - prevPixel;
                    temp = (prevPixel << 2);
                    temp += delta;
                    *pDst++ = temp >> 2;
                    temp += delta;
                    *pDst++ = temp >> 2;
                    temp += delta;
                    *pDst++ = temp >> 2;
                    *pDst++ = pixel;
                    prevPixel = pixel;
                    } while (--nPixelsM1 >= 0);
                }
            if (nEndPixels > 0) {
                nPixelsM1 = nEndPixels - 1;
                do {
                    *pDst++ = prevPixel;
                    } while (--nPixelsM1 >= 0);
                }
            }   /* END while lines */
        break;
        }   /* END case 2 */
        }   /* END switch shift */
}           /* END ilUpsampleHorizontal */


    /*  -------------------------- ilUpsampleVertical ------------------------------ */
    /*  Do vertical upsampling as necessary, after the horizontal upsampling is done.
        Vertical upsampling is done in the dst buffer: pLine points to first line
        (which contains an already upsampled line); rowBytes is bytes/row of buffer.
    */
static void ilUpsampleVertical (
    unsigned int    shift,          /* 0 = no upsample; 1 = * 2 (doubling); 2 = * 4  */
    long            width,          /* width of _whole_ (un-upsampled) image */
    long            nLines,         /* # of dst lines (after upsampling) */
    long   rowBytes,       /* bytes / row in src/dst image */
    ilPtr           pLine           /* ptr to first line of src/dst image */
    )
{
long                        nPixelsM1Init;
long               nPixelsM1;

        /*  Return if no pixels or lines to do. */
    if (nLines <= 0)
        return;
    nPixelsM1Init = width - 1;
    if (nPixelsM1Init < 0)
        return;

        /*  Upsample based on "shift", i.e. scale up by 2 ++ "shift". */
    switch (shift) {

        /*  0: no vertical upsampling; buffer is already complete */
      case 0:
        return;                         /* EXIT */

        /*  1: doubling.  Interpolate between pairs of lines, starting with the first
            and third lines, filling in the second with the average of the two.
        */
      case 1: {
        ilPtr  pDst, pSrc1, pSrc2;

        nLines--;                       /* don't count first line already in buffer */
        while (nLines >= 2) {           /* two src lines, line between to interpolate */
            nLines -= 2;
            pSrc1 = pLine;
            pLine += rowBytes;
            pDst = pLine;
            pLine += rowBytes;
            pSrc2 = pLine;
            nPixelsM1 = nPixelsM1Init;
            do {
                *pDst++ = (*pSrc1++ + *pSrc2++) >> 1;
                } while (--nPixelsM1 >= 0);
            }

            /*  Replicate last line to fill out to last line(s) */
        pDst = pLine;
        while (nLines-- > 0) {
            pDst += rowBytes;
            bcopy ((char *)pLine, (char *)(pDst), width);
            }
        break;
        }   /* END doubling */

        /*  1: quadrupling.  Interpolate between quadruples of lines, starting with lines
            0 and 4, interpolating 1..3 - see horizontal upsampling.
        */
      case 2: {
        ilPtr  pSrc, pDst;
        int    pixel, delta;
        long            rowBytesTimes4 = rowBytes << 2;

        nLines--;                       /* don't count first line already in buffer */
        while (nLines >= 4) {           /* four src lines, line between to interpolate */
            nLines -= 4;
            pSrc = pLine;
            pLine += rowBytesTimes4;    /* point 4 lines down */
            nPixelsM1 = nPixelsM1Init;
            do {
                pDst = pSrc;
                pixel = *pSrc++;        /* pixel from top src line; next pixel */
                delta = *(pDst + rowBytesTimes4) - pixel;  /* delta = bottom - top */
                pixel <<= 2;            /* work in pixels * 4, /4 before storing */
                pixel += delta;         
                pDst += rowBytes;
                *pDst = pixel >> 2;     /* store one interpolated */
                pixel += delta;         
                pDst += rowBytes;
                *pDst = pixel >> 2;     /* store one interpolated */
                pixel += delta;         
                pDst += rowBytes;
                *pDst = pixel >> 2;     /* store one interpolated */
                } while (--nPixelsM1 >= 0);
            }

            /*  Replicate last line to fill out to last line(s) */
        pDst = pLine;
        while (nLines-- > 0) {
            pDst += rowBytes;
            bcopy ((char *)pLine, (char *)(pDst), width);
            }
        break;
        }   /* END quadrupling */
        }   /* END switch shift */
}           /* END ilUpsampleVertical */


    /*  -------------------------- ilExecuteUpsample ------------------------------- */
    /*  Execute(): upsample as necessary "pPriv->nSamples" planes of the source image.
    */
static ilError ilExecuteUpsample (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
ilUpsamplePrivPtr pPriv;
ilImagePlaneInfo           *pSrcPlane, *pDstPlane;
int                         nSamples;
ilPtr                       pSrcLine, pDstLine;
long                        height, width, nLines;
long                        srcRowBytes, dstRowBytes;
ilUpsampleShiftPtr          pShift;

        /*  Get width and height of _upsampled_ image; exit if zero. */
    pPriv = (ilUpsamplePrivPtr)pData->pPrivate;
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

        srcRowBytes = pSrcPlane->nBytesPerRow;
        pSrcLine = pSrcPlane->pPixels + (pData->srcLine >> pShift->vert) * srcRowBytes;
        dstRowBytes = pDstPlane->nBytesPerRow;
        pDstLine = pDstPlane->pPixels + dstLine * dstRowBytes;

            /*  Upsample horizontal "height >> pShift->vert" src lines (e.g. half
                the lines for shift = 1).  Write the results starting at pDstLine,
                each line "dstRowBytes << pShift->vert" bytes after the other, e.g.
                if pShift->vert is 1 (double), leave one line gap between each dst line.
                    But first, handle very small image: if subsampling left nothing,
                fill with zeros.  Note that small strips will not cause this to happen, 
                because stripHeight checked when element added.
            */
        nLines = height >> pShift->vert;
        if ((nLines <= 0) || ((width >> pShift->horiz) <= 0)) {
            long i = height;
            while (i-- > 0) {
                bzero ((char *)pDstLine, width);
                pDstLine += dstRowBytes;
                }
            }
        else {
            ilUpsampleHorizontal (pShift->horiz, width, nLines,
                srcRowBytes, pSrcLine, dstRowBytes << pShift->vert, pDstLine);

                /*  Upsample vertically, interpolating between the lines just written. */
            ilUpsampleVertical (pShift->vert, width, height, dstRowBytes, pDstLine);
            }
        }

    return IL_OK;
}

    /*  ---------------------------- ilUpsampleYCbCr ----------------------------- */
    /*  Upsample and / or convert to gray the pipe image which must be a YCbCr image.
        If "toGray" is true, the Y plane only will be upsampled (or copied) resulting
        in a gray image; else a planar YCbCr image will result.
        If "upSample" is true must upsample; "upSample" and/or "toGray" must be true.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format, *pDes to the dst descriptor.
    */
IL_PRIVATE ilBool _ilUpsampleYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    ilBool                  toGray,
    ilBool                  upSample
    )
{
    ilUpsamplePrivPtr pUpPriv;
    ilImageDes              pipeDes;
    ilUpsampleShiftRec     *pShift;
    ilYCbCrSampleInfo      *pSample;
    int                     sample;
    ilDstElementData        dstData;
    ilSrcElementData        srcData;
    long           i, j;

        /*  Only 8 bit planar YCbCr can currently be upsampled; if not that and upsampling
            needed, error; if no upsampling (only convert to gray) convert to planar.
            NOTE: can't ilConvert() to planar if upsampling needed as ilConvert() will
            call this function back to upsample, thereby recursing "forever".
        */
    if ((pFormat->sampleOrder != IL_SAMPLE_PLANES)
     || (pFormat->nBitsPerSample[0] != 8)
     || (pFormat->nBitsPerSample[1] != 8)
     || (pFormat->nBitsPerSample[2] != 8)) {
        if (upSample)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);
        if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_3BYTE_PLANE, 0, NULL))
            return FALSE;
        *pFormat = *IL_FORMAT_3BYTE_PLANE;
        }

#if 0

    This code currently not included.  It tries the "fast" upsampling code, which
    is in fact faster than the "slow" upsampling code, but for some reason the resulting
    YCbCr data causes the conversion to RGB to be much slower, probably because more
    out-of-range (0..255) values are generated which cause the clip code to be hit.
    For now, the fast upsample code is used only by ilScale(..., IL_SCALE_SAMPLE) for
    2x and 4x scales up.

        /*  Try to use fast upsampling case, but not if "toGray" true, or if width/height
            of resulting image is not multiple of subsample factor.
        */
    if (!toGray) {
        ilBool  tryFast = TRUE;
        int     factor;
        for (sample = 0; sample < 3; sample++) {
            factor = pDes->typeInfo.YCbCr.sample[sample].subsampleHoriz;
            if (((factor == 2) && ((pInfo->width | pInfo->height) & 1))
             || ((factor == 4) && ((pInfo->width | pInfo->height) & 3)))
                tryFast = FALSE;
            }
        if (tryFast) {
            if (_ilFastUpsample (pipe, pInfo, pDes, pFormat, FALSE))
                return TRUE;                        /* handled; done */
            if (pipe->context->error)               /* not handled but error; exit */
                return FALSE;
            }
        }
#endif

        /*  Must do "slow" upsample: init dstData for filter(s) to be added. */
    pipeDes = *pDes;
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;

        /*  Demand constant strips, a multiple of maximum subsample (4). To avoid the
            problem of having the last strip have 3 lines or less (and therefore for * 4
            upsampling require that the previous line be copied - but no previous line
            available), bump stripHeight by 4 until stripHeight mod 4 > 3, or until
            requiring whole image as one strip.
        */
    i = 8;
    while (TRUE) {
        j = dstData.height % i;
        if ((j == 0) || (j > 3))
            break;
        if ((i += 4) >= dstData.height) {
            i = dstData.height;
            break;
            }
        }
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = i;
    srcData.constantStrip = TRUE;
    srcData.minBufferHeight = 0;

        /*  Add a filter to upsample; if "toGray" upsample/copy one plane and done. */
    if (toGray) {
        *pDes = *IL_DES_GRAY;
        *pFormat = *IL_FORMAT_BYTE;
        }
    else {
        pDes->typeInfo.YCbCr.sample[0].subsampleHoriz = 1;
        pDes->typeInfo.YCbCr.sample[0].subsampleVert = 1;
        pDes->typeInfo.YCbCr.sample[1].subsampleHoriz = 1;
        pDes->typeInfo.YCbCr.sample[1].subsampleVert = 1;
        pDes->typeInfo.YCbCr.sample[2].subsampleHoriz = 1;
        pDes->typeInfo.YCbCr.sample[2].subsampleVert = 1;   /* des now upsampled */
        }
    pUpPriv = (ilUpsamplePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                 sizeof (ilUpsamplePrivRec), 0, &srcData,
                 &dstData, IL_NPF, IL_NPF, IL_NPF, ilExecuteUpsample, NULL, 0);
    if (!pUpPriv)
        return FALSE;

        /*  Init pUpPriv. */
    pUpPriv->nSamples = (toGray) ? 1 : 3;
    pSample = pipeDes.typeInfo.YCbCr.sample;
    pShift = pUpPriv->shift;

    for (sample = 0; sample < pUpPriv->nSamples; sample++, pShift++, pSample++) {
        pShift->horiz = _ilSubsampleShift [pSample->subsampleHoriz];
        pShift->vert = _ilSubsampleShift [pSample->subsampleVert];
        }

    return TRUE;
}


