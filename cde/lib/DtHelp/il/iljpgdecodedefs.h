/* $XConsortium: iljpgdecodedefs.h /main/3 1995/10/23 15:54:41 rswiston $ */
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



#ifndef ILJPGDECODEDEFS_H
#define ILJPGDECODEDEFS_H

#include "ilint.h"
#include "ilerrors.h"

    /*  Equate ILJPG errors to IL errors */
#define ILJPG_ERROR_DECODE_MALLOC       IL_ERROR_MALLOC
#define ILJPG_ERROR_DECODE_JIF          IL_ERROR_COMPRESSED_DATA
#define ILJPG_ERROR_DECODE_DATA         IL_ERROR_COMPRESSED_DATA
#define ILJPG_ERROR_DECODE_INTERNAL     IL_ERROR_COMPRESSED_DATA
#define ILJPG_ERROR_DECODE_EOD          IL_ERROR_COMPRESSED_DATA
#define ILJPG_ERROR_DECODE_DCAC_TABLE   IL_ERROR_COMPRESSED_DATA
#define ILJPG_ERROR_DECODE_PARS         IL_ERROR_COMPRESSED_DATA

    /*  Define a JPEG stream: ptr to data in memory and # bytes left */
typedef struct {
    ilPtr           pData;
    long            nBytesLeft;
    } ilJPEGDecodeStream, *ilJPEGDecodeStreamPtr;

#define ILJPG_DECODE_STREAM ilJPEGDecodeStreamPtr

    /*  Macro to get next byte from a stream.  Return ILJPG_ERROR_DECODE_EOD 
        error if no more data.
    */
#define ILJPG_DECODE_GET_BYTE(_stream, _byte, _error) ( \
    (--_stream->nBytesLeft < 0) ? \
        (_error = ILJPG_ERROR_DECODE_EOD, 0) : \
        (_byte = *_stream->pData++, 1) )

#endif
