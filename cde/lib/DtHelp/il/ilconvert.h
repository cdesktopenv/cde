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
/* $XConsortium: ilconvert.h /main/3 1995/10/23 15:44:19 rswiston $ */
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

#ifndef ILCONVERT_H
#define ILCONVERT_H

#ifndef IL_H
#include "il.h"
#endif

        /*  Include file for /ilc/ilconvert.c and conversion code referenced by it;
            only those files should include this file.
        */

        /*  Values for ilConvertRec.srcFormatCode.
            Format converters (those which would have ilConvertRec.pDstDes null
            and ilConvertRec.pDstFormat non-null) MUST set IL_DONT_CHECK_STD_FORMAT.
            Descriptor converters must set another code, which identifies the
            required src format, as follows (fields not named are not checked):

                IL_STD_FORMAT_BIT           src format is IL_FORMAT_BIT: rowBitAlign
                                            == 32; bitsPerSample[0] == 1.
                IL_STD_FORMAT_BYTE          src format is IL_FORMAT_BYTE:
                                            bitsPerSample[0] == 8.
                IL_STD_FORMAT_3BYTE_PIXEL   src format is IL_FORMAT_3BYTE_PIXEL:
                                            bitsPerSample[0,1,2] == 8; sampleOrder ==
                                            IL_SAMPLE_PIXELS
        */
#define IL_DONT_CHECK_STD_FORMAT    0
#define IL_STD_FORMAT_BIT           1
#define IL_STD_FORMAT_BYTE          2
#define IL_STD_FORMAT_3BYTE_PIXEL   3

        /*  Standard definition for a conversion filter added by ilConvert().
            Basically the parameter values to be passed to ilAddPipeElement() to
            add a given pipe converter.  Some basic rules:
                - the filter must take any stripHeight, and output the same
                - it must not change the height/width of the image
                - pDstDes and pDstFormat in the structure must point to the output
                  des/format, or be null if the output des/format is the
                  same as the input (can't both be null - that is no conversion!)
            "srcFormatCode" is a code for the required src (input) format to
            this element.  See notes above.
            The function "AddElement()" if non-null is called after the pipe
            element is successfully added, as:
                (*AddElement) (<pPrivate from ilAddPipeElement>, pPalette);
            It can be used for example to init the private block with data
            other than what might be done by Init().
        */
typedef struct {
    ilError           (*CheckFormat)();         /* reserved: MUST BE IL_NPF! */
    unsigned int        srcFormatCode;          /* see above */
    ilError           (*AddElement)();          /* see above */
    const ilImageDes    *pDstDes;               /* ptr to new des or null */
    const ilImageFormat *pDstFormat;            /* ptr to new format or null */
    size_t              nBytesPrivate;
    ilError           (*Init)();
    ilError           (*Cleanup)();
    ilError           (*Destroy)();
    ilError           (*Execute)();
    } ilConvertRec, *ilConvertPtr;

        /*  Standard definition for a format filter added by ilConvert().
            Basically a subset of an ilConvertRec; see above.
        */
typedef struct {
    ilError           (*AddElement)();          /* see above */
    unsigned int        nBytesPrivate;
    ilError           (*Init)();
    ilError           (*Cleanup)();
    ilError           (*Destroy)();
    ilError           (*Execute)();
    } ilFormatRec, *ilFormatPtr;


    /*  In /ilc/ildither.c : */

IL_EXTERN const unsigned int _ilDitherKernel[];
IL_EXTERN const unsigned short _ilMul8[];
IL_EXTERN const unsigned short _ilMul4[];


    /*  Convert from source type (pDes->type) == RGB to Palette.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format. *pDes ->the src (pipe) des; on return *pDes is the new des.
    */
IL_EXTERN ilBool _ilConvertRGBToPalette (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes    *pDes,
    ilImageFormat          *pFormat,
    int                     option,
    ilConvertToPaletteInfo *pData
    );

        /*  Dithers using error diffusion to bitonal.
            Input image:  IL_DES_GRAY, IL_FORMAT_BYTE.
            Output image: IL_DES_BITONAL, IL_FORMAT_BIT
        */
IL_EXTERN ilConvertRec _ilDiffuseGrayToBitonal;

        /*  Convert using a user-supplied threshold.
            Input image:  IL_DES_GRAY (either form of blackIsZero), IL_FORMAT_BYTE.
            Output image: IL_DES_BITONAL, IL_FORMAT_BIT
        */
IL_EXTERN ilConvertRec _ilThresholdGrayToBitonal;

        /*  In /ilc/ilpalette.c :
            Converts byte palette image to RGB, each a byte
            Input image:   uncompressed IL_PALETTE, any # of levels, IL_FORMAT_3BYTE_PIXEL.
            Output image:  IL_DES_GRAY, IL_FORMAT_BYTE.
        */
