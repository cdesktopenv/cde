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
/* $XConsortium: il.h /main/6 1996/09/24 17:12:23 drk $ */
/**---------------------------------------------------------------------
***	
***    file:           il.h
***
***    description:    Main public include file for Image Library (IL).
***
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



#ifndef IL_H
#define IL_H 

#include <X11/Xos.h>
#include <X11/Xfuncs.h>

/* include Xmd.h for CARD32 and INT32 definitions */
#include <X11/Xmd.h>

#ifndef NeedFunctionPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define NeedFunctionPrototypes 1
#else
#define NeedFunctionPrototypes 0
#endif /* __STDC__ */
#endif /* NeedFunctionPrototypes */

#ifndef NeedWidePrototypes
#if defined(NARROWPROTO)
#define NeedWidePrototypes 0
#else
#define NeedWidePrototypes 1      /* default to make interropt. easier */
#endif
#endif

#ifdef __cplusplus               /* do not leave open across includes */
extern "C" {                                    /* for C++ V2.0 */
#endif


        /*  Values for ilImageDes. */

        /*  ilImageDes.nSamplesPerPixel max */
#define IL_MAX_SAMPLES      5

        /*  ilImageDes.type */
#define IL_BITONAL          0
#define IL_GRAY             1
#define IL_PALETTE          2
#define IL_RGB              3
#define IL_YCBCR            4

        /*  ilImageDes.compression */
#define IL_UNCOMPRESSED     0
#define IL_G3               1
#define IL_JPEG             2
#define IL_G4               3
#define IL_LZW              4
#define IL_PACKBITS         5

        /*  ilImageDes.compInfo.g3.flags masks.  Use IL_G3M_TIFF for 
            TIFF-compatible (comp 2); IL_G3M_CLASS_F for Class F (comp code 3).
        */
#define IL_G3M_LSB_FIRST     (1<<0)
#define IL_G3M_2D            (1<<1)
#define IL_G3M_UNCOMPRESSED  (1<<2)
#define IL_G3M_EOL_MARKERS   (1<<3)
#define IL_G3M_EOL_UNALIGNED (1<<4)
#define IL_G3M_K_FACTOR_2    (1<<5)     

#define IL_G3M_TIFF          0
#define IL_G3M_CLASS_F       (IL_G3M_LSB_FIRST | IL_G3M_EOL_MARKERS)

        /*  ilImageDes.compInfo.g4.flags masks. */
#define IL_G4M_LSB_FIRST     (1<<0)
#define IL_G4M_UNCOMPRESSED  (1<<2)

        /*  ilObject->objectType */
#define IL_NULL_OBJECT              0
#define IL_PIPE                     1
#define IL_FILE                     2
#define IL_INTERNAL_IMAGE           3
#define IL_CLIENT_IMAGE             4
#define IL_XWC                      5
#define IL_EFS_FILE_TYPE            6
#define IL_EFS_FILE                 7

        /*  ilImageFormat->byteOrder */
#define IL_MSB_FIRST                0
#define IL_LSB_FIRST                1

        /*  ilImageFormat->sampleOrder */
#define IL_SAMPLE_PIXELS            0
#define IL_SAMPLE_PLANES            1

        /*  ilExecutePipe() return values */
#define IL_EXECUTE_AGAIN            0
#define IL_EXECUTE_COMPLETE         1
#define IL_EXECUTE_ERROR            2

        /*  ilQueryPipe() return values */
#define IL_PIPE_INVALID             0
#define IL_PIPE_EMPTY               1
#define IL_PIPE_FORMING             2
#define IL_PIPE_COMPLETE            3
#define IL_PIPE_EXECUTING           4

        /* Value meaning "no error" for ilError: other errors in ilerrors.h */
#define IL_OK                       0

        /*  option to ilConvert() */
#define IL_CONVERT_NO_OPTION        0
#define IL_CONVERT_THRESHOLD        1
#define IL_THRESHOLD_TO_BITONAL     IL_CONVERT_THRESHOLD
#define IL_CONVERT_TO_PALETTE       2
#define IL_CONVERT_SOFT_INVERT      3

        /*  values for ilConvertToPaletteInfo.method */
#define IL_AREA_DITHER              0
#define IL_DIFFUSION                1
#define IL_QUICK_DIFFUSION          2
#define IL_CHOOSE_COLORS            3

        /*  Option1 to ilScale() */
#define IL_SCALE_SIMPLE             0
#define IL_SCALE_SAMPLE             1
#define IL_SCALE_BITONAL_TO_GRAY    2

        /*  Options to ilMirror() */
#define IL_MIRRORX                  0
#define IL_MIRRORY                  1

        /*  values for ilImageDes.flags */
#define IL_DITHERED_PALETTE         (1<<0)

typedef unsigned char ilByte;
typedef ilByte *ilPtr;
typedef int ilBool;
typedef short ilShortBool;
typedef int ilError;

        /*  ilImageDes and predefined image descriptors */

typedef struct {
    short       refBlack, refWhite;
    ilByte      subsampleHoriz, subsampleVert;
    } ilYCbCrSampleInfo;

typedef struct {
    ilYCbCrSampleInfo sample[3];    /* [Y,Cb,Cr] */
    short       lumaRed, lumaGreen, lumaBlue;      /* fractions of 10000 */
    short       positioning;        /* must be zero (0) */
   } ilYCbCrInfo;

typedef struct {
    short       levels[3];
    } ilPaletteInfo;

typedef struct {
    unsigned long       flags;
   } ilG3Info;

typedef struct {
    unsigned long       flags;
   } ilG4Info;

typedef struct {
    short               reserved;               /* must be zero (0) */
    short               process;
   } ilJPEGInfo;

#define IL_DES_RESERVED_SIZE  4

typedef struct _ilImageDes {
    unsigned short      type;
    unsigned short      compression;
    ilShortBool         blackIsZero;
    unsigned short      nSamplesPerPixel;
    unsigned long       noLongerUsed;           /* must set to 0x10001 */
    union {
        ilG3Info        g3;
        ilG4Info        g4;
        ilJPEGInfo      JPEG;
        } compInfo;
    long                reserved [IL_DES_RESERVED_SIZE];
    unsigned long       flags;
    union {
        ilYCbCrInfo     YCbCr;
        ilPaletteInfo   palette;
        long            filler [8];
        } typeInfo;
    long                nLevelsPerSample [IL_MAX_SAMPLES];
    } ilImageDes;

#define IL_INIT_IMAGE_DES(_pDes) {                \
    bzero ((char *)(_pDes), sizeof (ilImageDes)); \
    (_pDes)->noLongerUsed = 0x10001;              \
    }

