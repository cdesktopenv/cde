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
/* $XConsortium: ildecomplzw.c /main/5 1996/06/19 12:23:35 ageorge $ */
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

#include <stdlib.h>

#include "ilint.h"
#include "ilpipelem.h"
#include "ildecomp.h"
#include "ilerrors.h"



/* ========================================================================
   Local definitions
   ======================================================================== */

#define     CLEAR_CODE          256
#define     END_OF_INFORMATION  257
#define     MAX_NODES           0x1000
#define     OKAY                1



/* ========================================================================
   Decompression private data type declaration
   ======================================================================== */

typedef struct {                            /* decode node structure       */
    unsigned short      value;              /* byte value of node          */
    short               previous;           /* previous string node index  */
    } ilDenodeRec, *ilDenodePtr;

typedef struct {                            /* private image information   */
    ilDenodePtr         denodes;            /* table/tree for decode nodes */
    ilPtr               pString;            /* ptr to string table */
    unsigned long       length;             /* number of bytes read        */
    unsigned int        nextNode;           /* next node to be used        */
    unsigned long       nDstLineBytes;      /* width of dst in bytes       */
    } ilDecompPrivRec, *ilDecompPrivPtr;


/* ========================================================================    
             
     -------------------- ilDecompLZWInit/Cleanup -------------------
        Allocate / deallocate buffers for LZW decompression.
   ======================================================================== */

static ilError ilDecompLZWInit(
    ilDecompPrivPtr  pPriv,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )
{
    pPriv->denodes = (ilDenodePtr)NULL;
    pPriv->pString = (ilPtr)IL_MALLOC (MAX_NODES);
    if (!pPriv->pString) 
        return IL_ERROR_MALLOC;

    pPriv->denodes = (ilDenodePtr)IL_MALLOC (sizeof(ilDenodeRec) * MAX_NODES);
    if (!pPriv->denodes) {
        IL_FREE (pPriv->pString);
        pPriv->pString = (ilPtr)NULL;
        return IL_ERROR_MALLOC;
	    }

    return IL_OK;
}


static ilError ilDecompLZWCleanup (
    ilDecompPrivPtr  pPriv,
    ilBool           aborting
    )
{
    if (pPriv->pString) 
        IL_FREE (pPriv->pString);
    if (pPriv->denodes) 
        IL_FREE ((ilPtr)pPriv->denodes);
    return IL_OK;
}


/* ========================================================================    
             
        -------------------- ilCurrentBitsRead( ) -------------------
   Returns the number of bits per code word for the next node in the tree.

   ======================================================================== */

static int ilCurrentBitsRead (
    register ilDecompPrivPtr pPriv
    )
{
    int   retval;
    int   node;

    node = pPriv->nextNode + 2;
    if (node <= (512 - 2)) retval = 9;
 
    else {
        if (node <= (1024 - 2)) retval = 10;
        else {
            if (node <= (2048 - 2)) retval = 11;
            else retval = 12;
        }
    }
    return retval;
}
/* End ilCurrentBitsRead() */



/* ========================================================================    

           -------------------- ilStringFromCode --------------------
   Using the code as the starting node in the decode tree this routine
   builds the string by traversing backwards up the tree.

   ======================================================================== */

static void ilStringFromCode(
    ilDecompPrivPtr     pPriv,
    unsigned int        code,       /* Code to look for string with.                    */
    ilPtr               pString,    /* Pointer to value for the length of the string    */
    unsigned int       *pcount      /* Pointer to location to place string at in memory */
    )

{
    unsigned int  index;
    int           node;
    register ilDenodePtr   denodes = pPriv->denodes;

    if (code > 255) node = code - 2;    /* remove for extra codes */
    else node = code;
    index = 0;                          /* start at end of string */

    while (node != -1) {                /* build string backwards */
        pString[index++] = denodes[node].value;
        node = denodes[node].previous;
        }
    *pcount = index;
}




/* ========================================================================    

          -------------------- ilAddStringToTable --------------------
   Adds a new node to the decode table and links it back to the last code
   for the current string.
   ======================================================================== */

static void ilAddStringToTable (
    ilDecompPrivPtr pPriv,
    unsigned int    code,       /* Code for previous entry in table */ 
    unsigned char   khar        /* Value for new node               */               
    )

{
    ilDenodePtr   denodes = pPriv->denodes;

    denodes[pPriv->nextNode].value = khar;
    if (code < 256) denodes[pPriv->nextNode].previous = code;
    else denodes[pPriv->nextNode].previous = code - 2;
    pPriv->nextNode++;
}



