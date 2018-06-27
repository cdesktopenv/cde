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
/* $XConsortium: iljpgencodejif.c /main/3 1995/10/23 15:56:46 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1992 Hewlett-Packard Co.
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

#include "iljpgencodeint.h"


    /*  -------------------- iljpgPut2Bytes -------------------------- */
    /*  Put 2 bytes of "value" (which is really a short) to the given stream
        in big endian order, as defined for 16 bit values for JIF.
    */
static iljpgError iljpgPut2Bytes (
    int                 value,
    ILJPG_ENCODE_STREAM stream
    )
{
    ilByte              abyte;
    iljpgError          error;

    abyte = (value >> 8) & 0xff;
    if (!ILJPG_ENCODE_PUT_BYTE (stream, abyte, error))
        return error;
    abyte = value & 0xff;
    if (!ILJPG_ENCODE_PUT_BYTE (stream, abyte, error))
        return error;
    return 0;
}

    /*  -------------------- iljpgEncodePutBytes -------------------------- */
    /*  Put "nBytes" bytes pointed to by "pBytes" to the given stream.
    */
static iljpgError iljpgEncodePutBytes (
    iljpgPtr            pBytes,
    int                 nBytes,
    ILJPG_ENCODE_STREAM stream
    )
{
    ilByte              abyte;
    iljpgError          error;

    while (nBytes-- > 0) {
        abyte = *pBytes++;
        if (!ILJPG_ENCODE_PUT_BYTE (stream, abyte, error))
            return error;
        }
    return 0;
}

    /*  -------------------- iljpgWriteHuffmanTable -------------------------- */
    /*  Write the Huffman table pointed to by "pTable" to the given stream.
        "Th" is the table identifier: 0..3 for DC table, 16..19 for AC table.
    */
static iljpgError iljpgWriteHuffmanTable (
    iljpgPtr            pTable,
    int                 Th,
    ILJPG_ENCODE_STREAM stream
    )
{
    int        i, nBytes;
    iljpgError          error;

        /*  Huffman tables are: 16 bytes of # occurrences each # bits, followed by
            bytes for each of those # occurrences.  Size of table = 16 + sum(0..15).
        */
    for (i = 0, nBytes = 16; i < 16; i++)
        nBytes += pTable[i];

    if (!ILJPG_ENCODE_PUT_BYTE (stream, ILJPGM_FIRST_BYTE, error))
        return error;
    if (!ILJPG_ENCODE_PUT_BYTE (stream, ILJPGM_DHT, error))
        return error;

        /*  Write length = 2 (length) + 1 (Th) + nBytes (16 bytes + variable length) */
    if (error = iljpgPut2Bytes (3 + nBytes, stream))
        return error;
    if (!ILJPG_ENCODE_PUT_BYTE (stream, Th, error))
        return error;
    if (error = iljpgEncodePutBytes (pTable, nBytes, stream))
        return error;

    return 0;
}



    /*  -------------------- iljpgEncodeJIF -------------------------- */
    /*  Write a JIF (JPEG Interchange Format) header to the given "stream"
        given a data block (same as returned by iljpgDecodeJIF()) pointed
        to by "pData", defining the tables, etc.  The stream is left 
        positioned after the Scan header (SOS); the compressed data should 
        be written right after that.
            If "pOffsets" is non-null the offsets within the stream where
        various items were output are returned.
    */
ILJPG_PUBLIC iljpgError iljpgEncodeJIF (
    ILJPG_ENCODE_STREAM stream,
    iljpgDataPtr        pData,
    iljpgJIFOffsetsPtr  pOffsets
    )
{
    int        index, i;
    iljpgPtr   pTable;
    iljpgError error;
    iljpgCompDataPtr pComp;
    long                startOffset;

#   define PUT_BYTE(_byte)  {                              \
        if (!ILJPG_ENCODE_PUT_BYTE (stream, _byte, error)) \
            goto JIFError;                                 \
        }
