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
/* $XConsortium: ilconvert.c /main/5 1996/06/19 12:24:11 ageorge $ */
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
#include "ilpipeint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"


        /*  ---------------------- ilAddConversionFilter --------------------------- */
        /*  Add a conversion filter to the given pipe, given a ptr to a structure
            which defines the filter and its pipe element.  If failure, declare pipe
            invalid (which sets error code) and return false; else true.
                pInfo must point to the current pipe info; pSrcDes/Format to the pipe
            des/format.  The information they point to is updated after the pipe
            element(s) is/are added; on return they will contain up-to-date data.
        */
static ilBool ilAddConversionFilter (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pSrcDes,
    ilImageFormat          *pSrcFormat,
    void                   *pOptionData,
    register ilConvertPtr   pCvtData
    )
{
ilPtr                       pPriv;
ilDstElementData            dstData;
ilError                     error;

        /*  Check src format against code for src format demanded by this filter.
            If not the same, do ilConvert() to that format (recurse!), exit if error.
        */
    switch (pCvtData->srcFormatCode) {
        case IL_DONT_CHECK_STD_FORMAT:      /* filter converts format: dont check */
            break;

        case IL_STD_FORMAT_BIT:
            if ((pSrcFormat->rowBitAlign != 32)
             || (pSrcFormat->nBitsPerSample[0] != 1)) {
                *pSrcFormat = *IL_FORMAT_BIT;
                ilConvert (pipe, (ilImageDes *)NULL, pSrcFormat, 0, NULL);
                if (pipe->context->error) return FALSE;
                ilGetPipeInfo (pipe, FALSE, pInfo, (ilImageDes *)NULL, (ilImageFormat *)NULL);
                }
            break;

        case IL_STD_FORMAT_BYTE:
            if (pSrcFormat->nBitsPerSample[0] != 8) {
                *pSrcFormat = *IL_FORMAT_BYTE;
                ilConvert (pipe, (ilImageDes *)NULL, pSrcFormat, 0, NULL);
                if (pipe->context->error) return FALSE;
                ilGetPipeInfo (pipe, FALSE, pInfo, (ilImageDes *)NULL, (ilImageFormat *)NULL);
                }
            break;

        case IL_STD_FORMAT_3BYTE_PIXEL:
            if ((pSrcFormat->sampleOrder != IL_SAMPLE_PIXELS)
             || (pSrcFormat->nBitsPerSample[0] != 8)
             || (pSrcFormat->nBitsPerSample[1] != 8)
             || (pSrcFormat->nBitsPerSample[2] != 8)) {
                *pSrcFormat = *IL_FORMAT_3BYTE_PIXEL;
                ilConvert (pipe, (ilImageDes *)NULL, pSrcFormat, 0, NULL);
                if (pipe->context->error) return FALSE;
                ilGetPipeInfo (pipe, FALSE, pInfo, (ilImageDes *)NULL, (ilImageFormat *)NULL);
                }
            break;
        }

        /*  Add a filter, from in *pCvtData.  Only the des and format may change;
            other info (width, stripHeight, etc.) stays the same.
        */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pCvtData->pDstDes;
    dstData.pFormat = pCvtData->pDstFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = ilAddPipeElement (pipe, IL_FILTER, pCvtData->nBytesPrivate, 0,
                (ilSrcElementData *)NULL, &dstData, pCvtData->Init, pCvtData->Cleanup, 
                pCvtData->Destroy, pCvtData->Execute, NULL, 0);
    if (!pPriv) return FALSE;                               /* EXIT */

        /*  If successful: call converters init function if non-null, return true.
        */
    if (pCvtData->AddElement) {
        error = (*pCvtData->AddElement) (pPriv, pInfo->pPalette, pOptionData);
        if (error) {
            ilDeclarePipeInvalid (pipe, error);
            return FALSE;                                   /* EXIT */
            }
        }

        /*  Update the pipe info, return success.
        */
    ilGetPipeInfo (pipe, FALSE, pInfo, pSrcDes, pSrcFormat);
    return TRUE;
}


        /*  ---------------------- ilAddFormatFilter --------------------------- */
        /*  Add a filter to the given pipe which converts format only to *pNewFormat.  
            Similar to ilAddConversionFilter() but simpler.
            *pInfo is updated with the new pipe info.
        */
