/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isvars.c /main/3 1995/10/23 11:45:48 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isvars.c 1.7 90/08/15 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isvars.c
 *
 * Description:
 *	NetISAM gloabl variables
 *
 */

#include "isam_impl.h"

int iserrno;
int isreclen;
long isrecnum;

char   isstat1, isstat2, isstat3, isstat4;

static struct keydesc _nokey;
struct keydesc *nokey = &_nokey;

/* 
 * isdupl is used internally to indicate that some index contains
 * a duplicate value. Used only to set up isstat2 Cobol variable.
 */

int isdupl;				     /* 1 if duplicate found */