extern const ilImageDes ilBitonal0WhiteImageDes, ilGray256ImageDes, ilRGB256ImageDes;
extern const ilImageDes ilPaletteImageDes, ilYCbCrImageDes, ilYCbCr2ImageDes;

#define IL_DES_BITONAL          (&ilBitonal0WhiteImageDes)
#define IL_DES_GRAY             (&ilGray256ImageDes)
#define IL_DES_RGB              (&ilRGB256ImageDes)
#define IL_DES_PALETTE          (&ilPaletteImageDes)
#define IL_DES_YCBCR            (&ilYCbCrImageDes)
#define IL_DES_YCBCR_2          (&ilYCbCr2ImageDes)

        /*  ilImageFormat and predefined image formats */
#define IL_FORMAT_RESERVED_SIZE 4


        /* Byte flip table used for bitonal rotate & mirror filters and xbm decode */
extern  const unsigned char ilBitReverseTable [256];


typedef struct {
    unsigned short      sampleOrder;
    unsigned short      byteOrder;
    unsigned short      rowBitAlign;
    long                reserved [IL_FORMAT_RESERVED_SIZE];
    unsigned short      nBitsPerSample [IL_MAX_SAMPLES];
    } ilImageFormat;

#define IL_INIT_IMAGE_FORMAT(_pFormat) {                \
    bzero ((char *)(_pFormat), sizeof (ilImageFormat)); \
    }

extern const ilImageFormat ilBitImageFormat, ilByteImageFormat, 
                     il3BytePixelImageFormat, il3BytePlaneImageFormat;

#define IL_FORMAT_BIT           (&ilBitImageFormat)
#define IL_FORMAT_BYTE          (&ilByteImageFormat)
#define IL_FORMAT_3BYTE_PIXEL   (&il3BytePixelImageFormat)
#define IL_FORMAT_3BYTE_PLANE   (&il3BytePlaneImageFormat)


typedef struct {
    ilError     error;
    long        errorInfo;
    } ilContextPublicRec;
typedef ilContextPublicRec *ilContext;


typedef struct {
    ilContext   context;
    int         objectType;
    ilPtr       pPrivate;
    } ilObjectPublicRec;
typedef ilObjectPublicRec *ilObject;

typedef ilObject ilPipe;
typedef ilObject ilInternalImage;
typedef ilObject ilClientImage;

typedef struct {
    long                x;
    long                y;
    long                width;
    long                height;
    } ilRect;


typedef struct {
    ilPtr               pPixels;
    long                nBytesPerRow;
    long                bufferSize;
    long                filler;
    } ilImagePlaneInfo;

