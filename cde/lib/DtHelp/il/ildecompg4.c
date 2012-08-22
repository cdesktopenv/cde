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
/* $XConsortium: ildecompg4.c /main/6 1996/06/19 12:23:48 ageorge $ */
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


#include <math.h>
#include <stdio.h>       
#include <stdlib.h>

#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"
#include "ildecomp.h"
#include "ildecompg4.h"
#include "ildecompg4table.h"

/*  Table containing (array) the number of consecutive zeros 
    from the start (msb first), in chars  for 0x00 to 0xff 
    for e.g  number of consecutive zeros in 0x00 = 8 
                                         in 0x0f = 4               */

static unsigned char zeroruns[256] = {
	8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	/* 0x00 - 0x0f */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0x10 - 0x1f */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x20 - 0x2f */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x30 - 0x3f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x40 - 0x4f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x50 - 0x5f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x60 - 0x6f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x70 - 0x7f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 - 0x8f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 - 0x9f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 - 0xaf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 - 0xbf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 - 0xcf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 - 0xdf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 - 0xef */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xf0 - 0xff */
};


/*  Table containing (array) the number of consecutive ones 
    from the start (msb first), in chars  for 0x00 to 0xff         
    for e.g  number of consecutive ones  in 0x00 = 0 
                                         in 0xff = 8               */


static unsigned char oneruns[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00 - 0x0f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 - 0x1f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x20 - 0x2f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x30 - 0x3f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 - 0x4f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x50 - 0x5f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 - 0x6f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 - 0x7f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x80 - 0x8f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x90 - 0x9f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xa0 - 0xaf */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xb0 - 0xbf */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xc0 - 0xcf */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xd0 - 0xdf */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0xe0 - 0xef */
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	/* 0xf0 - 0xff */
};


/*  This module G4 Decompression has been totally rewritten ...  
     Earlier module handled images with LSB first only,
     and failing while handling  images Compressed in Strips.
     
     The present implementation uses the same tables, and takes care of the
     above issues. Additionally, it has been designed in such a way as that
     these decompression can be used for G3 - 2D decompression also.
                                                                             */

/* ========================================================================

        --------------------  _ilPutOnes(). --------------------
   Input : pointer to the current Image Destination line  (*sByte)
           startPixel - from which position 1's are to be put
           no_of_ones - how many pixels are to be put with ones
   Does  :

   Utility function to put 1's (no_of_ones) in the string, pointed by
   *sByte, from the position startPixel; Used to fill the Destination
   image while De-Compressing. Initially the Destination image is filled
   with Zeros 0's ; While De-Compressing,  1's are filled appropriately
   to construct the Image..                              
 
     ** used in G3 & G4 Decompression

   ======================================================================== */

IL_PRIVATE  void
_ilPutOnes(
register char *sByte,
register int startPixel,
register int  no_of_ones )

{
	static const unsigned char masks[] =
	{ 
		0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff 	};

	/* fill 1's in  the current  Byte */

	sByte += startPixel>>3;
	if (startPixel &= 7) {			/* align to byte boundary */
		if (no_of_ones < 8 - startPixel) {
			*sByte++ |= masks[no_of_ones] >> startPixel;
			return;
		}
		*sByte++ |= 0xff >> startPixel;
		no_of_ones -= 8 - startPixel;
	}

	/* fill 1's in  the consecutive  Full Bytes */
	memset(sByte,0xff,(no_of_ones >> 3));
	sByte += (no_of_ones >> 3 );
	no_of_ones = no_of_ones & 7 ;

	/* fill 1's in  the last partial  Byte */
	*sByte |= masks[no_of_ones];
}

/* ========================================================================

        --------------------  _ilGetAbsDiff(). --------------------
  Input : the pointer to the pointer to the strings (image Line)
          startPixel from which the diff. is to be found
          endPixel i.e upto which the diff can be checked.
          color of the current pixel (startPixel)
          nTimes - no. of times the counting is to be repeated i.e for 
                   how many changing elements, the operation is to be
                   performed.

  Does : 
    Calculates the count of pixels of the same color  and returns the absolute
    position of the next    changing element and Returns the Absolute Position 
    of the element

      **  used for G4,G3-2d compression and de-comression    
   ======================================================================== */

