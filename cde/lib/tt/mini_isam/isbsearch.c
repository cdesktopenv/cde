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
/*%%  $XConsortium: isbsearch.c /main/3 1995/10/23 11:35:44 rswiston $ 			 				 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isbsearch.c
 *
 * Description:
 *	Binary search function.
 *	If differs from bsearch(3) in that if an exact match is not found,
 *	the next lower key is returned. If key is lower than any key in table,
 *	NULL value is returned.
 *	
 */


char *_isbsearch (char *key, char *table, int nelems, int keylen, int (*cmpf) ())
{
    int len,l1,result;		     /* current length of array to search */
    char *p,*low;
    
    if (nelems <= 0) return (char *) 0;

    /* Check if key is lower than any key in the table. */
    result = (*cmpf) (key,table);
    if (result < 0 ) return (char *) 0;	     /* Key is lower */
    else if (result == 0) return table;	     /* Exact match */
    
    for (low = table,len = nelems; len > 0;) {
	l1 = len >> 1;
	p = low + keylen * l1;
	if ((result = (*cmpf) (key,p)) == 0)    /* Exact match found */
	    return p;
	
	if (result > 0 )	{		     /* Key is in higher half */
	    len -= l1 + 1;
	    low = p + keylen;
	}
	else   {			     /* Key is in lower half */
	    len = l1 ;
	}
    }

    /* No matching key found, return next lower key */
    return (result>0)?p:(p-keylen);		     
}


