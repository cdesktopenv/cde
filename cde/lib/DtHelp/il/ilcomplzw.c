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
/* $XConsortium: ilcomplzw.c /main/3 1995/10/23 15:42:35 rswiston $ */
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
#include "ilpipelem.h"
#include "ilcompress.h"
#include "ilerrors.h"

#define CLEAR_CODE          256         /* LZW codes */
#define END_OF_INFORMATION  257

#define MAX_NODES           0x1000      /* 4k */
#define LZW_MAX_BUFFER_WRITE    8           /* max # of dst bytes written after buffer check */

typedef struct {
    unsigned short  value;          /* byte value of node */
    short           left;           /* nodes less than this nodes value */
    short           right;          /* nodes greater than this nodes value */
    short           next;           /* next character node */
    } ilEnnodeRec, *ilEnnodePtr;

typedef struct {
    ilEnnodePtr     ennodes;        /* ptr to encode table */
    ilPtr           pString;        /* ptr to byte string table */
    unsigned int    nextNode;       /* next node to be used */
    unsigned int    bitCount;       /* # of bits waiting to be output */
/* compatibility problem with long or unsigned long data fields */
    CARD32          bits;           /* bits waiting to be output */
    long            nSrcLineBytes;  /* used width of src in bytes */
    ilPtr           pDst;           /* ptr to spot for next byte in output buffer */
    ilPtr           pDstBufferEnd;  /* ptr _past_ last available spot in buffer */
    ilImageInfo    *pDstImage;      /* ptr to dst image structure */
    } ilCompLZWPrivRec, *ilCompLZWPrivPtr;



        /*  ------------------------- ilCompLZWInit ------------------------------ */
        /*  Allocate the string and encode node table memory.
        */
static ilError ilCompLZWInit (
    ilCompLZWPrivPtr pPriv,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )
{
    pPriv->ennodes = (ilEnnodePtr)NULL;
    pPriv->pString = (ilPtr)IL_MALLOC (MAX_NODES);
    if (!pPriv->pString) 
        return IL_ERROR_MALLOC;

    pPriv->ennodes = (ilEnnodePtr)IL_MALLOC (sizeof(ilEnnodeRec) * MAX_NODES);
    if (!pPriv->ennodes) {
        IL_FREE (pPriv->pString);
        pPriv->pString = (ilPtr)NULL;
        return IL_ERROR_MALLOC;
	    }
    return IL_OK;
}


        /*  ------------------------- ilCompLZWCleanup ---------------------------- */
        /*  Deallocate those things allocated in Init().
        */
static ilError ilCompLZWCleanup (
    ilCompLZWPrivPtr pPriv,
    ilBool          aborting
    )
{
    if (pPriv->pString) 
        IL_FREE (pPriv->pString);
    if (pPriv->ennodes) 
        IL_FREE (pPriv->ennodes);
    return IL_OK;
}

        /*  ------------------------ ilLZWReallocBuffer ------------------------ */
        /*  Reallocate the output (compressed) buffer and reset pPriv->pDst(BufferEnd).
        */
static ilBool ilLZWReallocBuffer (
    ilCompLZWPrivPtr pPriv
    )
{
    unsigned long  offset;

    offset = pPriv->pDst - pPriv->pDstImage->plane[0].pPixels;
    if (!_ilReallocCompressedBuffer (pPriv->pDstImage, 0, offset + LZW_MAX_BUFFER_WRITE))
        return FALSE;
    pPriv->pDst = pPriv->pDstImage->plane[0].pPixels + offset;
    pPriv->pDstBufferEnd = pPriv->pDstImage->plane[0].pPixels +
        (pPriv->pDstImage->plane[0].bufferSize - LZW_MAX_BUFFER_WRITE);
    return TRUE;
}

        /*  ------------------- LZW Compression Utility Functions ------------------ */

        /*  ------------------------ ilNewEnnode --------------------------------- */
        /*  Initialize "node" in the given encoding table "ennode" to "value".
        */
