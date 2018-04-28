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
/* $XConsortium: ilutiljpeg.h /main/3 1995/10/23 16:02:42 rswiston $ */
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

#ifndef ILUTILJPEG_H
#define ILUTILJPEG_H

        /*  IL internal defs and functions for JPEG data.
            Exposes functions in ilutiljpeg.c .
        */

#ifndef IL_H
#include "il.h"
#endif
#ifndef ILJPGDECODE_H
#include "iljpgdecode.h"
#endif

    /*  Copy general data (but *not* table info) from IL data into 
        IL JPG package format.
    */
IL_EXTERN void _ilJPEGDataIn (
    const ilImageDes       *pDes,
    long                    width,
    long                    height,
    iljpgDataPtr            pDst
    );

    /*  Copy table info from IL format into IL JPG package format.
        Note: the restartInterval is also copied.
    */
IL_EXTERN void _ilJPEGTablesIn (
    ilJPEGData             *pSrc,
    iljpgDataPtr            pDst
    );

    /*  Free the tables in the given IL JPEG data block, which is not freed.
    */
IL_EXTERN void _ilJPEGFreeTables (
    ilJPEGData             *pData
    );

    /*  Copy data from pSrc to pDst, mallocing and copying contents of tables.
        If an error, free all malloc'd tables in dest, null them and return false.
    */
IL_EXTERN ilBool _ilJPEGCopyData (
    ilJPEGData             *pSrc,
    ilJPEGData             *pDst
    );

#endif
