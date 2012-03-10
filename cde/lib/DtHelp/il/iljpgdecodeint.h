/* $XConsortium: iljpgdecodeint.h /main/3 1995/10/23 15:54:54 rswiston $ */
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

#ifndef ILJPGDECODEINT_H
#define ILJPGDECODEINT_H

#ifndef ILJPGDECODE_H
#include "iljpgdecode.h"
#endif

#ifndef ILJPGINT_H
#include "iljpgint.h"
#endif

    /*  Codes returned from _iljpgHuffmanDecode() indicating which coefficients
        of the 8x8 block are non-zero.
    */
#define HUFF_DC_ONLY        0           /* only DCT non-zero */
#define	HUFF_FOURX4         1           /* only top-left 4x4 matrix non-zero */
#define HUFF_FULL           2           /* "whole" 8x8 non-zero */


    /*  Private data for decoding JPEG (not JIF), e.g. 
        _iljpgDecodeInit/Execute/Cleanup().
    */

typedef struct {
    float          *pRevScale;              /* built from DCT Q table */
    int             horiFactor, vertFactor; /* copied from iljpgDataRec */
    long            width, height;          /* size of output buffer */
    int             mcuXInc, mcuYInc;       /* value to inc (x,y) by, each MCU */
    long            x, y;                   /* where to store next MCU */
    int             lastDC;                 /* DC from previous 8x8 block */
    } iljpgCompRec, *iljpgCompPtr;

typedef struct {
    iljpgDataPtr        pData;              /* data passed to _iljpgDecodeInit() */
    int                 mcuRestartCount;    /* # of mcu's since last restart */
    iljpgPtr            pHuffPriv;          /* owned by iljpgdehuff.c */
    float              *DCTRevScaleTables[4];       /* owned by iljpgdedct.c */
    iljpgCompRec        compData[ILJPG_MAX_COMPS];  /* owned by iljpgdecode.c */
    } iljpgDecodePrivRec, *iljpgDecodePrivPtr;


    /*  Functions in iljpgdehuff.c */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffInit (
    iljpgDecodePrivPtr  pPriv
    );

    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffCleanup (
    iljpgDecodePrivPtr  pPriv
    );

    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffReset (
    iljpgDecodePrivPtr  pPriv
    );

    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffExecute (
    iljpgDecodePrivPtr  pPriv,
    ILJPG_DECODE_STREAM stream,
    int                 comp,
    int                *pOut,
    unsigned int       *pBlockType
    );

    /*  Functions in iljpgdedct.c */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDeDCTInit (
    iljpgDecodePrivPtr  pPriv
    );

    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDeDCTCleanup (
    iljpgDecodePrivPtr  pPriv
    );

    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCTFull (
    int                *pSrc,
    long                nBytesPerRow,
    iljpgPtr            ix,                 /* RETURNED */
    float              *pRevScale
    );

    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCT4x4 (
    int                *pSrc,
    long                nBytesPerRow,
    iljpgPtr            ix,                 /* RETURNED */
    float              *pRevScale
    );

    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCTDCOnly (
    int                *pSrc,
    long                nBytesPerRow,
    iljpgPtr            pDst,               /* RETURNED */
    float              *pRevScale
    );

#endif