static int
_ilGetAbsDiff(	unsigned char *sByte,
int startPixel,
int endPixel,
int color,
int nTimes)
{

	register unsigned char *bp;
	register int ini_diff;
	register int n, fin_diff;
	register unsigned char *table ;

	bp = sByte;

	bp += startPixel>> 3;			/* adjust byte offset */

	do {

		if ((startPixel == -1) ) {
			fin_diff = 1;
			bp       = sByte;
			goto done;
		}
		else {

			table = (color ? oneruns : zeroruns );
			ini_diff = endPixel- startPixel;

			/*  Find difference in the partial byte on the current Byte */

			if (ini_diff > 0 && (n = (startPixel & 7))) {
				fin_diff = table[(*bp << n) & 0xff];

				if (fin_diff > 8-n)		/* Consecutive bits extend beyond the current byte */
					fin_diff = 8-n;


				/* return the initial differece, if the current byte happens
                   to be the last byte of the Imageline and cosecutive bits
                   extend beyound that.                                      */

				if (fin_diff > ini_diff)
					fin_diff = ini_diff;

				if (n + fin_diff < 8 )	/* Consecutive bits does not  go upto the edge, so return the diff */
					goto done;
				ini_diff -= fin_diff;
				bp++;
			} else
				fin_diff = 0;

			/*  Count in the bytes, till opp. color is found
                i.e while the diff >= 8                          */

			while (ini_diff >= 8) {
				n = table[*bp];
				fin_diff += n;
				ini_diff -= n;
				if (n < 8)		/* end of run */
					goto done;

				bp++;
			}

			/*  Find difference in the partial byte on RHS */

			if (ini_diff > 0) {
				n = table[*bp];
				fin_diff += (n > ini_diff ? ini_diff : n);
			}
		}
done:
		startPixel +=  fin_diff;
		color       = !color;

	} while (--nTimes > 0 );

	return(startPixel);

}
/* --------------- End of  _ilGetAbsDiff()  ---------------- */

/* ========================================================================
       -------------------- _ilDeCompressG4Init -------------------
   Routine defined in ilDeCompG4 for initializing CCITT Group3  
   compression when the pipe gets executed.

   ======================================================================== */

static ilError _ilDecompG4Init(
ilDecompG3G4PrivPtr     pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{
	/* Allocate space for Reference line, needed for 2 dimensional coding */

	pPriv->gpRefLine = (ilPtr)IL_MALLOC(pPriv->nDstLineBytes );
	if (!pPriv->gpRefLine)
		return IL_ERROR_MALLOC;
	return IL_OK;

}
/* ========================================================================
       -------------------- _ilDeCompressG4Cleanup -------------------
   ======================================================================== */

static ilError _ilDecompG4Cleanup(
ilDecompG3G4PrivPtr     pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{
	/* De-Allocate the space for Reference line */
	if (pPriv->gpRefLine)
		IL_FREE( (ilPtr)pPriv->gpRefLine);
	return IL_OK;

}


/* Macros used in _ilDecompG4Line()            */
/* Macros for Getting the Number of White Runs */

#define G4_GET_WHITE_RUN(whiteRun) {                            \
  whiteRun = 0;                                                 \
   do {                                                         \
        if ( Is_Lsb_First )                                     \
           GET_VALUE_LSB(bits,ImageP, srcpos,  G4M_WhiteRun)    \
        else                                                    \
           GET_VALUE_MSB(bits,ImageP, srcpos,  G4M_WhiteRun)    \
        pDecodeTemp = pDecodeWhite+bits;                        \
        srcpos += pDecodeTemp->length;                          \
        whiteRun += pDecodeTemp->value;                         \
    } while (pDecodeTemp->type != G4K_CodetypeTerminator) ;     \
}

/* Macros for Getting the Number of Black Runs */

#define G4_GET_BLACK_RUN(blackRun) {                            \
  blackRun = 0;                                                 \
   do {                                                         \
        if ( Is_Lsb_First )                                     \
           GET_VALUE_LSB(bits,ImageP, srcpos,  G4M_BlackRun)    \
        else                                                    \
           GET_VALUE_MSB(bits,ImageP, srcpos,  G4M_BlackRun)    \
        pDecodeTemp = pDecodeBlack+bits;                        \
        srcpos += pDecodeTemp->length;                          \
        blackRun += pDecodeTemp->value;                         \
    } while (pDecodeTemp->type != G4K_CodetypeTerminator) ;     \
}

