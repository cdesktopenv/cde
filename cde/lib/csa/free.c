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
/* $XConsortium: free.c /main/1 1996/04/21 19:23:16 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdlib.h>
#include "csa.h"
#include "free.h"

#define _DtCM_OUR_MEMORY	0x55555555

typedef void (*_DtCmFreeProc)(uint num_elem, void *ptr);

typedef struct {
	int		mark;
	uint		num_elem;
	_DtCmFreeProc	fproc;
} _DtCmMemHeader;

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static void _DtCm_free_calendar_users(uint num_elem, void *vptr);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_calendar_user *
_DtCm_alloc_calendar_users(uint num_elem)
{
	char *ptr;
	_DtCmMemHeader	*hptr;

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) +
	    sizeof(CSA_calendar_user) * num_elem)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = num_elem;
		hptr->fproc = _DtCm_free_calendar_users;
		return ((CSA_calendar_user *)(ptr + sizeof(_DtCmMemHeader)));
	}
}

extern char **
_DtCm_alloc_character_pointers(uint num_elem)
{
	char *ptr;
	_DtCmMemHeader	*hptr;

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) +
	    sizeof(char *) * num_elem)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = num_elem;
		hptr->fproc = _DtCm_free_character_pointers;
		return ((char **)(ptr + sizeof(_DtCmMemHeader)));
	}
}

extern CSA_attribute *
_DtCm_alloc_attributes(uint num_elem)
{
	char *ptr;
	_DtCmMemHeader	*hptr;
	extern void _DtCm_free_attributes(uint size, void *attrs);

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) +
	    sizeof(CSA_attribute) * num_elem)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = num_elem;
		hptr->fproc = _DtCm_free_attributes;
		return ((CSA_attribute *)(ptr + sizeof(_DtCmMemHeader)));
	}
}

extern CSA_entry_handle *
_DtCm_alloc_entry_handles(uint num_elem)
{
	char *ptr;
	_DtCmMemHeader	*hptr;
	extern void _DtCm_free_entry_handles(uint num_entries, void *entries);

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) +
	    sizeof(CSA_entry_handle) * num_elem)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = num_elem;
		hptr->fproc = _DtCm_free_entry_handles;
		return ((CSA_entry_handle *)(ptr + sizeof(_DtCmMemHeader)));
	}
}

extern void *
_DtCm_alloc_entry(uint size)
{
	char *ptr;
	_DtCmMemHeader	*hptr;
	extern void _DtCm_free_entry_content(uint dummy, void *entry);

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) + size)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = 1;
		hptr->fproc = _DtCm_free_entry_content;
		return ((void *)(ptr + sizeof(_DtCmMemHeader)));
	}
}

extern CSA_reminder_reference *
_DtCm_alloc_reminder_references(uint num_elem)
{
	char *ptr;
	_DtCmMemHeader	*hptr;
	extern void _DtCm_free_reminder_references(uint num_rems, void *rems);

	if ((ptr = calloc(1, sizeof(_DtCmMemHeader) +
	    sizeof(CSA_reminder_reference) * num_elem)) == NULL)
		return (NULL);
	else {
		hptr = (_DtCmMemHeader *)ptr;
		hptr->mark = _DtCM_OUR_MEMORY;
		hptr->num_elem = num_elem;
		hptr->fproc = _DtCm_free_reminder_references;
		return ((CSA_reminder_reference *)(ptr +
			sizeof(_DtCmMemHeader)));
	}
}

extern CSA_return_code
_DtCm_free(void *ptr)
{
	_DtCmMemHeader *hptr;

	if (ptr == NULL)
		return (CSA_E_INVALID_MEMORY);

	hptr = (_DtCmMemHeader *)(((char *)ptr) - sizeof(_DtCmMemHeader));

	if (hptr->mark != _DtCM_OUR_MEMORY)
		return (CSA_E_INVALID_MEMORY);
	else {
		hptr->mark = 0;
		hptr->fproc(hptr->num_elem, ptr);
		free(hptr);
		return (CSA_SUCCESS);
	}
}

extern void
_DtCm_free_character_pointers(uint num_elem, void *vptr)
{
	char **ptr = (char **)vptr;
	int	i;

	for (i = 0; i < num_elem; i++) {
		if (ptr[i])
			free(ptr[i]);
		else
			break;
	}
}

/******************************************************************************
 * static functions used within in the file
 ******************************************************************************/

static void
_DtCm_free_calendar_users(uint num_elem, void *vptr)
{
	CSA_calendar_user *ptr = (CSA_calendar_user *)vptr;
	int	i;

	for (i = 0; i < num_elem; i++) {
		if (ptr[i].user_name)
			free(ptr[i].user_name);

		if (ptr[i].calendar_address)
			free(ptr[i].calendar_address);
	}
}