/* ========================================================================    

       --------------------ilInitializeDecodeTable --------------------
      Allocates the decode table.  Initializes the table with the first 
      256 entries.

   ======================================================================== */

static ilError ilInitializeDecodeTable (
    register ilDecompPrivPtr pPriv
    )
{
    unsigned int    i;
	
    for (i = 0;i < 256;i++) {
        pPriv->denodes[i].value = i;
        pPriv->denodes[i].previous = -1;
        }
    pPriv->nextNode = 256;
	
    for (i = 256;i < MAX_NODES;i++) {
        pPriv->denodes[i].value = 0;
        pPriv->denodes[i].previous = -1;
        }
    return IL_OK;
}


/* ========================================================================    

        -------------------- ilDecompLZWExecute() -------------------
    Routine defined in ilDecompLZW for executing LZW 
    decompression when the pipe gets executed.

   ======================================================================== */

static ilError ilDecompLZWExecute(
    ilExecuteData           *pData,
    unsigned long            dstLine,
    unsigned long           *pNLines
    )
{

/* ========================================================================
   ilDecompLZWExecute() definitions
   ======================================================================== */

    /*  Macro which returns whether a clear code exists at *_pByte: a clear code
        is 9 bits, = 256, or 0x80 in first byte and a zero in the upper bit of the next.
        Each strip must begin with a clear code; at end of strip, a search-ahead
        is done for the next clear code.
    */
#define IS_CLEAR_CODE(_pByte) ( (*(_pByte) == 0x80) && ((*(_pByte+1) & 0x80) == 0) )

    /* My defines */
    unsigned long       dstNBytes;              /* Number of destination image bytes per row                     */                    
    long                nLines;                 /* Number of lines per source image strip                        */  
    long                nBytesToGo;             /* Number of source image bytes left to unpack for current strip */  
    long                nDstLineBytesToGo;      /* Number of bytes left to write to this line of destination     */
    register ilPtr      pSrcByte;               /* Pointer to source image data                                  */  
    register ilPtr      pDstByte;               /* Pointer to destination image data first byte                  */  
    ilPtr               pSrcStart;              /* Pointer to initial src (comp) byte                            */
    ilPtr               pDstLine;               /* Pointer to destination image data first byte of each scanline */  
    register ilDecompPrivPtr pPriv;             /* Pointer to private image data                                 */  
    ilImagePlaneInfo   *pPlane;

    /* Greeley defines */
    register unsigned int bitCount;
    unsigned int        numBits;
/* compatibility problem with long and unsigned long data fields */
    CARD32              bits;
    long                k;
    register unsigned int code;
    unsigned int        oldCode;
    unsigned int        stringCount = 0;
    ilError             status      = IL_OK;
    ilPtr               pString;
	

/* ========================================================================
   ilDecompLZWExecute() set up for decompression algorithm code           
   ======================================================================== */

    pPriv = (ilDecompPrivPtr)pData->pPrivate;
    nLines = *pNLines;
    if (nLines <= 0)    
        return IL_OK; 

    nBytesToGo = pData->compressed.nBytesToRead;   
    if (nBytesToGo <= 0)    
        return IL_OK; 

    pPlane = &pData->pSrcImage->plane[0];
    if (!pPlane->pPixels) return IL_ERROR_NULL_COMPRESSED_IMAGE;
    pSrcStart = pPlane->pPixels + pData->compressed.srcOffset; 
    pSrcByte = pSrcStart;

    pPlane = &pData->pDstImage->plane[0];
    dstNBytes = pPlane->nBytesPerRow;         
    pDstLine  = pPlane->pPixels + dstLine * dstNBytes;   


/* ========================================================================
   ilDecompLZWExecute() decompression algorithm code
   ======================================================================== */

    /* get ptr to 4k string buffer */
    pString = pPriv->pString;

    /* decode each line of compressed data into the destination image */
    pPriv->length   = 0;
    pPriv->nextNode = 256;    /* set to keep ref in ReadCode() call to CurrentBits2() from bombing */
    bits            = 0;
    bitCount        = 0;

        /*  Each strip must begin with a clear code; error if not. */
    if (!IS_CLEAR_CODE (pSrcByte))
        return IL_ERROR_COMPRESSED_DATA;

    while (nLines-- > 0)  {
        pDstByte          = pDstLine;
        pDstLine         += dstNBytes;
        nDstLineBytesToGo = pPriv->nDstLineBytes;

        while (nDstLineBytesToGo > 0) {

            if (stringCount == 0) {                         /* get another string */

/*  BEGIN ReadCode */
                if (bitCount < 16) {                        /* Get next byte(s) of encoded source image data */
                    if (--nBytesToGo < 0)
                        return IL_ERROR_COMPRESSED_DATA;
                    if (--nBytesToGo < 0) {
                        bits |= (*pSrcByte++ << (24 - bitCount));
                        bitCount += 8;
                        }
                    else {
                        bits |= (*pSrcByte++ << (24 - bitCount));
                        bits |= (*pSrcByte++ << (16 - bitCount));
                        bitCount += 16;
                        }
                    }
                numBits   = ilCurrentBitsRead (pPriv);
                code      = (unsigned short)(bits >> (32 - numBits));
                bits    <<= numBits;
                bitCount -= numBits;
/*  END ReadCode */

                if (code == END_OF_INFORMATION) 
                    goto xit;                               /* done, EXIT */
				
                if (code == CLEAR_CODE) {
                    if ((status = ilInitializeDecodeTable (pPriv)) != IL_OK) goto xit;

/*  BEGIN ReadCode */
                if (bitCount < 16) {                        /* Get next byte(s) of encoded source image data */
                    if (--nBytesToGo < 0)
                        return IL_ERROR_COMPRESSED_DATA;
                    if (--nBytesToGo < 0) {
                        bits |= (*pSrcByte++ << (24 - bitCount));
                        bitCount += 8;
                        }
                    else {
                        bits |= (*pSrcByte++ << (24 - bitCount));
                        bits |= (*pSrcByte++ << (16 - bitCount));
                        bitCount += 16;
                        }
                    }
                numBits   = ilCurrentBitsRead (pPriv);
                code      = (bits >> (32 - numBits));
                bits    <<= numBits;
                bitCount -= numBits;
/*  END ReadCode */

                    if (code == END_OF_INFORMATION) goto xit;
                    ilStringFromCode (pPriv, code, pString, &stringCount);
                    oldCode = code;
                }
                else {

                    if ((code < 256) || ((code - 2) < pPriv->nextNode)) {     /* code is in table */
                        ilStringFromCode (pPriv, code, pString, &stringCount);
                        ilAddStringToTable (pPriv, oldCode, pString[stringCount - 1]);
                        oldCode = code;
                    }

                    else {
                        ilStringFromCode (pPriv, oldCode, pString, &stringCount);
     
                        for (k = stringCount - 1; k >= 0; k--) pString[k + 1] = pString[k];
                        pString[0] = pString[stringCount];
                        stringCount++;
                        ilAddStringToTable (pPriv, oldCode, pString[0]);
                        oldCode = code;
                    }
                }
            }

                /*  Store one byte, subtract line byte count */
            *pDstByte++ = pString[--stringCount];
            nDstLineBytesToGo--;
        }
    }

xit:	
    return status;
}
  

