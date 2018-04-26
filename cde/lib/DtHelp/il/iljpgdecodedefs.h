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