static ilBool ilAddFormatFilter (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    const ilImageFormat    *pNewFormat,
    register ilFormatPtr    pData
    )
{
ilPtr                       pPriv;
ilDstElementData            dstData;
ilError                     error;

        /*  Add a filter, from in *pData.  Only the format may change;
            other info (des, width, stripHeight, etc.) stays the same.
        */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = (ilImageDes *)NULL;
    dstData.pFormat = pNewFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = ilAddPipeElement (pipe, IL_FILTER, pData->nBytesPrivate, 0,
                (ilSrcElementData *)NULL, &dstData, pData->Init, pData->Cleanup, 
                pData->Destroy, pData->Execute, NULL, 0);
    if (!pPriv) return FALSE;                               /* EXIT */

        /*  If successful: call converters init function if non-null, return true.
        */
    if (pData->AddElement) {
        error = (*pData->AddElement) (pPriv, pInfo->pPalette);
        if (error) {
            ilDeclarePipeInvalid (pipe, error);
            return FALSE;                                   /* EXIT */
            }
        }

        /*  Update the pipe info, return success. */
    ilGetPipeInfo (pipe, FALSE, pInfo, (ilImageDes *)NULL, (ilImageFormat *)NULL);
    return TRUE;
}


        /*  ---------------------------- ilConvert -------------------------------- */
        /*  Public function; see spec.
                Convert the current pipe image to the given description (*pDes) and
            format (*pFormat).  Either or both (a noop) may be null.  A null pDes means
            "the same as before"; a null pFormat means "whatever is easiest/most 
            efficient."
                If the current pipe image already fits the bill, no pipe element is added.
            Note: compression is ignored in "pDes" - call ilCompress() to compress.
        */