static void NewEnnode (
    ilEnnodePtr         ennodes,
    unsigned int        node,
    unsigned int        value
    )
{
    ennodes[node].value = value;
    ennodes[node].left  = -1;
    ennodes[node].right = -1;
    ennodes[node].next  = -1;
}

        /*  ------------------------- ilInitEncodeTable -------------------------- */
        /*  Allocate the encode table the first time called.
            Initializes the encode table with the first 256 strings (i.e. 0-ff).
        */
static void ilInitEncodeTable (
    ilCompLZWPrivPtr pPriv
    )
{
    int        i;

    for (i = 0; i < 256; i++) {
        pPriv->ennodes[i].value = i;
        pPriv->ennodes[i].left  = -1;
        pPriv->ennodes[i].right = -1;
        pPriv->ennodes[i].next  = -1;
        }
    pPriv->nextNode = 256;
    
    for (i = 256; i < MAX_NODES; i++)  {
        pPriv->ennodes[i].value = 0;
        pPriv->ennodes[i].left  = -1;
        pPriv->ennodes[i].right = -1;
        pPriv->ennodes[i].next  = -1;
        }
}

        /*  ------------------------- ilStringInTable -------------------------- */
        /*  Operation:
                If this is the first character in the string the current node is set
                to the top level of the tree.  Otherwise we will transition to the next
                level in the tree and search for an occurrence of the character there.
                if one is found we return with TRUE.  If not the new node will be
                added to the tree and FALSE will be returned.
            Return:
                result      TRUE if string was already in table.
                            FALSE if string had to be added to table.
        */
static ilBool ilStringInTable (
    ilCompLZWPrivPtr        pPriv,
    ilPtr                   string,
    unsigned int            count,
    unsigned int  *pCurrentNode
    )
{
    unsigned int   byte; /* current character in string */
    ilEnnodePtr    ennodes;

    ennodes = pPriv->ennodes;
    byte = string[count-1];
    
    if (count == 1) { /* single character string, always first 256 entries in table */
        *pCurrentNode = byte;
        return TRUE;
        }
    else {
        if (ennodes[*pCurrentNode].next == -1) { /* if no next char entry */
            ennodes[*pCurrentNode].next = pPriv->nextNode;
            *pCurrentNode = pPriv->nextNode;
            NewEnnode (ennodes, *pCurrentNode, byte); /* pPriv->nextNode is always>255 */
            pPriv->nextNode++;
            return FALSE;
            }
        else {
            *pCurrentNode = ennodes[*pCurrentNode].next;
            while (TRUE) {
                if (byte == ennodes[*pCurrentNode].value)
                    return TRUE;
                else if (byte < ennodes[*pCurrentNode].value) {
                    if (ennodes[*pCurrentNode].left != -1)
                        *pCurrentNode = ennodes[*pCurrentNode].left;
                    else { /* create new left node */
                        ennodes[*pCurrentNode].left = pPriv->nextNode;
                        *pCurrentNode = pPriv->nextNode;
                        NewEnnode (ennodes, *pCurrentNode, byte);
                        pPriv->nextNode++;
                        return FALSE;
                        }
                    }
                else { /* byte must be greater than the current value */
                    if (ennodes[*pCurrentNode].right != -1)
                        *pCurrentNode = ennodes[*pCurrentNode].right;
                    else { /* create new right node */
                        ennodes[*pCurrentNode].right = pPriv->nextNode;
                        *pCurrentNode = pPriv->nextNode;
                        NewEnnode (ennodes, *pCurrentNode, byte);
                        pPriv->nextNode++;
                        return FALSE;
                        }
                    }
                }
            }
        }
}

        /*  ----------------------- ilWriteCode ---------------------------------- */
        /*  Adds "code" to the output stream, outputting bytes as necessary.
            If the bit stream is longer than 16 bits, two bytes will be written.
        */