/* ========================================================================    
    -------------------- _ilDecompG4Line -------------------
   Input : pointer to the Private data record or decompG3G4
           pointer to the Reference line
           pointer to the Destination image
   Does  : Reading the Source Image, De-Compresses One line for the destn.
           image by Two dimensional coding.
   ======================================================================== */
ilError   _ilDecompG4Line(
register ilDecompG3G4PrivPtr  pPriv,
ilPtr          pRefLine,
ilPtr          dstImageP
)
{

	int a0,b1,b2;            /* changing elements used  while decomressing */
	short color;             /* color of the pixel                         */
	int firstRun;            /* no. of consecutive pixels for M(a0a1) in  Horiz mode */
	int secondRun;           /* no. of consecutive pixels for M(a1a2) in  Horiz mode */

	long bits ;
	long width;
	ilBool Is_Lsb_First ;
	ilPtr ImageP;
	int srcpos;

	register ilDecompG4HuffTablePtrConst pDecodeWhite = ilArFax1DDecodeWhite;
	register ilDecompG4HuffTablePtrConst pDecodeTemp;
	register ilDecompG4HuffTablePtrConst pDecodeBlack = ilArFax1DDecodeBlack;


	/*  Deccompression Procedure  ....

    From the compressed data from Source Image .. retrieve a long bit; use this as
    index to determine the 2d coded mode (Pass or Horiz or Vert(-3 to 3) );
    Depending upon the mode, calculate the changing elements b1,b2 and adjust a0;
    If 1's have to filled to the destination image, call _ilPutOnes(..);
    If Horiz. mode is identified, call macros G4_GET_WHITERUN(..) & G4_GETBLACKRUN(..)
    to estimate the first and second runs and fill 1's;
    This loop will have to repeated until the value of a0 reaches the ImageWidth.
    See ilcompressg4.c for more infn. on G4 compression .

   */

	width = pPriv->width;

	a0    = -1;
	color = pPriv->white;
	Is_Lsb_First =  pPriv->Is_Lsb_First;
	srcpos       =  pPriv->srcpos;
	ImageP       =  pPriv->ImageP;

	do {                  /* till a complete Image line is DeCompressed */

		if ( Is_Lsb_First )
		    GET_VALUE_LSB(bits,ImageP, srcpos, G4M_Codeword)
		else
			GET_VALUE_MSB(bits,ImageP, srcpos, G4M_Codeword)

		if (ilArFax2DDecodeTable[bits].type != G4K_CodetypeTerminator)
			return  IL_ERROR_COMPRESSED_DATA ;

		srcpos += ilArFax2DDecodeTable[bits].length;

		switch (ilArFax2DDecodeTable[bits].value) {

			/* State mode is codeword, codeword type is TERMINATOR,
                     codeword value is PASS */

		case G4K_CodevaluePass:

			b2 = _ilGetAbsDiff(pRefLine, a0, width, !color,3);
			if (color) {
				if (a0 < 0)  	a0 = 0;
				if ( (b2-a0) > 0 )
					_ilPutOnes((char *)dstImageP, a0, b2 - a0);
			}
			a0 = b2;
			break;

			/* State mode is Codeword, codeword type is TERMINATOR,
                    codeword value is HORIZONTAL */

		case G4K_CodevalueHoriz:

			if (color == pPriv->white) {
				G4_GET_WHITE_RUN(firstRun) ;
				G4_GET_BLACK_RUN(secondRun);
			}
			else {
				G4_GET_BLACK_RUN(firstRun);
				G4_GET_WHITE_RUN(secondRun) ;
			}

			if (a0 < 0)  	a0 = 0;
			if (a0 + firstRun > width)
				firstRun = width - a0;
			if ( (color) && ( firstRun > 0 ) )
				_ilPutOnes((char *)dstImageP, a0, firstRun);

			a0 += firstRun;

			if (a0 + secondRun > width)
				secondRun = width - a0;
			if ( (!color) && ( secondRun > 0 ) )
				_ilPutOnes((char *)dstImageP, a0, secondRun);
			a0 += secondRun;
			break;


			/* State mode is codeword, codeword type is TERMINATOR, 
                     codeword value is VERTICAL */

		case G4K_CodevalueV0:
		case G4K_CodevalueVR1:
		case G4K_CodevalueVL1:
		case G4K_CodevalueVR2:
		case G4K_CodevalueVL2:
		case G4K_CodevalueVR3:
		case G4K_CodevalueVL3:

			b1 = _ilGetAbsDiff(pRefLine, a0, width, !color,2);
			b1 += ilArFax2DDecodeTable[bits].value;
			if (b1 > width)
               b1 = width;
			if (color) {
				if (a0 < 0)  a0 = 0;
				if ( (b1-a0) > 0 )
					_ilPutOnes((char *)dstImageP, a0, b1 - a0);
			}
			color = !color;
			a0 = b1;
			break;

		default:
			/* found invalid codeword values, return error  */
			return  IL_ERROR_COMPRESSED_DATA ;
		}

	} while (a0 < width);       /* End of Image line is reached */

	pPriv->srcpos =   srcpos;
	return IL_OK ;

}

