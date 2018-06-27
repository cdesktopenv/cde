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
/* $XConsortium: iljpgenhuff.c /main/3 1995/10/23 15:57:13 rswiston $ */
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

    /*  Indexed by "i" = 0..255, table of # of bits required to store "i". */
static iljpgByte iljpgBitsNeeded[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

    /*  Internal representation of a Huffman table.  "size" and "code" are
        each indexed by the value to be encoded (0..255).  "size" is the
        # of used (low-order) bits in "code", and "code" contains the actual
        Huffman code.
    */
typedef struct {
    int                 size[257];          /* called "EHUFSI" in JPEG spec */
    int                 code[257];          /* called "EHUFCO" in JPEG spec */
    } iljpgEnhuffTableRec, *iljpgEnhuffTablePtr;

    /*  Data private to this file.  Pointed to by iljpgEncodePrivRec.pHuffPriv.
        Each entry in DC/ACTables[] corresponds to the same table entry in 
        iljpgDataRec (public view), but has been converted for faster access.
        The entry in compDC/ACTables[] are the table for that component.
    */
typedef struct {
    iljpgEnhuffTablePtr DCTables[4];
    iljpgEnhuffTablePtr ACTables[4];
    iljpgEnhuffTablePtr compDCTables[ILJPG_MAX_COMPS];
    iljpgEnhuffTablePtr compACTables[ILJPG_MAX_COMPS];
    int                 huffBits;           /* unflushed Huffman encoded bits */
    int                 nHuffBits;          /* # of bits in huffBits */
    } iljpgEnhuffRec, *iljpgEnhuffPtr;


    /*  -------------------- _iljpgBuildEnhuffTable -------------------------- */
    /*  Build and return a local optimized version of the given standard JPEG
        Huffman DC/AC table.
    */
    ILJPG_PRIVATE 
iljpgError _iljpgBuildEnhuffTable (
    iljpgPtr       pTableIn,
    iljpgEnhuffTablePtr    *ppTableOut          /* RETURNED */
    )
{
    iljpgEnhuffTablePtr pTable;
    int            i, j, nValues, value;
    int                     huffSize[257];      /* called "HUFFSIZE" in JPEG spec */
    int                     huffCode[257];      /* called "HUFFCODE" in JPEG spec */

        /*  Malloc space for internal table, return ptr to it */
    pTable = (iljpgEnhuffTablePtr)ILJPG_MALLOC_ZERO (sizeof (iljpgEnhuffTableRec));
    if (!pTable)
        return ILJPG_ERROR_ENCODE_MALLOC;
    *ppTableOut = pTable;

        /*  Fill in internal table, based on method described in C.2 (pg C-2)
            of the JPEG spec.  pTableIn points to the standard JPEG table:
            16 bytes of the # of codes of each size, followed by "n" bytes of
            the value to be associated with the codes, where "n" = sum of the
            first 16 bytes.
                First generate local size table as in C.2 of spec, leaving
            "pTableIn" pointing past first 16 bytes, to "HUFFVAL" in JPEG spec.
        */
    for (i = 1, nValues = 0; i <= 16; i++) {
        value = *pTableIn++;
        if ((value + nValues) > 256)            /* sum of first 16 bytes too large */
            return ILJPG_ERROR_ENCODE_DCAC_TABLE;
        while (value-- > 0)
            huffSize[nValues++] = i;
        }
    huffSize[nValues] = 0;                      /* "nValues" now "LASTK" in spec */

        /*  Generate HUFFCODE as in fig C.2 (pg C-3) of spec */
    i = j = 0;                                  /* i = "K"; j = "CODE" in spec */
    value = huffSize[0];                        /* "SI" in spec */
    while (TRUE) {
        do {
            huffCode[i] = j;
            i++;
            j++;
            } while (huffSize[i] == value);
        if (huffSize[i] == 0)                   /* terminating 0 seen; done */
            break;
        do {
            j <<= 1;
            value++;
            } while (huffSize[i] != value);
        }

        /*  Reorder values created above into final tables; fig C.3 (C-4) in spec */
    for (j = 0; j < nValues; j++) {             /* j = "K" in spec */
        i = pTableIn[j];
        pTable->code[i] = huffCode[j];
        pTable->size[i] = huffSize[j];
        }

    return 0;
}

    /*  -------------------- _iljpgEnhuffInit -------------------------- */
    /*  Called by iljpgEncodeInit() to init for Huffman encoding.
    */
    ILJPG_PRIVATE 
iljpgError _iljpgEnhuffInit (
    iljpgEncodePrivPtr      pPriv
    )
{
    iljpgEnhuffPtr pHuffPriv;
    iljpgDataPtr   pData;
    iljpgError              error;
    int            i;

        /*  Allocate Huffman private area and point to it in encode private */
    pData = pPriv->pData;
    pHuffPriv = (iljpgEnhuffPtr)ILJPG_MALLOC_ZERO (sizeof (iljpgEnhuffRec));
    if (!pHuffPriv)
        return ILJPG_ERROR_ENCODE_MALLOC;
    pPriv->pHuffPriv = (iljpgPtr)pHuffPriv;

        /*  For each of caller's DC/AC tables, build a local one for more
            efficient access and point to it in private.
        */
    for (i = 0; i < 4; i++) {
        if (pData->DCTables[i]) {
            if (error = _iljpgBuildEnhuffTable (pData->DCTables[i], 
                                                &pHuffPriv->DCTables[i]))
                return error;
            }
        if (pData->ACTables[i]) {
            if (error = _iljpgBuildEnhuffTable (pData->ACTables[i], 
                                                &pHuffPriv->ACTables[i]))
                return error;
            }
        }

        /*  For each component, point to DC/AC tables for that component */
    for (i = 0; i < pData->nComps; i++) {
        pHuffPriv->compDCTables[i] = pHuffPriv->DCTables[pData->comp[i].DCTableIndex];
        pHuffPriv->compACTables[i] = pHuffPriv->ACTables[pData->comp[i].ACTableIndex];
        }

    return 0;
}

                       
    /*  -------------------- _iljpgEnhuffCleanup -------------------------- */
    /*  Called by iljpgEncodeCleanup() to cleanup after Huffman encoding.
    */
    ILJPG_PRIVATE 
iljpgError _iljpgEnhuffCleanup (
    iljpgEncodePrivPtr  pPriv
    )
{
    iljpgEnhuffPtr      pHuffPriv;
    int                 i;

        /*  Free the Huffman encode private data including lookup tables */
    pHuffPriv = (iljpgEnhuffPtr)pPriv->pHuffPriv;
    if (pHuffPriv) {
        for (i = 0; i < 4; i++) {
            if (pHuffPriv->DCTables[i])
                ILJPG_FREE (pHuffPriv->DCTables[i]);
            if (pHuffPriv->ACTables[i])
                ILJPG_FREE (pHuffPriv->ACTables[i]);
            }
        ILJPG_FREE (pHuffPriv);
        }

    return 0;
}


    /*  -------------------- _iljpgPackHuffman -------------------------- */
    /*  Pack the list of (size, value) pairs (pointed to by "pHuff", with
        "nHuff" pairs in it), with output to "stream".
    */
static iljpgError _iljpgPackHuffman (
    iljpgEnhuffPtr      pHuffPriv,
    int       *pHuff,
    int        nHuff,
    ILJPG_ENCODE_STREAM stream
    )
{
    int        size, value, huffBits, nHuffBits;
    iljpgError          error;
    static int          sizeMasks[9] =     /* masks for lower "i" bits, indexed by i */
                            {0x0, 0x1, 0x3, 0x7, 0x0f, 0x1f, 0x3f, 0x7f, 0x0ff};

        /*  For each (size, value) pair, pack the lower "size" bits from "value" and
            write to the output stream.  Bits are packed with MSB first packing.
            "nHuffBits" (0..7!) bits are left from before in "huffBits", in the
            low-order bits.
            After any 0xff, write a 0, as per JPEG spec F.1.2.3 (pg F-7).
        */
    huffBits = pHuffPriv->huffBits;
    nHuffBits = pHuffPriv->nHuffBits;

    while (nHuff-- > 0) {
        size = *pHuff++;
        value = *pHuff++;
        while ((nHuffBits + size) >= 8) {       /* while a byte or more worth */
            nHuffBits = 8 - nHuffBits;          /* now # bits that fit in this byte */
            huffBits <<= nHuffBits;             /* move existing bits up to make room */
            size -= nHuffBits;                  /* # bits from value -= # bits written */
            huffBits |= (value >> size) &       /* or in upper unwritten bits */
                           sizeMasks[nHuffBits];
            if (!ILJPG_ENCODE_PUT_BYTE (stream, huffBits, error))
                return error;                   /* write a byte */
            if (((iljpgByte)huffBits) == ((iljpgByte)0xff))
                if (!ILJPG_ENCODE_PUT_BYTE (stream, 0, error))
                    return error;               /* write a 0 after any 0xff */
            nHuffBits = 0;                      /* all Huff bits written out */
            }
        if (size > 0) {                         /* bits (if any) fit in less than byte */
            huffBits <<= size;                  /* make room for bits from value */
            huffBits |= value & sizeMasks[size]; /* add lower "size" bits from value */
            nHuffBits += size;                  /* "size" more bits now written */
            }
        }   /* END while (size, value) pairs */

        /*  Save unwritten bits and count of same back into private */
    pHuffPriv->huffBits = huffBits;
    pHuffPriv->nHuffBits = nHuffBits;
    return 0;
}


    /*  -------------------- _iljpgEnhuffExecute -------------------------- */
    /*  Huffman encode an 8x8 block (already zig-zagged) from "pSrc" (array of
        64 ints) out to "stream", for component index "comp".  The DC component
        (pSrc[0]) must have already had the lastDC value subtracted from it.
    */
    ILJPG_PRIVATE 
iljpgError _iljpgEnhuffExecute (
    iljpgEncodePrivPtr  pPriv,
    int                 comp,
    int                *pSrc,
    ILJPG_ENCODE_STREAM stream
    )
{
    iljpgEnhuffPtr  pHuffPriv;
    iljpgEnhuffTablePtr    pTable;
    int             huff[4 * 64 + 10];  /* room for 64 * 2 pairs plus some slop */
    int   *pHuff;
    int    size, value, nACLeft, nZeros;
#   define ENHUFF(_value) { \
        *pHuff++ = pTable->size[_value]; \
        *pHuff++ = pTable->code[_value]; \
        }

        /*  Build a list of (size, value) pairs to Huffman encode in "huff", two
            per DC or AC coefficient, possibly less for AC due to zero run-lengths.
            Use pHuff to index thru it.  When done, go back thru list and encode it.
            This is done so that encoding can be done without a function call per encode,
            and without having to replicate long code all over the place.
        */
    pHuffPriv = (iljpgEnhuffPtr)pPriv->pHuffPriv;
    pHuff = huff;

        /*  Encode the first (DC) coefficient, which must already be the difference
            from the previous DC.  See section F.1.2.1 (pg F-3) of the JPEG spec.
            Write Huff code for size of DC, then Huff for DC value, -1 if value < 0.
        */
    pTable = pHuffPriv->compDCTables[comp]; /* use DC table for this component */
    value = *pSrc++;
    if (value < 0) {
        size = -value;
        value -= 1;
        }
    else size = value;
    if (size < 256)
         size = iljpgBitsNeeded[size];
    else size = iljpgBitsNeeded[size>>8] + 8;
    ENHUFF (size)                           /* write huff code for size of DC */
    *pHuff++ = size;                        /* write "size" bits of DC value */
    *pHuff++ = value;

        /*  Encode 63 ACs.  See section F.1.2.2 (pg F-4) of JPEG spec.
            Each AC is represented by 8 bits: RRRRSSSS, where RRRR is the offset
            from the previous non-zero AC - i.e. the # of zero ACs before this one.
            0xf0 = 16 zero ACs; 0 = end of block (EOB), meaning rest of ACs are zero.
            SSSS is the "size" of the AC.  For each AC (excluding zeros), Huff encode
            the 8 bit representation, then write "size" bits as in DC encoding.
        */
    pTable = pHuffPriv->compACTables[comp]; /* use AC table for all other components */
    nACLeft = 63;
    nZeros = 0;
    while (nACLeft-- > 0) {
        value = *pSrc++;
        if (value == 0)
            nZeros++;                       /* AC == 0: count it and continue */
        else {
            while (nZeros >= 16) {          /* AC != 0: flush out runs of 16 or more */
                nZeros -= 16;
                ENHUFF (0xf0)
                }
            if (value < 0)                  /* get SSSS = size of "value" */
                size = -value;
            else size = value;
            if (size < 256)
                 size = iljpgBitsNeeded[size];
            else size = iljpgBitsNeeded[size>>8] + 8;
            nZeros = (nZeros << 4) + size;  /* now have RRRRSSSS in "nZeros" */
            ENHUFF (nZeros)                 /* write huff code for RRRRSSSS */
            nZeros = 0;                     /* reset count of zeros */
            *pHuff++ = size;                /* write "size" bits of AC value */
            if (value < 0) value--;         /* -1 if < 0 */
            *pHuff++ = value;
            }   /* END AC != 0 */
        }       /* END one AC */

        /*  If any zeros were at end of block, write special EOB value (0) */
    if (nZeros > 0) {
        ENHUFF (0)
        }

        /*  Now bit encode all (size, value) pairs in "huff" array */
    return _iljpgPackHuffman (pHuffPriv, huff, (pHuff - huff) / 2, stream);
}


    /*  -------------------- _iljpgEnhuffFlush -------------------------- */
    /*  Flush out any bits left over from Huffman encoding.
    */
    ILJPG_PRIVATE 
