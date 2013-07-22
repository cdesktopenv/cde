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
/* $XConsortium: ildata.c /main/3 1995/10/23 15:44:34 rswiston $ */
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

        /*  /ilc/ildata.c : Various statically-initialized read-only data.
        */

#include "ilint.h"

        /*  Standard image descriptors and formats, referenced by #defines in /ilinc/il.h 
        */
const ilImageDes ilBitonal0WhiteImageDes = {             /* IL_DES_BITONAL */
    /* type             */  IL_BITONAL,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  FALSE,
    /* nSamplesPerPixel */  1,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 0, 0, 0, 0 } },
    /* nLevelsPerSample */  { 2 }
    };

const ilImageDes ilGray256ImageDes = {                   /* IL_DES_GRAY */
    /* type             */  IL_GRAY,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  TRUE,
    /* nSamplesPerPixel */  1,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 0, 0, 0, 0 } },
    /* nLevelsPerSample */  { 256 }
    };

const ilImageDes ilRGB256ImageDes = {                    /*  IL_DES_RGB */
    /* type             */  IL_RGB,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  FALSE,
    /* nSamplesPerPixel */  3,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 0, 0, 0, 0 } },
    /* nLevelsPerSample */  { 256, 256, 256 }
    };

const ilImageDes ilYCbCrImageDes = {                    /*  IL_DES_YCBCR */
    /* type             */  IL_YCBCR,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  FALSE,
    /* nSamplesPerPixel */  3,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 255, 1, 1 }, { 128, 255, 1, 1 }, { 128, 255, 1, 1 } },
                             2990, 5870, 1140, 0} },
    /* nLevelsPerSample */  { 256, 256, 256 }
    };

const ilImageDes ilYCbCr2ImageDes = {                   /*  IL_DES_YCBCR_2 */
    /* type             */  IL_YCBCR,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  FALSE,
    /* nSamplesPerPixel */  3,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 255, 1, 1 }, { 128, 255, 2, 2 }, { 128, 255, 2, 2 } },
                             2990, 5870, 1140, 0} },
    /* nLevelsPerSample */  { 256, 256, 256 }
    };

const ilImageDes ilPaletteImageDes = {                   /* IL_DES_PALETTE */
    /* type             */  IL_PALETTE,
    /* compression      */  IL_UNCOMPRESSED,
    /* blackIsZero      */  FALSE,
    /* nSamplesPerPixel */  1,
    /* noLongerUsed */      0x10001,
    /* compInfo.g3 */       { {0} },
    /* reserved         */  {0, 0, 0, 0},
    /* flags            */  0,
    /* typeInfo.YCbCr   */  { { { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, 0, 0, 0, 0 } },
    /* nLevelsPerSample */  { 256 }
    };



/* Byte flip table used for bitonal rotate & mirror filters */
const unsigned char ilBitReverseTable [256] = 
                          {  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
                             0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
                             0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
                             0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
                             0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
                             0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
                             0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
                             0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,                            
                             0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,                            
                             0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,                            
                             0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,                            
                             0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,                            
                             0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,                            
                             0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,                            
                             0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,                            
                             0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF  };


const ilImageFormat ilBitImageFormat =                        /* IL_FORMAT_BIT */
    { IL_SAMPLE_PIXELS, IL_MSB_FIRST, 32, { 0, 0, 0, 0 }, { 1 } };

const ilImageFormat ilByteImageFormat =                       /* IL_FORMAT_BYTE */
    { IL_SAMPLE_PIXELS, IL_MSB_FIRST, 8, { 0, 0, 0, 0 }, { 8 } };

const ilImageFormat il3BytePixelImageFormat =                 /* IL_FORMAT_3BYTE_PIXEL */
    { IL_SAMPLE_PIXELS, IL_MSB_FIRST, 8, { 0, 0, 0, 0 }, { 8, 8, 8 } };

const ilImageFormat il3BytePlaneImageFormat =                 /* IL_FORMAT_3BYTE_PLANEL */
    { IL_SAMPLE_PLANES, IL_MSB_FIRST, 8,  { 0, 0, 0, 0 }, { 8, 8, 8 } };


        /*  Table of shift values, indexed by YCbCr subsample values (1, 2 or 4) */
IL_PRIVATE const int _ilSubsampleShift [5] = {0, 0, 1, 0, 2};