IL_EXTERN ilConvertRec _ilPaletteToRGBByte;


        /*  In /ilc/iltruegray.c :
            Converts rgb image to gray scale.
            Input image:   uncompressed IL_RGB, any # of levels, IL_FORMAT_3BYTE_PIXEL.
            Output image:  IL_DES_GRAY, IL_FORMAT_BYTE.
        */
IL_EXTERN ilConvertRec _ilRGBToGray;

        /*  In /ilc/iltruegray.c :
            Converts gray scale image to rgb.
            Input image:   uncompressed IL_GRAY, any # of levels, IL_FORMAT_BYTE.
            Output image:  IL_DES_RGB, IL_FORMAT_3BYTE_PIXEL.
        */
IL_EXTERN ilConvertRec _ilGrayToRGB;

        /*  In /ilc/ilformat.c :
            Reformats image to change the rowBitAlign only.
        */
IL_EXTERN ilFormatRec _ilFormatRowBitAlign;

        /*  Convert 4 to 8 bit or 8 to 4 bit per sample.
        */
IL_EXTERN ilFormatRec _ilFormat4To8Bit;

IL_EXTERN ilFormatRec _ilFormat8To4Bit;

        /*  Reformat an 8 bit/pixel 3 sample planar image to pixel format.
        */
IL_EXTERN ilFormatRec _ilFormat8Bit3PlaneToPixel;

        /*  Reformat an 8 bit/pixel 3 sample pixel image to planar format.
        */
IL_EXTERN ilFormatRec _ilFormat8Bit3PixelToPlane;

        /*  In /ilc/ilformat.c :
            Add a conversion filter to the given pipe which converts from the levels
            in "*pPipeDes" (i.e. the levels of the current pipe image) to those in
            "*pDes" (note - only nLevelsPerSample is reference from pDes).
            If failure, declares pipe invalid (which sets error code) and returns false; 
            else returns true and "*pInfo", "*pPipeDes" and "*pFormat" are updated
            with the new pipe state info.
        */
IL_EXTERN ilBool _ilAddLevelConversionFilter (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pPipeDes,
    const ilImageDes       *pDes,
    ilImageFormat          *pFormat
    );


        /*  In /ilc/ilinvert.c :
            Add a filter to invert a bitonal or gray image.
        */
IL_EXTERN ilBool _ilAddInvertFilter (
      ilPipe        pipe,
      ilImageDes    *pPipedes,
      ilPipeInfo    *pInfo
      );

    /*  In /ilc/ilupsample.c : */

    /*  Attempt to upsample and scale based on "scaleFactor" (0 = no scaling; 1 = double;
        2 = 4x), or return false if it cannot be done with the given pipe image (in which
        case pipe->context->error == 0) or if error occurs (error != 0).  Pipe image
        must be decompressed before calling this function.
    */
IL_EXTERN ilBool _ilFastUpsample (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    int                     scaleFactor
    );

    /*  Upsample and / or convert to gray the pipe image which must be a YCbCr image.
        If "toGray" is true, the Y plane only will be upsampled (or copied) resulting
        in a gray image; else a planar YCbCr image will result.
        If "upSample" is true must upsample; "upSample" and/or "toGray" must be true.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format, *pDes to the dst descriptor.
    */
IL_EXTERN ilBool _ilUpsampleYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    ilBool                  toGray,
    ilBool                  upSample
    );

    /*  In /ilc/ilycbcr.c : */

    /*  Subsample the pipe image which must be a fully upsampled YCbCr image.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format.  *pDes to the _destination_ des, i.e. has subsample values.
    */
IL_EXTERN ilBool _ilSubsampleYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    );

   /*   Convert from source type (pDes->type) == IL_YCBCR to RBB.  
        The YCbCr image must already be upsampled.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format, *pDes to the dst descriptor.
    */
IL_EXTERN ilBool _ilConvertYCbCrToRGB (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    );

    /*  Convert from source type (pDes->type) == RGB to YCbCr.
        pFormat points to the source format; on return, *pFormat is updated
        to the dst format. pNewDes points to the dst dest; *pDes to the src (pipe) des;
        on return it is updated to the new descriptor.
    */
IL_EXTERN ilBool _ilConvertRGBToYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    const ilImageDes *pNewDes,
    ilImageFormat          *pFormat
    );

    /*  Does conversions of some forms of YCbCr to dithered IL_PALETTE.
        Returns "true" if conversion handled, else "false" if not handled or if error.
        The pipe image must be an uncompressed YCbCr image.
    */
IL_EXTERN ilBool _ilDitherYCbCr (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    int                     option,
    ilConvertToPaletteInfo *pData
    );

#endif
