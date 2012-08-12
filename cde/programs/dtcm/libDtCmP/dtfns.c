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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*******************************************************************************
**
**  dtfns.c
**
**  $XConsortium: dtfns.c /main/3 1995/11/03 10:37:35 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */


#ifndef lint
static 	char sccsid[] = "@(#)dtfns.c 1.3 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
#endif

/*
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved
 */

#ifdef FNS

#include <EUSCompat.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <rpc/rpc.h>
#include <rpcsvc/nis.h>
#include <rpcsvc/nislib.h>
#include <rpcsvc/ypclnt.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fns/fns.h>

#include "debug.h"

#include "util.h"
#include "dtfns.h"

extern char *strdup(const char *);

/*
 * Names found in the initial context.  Used by isa_helix_name()
 *
 * XXX We have a bug here.  If a string happens to start with
 * thisuser, myorg, etc we'll recognize it as an FNS name even
 * though it may not be one.
 */
static char *initial_context_names[] = {
	"user:",
	"thisuser",
	"org:",
	"site:",
	"myorg",
	"hostorg",
	"host:",
	NULL,
};
static void		*libfns_handle = NULL;
static int		initialized = 0;
static NS_Context_t	*initial_ctx = NULL;

/*
 * These are dynamically bound entry points into libfns.  We dlopen()
 * libfns so that cm can still run even if libfns does not exist
 */
static NS_Context_t	   *(*NS_Context_get_initial_p)(NS_ContextStatus_t *);
static NS_ContextStatus_t *(*NS_ContextStatus_new_p)(void);
static void		(*NS_ContextStatus_delete_p)(NS_ContextStatus_t *);
static NS_Reference_t	   *(*NS_Context_lookup_p)(const NS_Context_t *,
						  const NS_CompositeName_t *,
							NS_ContextStatus_t *);
static int		   (*NS_Context_bind_p)(const NS_Context_t *,
						const NS_CompositeName_t *,
						const NS_Reference_t *,
						unsigned,
						NS_ContextStatus_t *);

static NS_Reference_t	*(*NS_Reference_new_p)(const NS_String_t *);
static void		(*NS_Reference_delete_p)(const NS_Reference_t *);
static NS_String_t	*(*NS_Reference_type_p)(const NS_Reference_t *);
static int		(*NS_Reference_count_p)(const NS_Reference_t *);
static NS_ReferenceAddress_t *(*NS_Reference_first_p)(const NS_Reference_t *,
							void **);
static NS_ReferenceAddress_t 	*(*NS_Reference_next_p)(const NS_Reference_t *,
							void **);
static int			(*NS_Reference_append_addr_p)(NS_Reference_t *,
					         const NS_ReferenceAddress_t *);
static int			(*NS_Reference_prepend_addr_p)(NS_Reference_t *,
					         const NS_ReferenceAddress_t *);
static int			(*NS_Reference_insert_addr_p)(NS_Reference_t *,
						 void **,
					         const NS_ReferenceAddress_t *);
static int			(*NS_Reference_delete_addr_p)(NS_Reference_t *,
						              void **);
static void			(*NS_ReferenceAddress_delete_p)(
						NS_ReferenceAddress_t *);

static NS_ReferenceAddress_t *(*NS_ReferenceAddress_new_p)(const NS_String_t *,
							     unsigned,
							     const void *);
static void *	(*NS_ReferenceAddress_data_p)(const NS_ReferenceAddress_t *);
static unsigned	(*NS_ReferenceAddress_length_p)(const NS_ReferenceAddress_t *);
static NS_String_t *(*NS_ReferenceAddress_type_p)(const NS_ReferenceAddress_t*);


static const char *	   (*NS_String_cstring_p)(const NS_String_t *);
static NS_String_t *	   (*NS_String_from_cstring_p)(const char *);
static NS_CompositeName_t * (*NS_CompositeName_from_cstring_p)(const char *);


static int get_helix_service_name(const char *name, const char *service,
				  const char *service_type, char **buf);
