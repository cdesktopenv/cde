/*
 * File:         MemoryMgr.c $XConsortium: MemoryMgr.c /main/3 1995/10/26 15:34:54 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifdef DOMAIN_ALLOW_MALLOC_OVERRIDE
#include "/usr/include/apollo/shlib.h"
#endif

#include <bms/sbport.h> /* NOTE: sbport.h must be the first include. */

#include <bms/bms.h>
#include <bms/XeUserMsg.h>
#include <bms/MemoryMgr.h>

#ifdef __cplusplus
#define CPLUSPLUS_MALLOC_TYPE (malloc_t)
#else
#define CPLUSPLUS_MALLOC_TYPE
#endif

/*-----------------------------------------------------------------------+*/
void * XeMalloc( size_t size )
/*-----------------------------------------------------------------------+*/
{
    XeString  ptr;
    
    if (!size) return 0;
    if (ptr = malloc(size)) {
	*ptr = (XeChar)NULL;   /* Force first byte to NULL for bozos who */
	                       /* think malloc zeros memory!             */
	return ptr;
    }
    
    _DtSimpleError(XeProgName, XeError,  XeString_NULL,
		   "><DtEncap: cannot malloc memory");
    exit(1);
    return (void*)NULL;
}

/*-----------------------------------------------------------------------+*/
void XeFree( void * ptr )
/*-----------------------------------------------------------------------+*/
{
    if (ptr) 
	free((char *)ptr); 
}
