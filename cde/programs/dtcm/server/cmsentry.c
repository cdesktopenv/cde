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
/* $XConsortium: cmsentry.c /main/4 1995/11/09 12:41:44 rswiston $ */
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
#include "cmsentry.h"
#include "cmsdata.h"
#include "nametbl.h"
#include "attr.h"

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static CSA_return_code
_ExtractEntryAttrsFromEntry(uint srcsize, cms_attribute *srcattrs,
	uint *dstsize, cms_attribute **dstattrs, boolean_t time_only);

/*****************************************************************************
 * extern functions
 *****************************************************************************/

/*
 * Given a hashed name table, initialize a new cms_entry with
 * the given list of attribute values
 */
extern CSA_return_code
_DtCmsMakeHashedEntry(
	_DtCmsCalendar	*cal,
	uint		num,
	cms_attribute	*attrs,
	cms_entry	**entry)
{
	int		i, index;
	cms_entry	*eptr;
	CSA_return_code	stat = CSA_SUCCESS;

	if (cal == NULL || entry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((eptr = _DtCm_make_cms_entry(cal->entry_tbl)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0; i < num && stat == CSA_SUCCESS; i++) {
		index = _DtCm_get_index_from_table(cal->entry_tbl,
			attrs[i].name.name);

		if (index > 0) {
			/* check type */
			if (index > _DtCM_DEFINED_ENTRY_ATTR_SIZE &&
			    attrs[i].value &&
			    attrs[i].value->type != cal->types[index])
				stat = CSA_E_INVALID_ATTRIBUTE_VALUE;
			else
				stat = _DtCm_copy_cms_attr_val(attrs[i].value,
					&eptr->attrs[index].value);
		} else if (attrs[i].value) {
			if ((stat = _DtCmExtendNameTable(attrs[i].name.name, 0,
			    attrs[i].value->type, _DtCm_entry_name_tbl,
			    _DtCM_DEFINED_ENTRY_ATTR_SIZE,
			    _CSA_entry_attribute_names, &cal->entry_tbl,
			    &cal->types)) == CSA_SUCCESS) {

				attrs[i].name.num = cal->entry_tbl->size;

				stat = _DtCmGrowAttrArray(&eptr->num_attrs,
					&eptr->attrs, &attrs[i]);
			}
		}
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_cms_entry(eptr);
		return (stat);
	}

	*entry = eptr;
	return (CSA_SUCCESS);
}

extern void
_DtCmsFreeEntryAttrResItem(cms_get_entry_attr_res_item *elist)
{
	cms_get_entry_attr_res_item *ptr;

	while (elist) {
		ptr = elist->next;

		_DtCm_free_cms_attributes(elist->num_attrs, elist->attrs);
		free(elist);

		elist = ptr;
	}
}

extern CSA_return_code
_DtCmsGetCmsEntryForClient(cms_entry *e, cms_entry **e_r, boolean_t time_only)
{
	cms_entry *ptr;
	CSA_return_code stat;

	if (e == NULL || e_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((ptr = (cms_entry *)calloc(1, sizeof(cms_entry))) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((stat = _ExtractEntryAttrsFromEntry(e->num_attrs, e->attrs,
	    &ptr->num_attrs, &ptr->attrs,time_only)) != CSA_SUCCESS) {
		free(ptr);
		return (stat);
	} else {
		ptr->key = e->key;
		*e_r = ptr;
		return (CSA_SUCCESS);
	}
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static CSA_return_code
_ExtractEntryAttrsFromEntry(
	uint		srcsize,
	cms_attribute	*srcattrs,
	uint		*dstsize,
	cms_attribute	**dstattrs,
	boolean_t	time_only)
{
	CSA_return_code	stat = CSA_SUCCESS;
	int		i, j;
	cms_attribute	*attrs;

	if (dstsize == NULL || dstattrs == NULL)
		return (CSA_E_INVALID_PARAMETER);

	*dstsize = 0;
	*dstattrs = NULL;

	if (srcsize == 0)
		return (CSA_SUCCESS);

	if ((attrs = calloc(1, sizeof(cms_attribute) * srcsize)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0, j = 0; i <= srcsize; i++) {
		if (srcattrs[i].value == NULL)
			continue;

		if ((stat = _DtCm_copy_cms_attribute(&attrs[j], &srcattrs[i],
		    B_TRUE)) != CSA_SUCCESS)
			break;
		else {
			if ( (i == CSA_ENTRY_ATTR_SUMMARY_I) && (time_only == B_TRUE) ) {
				if (attrs[j].value && attrs[j].value->item.string_value)
					attrs[j].value->item.string_value[0] = '\0';
			}

			j++;
		}
	}

	if (stat == CSA_SUCCESS && j > 0) {
		*dstsize = j;
		*dstattrs = attrs;
	} else {
		_DtCm_free_cms_attributes(j, attrs);
		free(attrs);
	}

	return (stat);
}