/*
 * Initialization.
 *
 * Load libfns (with dlopen) and bind in the entry points we use (with dlsym).
 *
 * We dynamically load the FNS library so that we don't have an explicit
 * dependency  on it.  This lets us run on systems which do not have FNS
 * installed
 *
 * Returns
 *		0	FNS not available
 *		1	Success
 */
int
dtfns_init(void)

{
	char	*libfns = FNS_LIBRARY;
	int	error;	

	if (libfns_handle != NULL) {
		return 1;
	}

	initialized = TRUE;

	libfns_handle = dlopen(libfns, RTLD_LAZY);
	if (libfns_handle == NULL) {
#ifdef CM_DEBUG
		char	*s = dlerror();
		if (s == NULL) {
			DP(("Could not dlopen %s\n", libfns));
		} else {
			DP(("Could not dlopen %s: %s\n", libfns, s));
		}
#endif
		return 0;
	}

	NS_Context_get_initial_p = (NS_Context_t *(*)(NS_ContextStatus_t *))
			dlsym(libfns_handle, "NS_Context_get_initial");

	NS_ContextStatus_new_p = (NS_ContextStatus_t *(*)(void))
			dlsym(libfns_handle, "NS_ContextStatus_new");
	NS_ContextStatus_delete_p = (void(*)(NS_ContextStatus_t *))
			dlsym(libfns_handle, "NS_ContextStatus_delete");

	NS_Context_lookup_p = (NS_Reference_t *(*)(const NS_Context_t *,
				    		  const NS_CompositeName_t *,
				    		  NS_ContextStatus_t *))
			dlsym(libfns_handle, "NS_Context_lookup");

	NS_Context_bind_p = (int(*)(const NS_Context_t *,
						const NS_CompositeName_t *,
						const NS_Reference_t *,
						unsigned,
						NS_ContextStatus_t *))
			dlsym(libfns_handle, "NS_Context_bind");

	NS_Reference_new_p = (NS_Reference_t *(*)(const NS_String_t *))
			dlsym(libfns_handle, "NS_Reference_new");
	NS_Reference_delete_p = (void(*)(const NS_Reference_t *))
			dlsym(libfns_handle, "NS_Reference_delete");;

	NS_Reference_type_p = (NS_String_t *(*)(const NS_Reference_t *))
			dlsym(libfns_handle, "NS_Reference_type");

	NS_Reference_count_p = (int(*)(const NS_Reference_t *))
			dlsym(libfns_handle, "NS_Reference_count");

	NS_Reference_first_p = (NS_ReferenceAddress_t *(*)
					(const NS_Reference_t *, void **))
			dlsym(libfns_handle, "NS_Reference_first");

	NS_Reference_next_p = (NS_ReferenceAddress_t *(*)
					(const NS_Reference_t *, void **))
			dlsym(libfns_handle, "NS_Reference_next");

	NS_Reference_append_addr_p = (int(*)(NS_Reference_t *,
					     const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_Reference_append_addr");

	NS_Reference_prepend_addr_p = (int(*)(NS_Reference_t *,
					        const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_Reference_prepend_addr");

	NS_Reference_insert_addr_p = (int(*)(NS_Reference_t *,
					  void **,
					  const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_Reference_insert_addr");

	NS_Reference_delete_addr_p = (int(*)(NS_Reference_t *, void **))
			dlsym(libfns_handle, "NS_Reference_delete_addr");

	NS_ReferenceAddress_delete_p = (void(*)(NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_ReferenceAddress_delete");


	NS_ReferenceAddress_new_p = (NS_ReferenceAddress_t *(*)
			(const NS_String_t *, unsigned, const void *))
			dlsym(libfns_handle, "NS_ReferenceAddress_new");

	NS_ReferenceAddress_data_p = (void *(*)(const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_ReferenceAddress_data");

	NS_ReferenceAddress_length_p = (unsigned(*)
					(const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_ReferenceAddress_length");

	NS_ReferenceAddress_type_p = (NS_String_t *(*)
					(const NS_ReferenceAddress_t *))
			dlsym(libfns_handle, "NS_ReferenceAddress_type");

	NS_String_cstring_p = (const char *(*)(const NS_String_t *))
			dlsym(libfns_handle, "NS_String_cstring");

	NS_String_from_cstring_p = (NS_String_t * (*)(const char *))
			dlsym(libfns_handle, "NS_String_from_cstring");

	NS_CompositeName_from_cstring_p =(NS_CompositeName_t *(*)(const char *))
			dlsym(libfns_handle, "NS_CompositeName_from_cstring");

	return 1;
}

/*
 * Check if FNS is available for use.
 *
 * You must call dfsinit() before calling this routine.
 *
 * First call to this routine may be costly as we get the initial context.
 * Subsequent calls are very cheap.
 *
 * Returns
 *		1	Yes, FNS is available.  
 *		0	No, FNS is not available.
 *		-1	You haven't called dtfns_init().
 */
int
dtfns_available(void)

{
	static int available;
	static int called;

	if (called) {
		return available;
	}

	if (libfns_handle != NULL) {

		/*
		 * libfns has been dlopened.  Now see if an FNS namespace
		 * is reachable by getting the initial context.
		 */
		if (dtfns_get_initial_ctx() == NULL) {
			available = 0;
		} else {
			available = 1;
		}
	} else if (initialized) {
		/* dlopen must have failed. FNS is not installed */
		available = 0;
	} else {
		available = -1;
	}
	called = 1;

	return available;
}

/*
 * Get the initial context.  This routine caches the initial context.
 * The first call to this routine may be costly, be subsequent calls
 * are very cheap.
 */
NS_Context_t *
dtfns_get_initial_ctx(void)

{
	DP(("dtfns_get_initial_ctx: Getting initial context\n"));

	if (initial_ctx == NULL) {
		NS_ContextStatus_t *	status;

		status = (*NS_ContextStatus_new_p)();
		initial_ctx = (*NS_Context_get_initial_p)(status);
		(*NS_ContextStatus_delete_p)(status);
	}

	return initial_ctx;
}

/*
 * Generate a full Helix name for a service. 
 *
 *	'name' may be a relative Helix name.  I.e.:
 *		smith
 *		smith:service
 *		smith:service:calendar
 *	In this case dtfns_service_name() will use the type, service and org
 *	parameters (if needed) to construct the full Helix name.
 *
 *	'name' may also be an absolute Helix name.  I.e.:
 *		user:smith
 *		org:ssi.eng:user:smith
 *		user:smith:service
 *	In this case dtfns_service_name() will use the type and service
 *	parameters (if needed) to construct the full Helix name.  The org
 *	parameter should be NULL (since the organization is already
 *	determined by the absolute Helix name).
 *
 *	'type' specifies the type of object 'name' refers to (ie "user").
 *
 *	'service' specifies what service is desired (ie "calendar").
 *
 *	'service_type' specifies the reference type of the service
 *	(ie SUNW_fns_calendar);
 *
 *	'org' is the organization name.  This must be NULL if 'name' is
 *	an absolute Helix name.  'org' may be NULL in all other cases if the
 *	default organization is to be used.
 *
 *	'buf' is a buffer provided by the caller in which the expanded name
 *	is placed.
 *
 *	'size' is the size of 'buf'
 *	
 *
 * The string returned in buf may be used in calls to dtfns_lookup_str() and
 * dtfns_lookup_ref().
 *
 *	Returns
 *		-1	Name not found / Error
 *		0	FNS not available
 *		1	Success
 */
int
dtfns_service_name(
	const char *name, 	/* Name to lookup */
	const char *type,	/* Type of object name is (ie "user") */
	const char *service,	/* Service name (ie "calendar") */
	const char *service_type, /* Service reference type	*/
	     			  /* (ie "SUNW_fns_calendar");	*/
	const char *org,	/* Org name (ie "ssi").  NULL for default org */
	char	   *buf,	/* Buffer to place name in */
	const int  size)	/* Size of value_buf */

{
	char	*tmp_buf;
	char	*type_str;

	if (libfns_handle == NULL) {
		return 0;
	}

	if (name == NULL) {
		return -1;
	}

	if (org != NULL && *org != '\0') {
		/* Sanity check for size */
		if (strlen(org) + strlen(name) > (size_t)size - 50) {
			return -1;
		}

		/*Construct Helix name from name and organization */
		sprintf(buf,"%s:%s:%s:%s:%s:%s", DTFNS_ORG_NAME,
			org, type, name, DTFNS_SERVICE_NAME, service);
	} else if (dtfns_isa_helix_name(name)) {
		/* Helix name. Expand it to point to calendar service */
		if (get_helix_service_name(name, service, service_type,
							&tmp_buf) < 0) {
			return -1;
		}
		buf[size - 1] = '\0';
		strncpy(buf, tmp_buf, size - 1);
		free(tmp_buf);

	} else {
		/* Construct Helix name from name */
		sprintf(buf,"%s:%s:%s:%s", type,
			name, DTFNS_SERVICE_NAME, service);
	}

	return 1;
}

/*
 * Return TRUE if 'name' is an absolute Helix name
 */
int
dtfns_isa_helix_name(const char *name)

{
	int	n, len;
	char	**p;

	for (p = initial_context_names; *p != NULL; p++) {
		len = strlen(*p);
		if (strncmp(name, *p, len) == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Get the org name for "myorg" (ie ssi.eng)
 */
void
dtfns_myorg_name(char *buf, int len)

{
	char		*__nis_local_root();
	static char	*myorg_name;
	char		*principal_name;
	char		*host_domain;
	char		*root_domain;
	char		*pd = NULL;
	char		*myorg_p, *root_p;

	if (myorg_name == NULL) {
		principal_name	= nis_local_principal();
		host_domain	= nis_local_directory();
		root_domain	= __nis_local_root();

		if (principal_name) {
			/* Get the domain this principal is in */
			pd = nis_domain_of(principal_name);
			if (pd && *pd == '.') {
				pd = NULL;
			}
		}

		/* If no domain for the principal, use the one for the host */
		myorg_name = strdup(pd ? pd : (host_domain ? host_domain : ""));

		/*
		 * We want to strip the root domain name off of the
		 * end of the myorg name.  Set pointers to the end
		 * of each name
		 */
		myorg_p = myorg_name + strlen(myorg_name) - 1;
		root_p  = root_domain + strlen(root_domain) - 1;

		while (tolower(*myorg_p) == tolower(*root_p)) {
			if (root_p == root_domain) {
				/* Matched all of root domain name. Truncate */
				if (myorg_p != myorg_name) {
					*--myorg_p = '\0';
				}
				break;
			} else if (myorg_p == myorg_name) {
				break;
			}
			--myorg_p;
			--root_p;
		}
	}

	if (myorg_name != NULL) {
		buf[len - 1] = '\0';
		strncpy(buf, myorg_name, len - 1);
	}

	return;
}

/*
 * Return the string value bound to an absolute Helix name
 *
 *	Returns
 *		-1	Error
 *		0	FNS not available
 *		1	Success
 */
int
dtfns_lookup_str(
	const char *name,	/* Absolute Helix name */
		char *types[],	/* Type of data to get */
	char	*dbuf,		/* Buffer to place data value in */
		int dsize,	/* Size of dbuf */
	char	*tbuf,		/* Buffer to place address type in */
		int tsize)	/* Size of tbuf */

{
	NS_Reference_t *	ref;
	const NS_ReferenceAddress_t *	addr;
	void			*iter_pos;

	if (libfns_handle == NULL) {
		return 0;
	}

	/* Get the reference from the name */
	if ((ref = dtfns_lookup_ref(name)) == NULL) {
		return -1;
	}

	/* Get the address that matches at least one of the specified types */
	if ((addr = dtfns_addr_from_ref(ref, types, &iter_pos)) == NULL) {
		(*NS_Reference_delete_p)(ref);
		return -1;
	}

	(*NS_Reference_delete_p)(ref);

	/* Return data bound to that address */
	return dtfns_str_from_addr(addr, dbuf, dsize, tbuf, tsize);
}

/*
 * 
 *	Bind a string to a name.
 *
 *	name		Absolute Helix name to bind string to
 *
 *	ref_type	Reference type.  We need this to create
 *			the reference if it does not already exist.
 *	
 *	types		Array of address types. This routine looks for
 *			the address that matches at least one of these
 *			types.  The first type is used when the new
 *			string is bound.
 *
 *	new_str		String to bind to name
 *
 *	Returns
 *		-1	Error
 *		0	FNS not available
 *		1	Success
 */
int
dtfns_bind_str(
	const char 	*name,	/* Absolute Helix name */
	const char	*ref_type,	/* Reference type */
	      char	*types[],	/* Address type of data to operate on */
	const char	*new_str	/* String to bind to name */
)
{
	NS_Reference_t		*ref = NULL;
	const NS_ReferenceAddress_t	*addr = NULL;
	NS_ReferenceAddress_t		*new_addr = NULL;
	NS_CompositeName_t	*comp_name;
	NS_ContextStatus_t	*status = NULL;
	NS_Context_t		*ctx = NULL;
	NS_String_t 		*nstr;
	void			*iter_pos = NULL;
	char			buf[256];
	int			rcode = -1;

	if (libfns_handle == NULL) {
		return 0;
	}

	/* Get the reference from the name */
	if ((ref = dtfns_lookup_ref(name)) != NULL) {
		/*
		 * A value is already bound to this name.
		 * Get the address that matches at least one of
		 * the specified types
		 */
		addr = dtfns_addr_from_ref(ref, types, &iter_pos);
		if (addr != NULL) {
			/* Get string bound to that address */
			*buf = '\0';
			dtfns_str_from_addr(addr, buf, sizeof(buf), NULL, 0);

			/* Are we changing the string? */
			if (strcmp(buf, new_str) == 0) {
				/* Bound value is the same as new value */
				rcode = 1;
				goto EXIT;
			} else {
				/* Delete old address string */
				(*NS_Reference_delete_addr_p)(ref, &iter_pos);
			}
		}
	} else {
		/* Nothing bound to name.  Create an empty reference */
		nstr = (*NS_String_from_cstring_p)(ref_type);
		ref = (*NS_Reference_new_p)(nstr);
		iter_pos = NULL;
	}

	/* Create new address */
	if ((new_addr = dtfns_create_str_addr(new_str, types[0])) == NULL) {
		goto EXIT;
	}

	/* Insert new address into reference */
	if (dtfns_insert_addr(ref, &iter_pos, new_addr) < 1) {
		goto EXIT;
	}

	/* Bind reference into name space */
	if ((ctx = dtfns_get_initial_ctx()) == NULL) {
		goto EXIT;
	}

	status = (*NS_ContextStatus_new_p)();
	comp_name = (*NS_CompositeName_from_cstring_p)(name);

	if ((*NS_Context_bind_p)(ctx, comp_name, ref, 0, status) < 1) {
		goto EXIT;
	}

	rcode = 1;

EXIT:
	/* Clean up and return */
	if (new_addr != NULL) (*NS_ReferenceAddress_delete_p)(new_addr);
	if (ref    != NULL) (*NS_Reference_delete_p)(ref);
	if (status != NULL) (*NS_ContextStatus_delete_p)(status);
	return rcode;
}


/*
 * Return the reference bound to an absolute Helix name
 *
 *	Returns
 *		NULL	Error
 *		NS_Reference_t * for reference
 */
NS_Reference_t *
dtfns_lookup_ref(
	const char     *name)/* Absolute Helix name to lookup */

{
	NS_Reference_t *	ref = NULL;
	NS_ContextStatus_t *	status = NULL;
	NS_CompositeName_t *	comp_name = NULL;
	NS_Context_t *		ctx;

	if (libfns_handle == NULL) {
		return NULL;
	}

	status = (*NS_ContextStatus_new_p)();
	if ((ctx = dtfns_get_initial_ctx()) != NULL) {
		/* Lookup the Helix name specified by "name" */
		comp_name = (*NS_CompositeName_from_cstring_p)(name);
		ref = (*NS_Context_lookup_p)(ctx, comp_name, status);
	}

	(*NS_ContextStatus_delete_p)(status);
	return ref;
}

/*
 * Takes a Helix name and converts it to a full name for a service.
 * I.e. if service is "calendar":
 *
 *		name				 buf
 *	org:ss-eng:user:dipol 	-->	org:ss-eng:user:dipol:service:calendar
 *	org:ss-eng		-->	org:ss-eng:service:calendar
 *	org:ss-eng:service:	-->	org:ss-eng:service:calendar
 *
 * Returns
 *		-1 	Error.  buf not set
 *		0	name already points to the service
 *			buf is a duplicate of name.
 *		1	name resolved.  buf is set.  Caller
 *			is responsible for freeing memory.
 */
static int
get_helix_service_name(
	const char *name,
	const char *service,
	const char *service_type,
	char **buf)

{
	int		n;
	char		*tmp_name;
	NS_Reference_t	*ref;
	NS_String_t	*nstr;
	const char		*type;
	char		*p;

	n = strlen(name);
	tmp_name = strdup(name);

	/* Remove trailing : if there is one */
	if (tmp_name[n - 1] == ':') {
		tmp_name[n - 1] = '\0';
		n--;
	}

	/* Get reference and extract reference type */
	if ((ref = dtfns_lookup_ref(tmp_name)) == NULL) {
		return -1;
	}

	nstr = (*NS_Reference_type_p)(ref);
	type = (*NS_String_cstring_p)(nstr);

	(*NS_Reference_delete_p)(ref);

	/* If name is already bound to a calendar service then we are done */
	if (strcmp(type, service_type) == 0) {
		*buf = strdup(name);
		return 0;
	}

	if ((p = strrchr(tmp_name, ':')) != NULL) {
		p++;
	}

	/* 
	 * If name is a service context or the "service" null context
	 * then we want to append ":calendar"
	 */
	if (strcmp(type, DTFNS_SERVICE_CONTEXT_TYPE) == 0 ||
	    (strcmp(type, DTFNS_NULL_CONTEXT_TYPE) == 0 && p != NULL &&
	    strcmp(p, DTFNS_SERVICE_NAME) == 0)) {

		*buf = (char *)malloc(n + strlen(service) + 2);
		if (*buf == NULL) {
			return -1;
		}
		strcpy(*buf, tmp_name);
		strcat(*buf, ":");
		strcat(*buf, service);
		return 1;
	} else {
		/* Append ":service:calendar" */
		*buf = (char *)malloc(n + 
						   strlen(DTFNS_SERVICE_NAME) +
						   strlen(service) + 3);
		sprintf(*buf, "%s:%s:%s", tmp_name,
					DTFNS_SERVICE_NAME, service);
		return 1;
	}
}

/*
 * Get an FNS Address from a Reference.
 *
 *	ref 		The Reference to get the address from
 *
 *      types		NULL terminated array of one or more type strings.
 *			This routine returns the first address that matches
 *			a type specified in this array.  NULL to just
 *			get the first address.
 *
 *	iter_pos	Updated to point after retrieved address.
 *
 *	Returns
 *		Pointer to the found address
 *		NULL if no address is found
 *
 */
const NS_ReferenceAddress_t *
dtfns_addr_from_ref(
	const NS_Reference_t	*ref,	/* Reference to get addr from */
	      char	*types[],	/* Types of addrs to get */
	void		**iter_pos	/* Returned pos where addr was found */
)

{
	const NS_ReferenceAddress_t	*addr;
	char			type[128];
	char			**p;

	if (libfns_handle == NULL) {
		return 0;
	}

	addr = (*NS_Reference_first_p)(ref, iter_pos);
	if (types == NULL) {
		return addr;
	}

	while (addr != NULL) {
		/* Get the type, and see if it is one we want */
		dtfns_str_from_addr(addr, NULL, 0, type, sizeof(type));
		for (p = types; *p != NULL && strcmp(*p, type) != 0; *p++)
			;

		if (*p != NULL) {
			/* Found it! */
			break;
		} else {
			/* Get next address */
			addr = (*NS_Reference_next_p)(ref, iter_pos);
		}
	}

	return addr;
}

/*
 * Get the string and type out of an Address
 *
 *	Returns
 *		-1 	Error
 *		0 	FNS not available
 *		1	Success
 */
int
dtfns_str_from_addr(
	const NS_ReferenceAddress_t	*addr,	/* Addr to get string from */
	char			*dbuf,		/* Buf to hold string data */
	int			dbuf_size,	/* Size of dbuf */
	char			*tbuf,		/* Buf to hold string type */
	int			tbuf_size	/* size of tbuf */
)

{
	XDR		xdr;
	char		*s = NULL;
	NS_String_t	*nstr;
	const char	*cs;

	if (libfns_handle == NULL) {
		return 0;
	}

	if (dbuf != NULL) {
		/* Convert data from XDR to a string */
		xdrmem_create(&xdr,
			(caddr_t)(*NS_ReferenceAddress_data_p)(addr),
			(*NS_ReferenceAddress_length_p)(addr), XDR_DECODE);

		if (xdr_string(&xdr, &s, ~0) == FALSE) {
			return -1;
		}

		dbuf[dbuf_size - 1] = '\0';
		strncpy(dbuf, s, dbuf_size - 1);
		free(s);
	}

	if (tbuf != NULL) {
		tbuf[tbuf_size - 1] = '\0';
		nstr = (*NS_ReferenceAddress_type_p)(addr);
		cs = (*NS_String_cstring_p)(nstr);
		strncpy(tbuf, cs, tbuf_size - 1);
	}

	return 1;
}

/*
 * Create an Address of type "type" for the string specified by "data"
 *
 *	Returns
 *		A new address contianing the specified data
 *		NULL on an error
 */
NS_ReferenceAddress_t *
dtfns_create_str_addr(
	const char	*data,
	const char	*type
)

{
	XDR	xdr;
	u_char	buf[1024];
	NS_String_t	*nstring;

	if (libfns_handle == NULL) {
		return NULL;
	}

	xdrmem_create(&xdr, (caddr_t)buf, sizeof(buf), XDR_ENCODE);
	if (xdr_string(&xdr, (char**)&data, ~0) == FALSE) {
		return NULL;
	}

	nstring = (*NS_String_from_cstring_p)(type);
	return (*NS_ReferenceAddress_new_p)(nstring, xdr_getpos(&xdr), buf);
}

/*
 * Add an address to a reference at the location specified by iter_pos.
 * The address is inserted before iter_pos.
 * If *iter_pos is NULL then put the address in the first slot.
 *
 *	Returns
 *		-1 	Error
 *		0 	FNS not available
 *		1	Success
 */
int
dtfns_insert_addr(
	NS_Reference_t		*ref,
	void			**iter_pos,
	const NS_ReferenceAddress_t	*addr
)

{
	if (libfns_handle == NULL) {
		return 0;
	}

	if (*iter_pos == NULL) {
		if ((*NS_Reference_prepend_addr_p)(ref, addr) < 1)
			return -1;
	} else {
		if ((*NS_Reference_insert_addr_p)(ref, iter_pos, addr) < 1)
			return -1;
	}

	return 1;
}

/*
 * Delete the Address at position "iter_pos" from a Reference
 *
 *	Returns
 *		-1 	Error
 *		0 	FNS not available
 *		1	Success
 */
int
dtfns_delete_addr(
	NS_Reference_t		*ref,
	void			**iter_pos
)

{
	if (libfns_handle == NULL) {
		return 0;
	}

	(*NS_Reference_delete_addr_p)(ref, iter_pos);

	return 1;
}

#endif /* FNS */
