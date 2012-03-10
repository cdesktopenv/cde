/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isbsearch.c /main/3 1995/10/23 11:35:44 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isbsearch.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif

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


char *_isbsearch (key,table,nelems,keylen,cmpf)
    char *key;
    char *table;
    int nelems;
    int keylen;
    int (*cmpf) ();
{
    register int len,l1,result;		     /* current length of array to search */
    register char *p,*low;
    
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


