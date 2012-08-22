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
/* $XConsortium: ilutiljpeg.c /main/3 1995/10/23 16:02:31 rswiston $ */
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
#include "ilerrors.h"
#include "iljpgdecode.h"
#include "iljpgencode.h"
#include "ilutiljpeg.h"


    /*  -------------------- _ilJPEGDataIn -------------------------- */
    /*  Copy general data (but *not* table info) from IL data into 
        IL JPG package format.
    */
IL_PRIVATE void _ilJPEGDataIn (
    register const ilImageDes    *pDes,
    long                          width,
    long                          height,
    register iljpgDataPtr         pDst
    )
{
    int                     maxSubsample;

    pDst->width = width;
    pDst->height = height;
    pDst->nComps = pDes->nSamplesPerPixel;

        /*  Set IL subsample info into iljpg Hori/VertFactor info, which is essentially
            the inverse of IL subsample data: for each sample/comp, hori/vert:
                iljpgFactor[comp] = maxILSubsample/ILSubsample[comp]
            NOTE: currently only YCbCr data in IL is subsampled!
        */
    if (pDes->type == IL_YCBCR) {
        register const ilYCbCrSampleInfo *pSample = pDes->typeInfo.YCbCr.sample;

        maxSubsample = pSample[0].subsampleHoriz;
        if (pSample[1].subsampleHoriz > maxSubsample)
            maxSubsample = pSample[1].subsampleHoriz;
        if (pSample[2].subsampleHoriz > maxSubsample)
            maxSubsample = pSample[2].subsampleHoriz;
        pDst->maxHoriFactor = maxSubsample;
        pDst->comp[0].horiFactor = maxSubsample / pSample[0].subsampleHoriz;
        pDst->comp[1].horiFactor = maxSubsample / pSample[1].subsampleHoriz;
        pDst->comp[2].horiFactor = maxSubsample / pSample[2].subsampleHoriz;

        maxSubsample = pSample[0].subsampleVert;
        if (pSample[1].subsampleVert > maxSubsample)
            maxSubsample = pSample[1].subsampleVert;
        if (pSample[2].subsampleVert > maxSubsample)
            maxSubsample = pSample[2].subsampleVert;
        pDst->maxVertFactor = maxSubsample;
        pDst->comp[0].vertFactor = maxSubsample / pSample[0].subsampleVert;
        pDst->comp[1].vertFactor = maxSubsample / pSample[1].subsampleVert;
        pDst->comp[2].vertFactor = maxSubsample / pSample[2].subsampleVert;
        }
    else {
        pDst->maxHoriFactor = 1;
        pDst->maxVertFactor = 1;
        pDst->comp[0].horiFactor = 1;
        pDst->comp[0].vertFactor = 1;
        pDst->comp[1].horiFactor = 1;
        pDst->comp[1].vertFactor = 1;
        pDst->comp[2].horiFactor = 1;
        pDst->comp[2].vertFactor = 1;
        pDst->comp[3].horiFactor = 1;
        pDst->comp[3].vertFactor = 1;
        }
}

    /*  -------------------- _ilJPEGTablesIn -------------------------- */
    /*  Copy table info from IL format into IL JPG package format.
        Note: the restartInterval is also copied.
    */
IL_PRIVATE void _ilJPEGTablesIn (
    register ilJPEGData    *pSrc,
    register iljpgDataPtr   pDst
    )
{

        /*  Copy the restartInterval; just convenient to do so in this function */
    pDst->restartInterval = pSrc->restartInterval;

        /*  Tables are independent of samples; copy all 4 */
    pDst->QTables[0] = pSrc->QTables[0];
    pDst->QTables[1] = pSrc->QTables[1];
    pDst->QTables[2] = pSrc->QTables[2];
    pDst->QTables[3] = pSrc->QTables[3];

    pDst->DCTables[0] = pSrc->DCTables[0];
    pDst->DCTables[1] = pSrc->DCTables[1];
    pDst->DCTables[2] = pSrc->DCTables[2];
    pDst->DCTables[3] = pSrc->DCTables[3];

    pDst->ACTables[0] = pSrc->ACTables[0];
    pDst->ACTables[1] = pSrc->ACTables[1];
    pDst->ACTables[2] = pSrc->ACTables[2];
    pDst->ACTables[3] = pSrc->ACTables[3];

        /*  Copy 4 samples / components of indices into above tables */
    pDst->comp[0].QTableIndex = pSrc->sample[0].QTableIndex;
    pDst->comp[0].DCTableIndex = pSrc->sample[0].DCTableIndex;
    pDst->comp[0].ACTableIndex = pSrc->sample[0].ACTableIndex;

    pDst->comp[1].QTableIndex = pSrc->sample[1].QTableIndex;
    pDst->comp[1].DCTableIndex = pSrc->sample[1].DCTableIndex;
    pDst->comp[1].ACTableIndex = pSrc->sample[1].ACTableIndex;

    pDst->comp[2].QTableIndex = pSrc->sample[2].QTableIndex;
    pDst->comp[2].DCTableIndex = pSrc->sample[2].DCTableIndex;
    pDst->comp[2].ACTableIndex = pSrc->sample[2].ACTableIndex;

    pDst->comp[3].QTableIndex = pSrc->sample[3].QTableIndex;
    pDst->comp[3].DCTableIndex = pSrc->sample[3].DCTableIndex;
    pDst->comp[3].ACTableIndex = pSrc->sample[3].ACTableIndex;
}


    /*  -------------------- _ilJPEGFreeTables -------------------------- */
    /*  Free the tables in the given IL JPEG data block, which is not freed.
    */
