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
/*%%  $XConsortium: isfname.c /main/3 1995/10/23 11:40:04 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfname.c
 *
 * Description:
 *	Functions to translate ISAM file name to .rec, .ind, and .var file
 *	names. Also, several auxiliary functions that deal with path names.
 *
 */

#include "isam_impl.h"

void _removelast();
char *_lastelement();

/*
 * _makedat_isfname(isfname)
 *
 * Return path to .rec file corresponding to the ISAM file isfname.
 * Conversion is done in place.
 */

void
_makedat_isfname(char *isfname)
{
    /* Append .rec */
    (void) strcat(isfname, DAT_SUFFIX);
}

/*
 * _makeind_isfname(isfname)
 *
 * Return path to .ind file corresponding to the ISAM file isfname.
 * Conversion is done in place.
 */

void
_makeind_isfname(char *isfname)
{
    /* Append .ind */
    (void) strcat(isfname, IND_SUFFIX);
}

/*
 * _makevar_isfname(isfname)
 *
 * Return path to .var file corresponding to the ISAM file isfname.
 * Conversion is done in place.
 */

void
_makevar_isfname(char *isfname)
{
    /* Append .var */
    (void) strcat(isfname, VAR_SUFFIX);
}


/*
 * _removelast(path)
 *
 * Remove last element of path. E.g. /usr/db/part yields /usr/db.
 */

void
_removelast(char *path)
{
    char	*p;

    for (p = path + strlen(path); *--p != '/' && p >= path; ) 
	*p = '\0';
}

/*
 * _lastelement(path)
 *
 * Return pointer to the last element in the path. 
 * E.g.: _lastelement("/usr/temp") returns "temp".
 */

char *
_lastelement(char *path)
{
    char	*p;
    
    p = path + strlen(path);

    while (*--p != '/' && p > path)
	{ ; }

    return ((*p == '/') ? (p + 1) : p);
}
