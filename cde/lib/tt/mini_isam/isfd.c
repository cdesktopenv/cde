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
/*%%  $XConsortium: isfd.c /main/3 1995/10/23 11:39:10 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfd.c
 *
 * Description:
 *	The ISAM file descriptors (isfd) are used as index to a table of 
 *	pointers to File Access Block objects. Isfd.c maintains the table
 *	of the pointers (isfdtab).
 *
 */

#include "isam_impl.h"

static Fab *isfdtab[MAXISFD];		     /* Table of pointers */


/*
 * _isfd_insert(fab)
 *
 * Insert a pointer to an Fab object to table of ISAM file descriptors.
 * Return an ISAM file descriptor, or NOISFD if the table is full.
 */

Isfd
_isfd_insert(Fab *fab)
{
    Isfd	i;

    for (i = 0; i < MAXISFD; i++) {
	if (isfdtab[i] == NULL)		     /* Empty entry found */
	    break;
    }

    if (i == MAXISFD)
	return (NOISFD);		     /* isfdtab is full */

    isfdtab[i] = fab;
    return (i);
}


/* 
 * _isfd_find(isfd) 
 *
 * Return a pointer to Fab object associated with the ISAM file 
 * descriptor isfd. If isfd is not a file descriptor of an open ISAM file,
 * return NULL.
 */

Fab *
_isfd_find(Isfd isfd)
{
    if (isfd < 0 || isfd >= MAXISFD || isfdtab[isfd] == NULL)
	return (NULL);
    else
	return (isfdtab[isfd]);
}

/*
 * _isfd_delete(isfd)
 *
 * Delete an entry from isfdtab. No check is made the entry exists.
 */

void
_isfd_delete(Isfd isfd)
{
    if (isfd >= 0 && isfd < MAXISFD)
	isfdtab[isfd] = NULL;
}

