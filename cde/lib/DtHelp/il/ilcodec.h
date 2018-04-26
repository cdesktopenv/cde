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
/* $XConsortium: ilcodec.h /main/3 1995/10/23 15:42:05 rswiston $ */
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

#ifndef ILCODEC_H
#define ILCODEC_H

#ifndef ILINT_H
#include "ilint.h"
#endif

#ifndef ILPIPELEM_H
#include "ilpipelem.h"
#endif



    /*  Realloc (or alloc the first time) the pixel buffer for plane "plane" of the 
        compressed image "*pImage", so that its "bufferSize" is a minimum of 
        "minNewSize" bytes in size.
       
        Returns: TRUE if successful (re)alloc;
                 else false (malloc error -return IL_ERROR_MALLOC).
    */
IL_EXTERN ilBool _ilReallocCompressedBuffer (
    ilImageInfo        *pImage,
    unsigned int        plane,
    unsigned long       minNewSize
    );

        

    /*  Add a filter to "pipe" (guaranteed to be a pipe in IL_PIPE_FORMING state)
        which decompresses the compressed pipe image.
    */
IL_EXTERN void _ilDecompress (
    ilPipe              pipe
    );


#endif
