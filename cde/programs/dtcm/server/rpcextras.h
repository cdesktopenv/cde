/* $XConsortium: rpcextras.h /main/4 1995/11/09 12:51:20 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _RPCEXTRAS_H
#define _RPCEXTRAS_H

#include "ansi_c.h"

#define RPCGEN_ACTION(routine) routine

struct rpcgen_table {
    char	*(*proc)();
    xdrproc_t	xdr_arg;
    unsigned	len_arg;
    xdrproc_t	xdr_res;
    unsigned	len_res;
};

/* you might want to consider a program list rather than a table */
/* a list would be cleaner, a table easier.  it's a table here for clarity */
typedef struct prog_table {
	struct rpcgen_table *vers;
	u_long nproc;
	} program_table;

typedef struct prog_object {
	program_table *prog;
	u_long nvers;
	u_long program_num;
	} program_object;

typedef program_object *program_handle;

extern program_handle newph	P(());
extern program_handle getph	P(());

#endif