#   define PUT_2BYTES(_value)  {                           \
        if (error = iljpgPut2Bytes (_value, stream))       \
            goto JIFError;                                 \
        }

        /*  Write a single frame, single scan JIF image.  The format will be:
                SOI
                <quant tables> = one or more DQTs
                <frame> = SOF0 ...
                <Huffman tables> = one or more DHTs
                <scan> = SOS ...
                <data> (not written - stream left positioned here)
            Note that the order of the tables could be different but this is how JFIF
            files have the tables and other readers may require it.
                When writing length, remember that the length includes
            the size of the length itself, = 2 (bytes).
        */

        /*  If offsets to be returned save starting offset and init values to 0 */
    if (pOffsets) {
        startOffset = ILJPG_ENCODE_OFFSET(stream);
        for (i = 0; i < 4; i++) {
            pOffsets->QTables[i] = 0;
            pOffsets->DCTables[i] = 0;
            pOffsets->ACTables[i] = 0;
            }
        }

        /*  Write SOI marker */
    PUT_BYTE (ILJPGM_FIRST_BYTE)
    PUT_BYTE (ILJPGM_SOI)

        /*  Write a DRI (restart interval) marker if restartInterval non-zero */
    if (pData->restartInterval) {
        PUT_BYTE (ILJPGM_FIRST_BYTE)
        PUT_BYTE (ILJPGM_DRI)
        PUT_2BYTES (4)                          /* length = 4 bytes */
        PUT_2BYTES (pData->restartInterval)     /* restart interval */
        }

        /*  Write DQT and quantization tables for all non-null tables.
            Note that Q tables must already be in zigzag order.
        */
    for (index = 0; index < 4; index++) {
        if (pTable = pData->QTables[index]) {
            PUT_BYTE (ILJPGM_FIRST_BYTE)
            PUT_BYTE (ILJPGM_DQT)
            PUT_2BYTES (67)                     /* length = 67 bytes */
            PUT_BYTE (index)                    /* Pq = 0; Tq (id) = index */
            if (pOffsets)
                pOffsets->QTables[index] = ILJPG_ENCODE_OFFSET(stream) - startOffset;
            if (error = iljpgEncodePutBytes (pTable, 64, stream))
                goto JIFError;
            }
        }

        /*  Write frame (SOF0) */
    PUT_BYTE (ILJPGM_FIRST_BYTE)
    PUT_BYTE (ILJPGM_SOF0)
    PUT_2BYTES (8 + 3 * pData->nComps)          /* length = 8 + 3 * # components */
    PUT_BYTE (8)                                /* P = precision = 8 for baseline */
    PUT_2BYTES (pData->height)                  /* Y = # of lines = image height */
    PUT_2BYTES (pData->width)                   /* X = # of samples = image width */
    PUT_BYTE (pData->nComps)                    /* Ns = # of components */

        /*  Write per-component frame data: id is arbitrary unique id (0..255).
            However, a bug in the IL's imageutil (ilujfif.c) required that the 
            component ids be 1..3 for samples 0..2.  So use use "index" + 1; also below.
            Write hori/vert sample factor, Q table selector (QTableIndex).
        */
    for (index = 0, pComp = pData->comp; index < pData->nComps; index++, pComp++) {
        PUT_BYTE (index + 1)                    /* id = component index; see above */
        i = (pComp->horiFactor << 4) | pComp->vertFactor;
        PUT_BYTE (i)                            /* Hi/Vi = hori/vert sample factors */
        PUT_BYTE (pComp->QTableIndex)           /* Tq = Q table selector */
        }

        /*  Write DHT and Huffman tables for all non-null tables.
            The DC tables have ids = index (0..3 - really, 0..1 for baseline)
            while the AC tables have ids = index + 16 (16..17).
            Add "5" to offsets; 5 bytes (marker, length, Th) written before code lengths.
        */
    for (index = 0; index < 4; index++) {
        if (pTable = pData->DCTables[index]) {
            if (pOffsets)
                pOffsets->DCTables[index] = ILJPG_ENCODE_OFFSET(stream) - startOffset + 5;
            if (error = iljpgWriteHuffmanTable (pTable, index, stream))
                goto JIFError;
            }
        }

    for (index = 0; index < 4; index++) {
        if (pTable = pData->ACTables[index]) {
            if (pOffsets)
                pOffsets->ACTables[index] = ILJPG_ENCODE_OFFSET(stream) - startOffset + 5;
            if (error = iljpgWriteHuffmanTable (pTable, 16 + index, stream))
                goto JIFError;
            }
        }

        /*  Write scan (SOS); the actual compressed data follows this */
    PUT_BYTE (ILJPGM_FIRST_BYTE)
    PUT_BYTE (ILJPGM_SOS)
    PUT_2BYTES (6 + 2 * pData->nComps)          /* length = 6 + 2 * # components */
    PUT_BYTE (pData->nComps)                    /* Ns = # of components */

        /*  Write per-component frame data: scan component selector, = id in
            frame, = index+1; Td/Ta = DC/AC table selector = DC/AC table index (0..1).
        */
    for (index = 0, pComp = pData->comp; index < pData->nComps; index++, pComp++) {
        PUT_BYTE (index + 1)                    /* id; see notes for frame header */
        i = (pComp->DCTableIndex << 4) | pComp->ACTableIndex;
        PUT_BYTE (i)                            /* Hi/Vi = hori/vert sample factors */
        }

        /*  Write Ss, Se and Ah/Al to finish off scan header */
    PUT_BYTE (0)                                /* Ss = 0 for baseline */
    PUT_BYTE (63)                               /* Se = 63 for baseline */
    PUT_BYTE (0)                                /* Ah = 0, 1st scan; Al = 0, baseline */

    /*  Branch point if other error; return "error" */
JIFError:
    return error;

}