IL_PRIVATE void _ilJPEGFreeTables (
    register ilJPEGData    *pData
    )
{
    register int            i;

    for (i = 0; i < 4; i++) {
        if (pData->QTables[i]) {
            IL_FREE (pData->QTables[i]);
            pData->QTables[i] = (ilPtr)NULL;
            }
        if (pData->DCTables[i]) {
            IL_FREE (pData->DCTables[i]);
            pData->DCTables[i] = (ilPtr)NULL;
            }
        if (pData->ACTables[i]) {
            IL_FREE (pData->ACTables[i]);
            pData->ACTables[i] = (ilPtr)NULL;
            }
        }
}


    /*  -------------------- _ilJPEGCopyHuffmanTable -------------------------- */
    /*  Copy the given JPEG Huffman table into a malloc'd space and return a ptr
        to it or null if malloc failed.
    */
static ilPtr _ilJPEGCopyHuffmanTable (
    register ilPtr          pSrc
    )
{
    register int            i, nBytes;
    register ilPtr          pDst;

        /*  Huffman tables are: 16 bytes of # occurrences each # bits, followed by
            bytes for each of those # occurrences.  Size of table = 16 + sum(0..15).
        */
    for (i = 0, nBytes = 16; i < 16; i++)
        nBytes += pSrc[i];
    if (!(pDst = (ilPtr)IL_MALLOC (nBytes)))
        return (ilPtr)NULL;
    bcopy ((char *)pSrc, (char *)pDst, nBytes);
    return pDst;
}


    /*  -------------------- _ilJPEGCopyData -------------------------- */
    /*  Copy data from pSrc to pDst, mallocing and copying contents of tables.
        If an error, free all malloc'd tables in dest, null them and return false.
    */
IL_PRIVATE ilBool _ilJPEGCopyData (
    register ilJPEGData    *pSrc,
    register ilJPEGData    *pDst
    )
{
    register int            i;

        /*  Zero table ptrs in *pDst in case of error; copy index info */
    bzero ((char *)pDst, sizeof (ilJPEGData));
    for (i = 0; i < IL_MAX_SAMPLES; i++)
        pDst->sample[i] = pSrc->sample[i];

        /*  For each non-null table in *pSrc, malloc space, copy contents, set pDst */
    for (i = 0; i < 4; i++) {
        if (pSrc->QTables[i]) {
            if (!(pDst->QTables[i] = (ilPtr)IL_MALLOC (64)))
                goto cdMallocError;
            bcopy ((char *)pSrc->QTables[i], (char *)pDst->QTables[i], 64);
            }
        if (pSrc->DCTables[i])
            if (!(pDst->DCTables[i] = _ilJPEGCopyHuffmanTable (pSrc->DCTables[i])))
                goto cdMallocError;
        if (pSrc->ACTables[i])
            if (!(pDst->ACTables[i] = _ilJPEGCopyHuffmanTable (pSrc->ACTables[i])))
                goto cdMallocError;
        }

        /*  Copy restart interval */
    pDst->restartInterval = pSrc->restartInterval;

    return TRUE;

        /*  Goto point on malloc error: free all tables in pDst, return false */
cdMallocError:
    _ilJPEGFreeTables (pDst);
    return FALSE;
}


    /*  -------------------- _ilReallocJPEGEncode -------------------------- */
    /*  Called by ILJPG_ENCODE_PUT_BYTE() macro to reallocate the buffer 
        defined by "pStream" so that there is space for *exactly* "nBytes" bytes
        to be written.  Zero (0) is returned if success else an error code.
    */
IL_PRIVATE iljpgError _ilReallocJPEGEncode (
    register ilJPEGEncodeStreamPtr pStream,
    long                    nBytes
    )
{
    register long           offset;

        /*  Allocate space so exactly "nBytes" more bytes will fit in output buffer.
            There are "offset" bytes in the buffer now so allocate offset + nBytes.
        */
    if (pStream->pBuffer) {
        offset = pStream->pDst - pStream->pBuffer;
        pStream->pBuffer = (ilPtr)IL_REALLOC (pStream->pBuffer, offset + nBytes);
        }
    else {
        offset = 0;                         /* no buffer yet; allocate one */
        pStream->pBuffer = (ilPtr)IL_MALLOC (offset + nBytes);
        }
    if (!pStream->pBuffer) {
        pStream->pDst = pStream->pPastEndBuffer = (ilPtr)NULL;
        return ILJPG_ERROR_ENCODE_MALLOC;
        }

        /*  Successful realloc: set end of buffer to pBuffer + new size; set pDst
            to pBuffer + offset and return success.
        */
    pStream->pPastEndBuffer = pStream->pBuffer + offset + nBytes;
    pStream->pDst = pStream->pBuffer + offset;

    return 0;
}


