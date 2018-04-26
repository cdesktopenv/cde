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
/* $XConsortium: iljpgdecode.h /main/3 1995/10/23 15:54:28 rswiston $ */
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



#ifndef ILJPGDECODE_H
#define ILJPGDECODE_H

#ifndef ILJPG_H
#include "iljpg.h"
#endif
#ifndef ILJPGDECODEDEFS_H
#include "iljpgdecodedefs.h"
#endif

    ILJPG_PUBLIC_EXTERN 
iljpgError iljpgDecodeJIF (
    ILJPG_DECODE_STREAM stream,
    iljpgDataPtr       *ppData                  /* RETURNED */
    );

    ILJPG_PUBLIC_EXTERN 
iljpgError iljpgFreeData (
    iljpgDataPtr        pData
    );

    ILJPG_PUBLIC_EXTERN 
iljpgError iljpgDecodeInit (
    iljpgDataPtr        pData,
    iljpgPtr           *pPrivate                /* RETURNED */
    );

    ILJPG_PUBLIC_EXTERN 
iljpgError iljpgDecodeCleanup (
    iljpgPtr           pPrivate
    );

    ILJPG_PUBLIC_EXTERN 
iljpgError iljpgDecodeExecute (
    iljpgPtr            pPrivate,
    ILJPG_DECODE_STREAM stream,
    int                 doReset,
    long                nDstLines,
    iljpgPtr            pPixels[],
    long                nBytesPerRow[]
    );

#endif
