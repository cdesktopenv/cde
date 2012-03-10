/* $XConsortium: ilcompress.h /main/3 1995/10/23 15:42:59 rswiston $ */
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

#ifndef ILCOMPRESS_H
#define ILCOMPRESS_H

#ifndef ILPIPELEM_H
#include "ilpipelem.h"
#endif

#ifndef ILINT_H
#include "ilint.h"
#endif

IL_EXTERN ilBool _ilCompressG3 (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData,
    ilPtr               pCompData
    );

IL_EXTERN ilBool _ilCompressG4 (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData,
    ilPtr               pCompData 
    );

IL_EXTERN ilBool _ilCompressLZW (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData
    );

IL_EXTERN ilBool _ilCompressPackbits (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData
    );

IL_EXTERN ilBool _ilCompressJPEG (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat,
    ilSrcElementData   *pSrcData,
    ilPtr              pCompData
    );

#endif
