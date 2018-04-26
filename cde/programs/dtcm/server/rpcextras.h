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