/* ========================================================================

        -------------------- ilDecompLZW() --------------------
    Code entry point for LZW decompression.  This includes image descriptor
    parameter error checking and function calls for: strip handler
    initialization, adding the filter element to the pipe, pipe
    initialization, execution and destruction, decompression algorithm.....

   ======================================================================== */

IL_PRIVATE ilBool _ilDecompLZW (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat
    )

{
    ilDstElementData    dstdata;
    ilDecompPrivPtr     pPriv;
    ilImageDes          des;
    ilImageFormat       format;
    long                bytesPerRow [IL_MAX_SAMPLES];

    dstdata.producerObject  =  (ilObject) NULL;
    des                     = *pimdes;
    des.compression         =  IL_UNCOMPRESSED;
    dstdata.pDes            =  &des;
    format                  = *pimformat;
    format.rowBitAlign      = (format.nBitsPerSample[0] == 1) ? 32 : 8;
    dstdata.pFormat         =  &format;
    dstdata.width           =  pinfo->width;
    dstdata.height          =  pinfo->height;
    dstdata.pPalette        =  pinfo->pPalette;
    dstdata.stripHeight     =  pinfo->stripHeight;
    dstdata.constantStrip   =  pinfo->constantStrip;

    pPriv = (ilDecompPrivPtr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilDecompPrivRec), 0,
                    (ilSrcElementData *)NULL, &dstdata, 
                    ilDecompLZWInit, ilDecompLZWCleanup, IL_NPF, ilDecompLZWExecute, NULL, 0);
    if (!pPriv) return FALSE;

        /*  Store info in *pPriv.  For nDstLineBytes, need minimum # of bytes uncompressed
            dst data would take - get bytes/row using rowBitAlign of 8 (packed).
        */
    format.rowBitAlign = 8;
    ilGetBytesPerRow (pimdes, &format, pinfo->width, bytesPerRow);
    pPriv->nDstLineBytes = bytesPerRow[0];

    return TRUE;
}

