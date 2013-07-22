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
/* $XConsortium: iljpgdecodejif.c /main/3 1995/10/23 15:55:08 rswiston $ */
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

#include <stdlib.h>

#include "iljpgdecodeint.h"


    /*  -------------------- iljpgFreeData -------------------------- */
    /*  Free the given data block and associated tables, for example as 
        created by iljpgDecodeJIF().
    */
    ILJPG_PUBLIC_EXTERN
iljpgError iljpgFreeData (
    iljpgDataPtr        pData
    )
{
    int                 i;

    if (!pData)                                 /* null data ptr; exit */
        return 0;
    for (i = 0; i < 4; i++) {
        if (pData->QTables[i])
            ILJPG_FREE (pData->QTables[i]);
        if (pData->DCTables[i])
            ILJPG_FREE (pData->DCTables[i]);
        if (pData->ACTables[i])
            ILJPG_FREE (pData->ACTables[i]);
        }
    ILJPG_FREE (pData);
    return 0;
}

    /*  -------------------- iljpgGet2Bytes -------------------------- */
    /*  Get 2 bytes from the given stream and return (short) value or error.
    */
static iljpgError iljpgGet2Bytes (
    ILJPG_DECODE_STREAM stream,
    int                *pValue                  /* RETURNED */
    )
{
    int                 byte0, byte1;
    iljpgError          error;

    if (!ILJPG_DECODE_GET_BYTE (stream, byte0, error))
        return error;
    if (!ILJPG_DECODE_GET_BYTE (stream, byte1, error))
        return error;
    *pValue = (byte0 << 8) + byte1;
    return 0;
}

    /*  -------------------- iljpgSkipBytes -------------------------- */
    /*  Skip given # of bytes in JPEG stream
    */
static iljpgError iljpgSkipBytes (
    ILJPG_DECODE_STREAM stream,
    int                 nBytes
    )
{
    int                 byte;
    iljpgError          error;

    while (nBytes-- > 0) {
        if (!ILJPG_DECODE_GET_BYTE (stream, byte, error))
            return error;
        }
    return 0;
}


    /*  -------------------- iljpgDecodeJIF -------------------------- */
    /*  Parse the given stream which must point to the beginning of a
        JIF (JPEG Interchange Format) stream.  Return a ptr to a created
        iljpgData block, which can be freed by iljpgFreeData(), or null
        if an error occurs (non-zero error code returned).
    */
    ILJPG_PUBLIC_EXTERN
