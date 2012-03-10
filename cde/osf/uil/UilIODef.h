/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
/*   $XConsortium: UilIODef.h /main/10 1995/07/14 09:34:21 drk $ */

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This include file defines the interface to the operating system
**	io facilities.  
**
**--
**/

#ifndef UilIODef_h
#define UilIODef_h


/*
**  Define a UIL File Control Block or FCB.
*/


#include <stdio.h>
#ifndef NULL
#define NULL 0L
#endif

typedef struct  
{
    FILE	*az_file_ptr;
    char	*c_buffer;
    boolean	v_position_before_get;
    z_key	last_key;
    char	expanded_name[ 256 ];
} uil_fcb_type;

#endif /* UilIODef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
