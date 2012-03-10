/* $XConsortium: ildecomp.h /main/3 1995/10/23 15:44:41 rswiston $ */
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

#ifndef ILDECOMP_H
#define ILDECOMP_H

#ifndef ILPIPELEM_H
#include "ilpipelem.h"
#endif

#ifndef ILINT_H
#include "ilint.h"
#endif


IL_EXTERN ilBool _ilDecompG3 (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes
    );

IL_EXTERN ilBool _ilDecompG4 (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes
    );

IL_EXTERN ilBool _ilDecompLZW (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,                              
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat
    );

IL_EXTERN ilBool _ilDecompPackbits (
    ilPipe              pipe,
    ilPipeInfo         *pinfo,
    ilImageDes         *pimdes,
    ilImageFormat      *pimformat
    );

#endif
