/* $XConsortium: programtable.c /main/4 1995/11/09 12:47:18 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/* implements the program table for this program number */
/* this module doesn't know anything about the specifics of any */
/* rtable except how many rtables are supported */
/*  - ie it doesn't include rtable*.h.  */
/* The actual program table entries are filled in by the rtable*.c's */

#include <EUSCompat.h>
#include <stdio.h>
#include <rpc/rpc.h>
#include "rpcextras.h"

program_table ptable[] = {
	(struct rpcgen_table *)NULL, 0, /* rtable 0 no longer supported */
	(struct rpcgen_table *)NULL, 0, /* rtable 1 no longer supported */
	(struct rpcgen_table *)NULL, 0, /* rtable 2 filled in by rtable2.c */
	(struct rpcgen_table *)NULL, 0, /* rtable 3 filled in by rtable3.c */
	(struct rpcgen_table *)NULL, 0, /* rtable 4 filled in by rtable4.c */
	(struct rpcgen_table *)NULL, 0, /* rtable 5 filled in by cmsfunc.c */
	}; 
	
/* program_num is filled in from one of the rtable*.c's so that */
/* it can be declared from one of the rtable*.h's */
program_object po = {
	&ptable[0], 0, 0,
	};
	
program_handle program = &po;

program_handle newph() 
{
	extern void initrtable2();
	extern void initrtable3();
	extern void initrtable4();
	extern void initfunctable();
	
	program->nvers = sizeof(ptable)/sizeof(ptable[0]);
	initrtable2(program);
	initrtable3(program); 
	initrtable4(program);
	initfunctable(program);
	return(program);
}

program_handle getph() 
{
	return(program);
}