static ilError ilWriteCode (
    ilCompLZWPrivPtr pPriv,
    unsigned int        code
    )
{
    CARD32          newBits, nBits;

    if (pPriv->nextNode <= (512-2)) nBits = 9;
    else if (pPriv->nextNode <= (1024-2)) nBits = 10;
    else if (pPriv->nextNode <= (2048-2)) nBits = 11;
    else nBits = 12;

    newBits = (CARD32)code << (16 - nBits);
    pPriv->bits |= newBits << (16 - pPriv->bitCount);
    pPriv->bitCount += nBits;
    
    if (pPriv->bitCount > 16) {

            /*  Output 2 bytes; check for room in buffer; realloc if not room.
            */
        if (pPriv->pDst >= pPriv->pDstBufferEnd)
            if (!ilLZWReallocBuffer (pPriv))
                return IL_ERROR_MALLOC;

        *pPriv->pDst++ = (pPriv->bits >> 24) & 0xff;
        *pPriv->pDst++ = (pPriv->bits >> 16) & 0xff;
        pPriv->bits <<= 16;
        pPriv->bitCount -= 16;
    }
    return IL_OK;
}

        /*  ------------------------- ilCompLZWExecute ---------------------------- */
        /*  Compress one strip of LZW compressed data.
        */
static ilError ilCompLZWExecute (
    ilExecuteData  *pData,
    unsigned long   dstLine,
    unsigned long  *pNLines
    )
{
    ilCompLZWPrivPtr pPriv;
    ilPtr                   pSrcLine, pSrc;
    ilByte                  srcByte;
    long                    nLines, srcRowBytes, nSrcBytes;
    ilImagePlaneInfo       *pPlane;
    ilError                 error;
    ilPtr                   string;
    unsigned int            currentNode, stringCount, codeIndex, lastNode;


    pPriv = (ilCompLZWPrivPtr)pData->pPrivate;
    nLines = *pNLines;
    if (nLines <= 0)    
        return IL_OK; 

    pPlane = &pData->pSrcImage->plane[0];
    srcRowBytes = pPlane->nBytesPerRow;         
    pSrcLine  = pPlane->pPixels + pData->srcLine * srcRowBytes;

        /*  Make sure dst compressed bufferSize is min # writes in size.
            Set pDst to beginning of dst buffer + dst offset (= 0 unless writing
            to an image, in which case = byte past where last strip was written).
            Set pDstBufferEnd to begin of buffer + bufferSize - max # bytes written,
            so that pDst <= pDstBufferEnd can check for room for writing max # bytes.
        */
    pPriv->pDstImage = pData->pDstImage;
    if (pPriv->pDstImage->plane[0].bufferSize < LZW_MAX_BUFFER_WRITE)
        if (!_ilReallocCompressedBuffer (pPriv->pDstImage, 0, LZW_MAX_BUFFER_WRITE))
            return IL_ERROR_MALLOC;
    pPriv->pDst = pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset;
    pPriv->pDstBufferEnd = pPriv->pDstImage->plane[0].pPixels + 
        (pPriv->pDstImage->plane[0].bufferSize - LZW_MAX_BUFFER_WRITE);

    string = pPriv->pString;
    ilInitEncodeTable (pPriv);
    pPriv->bits = 0;
    pPriv->bitCount = 0;
    if (error = ilWriteCode (pPriv, CLEAR_CODE))
        return error;

    stringCount = 0;
    
    while (nLines-- > 0) {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        nSrcBytes = pPriv->nSrcLineBytes;

        while (nSrcBytes-- > 0) {
            srcByte = *pSrc++;
            string[stringCount++] = srcByte;
            lastNode = currentNode;
            if (!ilStringInTable (pPriv, string, stringCount, &currentNode)) {
                codeIndex = (lastNode < 256) ? lastNode : lastNode + 2;
                if (error = ilWriteCode (pPriv, codeIndex))
                    return error;
                string[0] = srcByte;
                currentNode = srcByte;
                stringCount = 1;
                }
            if (pPriv->nextNode > (MAX_NODES-3)) {
                codeIndex = (currentNode < 256) ? currentNode : currentNode + 2;
                if ( (error = ilWriteCode (pPriv, codeIndex)) ||
                     (error = ilWriteCode (pPriv, CLEAR_CODE)) )
                    return error;
                ilInitEncodeTable (pPriv);
                stringCount = 0;
                currentNode = 0;
                lastNode = 0;
                }
            }
        }

        /* terminate output buffer */
    if (stringCount != 0) {
        codeIndex = (currentNode < 256) ? currentNode : currentNode + 2;
        if (error = ilWriteCode (pPriv, codeIndex))
            return error;
        }
    if (error = ilWriteCode (pPriv, END_OF_INFORMATION))
        return error;

        /*  Check for room for bytes; write out any remaining bits.
        */
    if (pPriv->pDst >= pPriv->pDstBufferEnd)
        if (!ilLZWReallocBuffer (pPriv))
            return IL_ERROR_MALLOC;

    if (pPriv->bitCount > 0)
        *pPriv->pDst++ = (pPriv->bits >> 24) & 0xff;
    if (pPriv->bitCount > 8)
        *pPriv->pDst++ = (pPriv->bits >> 16) & 0xff;

        /*  Pass the number of bytes written (ptr to next byte - start) to next filter.
        */
    *pData->compressed.pNBytesWritten = pPriv->pDst - 
        (pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset);
    return IL_OK;
}

        /*  ------------------------- ilCompressLZW ---------------------------------- */
        /*  Called by ilCompress() in /ilc/ilcodec.c .
            Compresses each strip coming down the pipe using LZW compression.
        */
