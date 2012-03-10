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