ilBool ilConvert (
    ilPipe              pipe,
    const ilImageDes    *pDes,
    const ilImageFormat *pFormat,
    int                 option,
    void               *pOptionData
    )
{
ilPipeInfo              info;
ilImageDes              pipeDes;
ilImageFormat           pipeFormat;
unsigned int            sample;
ilBool                  doConvert, doSubsample;
static unsigned long    defaultThreshold = 128; /* dflt threshold for gray->bitonal cvt */
#define ADD_CVT_FILTER(_filter) \
    (ilAddConversionFilter (pipe, &info, &pipeDes, &pipeFormat, pOptionData, (_filter)))

    /*  Macro to determine if the given YCbCr must be upsampled (any bits != 1) */
#define MUST_UPSAMPLE(_des)                              \
    ( ((_des).typeInfo.YCbCr.sample[0].subsampleHoriz |  \
       (_des).typeInfo.YCbCr.sample[0].subsampleVert |   \
       (_des).typeInfo.YCbCr.sample[1].subsampleHoriz |  \
       (_des).typeInfo.YCbCr.sample[1].subsampleVert |   \
       (_des).typeInfo.YCbCr.sample[2].subsampleHoriz |  \
       (_des).typeInfo.YCbCr.sample[2].subsampleVert) != 1)

        /*  Overall approach: if pDes specified, try to convert image descriptor
            if different, by adding filters with ilAddConversionFilter(): the des 
            converting filter may add a reformat filter before it, and may change
            the format - the new des and format are returned if changed.
                After converting the description if necessary, check format if
            pFormat is non-null and reformat as necessary.  A format converter
            cannot change the des.
                Note that we need not validate the format and des together.
            ilAddPipeElement() will do that and return an error if they are incompatible
            (e.g. des.type = IL_RGB with des.nSamplesPerPixel != 3).
        */

        /*  Get pipe info and decompress! if not ready for a filter, exit. 
        */
    if (ilGetPipeInfo (pipe, TRUE, &info, &pipeDes, &pipeFormat) != IL_PIPE_FORMING) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;                                   /* EXIT */
        }

        /*  If no pDes, point pDes to pipe des for format check below, else check for
            descriptor conversions.
        */
    doSubsample = FALSE;    /* assume no subsampling; may be set below */
    if (!pDes)
        pDes = &pipeDes;
    else {

            /*  ilConvert() does not compress - use ilCompress() to compress. */
        if (pDes->compression != IL_UNCOMPRESSED)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_CONVERT_COMPRESSION);

        if (pipeDes.type == pDes->type)             /* cvt within type */
            switch (pipeDes.type) {

                /*  Within YCbCr: check for changes to luma or refBlack/White.  If any, 
                    convert to RGB; below cvt back to YCbCr will handle luma and ref.
                */
              case IL_YCBCR: {
                const ilYCbCrSampleInfo   *pSrcSample, *pDstSample;
                ilBool              doCvtYCbCr;

                    /* If must convert refBlack/White or lumaRed/Green/Blue, convert to 
                       RGB and back to YCbCr; only those conversions handle this.
                    */
                if ((pipeDes.typeInfo.YCbCr.lumaRed != pDes->typeInfo.YCbCr.lumaRed)
                 || (pipeDes.typeInfo.YCbCr.lumaGreen != pDes->typeInfo.YCbCr.lumaGreen)
                 || (pipeDes.typeInfo.YCbCr.lumaBlue != pDes->typeInfo.YCbCr.lumaBlue))
                    doCvtYCbCr = TRUE;
                else {
                    for (pSrcSample = pipeDes.typeInfo.YCbCr.sample,
                         pDstSample = pDes->typeInfo.YCbCr.sample,
                         doCvtYCbCr = FALSE, sample = 0; 
                         sample < 3; sample++, pSrcSample++, pDstSample++) 
                            if ((pSrcSample->refBlack != pDstSample->refBlack)
                             || (pSrcSample->refWhite != pDstSample->refWhite))
                                doCvtYCbCr = TRUE;
                    }
                if (doCvtYCbCr) {    /* cvt to RGB */
                    if (!ilConvert (pipe, IL_DES_RGB, (ilImageFormat *)NULL, 0, NULL))
                        return FALSE;
                    ilGetPipeInfo (pipe, FALSE, &info, &pipeDes, &pipeFormat);
                    }
                break;
                }

                /*  Within palette: if option is zero (0), or if option says to dither
                    and image already dithered to requested levels: fine, done; otherwise
                    cvt first to RGB, so below conversion to palette will be enabled.
                */
              case IL_PALETTE:
                if (option == 0)
                    break;                      /* default = accept as is; fine */
                if (option == IL_CONVERT_TO_PALETTE) {
                    ilConvertToPaletteInfo *pData = (ilConvertToPaletteInfo *)pOptionData;
                    if ((pipeDes.flags & IL_DITHERED_PALETTE)
                     && (pipeDes.typeInfo.palette.levels[0] == pData->levels[0])
                     && (pipeDes.typeInfo.palette.levels[1] == pData->levels[1])
                     && (pipeDes.typeInfo.palette.levels[2] == pData->levels[2]))
                        break;                  /* dithered to requested levels; fine */
                    }
                if (!ADD_CVT_FILTER (&_ilPaletteToRGBByte))  /* cvt up to RGB */
                    return FALSE;
                break;
            }       /* END cvt within type */

            /*  Loop converting image type until desired type reached.  May take some 
                iterations, e.g pal->bitonal = pal->rgb; rgb->gray; gray->bitonal.
                    For conversion to YCbCr, cvt to RGB first.
                    For conversion to palette, cvt to RGB first.
            */
        while (pipeDes.type != pDes->type) {
            switch (pipeDes.type) {

                /*  Bitonal: cvt to gray, then to other types as necessary. */
              case IL_BITONAL:
                switch (pDes->type) {
                  case IL_GRAY:
                  case IL_YCBCR:
                  case IL_PALETTE:
                  case IL_RGB: {
                    if (!ilScale (pipe, info.width, info.height, IL_SCALE_BITONAL_TO_GRAY, NULL))
                        return FALSE;
                    ilGetPipeInfo (pipe, FALSE, &info, &pipeDes, &pipeFormat);
                    break;
                    }

                  default:
                    goto CantConvert;       /* unsupported conversion(s) */
                    }
                break;

              case IL_GRAY:

                    /*  If not 256 levels/sample, cvt to 256 if possible. */
                if (pipeDes.nLevelsPerSample[0] != 256)
                    if (!_ilAddLevelConversionFilter (pipe, &info, &pipeDes, IL_DES_GRAY, &pipeFormat))
                        return FALSE;

                switch (pDes->type) {
                  case IL_BITONAL:
                    if (option == IL_CONVERT_THRESHOLD) {
                        if (!pOptionData)
                            pOptionData = &defaultThreshold;
                        if (!ADD_CVT_FILTER (&_ilThresholdGrayToBitonal)) 
                            return FALSE;   /* error, EXIT */
                        }
                    else if (!ADD_CVT_FILTER (&_ilDiffuseGrayToBitonal)) 
                        return FALSE;       /* error, EXIT */
                    break;

                  case IL_YCBCR:
                  case IL_PALETTE:
                  case IL_RGB:
                    if (!ADD_CVT_FILTER (&_ilGrayToRGB))
                        return FALSE;       /* error, EXIT */
                    break;

                  default:
                    goto CantConvert;       /* unsupported conversion(s) */
                    }
                break;

                /*  Palette to anything (except self - but that case wont get here)
                    is currently done by converting to RGB, then to des type if necessary.
                */
              case IL_PALETTE:
                if (!ADD_CVT_FILTER (&_ilPaletteToRGBByte)) 
                    return FALSE;           /* error, EXIT */
                break;

                /*  RGB goes to gray for gray or bitonal; handle conversion to YCbCr.
                    If not 256 levels/sample, cvt to 256 if possible.
                */
              case IL_RGB:
                if ((pipeDes.nLevelsPerSample[0] != 256)
                 || (pipeDes.nLevelsPerSample[1] != 256)
                 || (pipeDes.nLevelsPerSample[2] != 256))
                    if (!_ilAddLevelConversionFilter (pipe, &info, &pipeDes, IL_DES_RGB, &pipeFormat))
                        return FALSE;
        
                switch (pDes->type) {
                  case IL_BITONAL:
                  case IL_GRAY:
                    if (!ADD_CVT_FILTER (&_ilRGBToGray))
                        return FALSE;       /* error, EXIT */
                    break;

                  case IL_YCBCR:
                    if (!_ilConvertRGBToYCbCr (pipe, &info, &pipeDes, pDes, &pipeFormat))
                        return FALSE;
                    break;

                    /*  Cvt RGB to palette: set pipeDes.type to IL_PALETTE to terminate
                        loop: private type may actually have been created.
                    */
                  case IL_PALETTE:
                    if (!_ilConvertRGBToPalette (pipe, &info, &pipeDes, &pipeFormat,
                                    option, (ilConvertToPaletteInfo *)pOptionData))
                        return FALSE;
                    pipeDes.type = IL_PALETTE;          /* see note above */
                    break;

                  default:
                    goto CantConvert;       /* unsupported conversion(s) */
                    }
                break;

                /*  YCbCr (aka LUV): convert based on dst type and set "convertDone"
                    to true, else convert to RGB, and then loop again until desired type.
                */
              case IL_YCBCR: {
                ilBool  convertDone, upSample;

                upSample = MUST_UPSAMPLE (pipeDes);
                convertDone = FALSE;

                switch (pDes->type) {
                  case IL_BITONAL:
                  case IL_GRAY:         /* extract Y plane if Y range 0..255 */
                    if ((pipeDes.typeInfo.YCbCr.sample[0].refBlack == 0)
                     && (pipeDes.typeInfo.YCbCr.sample[0].refWhite == 255)) {
                        if (!_ilUpsampleYCbCr (pipe, &info, &pipeDes, &pipeFormat, TRUE, 
                                               upSample))
                            return FALSE;
                        convertDone = TRUE;
                        }
                    break;
                  case IL_PALETTE:      /* try direct YCbCr to palette conversion */
                    if (_ilDitherYCbCr (pipe, &info, &pipeDes, &pipeFormat, option,
                                        (ilConvertToPaletteInfo *)pOptionData)) {
                        convertDone = TRUE;
                        pipeDes.type = IL_PALETTE;  /* see note above RGB->palette */
                        }
                    else if (pipe->context->error)
                        return FALSE;   /* otherwise, no error; filter just cant do it */
                    break;
                    }

                if (!convertDone) {     /* no conversion; convert YCbCr to RGB */
                    if (upSample && !_ilUpsampleYCbCr (pipe, &info, &pipeDes, 
                                                       &pipeFormat, FALSE, upSample))
                            return FALSE;
                    if (!_ilConvertYCbCrToRGB (pipe, &info, &pipeDes, &pipeFormat))
                        return FALSE;
                    }
                break;
                }
                }   /* END switch source type */
            }       /* END while convert type */


            /*  Image type converted if different; check rest of des. */
        if (pipeDes.nSamplesPerPixel != pDes->nSamplesPerPixel)
            goto CantConvert;

            /*  Check levels/sample, but not for palette images, where the levels/sample
                must be in range 2 .. (1<<nbits) - 1, but is otherwise ignored.
            */
        for (sample = 0; sample < pDes->nSamplesPerPixel; sample++)
            if (pipeDes.nLevelsPerSample[sample] != pDes->nLevelsPerSample[sample]) {
                if ((pipeDes.type != IL_PALETTE) &&
                 !_ilAddLevelConversionFilter (pipe, &info, &pipeDes, pDes, &pipeFormat))
                    return FALSE;
                break;
                }

            /*  Do image type-specific checks and conversions. */
        switch (pipeDes.type) {

            /*  "blackIsZero" applies only to bitonal and gray scale images. If different
                then invert bits if not "soft" invert (which means leave bits alone);
                if same, invert bits if "hard" invert (which means invert bits).
            */
          case IL_BITONAL:
          case IL_GRAY:
            if (pipeDes.blackIsZero != pDes->blackIsZero) {
                pipeDes.blackIsZero = pDes->blackIsZero;
                if (option == IL_CONVERT_SOFT_INVERT)
                    _ilSetPipeDesFormat (pipe, &pipeDes, (ilImageFormat *)NULL);
                else if (!_ilAddInvertFilter (pipe, &pipeDes, &info)) 
                    goto CantConvert;
                 }
            break;

            /*  YCbCr: check sub/upsampling; up/subsample as necessary */
          case IL_YCBCR: {
                const ilYCbCrSampleInfo   *pSrcSample, *pDstSample;
                ilBool              doUpsample;

                doSubsample = FALSE;        /* checked below */
                for (pSrcSample = pipeDes.typeInfo.YCbCr.sample,
                     pDstSample = pDes->typeInfo.YCbCr.sample,
                     doUpsample = FALSE, sample = 0; 
                     sample < 3; sample++, pSrcSample++, pDstSample++) 
                    {
                    if (pSrcSample->subsampleHoriz < pDstSample->subsampleHoriz)
                        doSubsample = TRUE;
                    else if (pSrcSample->subsampleHoriz > pDstSample->subsampleHoriz)
                        doUpsample = TRUE;
                    if (pSrcSample->subsampleVert < pDstSample->subsampleVert)
                        doSubsample = TRUE;
                    else if (pSrcSample->subsampleVert > pDstSample->subsampleVert)
                        doUpsample = TRUE;
                    }
                if (doUpsample)
                    if (!_ilUpsampleYCbCr (pipe, &info, &pipeDes, &pipeFormat, FALSE, TRUE))
                        return FALSE;
                if (doSubsample) {
                    pipeDes = *pDes;        /* pass subsample pars; des convert done */
                    if (!_ilSubsampleYCbCr (pipe, &info, &pipeDes, &pipeFormat))
                        return FALSE;
                    }
                }   /* END YCbCr */
            break;
            }
        }   /* END check pDes */


        /*  If pFormat given, convert to requested format.
            WARNING !!!!! - ilCheckPipeFormat() below also does format conversions -
            May have to change it when adding new format filters !!!!
        */
    if (pFormat) {

            /*  Check if bits/sample needs conversion and if that conversion supported.
                Currently supported: 4 to 8 bit, pixel mode only if multi-sample.
            */
        doConvert = FALSE;
        for (sample = 0; sample < pDes->nSamplesPerPixel; sample++)
            if (pipeFormat.nBitsPerSample[sample] != pFormat->nBitsPerSample[sample]) {
                doConvert = TRUE;
                break;
                }

            /*  If any format conversions and YCbCr, upsample if necessary.
                However, if subsampling was done before, error: cant convert.
            */
        if ((pipeDes.type == IL_YCBCR) 
         && MUST_UPSAMPLE (pipeDes)
         && (doConvert || (pipeFormat.sampleOrder != pFormat->sampleOrder)
                       || (pipeFormat.rowBitAlign != pFormat->rowBitAlign))) {
            if (doSubsample)
                goto CantConvert;       /* cant upsample - subsampled before */
            if (!_ilUpsampleYCbCr (pipe, &info, &pipeDes, &pipeFormat, FALSE, TRUE))
                return FALSE;
            }

            /*  If doConvert, then pipe image does not have desired # of bits/sample.
                Check if all planes have same # src, dst bits; else can't convert.
                Currently can convert 4->8 or 8->4.
            */
        if (doConvert) {
            int         nSrcBits, nDstBits;
            if ((pipeDes.nSamplesPerPixel > 1) && (pFormat->sampleOrder != IL_SAMPLE_PIXELS))
                goto CantConvert;
            nSrcBits = pipeFormat.nBitsPerSample[0];
            nDstBits = pFormat->nBitsPerSample[0];
            for (sample = 1; sample < pDes->nSamplesPerPixel; sample++)
                if ((nSrcBits != pipeFormat.nBitsPerSample[sample])
                 || (nDstBits != pFormat->nBitsPerSample[sample]))
                    goto CantConvert;           /* not same # src,dst bits each plane */

            if ((nSrcBits == 4) && (nDstBits == 8)) {
                if (!ilAddFormatFilter (pipe, &info, pFormat, &_ilFormat4To8Bit))
                    return FALSE;
                }
            else if ((nSrcBits == 8) && (nDstBits == 4)) {
                if (!ilAddFormatFilter (pipe, &info, pFormat, &_ilFormat8To4Bit))
                    return FALSE;
                }
            else goto CantConvert;
            }

            /*  Change sample order: only handles 3 samples, each 8 bits. */
        if (pipeFormat.sampleOrder != pFormat->sampleOrder) {
            if ((pipeDes.nSamplesPerPixel != 3)
             || (pipeFormat.nBitsPerSample[0] != 8)
             || (pipeFormat.nBitsPerSample[1] != 8)
             || (pipeFormat.nBitsPerSample[2] != 8))
                goto CantConvert;
            if (pipeFormat.sampleOrder == IL_SAMPLE_PIXELS) {
                 if (!ilAddFormatFilter (pipe, &info, pFormat, &_ilFormat8Bit3PixelToPlane))
                    return FALSE;
                }
            else if (!ilAddFormatFilter (pipe, &info, pFormat, &_ilFormat8Bit3PlaneToPixel))
                return FALSE;
            }

        if (pipeFormat.rowBitAlign != pFormat->rowBitAlign) {
            if (!ilAddFormatFilter (pipe, &info, pFormat, &_ilFormatRowBitAlign))
                return FALSE;
            }
        }

        /*  Succesful conversion.
        */
    pipe->context->error = IL_OK;
    return TRUE;                                   /* EXIT */


    /*  GOTO point for when conversion not supported.  
    */
