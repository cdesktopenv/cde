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
/* $XConsortium: cmsdata.c /main/1 1996/04/21 19:22:08 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csa.h"
#include "cmsdata.h"
#include "nametbl.h"
#include "attr.h"

/* standard calendar attr name and entry attr name table */
_DtCmNameTable *_DtCm_cal_name_tbl = NULL;
_DtCmNameTable *_DtCm_entry_name_tbl = NULL;

/*
 * allocate a cms_entry structure and initialized with
 * all the library defined attribute names
 */
extern cms_entry *
_DtCm_make_cms_entry(_DtCmNameTable *tbl)
{
	int	i;
	cms_entry *ptr;

	if ((ptr = (cms_entry *)calloc(1, sizeof(cms_entry))) == NULL)
		return (NULL);

	/* initialize the entry with attribute names */

	if ((ptr->attrs = (cms_attribute *)calloc(1,
	    sizeof(cms_attribute)*(tbl->size + 1))) == NULL) {
		free(ptr);
		return (NULL);
	}

	for (i = 1; i <= tbl->size; i++) {
		if ((ptr->attrs[i].name.name = strdup(tbl->names[i])) == NULL) {
			/* clean up */
			ptr->num_attrs = i - 1;
			_DtCm_free_cms_entry(ptr);
			return (NULL);
		}

		ptr->attrs[i].name.num = i;
	}

	ptr->num_attrs = tbl->size;

	return (ptr);
}

extern CSA_return_code
_DtCm_copy_cms_entry(cms_entry *e, cms_entry **e_r)
{
	cms_entry *ptr;
	CSA_return_code stat;

	if (e == NULL || e_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((ptr = (cms_entry *)calloc(1, sizeof(cms_entry))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((stat = _DtCm_copy_cms_attributes(e->num_attrs, e->attrs,
	    &ptr->num_attrs, &ptr->attrs)) != CSA_SUCCESS) {
		free(ptr);
		return (stat);
	} else {
		ptr->key = e->key;
		*e_r = ptr;
		return (CSA_SUCCESS);
	}
}

extern void
_DtCm_free_cms_entry(cms_entry *entry)
{
	if (entry == NULL)
		return;

	if (entry->num_attrs > 0) {
		_DtCm_free_cms_attributes(entry->num_attrs + 1, entry->attrs);
		free(entry->attrs);
	}

	free(entry);
}

extern void
_DtCm_free_cms_entries(cms_entry *entry)
{
	cms_entry *ptr;

	while (entry) {
		ptr = entry->next;

		_DtCm_free_cms_entry(entry);

		entry = ptr;
	}
}

extern void
_DtCm_init_hash()
{
	static	boolean_t	done = B_FALSE;

	if (done == B_FALSE) {

		/* need to check whether table is actually created */
		_DtCm_cal_name_tbl = _DtCm_make_name_table(
					_DtCM_DEFINED_CAL_ATTR_SIZE,
					_CSA_calendar_attribute_names);
		_DtCm_entry_name_tbl = _DtCm_make_name_table(
					_DtCM_DEFINED_ENTRY_ATTR_SIZE,
					_CSA_entry_attribute_names);
		done = B_TRUE;
	}
}

/*
 * attr->name.num contains the correct index for the attribute
 */
extern CSA_return_code
_DtCmGrowAttrArray(uint *num_attrs, cms_attribute **attrs, cms_attribute *attr)
{
	cms_attribute	*newptr;
	CSA_return_code	stat;
	int		index;

	index = attr->name.num;

	if ((newptr = (cms_attribute *)realloc(*attrs,
	    sizeof(cms_attribute) * (index + 1))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);
	else {
		*attrs = newptr;
		memset((void *)&(*attrs)[*num_attrs+1], 0,
			sizeof(cms_attribute) * (index - *num_attrs));
	}

	if ((stat = _DtCm_copy_cms_attribute(&(*attrs)[index], attr, B_TRUE))
	    == CSA_SUCCESS) {

		*num_attrs = index;
	}

	return (stat);
}