IL_PRIVATE ilBool _ilCompressLZW (
    ilPipe              pipe,
    ilPipeInfo         *pInfo,
    ilImageDes         *pDes,
    ilImageFormat      *pFormat,
    ilSrcElementData   *pSrcData
    )
{
    long                bytesPerRow [IL_MAX_SAMPLES];
    ilImageDes          des;
    ilImageFormat       format;
    ilDstElementData    dstData;
    ilCompLZWPrivPtr    pPriv;

        /*  Only pixel order supported - if planar order, try to convert to pixel order.
        */
    des = *pDes;
    format = *pFormat;
    if ((pDes->nSamplesPerPixel > 1) && (pFormat->sampleOrder != IL_SAMPLE_PIXELS)) {
        format.sampleOrder = IL_SAMPLE_PIXELS;
        if (!ilConvert (pipe, (ilImageDes *)NULL, &format, 0, (ilPtr)NULL))
            return FALSE;
        ilGetPipeInfo (pipe, FALSE, pInfo, &des, &format);
        }

        /*  Add pipe element: no change to des except now compressed; same format.
            NOTE: pSrcData->constantStrip should be TRUE, from ilCompress().
        */
    dstData.producerObject = (ilObject)NULL;
    des.compression = IL_LZW;
    dstData.pDes = &des;
    dstData.pFormat = (ilImageFormat *)NULL;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = pSrcData->stripHeight;
    dstData.constantStrip = pSrcData->constantStrip;
    dstData.pPalette = pInfo->pPalette;
    dstData.pCompData = (ilPtr)NULL;
    pPriv = (ilCompLZWPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilCompLZWPrivRec),
                0, pSrcData, &dstData,
                ilCompLZWInit, ilCompLZWCleanup, IL_NPF, ilCompLZWExecute, 0);
    if (!pPriv) 
        return FALSE;

        /*  Store info in *pPriv.  For nSrcLineBytes, need minimum # of bytes uncompressed
            dst data would take - get bytes/row using rowBitAlign of 8 (packed).
        */
    format.rowBitAlign = 8;
    ilGetBytesPerRow (pDes, &format, pInfo->width, bytesPerRow);
    pPriv->nSrcLineBytes = bytesPerRow[0];
    return TRUE;
}