typedef struct {
    const ilImageDes    *pDes;
    const ilImageFormat *pFormat;
    long                width, height;
    unsigned short      *pPalette;
    ilPtr               pCompData;
    ilShortBool         clientPalette;
    ilShortBool         clientCompData;
    ilShortBool         clientPixels;
    short               filler [9];
    ilImagePlaneInfo    plane [IL_MAX_SAMPLES];
    } ilImageInfo;

typedef struct {
    short               mustbezero;
    short               Q;
    } ilJPEGEncodeControl;

typedef struct {
    short               method;
    short               levels [3];
    short               kernelSize;
    short               dstType;
    ilObject            mapImage;
    } ilConvertToPaletteInfo;



#define IL_INTERNAL_VERSION     4
extern ilError ilInternalCreateContext (
#if NeedFunctionPrototypes
    int                 versionCheck,
    ilContext          *pContextReturn,         /* RETURNED */
    unsigned long       mustBeZero
#endif
    );

#define IL_CREATE_CONTEXT(_pContext, _zero) \
    (ilInternalCreateContext (IL_INTERNAL_VERSION, \
         (_pContext), (_zero)))

extern ilBool ilDestroyContext (
#if NeedFunctionPrototypes
    ilContext           context
#endif
    );


extern ilBool ilDestroyObject (
#if NeedFunctionPrototypes
    ilObject            object
#endif
    );

extern ilInternalImage ilCreateInternalImage (
#if NeedFunctionPrototypes
    ilContext           context,
    long                width,
    long                height,
    ilImageDes         *pImageDes,
    unsigned long       mustBeZero
#endif
    );

extern ilBool ilQueryInternalImage (
#if NeedFunctionPrototypes
    ilInternalImage     image,
    long               *pWidth,                 /* RETURNED */
    long               *pHeight,                /* RETURNED */
    ilImageDes         *pImageDes,              /* RETURNED */
    unsigned long       mustBeZero
#endif
    );

extern unsigned int ilGetPrivateType (
#if NeedFunctionPrototypes
    ilContext           context
#endif
    );

extern ilClientImage ilCreateClientImage (
#if NeedFunctionPrototypes
    ilContext           context,
    ilImageInfo        *pInfo,
    unsigned long       mustBeZero
#endif
    );

extern ilBool ilQueryClientImage (
#if NeedFunctionPrototypes
    ilClientImage       image,
    ilImageInfo       **ppInfo,                 /* RETURNED */
    unsigned long       mustBeZero
#endif
    );

extern ilPipe ilCreatePipe (
#if NeedFunctionPrototypes
    ilContext           context,
    unsigned long       mustBeZero
#endif
    );

extern ilBool ilEmptyPipe (
#if NeedFunctionPrototypes
    ilPipe              pipe
#endif
    );


extern unsigned int ilQueryPipe (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    long               *pWidth,                 /* RETURNED */
    long               *pHeight,                /* RETURNED */
    ilImageDes         *pImageDes               /* RETURNED */
#endif
    );

extern ilBool ilAbortPipe (
#if NeedFunctionPrototypes
    ilPipe              pipe
#endif
    );

extern int ilExecutePipe (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    long                nStrips,
    float               ratio
#endif
    );

extern ilBool ilReadImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilObject            image
#endif
    );

extern ilBool ilWriteImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilObject            image
#endif
    );

extern ilBool ilScale (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    long                dstWidth,
    long                dstHeight,
    int                 option,
    void               *pOptionData
#endif
    );

extern ilBool ilRotate90 (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    int                 factor
#endif
    );

extern ilBool ilMirror (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    int                 direction
#endif
    );

extern ilBool ilCrop (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilRect             *pRect
#endif
    );

extern ilBool ilMap (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilClientImage       mapImage
#endif
    );

extern ilBool ilConvert (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    const ilImageDes    *pDes,
    const ilImageFormat *pFormat,
    int                 option,
    void                *pOptionData
#endif
    );

extern ilBool ilCompress (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    unsigned int        compression,
    ilPtr               pData,
    long                dstStripHeight,
    unsigned long       mustBeZero
#endif
    );
      
extern ilBool ilFeedFromImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilObject            image,
    long                height,
    long                stripHeight,
    ilBool              constantStrip
#endif
    );

extern int ilFeedPipe (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    long                start,
    long                nLines,
    long                nCompBytes,
    unsigned long       mustBeZero
#endif
    );

extern ilPipe ilTapPipe (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    unsigned long       mustBeZero,
    void               *mustBeNull
#endif
    );

#ifdef __cplusplus
}                                /* for C++ V2.0 */
#endif

#endif
