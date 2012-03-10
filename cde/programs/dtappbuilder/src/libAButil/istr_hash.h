
/*
 *	$XConsortium: istr_hash.h /main/4 1995/11/06 18:51:18 rswiston $
 *
 * @(#)istr_hash.h	1.5 01 Feb 1994
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#ifndef _HASH_H
#define _HASH_H

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <ab/ABTypes.h>
#include <libAButils/istr.h>

#define HASHSIZE 211
#define BUCKETSIZE 10

typedef struct ENTRY
{
    ISTRING value;
    void *entry;
}ENTRY;

typedef struct HASH
{
    struct HASH  *next;
    ENTRY   value[BUCKETSIZE];
}HASH;

extern HASH *hash_create();

extern int hash_destroy(HASH *table);

extern int insert(HASH *table, ISTRING istring, void *ptr);

extern int delete(HASH *table, ISTRING istring);

extern void *hash_lookup(HASH *hashtable, ISTRING istr);

#endif