iljpgError _iljpgEnhuffFlush (
    iljpgEncodePrivPtr  pPriv,
    ILJPG_ENCODE_STREAM stream
    )
{
    iljpgEnhuffPtr pHuffPriv;
    int        nHuffBits, huffBits;
    iljpgError          error;

        /*  If any bits left, flush them out.  Pad with binary 1's, and
            stuff a 0 after any 0xff, as per JPEG spec F.1.2.3 (pg F-7).
        */
    pHuffPriv = (iljpgEnhuffPtr)pPriv->pHuffPriv;
    nHuffBits = pHuffPriv->nHuffBits;
    if (nHuffBits > 0) {
        huffBits = pHuffPriv->huffBits;
        huffBits <<= (8 - nHuffBits);           /* move bits to upper bits of byte */
        huffBits |= (1 << (8 - nHuffBits)) - 1; /* or in all 1's in unused bits */
        pHuffPriv->nHuffBits = 0;               /* now no bits left to output */
        if (!ILJPG_ENCODE_PUT_BYTE (stream, huffBits, error))
            return error;                       /* write a byte */
        if (((iljpgByte)huffBits) == ((iljpgByte)0xff))
            if (!ILJPG_ENCODE_PUT_BYTE (stream, 0, error))
                return error;                   /* write a 0 after any 0xff */
        }

    return 0;
}

