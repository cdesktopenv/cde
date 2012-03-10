/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isfab.c /main/3 1995/10/23 11:38:24 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isfab.c 1.8 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfab.c
 *
 * Description:
 *	The ISAM file access block functions.
 *
 */

#include "isam_impl.h"


/*
 * fab = *fab_new(isfname, mode, varlen, minreclen, maxreclen)
 *
 * Fab_new() creates an File access block (fab object) that is used
 * for all subsequent operations in this file. Return a pointer to 
 * the fab object, or NULL in the case of an error.
 */

Fab *
_fab_new(isfname, openmode,
	 varlen, minreclen, maxreclen)
    char		*isfname;	     /* Local path on the host */
    enum openmode	openmode;
    Bool		varlen;		     /* 0/1 flag */
    int			minreclen, maxreclen;
{
    register Fab	*fab;

    /* Allocate memory for the fab object. */
    fab = (Fab *) _ismalloc(sizeof(*fab));
    memset((char *)fab, 0, sizeof(*fab));

    if (fab == NULL) {
      iserrno = EFATAL;
      return (NULL);
    }

    /* Set fields in the fab objects. */
    fab->openmode = openmode;
    fab->varlength = varlen;
    fab->minreclen = minreclen;
    fab->maxreclen = maxreclen;
    fab->isfname = _isallocstring(isfname);

    if (fab->isfname == NULL) {
      free((char *)fab);
      iserrno = EFATAL;
      return (NULL);
    }

    return (fab);
}

void
_fab_destroy(fab)
    register Fab	*fab;
{
    assert(fab != NULL);
    assert(fab->isfname != NULL);

    _isfreestring(fab->isfname);

    _bytearr_free(&fab->isfhandle);
    _bytearr_free(&fab->curpos);

    free((char *)fab);
}
