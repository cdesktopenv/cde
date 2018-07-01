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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isalloc.c /main/3 1995/10/23 11:33:43 rswiston $ 			 				 */
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
#include <stdlib.h>
#include <string.h>

/*
 * _ismalloc(nbytes)
 *
 * Allocate nbytes.
 */

char *_ismalloc(unsigned int nbytes)
{
    char	*p;
    
    if ((p = (char *) malloc (nbytes)) == NULL)
	_isfatal_error("malloc() failed");

    return (p);
}

char *_isrealloc(char *oldaddr, unsigned int nbytes)
{
    char	*p;
    
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
_isallocstring(char *str)
{
    char	*p;

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
_isfreestring(char *str)
{
    assert(str != NULL);
    free(str);
}
