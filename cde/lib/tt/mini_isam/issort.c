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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: issort.c /main/3 1995/10/23 11:44:55 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)issort.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * issort.c
 *
 * Description:
 *	ISAM sort package (sort in memory)
 */
#include <stdlib.h>

#include "isam_impl.h"

extern char *_isunix_malloc();

/* 
 * _issort_create() 
 *
 * Create a sorter
 */

Issort *
_issort_create(reclen,nrecs,compfunc)
    int		reclen;
    int		nrecs;
    int		(*compfunc)();
{	
    Issort	*p;

    p = (Issort *)_ismalloc(sizeof(*p));     /* Allocate new sorter */
    memset((char *)p, 0,sizeof(*p));

    p->ist_reclength = reclen;		     /* Record length in bytes */
    p->ist_allocrecs = nrecs;		     /* Maximum number of records */
					     /* that can inserted */
    p->ist_nrecs = 0;			     /* Current number of records */
    p->ist_currec;			     /* Current position */
    p->ist_compf = compfunc;		     /* Comparison function */

    p->ist_array = _ismalloc((unsigned)(reclen * nrecs)); /* Allocate array */
					     /* for storing records */

    return(p);
}

/* 
 * _issort_destroy() 
 *
 * Destroy a sorter 
 */

void
_issort_destroy(srt)
    Issort	*srt;
{
    assert(srt->ist_array != (char *) 0);

    (void) free(srt->ist_array);
    (void) free((char *)srt);
}

/* 
 * _issort_insert() 
 *
 * Insert record to sorter 
 */

void
_issort_insert(srt,record)
    register Issort	*srt;
    char		*record;
{
    assert(srt->ist_nrecs < srt->ist_allocrecs);

    memcpy(srt->ist_array + srt->ist_nrecs * srt->ist_reclength,record,
	   srt->ist_reclength);

    srt->ist_currec = srt->ist_nrecs++;
}

/* 
 * _issort_sort() 
 *
 *  Sort records 
 */

void
_issort_sort(srt)
    Issort	*srt;
{
    if (srt->ist_nrecs > 1)
	qsort(srt->ist_array,srt->ist_nrecs,srt->ist_reclength,srt->ist_compf);

    _issort_rewind(srt);		     /* Rewind for subsequent reads */
}

/* 
 * _issort_rewind() 
 * 
 * Rewind sorter 
 */

void
_issort_rewind(srt)
    Issort	*srt;
{
    srt->ist_currec = 0;
}

/* 
 * _issort_read() 
 * 
 * Read record from sorter 
 */

char *
_issort_read(srt)
    register Issort	*srt;
{
    return((srt->ist_currec < srt->ist_nrecs) ?
	(srt->ist_array + srt->ist_currec++ * srt->ist_reclength) :
	    (char *) 0);
}
