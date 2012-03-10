/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isalloc.c /main/3 1995/10/23 11:33:43 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isalloc.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isalloc.c
 *
 * Description:
 *	Functions that allocate and deallocate memory.
 *	All errors are treated as fatal.
 */

#include "isam_impl.h"
#include <malloc.h>

/* strdup is not defined in Ultrix' string.h, so it is implemented in a separate
 * file named strdup.ultrix.c
 */
#ifdef __ultrix__
extern char *strdup();
#endif

/*
 * _ismalloc(nbytes)
 *
 * Allocate nbytes.
 */

char *_ismalloc(nbytes)
    unsigned int	nbytes;
{
    register char	*p;
    
    if ((p = (char *) malloc (nbytes)) == NULL)
	_isfatal_error("malloc() failed");

    return (p);
}

char *_isrealloc(oldaddr, nbytes)
    char		*oldaddr;
    unsigned int	nbytes;
{
    register char	*p;
    
    if ((p = (char *) realloc (oldaddr, nbytes)) == NULL)
	_isfatal_error("realloc() failed");

    return (p);
}


/*
 * _isallocstring(str)
 *
 * Create a duplicate of string in dynamic memory.
 */

char *
_isallocstring(str)
    char	*str;
{
    register char	*p;

    if ((p = strdup(str)) == NULL) 
	_isfatal_error("strdup() failed");

    return (p);
}

/*
 * _isfreestring(str)
 *
 * Free dynamically allocated string.
 */

void
_isfreestring(str)
    char	*str;
{
    assert(str != NULL);
    free(str);
}