CantConvert:
    ilDeclarePipeInvalid (pipe, IL_ERROR_CANT_CONVERT);
    return FALSE;
}

#ifdef NOTDEF

    IN PROGRESS - NOT DONE YET !!!!!!!!!!

    /*  ------------------------ ilCheckPipeFormat --------------------------------- */
    /*  Return an IL_CPF_? code for the current pipe image format, or 
        IL_CPF_ERROR if error or can't match formats in "formatMask".
        The pipe info, des and/or format are returned to non-null
        pInfo/Des/Format if IL_CPF_ERROR not returned.
    */
unsigned int ilCheckPipeFormat (
    ilPipe                  pipe,
    register unsigned long  formatMask,
    ilPipeInfo             *pInfo,              /* RETURNED */
    register ilImageDes    *pDes,               /* RETURNED */
    register ilImageFormat *pFormat             /* RETURNED */
    )
{
    ilPipeInfo          localInfo;
    ilImageDes          localDes;
    ilImageFormat       localFormat;

        /*  Point pInfo/Des/Format to local structs if caller passed NULL */
    if (!pInfo) pInfo = &localInfo;
    if (!pDes) pDes = &localDes;
    if (!pFormat) pFormat = &localFormat;

        /*  Get pipe info and decompress; if pipe not in IL_PIPE_FORMING state: error. */                          
    if (ilGetPipeInfo (pipe, TRUE, pInfo, pDes, pFormat) != IL_PIPE_FORMING) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return IL_CPF_ERROR;
        }

        /*  Look at des, format and try to match/convert to format(s) caller accepts */
    if (pDes->nSamplesPerPixel == 1) {

            /*  1 sample/pixel: if bit/pixel and caller accepts, check rowBitAlign */
        if ((formatMask & IL_CPF_BIT) && (pFormat->nBitsPerSample[0] == 1)) {
            if (pFormat->rowBitAlign != 32) {
                pFormat->rowBitAlign = 32;
                if (!ilAddFormatFilter (pipe, pInfo, pFormat, &ilFormatRowBitAlign))
                    return IL_CPF_ERROR;
                return IL_CPF_BIT;                  /* match found: bit/pixel */
                }
            }
        else {
                /*  1 sample, bit not accepted; error if byte not accepted, make byte */
            if (!(formatMask & IL_CPF_BYTE))
                return IL_CPF_ERROR;
            if (pFormat->nBitsPerSample[0] != 8) {
                if (pFormat->nBitsPerSample[0] == 4) {
                    pFormat->nBitsPerSample[0] = 8;
                    if (!ilAddFormatFilter (pipe, pInfo, pFormat, &ilFormat4To8Bit))
                        return IL_CPF_ERROR;
                    }
                else return IL_CPF_ERROR;   /* NOTE!! currently only 4-8 supported!!! */
                }
            return IL_CPF_BYTE;                     /* match found: 1 byte/pixel */
            }
        }       /* END 1 sample/pixel */
    else {      /* multi-sample/pixel */
        }

}

#endif