iljpgError iljpgDecodeJIF (
    ILJPG_DECODE_STREAM stream,
    iljpgDataPtr       *ppData                  /* RETURNED */
    )
{
    register iljpgDataPtr pData;
    iljpgBool           firstMarker, SOSFound, SOF0Found;
    iljpgError          error;
    int                 value, nFrameComps, length;
    iljpgPtr           *ppTable;
    iljpgCompDataPtr    pComp;
    register int        i, index;
    register iljpgPtr   pTable;
    struct {
        int             id, horiFactor, vertFactor, QTableIndex;
        }               frameComp[ILJPG_MAX_COMPS], *pFrame;


    pData = (iljpgDataPtr)ILJPG_MALLOC_ZERO (sizeof (iljpgDataRec));
    if (!pData)
        return ILJPG_ERROR_DECODE_MALLOC;

    firstMarker = TRUE;
    SOSFound = FALSE;
    SOF0Found = FALSE;

        /*  Parse input stream until SOS marker found.
            Note that the "length" value following most markers includes the length
            of length itself, and thus must be made -2 to get length of rest of data.
        */
    while (!SOSFound) {
        do {                        /* get a marker */
            do {
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                    goto JIFError;
                } while (value != ILJPGM_FIRST_BYTE);
            do {
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                    goto JIFError;
                } while (value == ILJPGM_FIRST_BYTE);
            } while (!value);          /* skip "0" after ff */
            
            /*  First marker must be SOI or error; else parse marker */
        if (firstMarker) {
            if (value != ILJPGM_SOI)
                goto BadJIFData;
            firstMarker = FALSE;
            }
        else switch (value) {
          case ILJPGM_TEM:
            break;              /* legal (?) "stand-alone" markers - no data to skip */
          case ILJPGM_SOI:
          case ILJPGM_RST0: case ILJPGM_RST1: case ILJPGM_RST2: case ILJPGM_RST3:  
          case ILJPGM_RST4: case ILJPGM_RST5: case ILJPGM_RST6: case ILJPGM_RST7:
          case ILJPGM_EOI:
            goto BadJIFData;
            break;              /* illegal markers - should not see these before SOS */

            /*  DRI: get value and save into restartInterval */
          case ILJPGM_DRI:
            if ((error = iljpgGet2Bytes (stream, &value)))        /* length */
                goto JIFError;
            if ((error = iljpgGet2Bytes (stream, &value)))        /* restart interval */
                goto JIFError;
            pData->restartInterval = value;
            break;

            /*  Start Of Frame 0 (SOF0): save Q table indices; note marker seen. */
          case ILJPGM_SOF0:
            SOF0Found = TRUE;
            if ((error = iljpgGet2Bytes (stream, &value)))        /* length */
                goto JIFError;
            if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                goto JIFError;
            if (value != 8)                     /* precision != 8; not supported */
                goto BadJIFData;
            if ((error = iljpgGet2Bytes (stream, &pData->height)))
                goto JIFError;
            if ((error = iljpgGet2Bytes (stream, &pData->width)))
                goto JIFError;
            if (!ILJPG_DECODE_GET_BYTE (stream, nFrameComps, error))
                goto JIFError;
            if (nFrameComps > ILJPG_MAX_COMPS)
                goto BadJIFData;

                /*  Get and store info for each frame component (Ci, Hi, Vi, Tqi) */
            for (i = 0, pFrame = frameComp; i < nFrameComps; i++, pFrame++) {
                if (!ILJPG_DECODE_GET_BYTE (stream, pFrame->id, error))
                    goto JIFError;
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                    goto JIFError;
                pFrame->horiFactor = value >> 4;                /* Hi in upper nibble */
                pFrame->vertFactor = value & 0xf;               /* Vi in lower nibble */
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                    goto JIFError;
                if (value > 3)                                  /* Tqi must be 0..3 */
                    goto BadJIFData;
                pFrame->QTableIndex = value;
                }
            break;
          
            /*  Define Quantization Table (DQT): malloc memory for table and set into 
                pData->QTables for this index, freeing table if already there.
                Multiple tables ("n") can be defined: n = (length-2)/65;
            */
          case ILJPGM_DQT:
            if ((error = iljpgGet2Bytes (stream, &length)))       /* length */
                goto JIFError;
            length -= 2;                                        /* includes itself */
            while (length > 0) {
                if ((length -= 65) < 0)                         /* at least 65 bytes */
                    goto JIFError;                              
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Pq (0), Tq */
                    goto JIFError;
                value &= 0xf;                                   /* table index (0..3) */
                if (value > (ILJPG_MAX_COMPS-1))
                    goto BadJIFData;

                    /*  Read bytes into malloc'd space, free old if non-null and store */
                if (!(pTable = (iljpgPtr)ILJPG_MALLOC (64))) {
                    error = ILJPG_ERROR_DECODE_MALLOC;
                    goto JIFError;
                    }
                if (pData->QTables[value])
                    ILJPG_FREE (pData->QTables[value]);
                pData->QTables[value] = pTable;
                for (i = 0; i < 64; i++) {
                    if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                        goto JIFError;
                    *pTable++ = value;
                    }
                }
            break;
          
            /*  Define Huffman Table (DHT): see comment for DQT. 
                Multiple tables can be defined in this marker; the "length" value must
                be used; = 2 * n * (17 + m), where "n" is the number of tables, and "m"
                is the sum of 16 length bytes (bytes 1..16, after Th).
                The table index (Th) is 0,1 for DC tables 0,1 or 16, 17 for AC tables 0,1.
                4 AC or DC tables are allowed,used by "extended" DCT mode.
            */
          case ILJPGM_DHT:
            if ((error = iljpgGet2Bytes (stream, &length)))
                goto JIFError;
            length -= 2;                            /* length includes itself */
            while (length > 0) {
                iljpgByte       L[16];
                int             sumL;

                if ((length -= 17) < 0)             /* sub Th, 16 bytes of Li */
                    goto JIFError;
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Th */
                    goto JIFError;
                i = value & 0xf;
                if (i > (ILJPG_MAX_COMPS-1))
                    goto BadJIFData;
                if (value >= 16)
                     ppTable = &pData->ACTables[i];
                else ppTable = &pData->DCTables[i];

                    /*  Read 16 bytes of Li; set sumL to their sum */
                for (i = 0, sumL = 0; i < 16; i++) {
                    if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Th */
                        goto JIFError;
                    sumL += value;
                    L[i] = value;
                    }

                    /*  Malloc space for table; copy 1st 16 bytes; read in rest */
                if ((length -= sumL) < 0)           /* sub # bytes of huff codes */
                    goto JIFError;
                if (!(pTable = (iljpgPtr)ILJPG_MALLOC (16 + sumL))) {
                    error = ILJPG_ERROR_DECODE_MALLOC;
                    goto JIFError;
                    }
                if (*ppTable)
                    ILJPG_FREE (*ppTable);
                *ppTable = pTable;
                for (i = 0; i < 16; i++)
                    *pTable++ = L[i];
                for (i = 0; i < sumL; i++) {
                    if (!ILJPG_DECODE_GET_BYTE (stream, value, error))
                        goto JIFError;
                    *pTable++ = value;
                    }
                }   /* END while length > 0 */
            break;

            /*  Start Of Scan (SOS): read the scan header and leave stream positioned
                right after header, i.e. pointing to the image data.
            */
          case ILJPGM_SOS:
            SOSFound = TRUE;                        /* terminate while loop */
            if (!SOF0Found)                         /* SOS before SOF0 is an error */
                goto BadJIFData;
            if ((error = iljpgGet2Bytes (stream, &value)))        /* length */
                goto JIFError;

            if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Ns (0..3) */
                goto JIFError;
            if (value > (ILJPG_MAX_COMPS-1))
                goto BadJIFData;
            pData->nComps = value;

                /*  For each scan component: get "id" and find component with same id
                    in frameComponents or error.  Copy component data from frame to 
                    pData->component, and get DC/AC table indices from stream.
                    Set pData->maxHori/VertFactor to largest component factor found.
                */
            for (i = 0, pComp = pData->comp; i < pData->nComps; i++, pComp++) {
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Csj (id) */
                    goto JIFError;
                pFrame = &frameComp[nFrameComps - 1];
                while (TRUE) {
                    if (pFrame->id == value)
                        break;
                    if (pFrame == frameComp)
                        goto BadJIFData;            /* frames searched; id not found */
                    pFrame--;                       /* seach back one */
                    }

                    /*  pFrame points to frame with given id; copy data to *pComp */
                pComp->horiFactor = pFrame->horiFactor;
                if (pComp->horiFactor > pData->maxHoriFactor)
                    pData->maxHoriFactor = pComp->horiFactor;
                pComp->vertFactor = pFrame->vertFactor;
                if (pComp->vertFactor > pData->maxVertFactor)
                    pData->maxVertFactor = pComp->vertFactor;

                    /*  Get DC/AC table index; error if that or Q table not defd yet. */
                if (!ILJPG_DECODE_GET_BYTE (stream, value, error))  /* Tdj/Taj */
                    goto JIFError;
                index = (value >> 4) & 3;               /* DC table index, 0..3 */
                if (!pData->DCTables[index])
                    goto BadJIFData;
                pComp->DCTableIndex = index;
                index = value & 3;                      /* AC table index, 0..3 */
                if (!pData->ACTables[index])
                    goto BadJIFData;
                pComp->ACTableIndex = index;
                pComp->QTableIndex = pFrame->QTableIndex;
                if (!pData->QTables[pComp->QTableIndex])
                    goto BadJIFData;
                }   /* END each scan header component */

            if ((error = iljpgSkipBytes (stream, 3)))     /* skip Ss, Se, Ah/Al */
                goto JIFError;
            break;

                /* All other markers have lengths: get length and skip length-2 bytes */
          default:
            if ((error = iljpgGet2Bytes (stream, &value)))
                goto JIFError;
            if ((error = iljpgSkipBytes (stream, value - 2))) /* "length" already skipped */
                goto JIFError;
            break;
            }   /* END switch marker */
        }       /* END while (!SOSFound) */

    *ppData = pData;                                /* return filled in data block */
    return 0;

    /*  Branch point if bad JPEG data */
BadJIFData:
    iljpgFreeData (pData);
    return ILJPG_ERROR_DECODE_JIF;

    /*  Branch point if other error; return "error" */
JIFError:
    iljpgFreeData (pData);
    return error;
}