/* ========================================================================    
          
    -------------------- ilDecompG4Execute -------------------
    Routine defined in ilDecompG4 for executing CCITT Group4
    decompression when the pipe gets executed.

   ======================================================================== */

static ilError _ilDecompG4Execute(
ilExecuteData      *pData,
unsigned long       dstLine,
unsigned long      *pNLines
)
{
	/* ========================================================================
   ilDecompG4Execute() definitions
   ======================================================================== */

	ilImagePlaneInfo   *pSrcPlane;         /* Pointer to the Source Image Plane     */
	ilImagePlaneInfo   *pDstPlane;         /* Pointer to the Source Image Plane     */
	ilPtr               pSrcLine;          /* Pointer to the Source Image FirstLine */
	ilDecompG3G4PrivPtr pPriv;             /* Pointer to private image data         */

	ilPtr               dstImageP;         /* Pointer to the Destn. Image           */
	ilPtr               pRefLine;          /* Pointer to the Reference line         */
	ilError             error;             /* Returned error                        */
	int                 dstBytesPerRow;    /* no.of byte per Row in the dest image  */
    long                nLines;            /* no. of lines in the current strip     */

	/* ========================================================================
   ilDecompG4Execute() set up for decompression algorithm code
   ======================================================================== */

	pPriv   = (ilDecompG3G4PrivPtr) pData->pPrivate;

	if ( *pNLines <= 0 ) return IL_OK ;
	if ( pData->compressed.nBytesToRead <= 0 ) return IL_OK ;                         
    nLines  = *pNLines;

	/* Exit if pointer to pPixels is NULL */
	pSrcPlane           = &pData->pSrcImage->plane[0];
	if (!pSrcPlane->pPixels) return IL_ERROR_NULL_COMPRESSED_IMAGE;
	pSrcLine            =  pSrcPlane->pPixels + pData->compressed.srcOffset; /* image location pointer */

	pDstPlane           = &pData->pDstImage->plane[0];

	/* The destination image line pointer gets updated at the beginning of each strip */
	dstImageP          = (pDstPlane->pPixels + (dstLine * pDstPlane->nBytesPerRow));
	dstBytesPerRow     = pDstPlane->nBytesPerRow ;

	/* ========================================================================
    Zero the output (dst) buffer.  _ilPutOnes()  writes only ones, and expects 
    that    the dst lines have already been zeroed. 
    ======================================================================== */
	bzero ((char *)dstImageP, (pDstPlane->nBytesPerRow * *pNLines) );

	/* Allocate space for the Reference line and set to zero Or 1's         */

	if (pPriv->white)
		memset(pPriv->gpRefLine,0xff,(pDstPlane->nBytesPerRow ));
	else
		memset(pPriv->gpRefLine,0x00,(pDstPlane->nBytesPerRow ));

	pRefLine = pPriv->gpRefLine;

	pPriv->srcpos        =  0;
	pPriv->ImageP        = pSrcLine;
	pPriv->Is_Lsb_First  =  ( ( pPriv->compFlags  & IL_G4M_LSB_FIRST) ? 1 : 0 );

    /* this pPriv->maxSrcPos is set to the bits that could be read, to prevent the
       program from reading beyond the compressed bytes; this check is done by
       the macro Get_value_msb & lsb                                              */

    pPriv->maxSrcPos     =  pData->compressed.nBytesToRead * 8 ;

	/* till the Destination Image is filled up , call the decompress from the
       source line by line.. If any error in reading the compressed data, i.e
       invalid Code words or invalid Modes are found will retrun the error
       IL_ERROR_COMPRESSED_DATA .
       After decompressing each line, the decompressed line is set as reference
       line for the next line to be decompressed.
       Destination image size and Pointer are suitably decremented & incremented.
    */

	while ( nLines-- > 0 )
	{

		if (error = _ilDecompG4Line(pPriv,pRefLine,dstImageP) )
			return error;

		pRefLine       =   dstImageP;
		dstImageP     +=   dstBytesPerRow;

	}

	return IL_OK;
}
/* End ilDecompG4Execute() */

