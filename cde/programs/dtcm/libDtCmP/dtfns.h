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
/* $XConsortium: dtfns.h /main/3 1995/11/03 10:37:45 rswiston $ */
/*	@(#)dtfns.h 1.3 94/11/07 SMI	*/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */


#ifndef _DTFNS_H
#define _DTFNS_H

#ifdef FNS

#include <fns/fns.h>

#define DTFNS_SEPARATOR		':'

#define DTFNS_ORG_NAME		"org"
#define DTFNS_ORG_LEN		3

#define DTFNS_SITE_NAME		"site"
#define DTFNS_SITE_LEN		4

#define DTFNS_MYORG_NAME	"myorg"
#define DTFNS_MYORG_LEN		5

#define DTFNS_HOSTORG_NAME	"hostorg"
#define DTFNS_HOSTORG_LEN	7

#define DTFNS_HOST_NAME		"host"
#define DTFNS_HOST_LEN		4

#define DTFNS_THISUSER_NAME	"thisuser"
#define DTFNS_THISUSER_LEN	8

#define DTFNS_USER_NAME		"user"
#define DTFNS_USER_LEN		4

#define DTFNS_SERVICE_NAME	"service"
#define DTFNS_SERVICE_LEN	7

#define DTFNS_LONG_NAME_LEN	8		/* Longest of above names */

#define DTFNS_NULL_CONTEXT_TYPE		"SUNW_nscontext_null"
#define DTFNS_SERVICE_CONTEXT_TYPE	"SUNW_nscontext_service"

#define FNS_LIBRARY		"libfns.so.1"

extern int dtfns_init(void);
extern int dtfns_available(void);
extern int dtfns_service_name(const char *name, const char *type,
			      const char *service, const char *service_type,
			      const char *org, char *buf, const int size);
extern int dtfns_isa_helix_name(const char *);
extern int dtfns_lookup_str(const char *, char **, char *, int,
			    char *, int);
extern int dtfns_str_from_ref(NS_Reference_t *, int posn,
			      char *addr_buf, int addr_size,
			      char *type_buf, int type_size);
extern int dtfns_append_to_ref(NS_Reference_t *, const char *addr,
			       const char *addr_type);
extern int dtfns_bind_str(const char *name, const char *ref_type,
			  char *types[], const char *new_str);

extern NS_Reference_t *	dtfns_lookup_ref(const char *);
extern void 		dtfns_myorg_name(char *, int);
extern NS_Context_t *	dtfns_get_initial_ctx(void);

extern const NS_ReferenceAddress_t * dtfns_addr_from_ref(const NS_Reference_t *,
				char**, void**);
extern int   dtfns_str_from_addr(const NS_ReferenceAddress_t *, char *dbuf, int,
						char *tbuf, int);
extern NS_ReferenceAddress_t * dtfns_create_str_addr(const char *data,
						    const char *type);

extern int dtfns_insert_addr(NS_Reference_t *, void **,
				const NS_ReferenceAddress_t *);
extern int dtfns_delete_addr(NS_Reference_t *, void **);

#endif /* FNS */

#endif /* _DTFNS_H */

