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
/* $XConsortium: ildecompg3.c /main/6 1996/06/19 12:23:58 ageorge $ */
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


/* ========================================================================
             
       -------------------- _ilDeCompressG3Init -------------------
   Routine defined in ilDecompG3 for initializing CCITT Group3  
   compression when the pipe gets executed.

   ======================================================================== */

static ilError _ilDecompG3Init(
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
       -------------------- _ilDeCompressG3Cleanup -------------------
   Routine defined in ilDecompG3 for Cleaning up CCITT Group3  
   compression when the pipe gets executed.
   ======================================================================== */

static ilError _ilDecompG3Cleanup(
ilDecompG3G4PrivPtr     pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{

	if (pPriv->gpRefLine)
		IL_FREE( (ilPtr)pPriv->gpRefLine);

	return IL_OK;

}
/* ======================================================================== */
/* ========================================================================    
    --------------------  _ilDecompG3Line  -------------------
   Input : pointer to the Private data record or decompG3G4
           pointer to the Destination image
   Does  : Reading the Source Image, De-Compresses One line for the destn.
           image by One dimensional coding.

   ======================================================================== */

static ilError   _ilDecompG3Line(
register ilDecompG3G4PrivPtr  pPriv,
ilPtr                     dstImageP
)
{

	int a0;                  /* changing elements used  while decomressing */
	short color;             /* color of the pixel                         */
	int firstRun;            /* no. of consecutive pixels for M(a0a1) in  Horiz mode */
/*	int secondRun;              no. of consecutive pixels for M(a1a2) in  Horiz mode */

	long bits ;
	long width;
	ilBool Is_Lsb_First ;
	ilPtr ImageP;
	int srcpos;

    register ilDecompG4HuffTablePtrConst pDecodeWhite = ilArFax1DDecodeWhite;
    register ilDecompG4HuffTablePtrConst pDecodeTemp;
    register ilDecompG4HuffTablePtrConst pDecodeBlack = ilArFax1DDecodeBlack;
    register ilPtr sByte;
    register int  no_of_ones;
    register int  startPixel;

	static const unsigned char fillmasks[] =
	{ 
		0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff 	};

	/*  Deccompression Procedure  ....

    From the compressed data from Source Image .. retrieve a long bit; use this as
    index to determine the number of White Pixels and then the number of
    Black Pixels... Keep retrieving, till the end of line is reached..
    (    Until the value of a0 reaches the ImageWidth. )
    The new line would have always started with a White Pixel code.
    Until the value of a0 reaches the ImageWidth.
    See ilcompressg3.c for more infn. on G3 compression .

   */

	width = pPriv->width;

	a0    = 0;

	color        = pPriv->white;
	Is_Lsb_First =  pPriv->Is_Lsb_First;
	srcpos       =  pPriv->srcpos;
	ImageP       =  pPriv->ImageP;

	do {                  /* till a complete Image line is DeCompressed */

		if (color == pPriv->white) {                 /* white is run is expected first always... */
			firstRun = 0;
			do {
				if ( Is_Lsb_First )
				    GET_VALUE_LSB(bits,ImageP, srcpos,  G4M_WhiteRun)
				else 
				    GET_VALUE_MSB(bits,ImageP, srcpos,  G4M_WhiteRun)
				pDecodeTemp = pDecodeWhite+bits;


				/* if invalid data is found .... */
				if ( ( pDecodeTemp->length == 0 ) && ( pDecodeTemp->value == 0 )
				    && ( pDecodeTemp->type == 0  ) )
					return  IL_ERROR_COMPRESSED_DATA ;

				srcpos += pDecodeTemp->length;
				firstRun += pDecodeTemp->value;

			} while (pDecodeTemp->type != G4K_CodetypeTerminator) ;

		}
		else {                                         /* look for black Run         */
			firstRun = 0;
			do {
				if ( Is_Lsb_First )
					GET_VALUE_LSB(bits,ImageP, srcpos,  G4M_BlackRun)
				else 
				    GET_VALUE_MSB(bits,ImageP, srcpos,  G4M_BlackRun)
				pDecodeTemp = pDecodeBlack+bits;

				/* if invalid data is found .... */
				if ( ( pDecodeTemp->length == 0 ) && ( pDecodeTemp->value == 0 )
				    && ( pDecodeTemp->type == 0  ) )
					return  IL_ERROR_COMPRESSED_DATA ;

				srcpos   += pDecodeTemp->length;
				firstRun += pDecodeTemp->value;
			} while (pDecodeTemp->type != G4K_CodetypeTerminator) ;

		}

		if (a0 + firstRun > width)
			firstRun = width - a0;

		if ( (color)  && (firstRun > 0)  )                   /* fill the dst image with 1's */
           {

            /* the whole block was written as a function, In-lined later
               to speedup performance in Hpux     ..                     */

        	/* fill 1's in  the current  Byte */
            sByte  = dstImageP ;
            no_of_ones = firstRun; 
            startPixel = a0 ;

        	sByte += startPixel>>3;
         	if (startPixel &= 7) {			/* align to byte boundary */
         		if (no_of_ones < 8 - startPixel) {
	        		*sByte++ |= fillmasks[no_of_ones] >> startPixel;
		    	goto putones_done;
     	    	}
	    	*sByte++ |= 0xff >> startPixel;
     		no_of_ones -= 8 - startPixel;
            if (!no_of_ones) 
		    	goto putones_done;
        	}

        	/* fill 1's in  the consecutive  Full Bytes */
        	memset(sByte,0xff,(no_of_ones >> 3));
        	sByte += (no_of_ones >> 3 );
         	no_of_ones = no_of_ones & 7 ;

        	/* fill 1's in  the last partial  Byte */
         	*sByte |= fillmasks[no_of_ones];

           }
         
putones_done:
		a0 += firstRun;
		color = !color ;

	} while (a0 < width);       /* End of Image line is reached */

	pPriv->srcpos =   srcpos;
	return IL_OK ;
}

/* ========================================================================    
    -------------------- ilDecompG3Execute -------------------
    Routine defined in ilDecompG3 for executing CCITT Group4
    decompression when the pipe gets executed.
   ======================================================================== */

static ilError _ilDecompG3Execute(
ilExecuteData      *pData,
unsigned long       dstLine,
unsigned long      *pNLines
)
{
	/* ========================================================================
   ilDecompG3Execute() definitions
   ======================================================================== */

	ilImagePlaneInfo   *pSrcPlane;         /* Pointer to the Source Image Plane     */
	ilImagePlaneInfo   *pDstPlane;         /* Pointer to the Source Image Plane     */
	ilPtr               pSrcLine;          /* Pointer to the Source Image FirstLine */
	ilDecompG3G4PrivPtr pPriv;             /* Pointer to private image data         */

	ilPtr               dstImageP;         /* Pointer to the Destn. Image           */
	ilPtr               pRefLine;          /* Pointer to the Reference line         */
	ilError             error;             /* Returned error                        */
	int                 dstBytesPerRow;    /* no.of byte per Row in the dest image  */
	ilBool              Is_2DCoding;       /* G3 2 D coding is present,  if True    */
	ilBool              Is_EOLs;           /* EOL markers are set, and will have to be decoded  */
	int                 bits;              /* to store the current bit from the source image    */
	int                 temp;              /* some temp var..                            */
	ilBool              tag_bit;           /* True or 1 for G3 1D coded line ,else G3 2d */
    long                nLines;            /* no. of lines in the current strip     */
	register ilDecompG4HuffTablePtrConst pDecodeWhite = ilArFax1DDecodeWhite;
	register ilDecompG4HuffTablePtrConst pDecodeTemp;


	/* ========================================================================
   ilDecompG3Execute() set up for decompression algorithm code
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

   /* this pPriv->maxSrcPos is set to the bits that could be read, to prevent the
       program from reading beyond the compressed bytes; this check is done by
       the macro Get_value_msb & lsb                                              */

    pPriv->maxSrcPos     =  pData->compressed.nBytesToRead * 8 ;

	pPriv->srcpos        =  0;
	pPriv->ImageP        = pSrcLine;
	pPriv->Is_Lsb_First  =   ( pPriv->compFlags  & IL_G3M_LSB_FIRST);
	Is_2DCoding          =   ( pPriv->compFlags  & IL_G3M_2D);
	Is_EOLs              =   ( pPriv->compFlags  & IL_G3M_EOL_MARKERS);


	/* For G3 - 2D files, EOLs must be present, if not Error in the Compressed
       file... so check for that                                                */
	if ( (Is_2DCoding) && (!(Is_EOLs)) )
		return        IL_ERROR_COMPRESSED_DATA ;

	/* Allocate space for the Reference line and set to zero Or 1's         */
	/* if G3 - 2 dimensional Coding is present                              */
	if ( Is_2DCoding  ) {
		if (pPriv->white)
			memset(pPriv->gpRefLine,0xff,(pDstPlane->nBytesPerRow ));
		else
			memset(pPriv->gpRefLine,0x00,(pDstPlane->nBytesPerRow ));

		pRefLine = pPriv->gpRefLine;
	}


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

		/*  If EOLs are present in the compressed Image, (they will be there at the begining
           of Each line, not at the end contrary to the meaning of EOL !! ), they have to be
           skipped... ; This loop takes care of this whether the EOL's are aligned or Not..  */

		if ( Is_EOLs ) {

			do {
				if ( pPriv->Is_Lsb_First )
					GET_VALUE_LSB(bits, pPriv->ImageP, pPriv->srcpos,  G4M_WhiteRun)
				else 
					GET_VALUE_MSB(bits, pPriv->ImageP, pPriv->srcpos,  G4M_WhiteRun)
				pDecodeTemp = pDecodeWhite+bits;

				if ( ( pDecodeTemp->length == 12 ) && ( pDecodeTemp->value == 1 )
				    && ( pDecodeTemp->type == G4K_CodetypeEol ) )
				{
					pPriv->srcpos += 1 ;
					continue;
				}
			} while ( ( pDecodeTemp->length == 12 ) && ( pDecodeTemp->value == 1 )
			    && ( pDecodeTemp->type == G4K_CodetypeEol ) ) ;

			pPriv->srcpos += 12 ;
		}                                                 /* Block Is_EOLs    */

		/*  If 2D compressed Image, then, after EOL look for the tag bit, which indicates
           the mode of compression for the next line;
           PIXEL(...), retrieves the next bit i.e tag bit, and if it is equal to 1, then
           next line sholud be decompressed 1 dimesionally...;
           so, call _ilDecompG3Line(...); 
           else, next line sholud be decompressed 2 dimesionally...and call
           _ilDecompG4Line(...);            

           If not 2D compressed Image, and if Not EOLs, then, the next row of image, will
           be from the start of the next byte, so check for that and skip to the next 
           byte...
                                                                                             */
		if ( Is_2DCoding  )  {

			/*  Retrieve the tag_bit, depending on the bit Order           */
			if ( pPriv->Is_Lsb_First )
				tag_bit =  PIXEL_LSB(pPriv->ImageP,pPriv->srcpos);
			else
				tag_bit =  PIXEL(pPriv->ImageP,pPriv->srcpos);

			if ( tag_bit ) {
				pPriv->srcpos++;
				if (error = _ilDecompG3Line(pPriv,dstImageP) )
					return error;
			}
			else {
				pPriv->srcpos++;
				if (error = _ilDecompG4Line(pPriv,pRefLine,dstImageP) )
					return error;
			}

		} else {

			if ( ( !Is_EOLs) && ( ( temp = pPriv->srcpos % 8) != 0 )  )
				pPriv->srcpos += (8-temp) ;
			if (error = _ilDecompG3Line(pPriv,dstImageP) )
				return error;
		}                                        /* is 2d coding */

		pRefLine       =   dstImageP;
		dstImageP     +=   dstBytesPerRow;

	}

	return IL_OK;
}
/* End ilDecompG3Execute() */

/* ========================================================================

          -------------------- ilDecompG3() -------------------
    Entry point of code for CCITT Group3 decompression.  This
    includes image descriptor parameter error checking and function calls
    for: strip handler initialization, adding the filter element to the
    pipe, pipe initialization and execution, decompression algorithm,
    along with cleanup and destruction of allocated memory.....

   ======================================================================== */

IL_PRIVATE
ilBool _ilDecompG3 (
ilPipe              pipe,
ilPipeInfo         *pinfo,                              
ilImageDes         *pimdes
)
{

	ilDstElementData    dstdata;
	ilDecompG3G4PrivPtr pPriv;
	ilImageDes          des;

	/*  Validate that image is bitonal */
	if (pimdes->type != IL_BITONAL)
		return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);

	/*
        Check for Group3, uncompressed, or any undefined bits on.  These
        are not supported!
    */
	if (pimdes->compInfo.g3.flags & IL_G4M_UNCOMPRESSED )
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

	pPriv = (ilDecompG3G4PrivPtr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilDecompG3G4PrivRec), 0, (ilSrcElementData
	    *)NULL,
	    &dstdata, _ilDecompG3Init,_ilDecompG3Cleanup ,IL_NPF, _ilDecompG3Execute, NULL, 0);

	if (!pPriv) return FALSE; /* EXIT */

	/* save private data */
	pPriv->width         = pinfo->width;
	pPriv->white         =  ( des.blackIsZero ? 1 : 0 );
	pPriv->compFlags     = pimdes->compInfo.g4.flags;
	pPriv->nDstLineBytes = (pPriv->width + 7) / 8;
	return TRUE;
}