/* ========================================================================

          -------------------- ilDecompG4() -------------------
    Entry point of code for CCITT Group4 decompression.  This
    includes image descriptor parameter error checking and function calls
    for: strip handler initialization, adding the filter element to the
    pipe, pipe initialization and execution, decompression algorithm,
    along with cleanup and destruction of allocated memory.....

   ======================================================================== */

IL_PRIVATE
ilBool _ilDecompG4 (
ilPipe              pipe,
ilPipeInfo         *pinfo,                              
ilImageDes         *pimdes
)
{

	ilDstElementData    dstdata;
	ilDecompG3G4PrivPtr       pPriv;
	ilImageDes          des;

	/*  Validate that image is bitonal */
	if (pimdes->type != IL_BITONAL)
		return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);

	/*
        Check for Group4, uncompressed, or any undefined bits on.  These
        are not supported!
    */
	if (pimdes->compInfo.g4.flags & IL_G4M_UNCOMPRESSED )
		return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);


	/* dstdata describes strips being output to next pipe element */
	dstdata.producerObject  =  (ilObject) NULL;
	des                     = *pimdes;
	des.compression         =  IL_UNCOMPRESSED;
	des.compInfo.g4.flags   =  NULL;
	dstdata.pDes            =  &des;
	dstdata.pFormat         =  IL_FORMAT_BIT;
	dstdata.width           =  pinfo->width;
	dstdata.height          =  pinfo->height;
	dstdata.pPalette        =  (unsigned short *)NULL;

	/* set output strip height */
	dstdata.stripHeight     =  pinfo->stripHeight;
	dstdata.constantStrip   =  pinfo->constantStrip;

	dstdata.pCompData       =  (ilPtr)NULL;

	pPriv = (ilDecompG3G4PrivPtr) ilAddPipeElement(pipe, IL_FILTER, 
	    sizeof(ilDecompG3G4PrivRec), 0,
	    (ilSrcElementData *)NULL,
	    &dstdata, _ilDecompG4Init,_ilDecompG4Cleanup,
	    IL_NPF, _ilDecompG4Execute, NULL, 0);

	if (!pPriv) return FALSE; /* EXIT */

	/* save private data */
	pPriv->width         = pinfo->width;
	pPriv->white         =  ( des.blackIsZero ? 1 : 0 );
	pPriv->compFlags     = pimdes->compInfo.g4.flags;
	pPriv->nDstLineBytes = (pPriv->width + 7) / 8;
	return TRUE;
}
