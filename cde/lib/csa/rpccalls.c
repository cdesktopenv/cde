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
/* $XConsortium: rpccalls.c /main/1 1996/04/21 19:24:20 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*****************************************************************************
 * interface to cm rpc calls
 * - this files make calls to v5 backend
 * - calls to v4 and before are implemented in table.c
 *****************************************************************************/

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>  		/* define MAXHOSTNAMELEN */
#include <sys/utsname.h>	/* SYS_NMLN */
#include <unistd.h>
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include "debug.h"
#include "rtable4.h" 
#include "cm.h"
#include "attr.h"
#include "connection.h"
#include "convert5-4.h"
#include "convert4-5.h"
#include "rpccalls.h"
#include "table.h"
#include "agent.h"
#include "cmcb.h"
#include "cmsdata.h"
#include "iso8601.h"
#include "misc.h"
#include "updateattrs.h"
#include "free.h"
#include "lutil.h"
#include "laccess.h"
#include "match.h"

extern u_long	_DtCm_transient;

static char *nullstr = "";

/*****************************************************************************
 * forward declaration of static functions
 *****************************************************************************/
static CSA_return_code csa2cmsattrs(CSA_uint32 num_attrs, CSA_attribute *csaattrs,
					CSA_enum *ops, CSA_uint32 *newnum,
					cms_attribute **newattrs, CSA_enum **newops);
static void free_cmsattrs(CSA_uint32 num_attrs, cms_attribute *attrs);
static CSA_return_code csa2cmsnames(CSA_uint32 num_names, char **reminder_names,
					CSA_uint32 *newnum, cms_attr_name **newnames);
static CSA_return_code _GetV4UserAccess(Calendar *cal, cms_access_entry *alist);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_return_code
_DtCm_rpc_open_calendar(Calendar *cal)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat;

	DP(("rpccalls.c: _DtCm_rpc_open_calendar\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_open_args	arg;
		cms_open_res	*res;

		arg.cal = cal->name;
		arg.pid = getpid();

		res = cms_open_calendar_5(&arg, &cal->conn);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				cal->rpc_version = res->svr_vers;
				cal->file_version = res->file_vers;
				cal->access = res->user_access;
			}

			xdr_free((xdrproc_t)xdr_cms_open_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else {
		_DtCm_libentry		*entries;
		cms_access_entry	*alist;

		cal->rpc_version = ci->vers_out;
		cal->file_version = ci->vers_out - 1;
 
		/*
		 * call lookup_range to check the existence of
		 * the calendar
		 */
		if ((stat = _DtCm_table_lookup_range(cal, 0, 0, B_TRUE, 0, 0,
		    0, 0, NULL, NULL, &entries)) == CSA_SUCCESS) {
			if (entries) _DtCm_free_libentries(entries);

			if ((stat = _DtCm_table_get_access(cal, &alist))
			    == CSA_SUCCESS) {
				stat = _GetV4UserAccess(cal, alist);
				_DtCm_free_cms_access_entry(alist);
			}
		}
	}
	return (stat);
}

extern CSA_return_code
_DtCm_rpc_create_calendar(
	Calendar *cal,
	CSA_uint32 num_attrs,
	CSA_attribute * attrs)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_create_calendar\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		cms_create_args args;
		CSA_return_code *res;
		CSA_uint32	num_cmsattrs;
		cms_attribute	*cmsattrs;

		if ((stat = csa2cmsattrs(num_attrs, attrs, NULL, &num_cmsattrs,
		    &cmsattrs, NULL)) != CSA_SUCCESS)
			return (stat);

		args.cal	= cal->name;
		args.char_set	= nullstr;
		args.pid	= getpid();
		args.num_attrs	= num_cmsattrs;
		args.attrs	= cmsattrs;

		res = cms_create_calendar_5(&args, &cal->conn);

		if (num_cmsattrs) free_cmsattrs(num_cmsattrs, cmsattrs);

		if (res != NULL) {
			stat = *res;
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else if (ci->vers_out == TABLEVERS_4) {

		if ((stat = _DtCm_table_create(cal))
		    == CSA_SUCCESS && num_attrs > 0) {

			int	i;

			/*
			 * for old backends, the only settable
			 * calendar attribute is access list
			 */
			for (i = num_attrs - 1; i >= 0; i--) {
				if (attrs[i].name)
					break;
			}
			if (attrs[i].value == NULL)
				stat = _DtCm_table_set_access(cal, NULL);
			else
				stat = _DtCm_table_set_access(cal,
					attrs[i].value->item.access_list_value);
		}

	} else if (ci->vers_out < TABLEVERS_4) {
		_DtCm_libentry		*entries;
		char			*ptr, *owner;
		boolean_t		isuser;

		/* check to make sure user is not creating a calendar
		 * using another user's name
		 */
		if (ptr = strchr(cal->name, '@')) *ptr = '\0';
		if (_DtCmIsUserName(cal->name) == B_TRUE) {

			if ((owner = _DtCmGetUserName()) == NULL)
				stat = CSA_E_FAILURE;

			if (strcmp(cal->name, owner))
				stat = CSA_E_NO_AUTHORITY;
		} else
			stat = CSA_E_INVALID_PARAMETER;
		if (ptr) *ptr = '@';

		/*
		 * for v2 and v3 servers, calling a lookup routine has
		 * the side effect of having a calendar created
		 * the calendar
		 */
		if (stat == CSA_SUCCESS &&
		    (stat = _DtCm_table_lookup_range(cal, 0, 0, B_TRUE, 0, 0,
		    0, 0, NULL, NULL, &entries)) == CSA_SUCCESS) {
			if (entries) _DtCm_free_libentries(entries);
		}
	}
	return (stat);
}

extern CSA_return_code
_DtCm_rpc_delete_calendar(Calendar *cal)
{
	CSA_return_code		stat;
	_DtCm_Client_Info	*ci;

	DP(("rpccalls.c: _DtCm_rpc_delete_calendar\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}

	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		cms_remove_args	arg;
		CSA_return_code	*res;

		arg.cal = cal->name;
		arg.pid = getpid();

		res = cms_remove_calendar_5(&arg, &cal->conn);

		if (res != NULL) {
			stat = *res;
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else
		stat = CSA_E_NOT_SUPPORTED;

	return (stat);
}

extern CSA_return_code
_DtCm_rpc_register_client(Calendar *cal, unsigned long update_type)
{
	CSA_return_code	stat;
	_DtCm_Client_Info *ci = NULL;

	DP(("rpccalls.c: _DtCm_rpc_register_client\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}

	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	return (_DtCm_do_registration(&cal->conn, cal->name, update_type));
}

extern CSA_return_code
_DtCm_rpc_unregister_client(Calendar *cal, unsigned long update_type)
{
	CSA_return_code	stat;
	_DtCm_Client_Info *ci = NULL;

	DP(("rpccalls.c: _DtCm_rpc_unregister_client\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}

	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	return (_DtCm_do_unregistration(&cal->conn, cal->name, update_type));
}

extern CSA_return_code
_DtCm_rpc_lookup_reminder(
	Calendar *cal,
	time_t tick,
	CSA_uint32 num_names,
	char **reminder_names,
	CSA_uint32 *num_rems,
	CSA_reminder_reference **rems)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_lookup_reminder\n"));

	if (cal == NULL || num_rems == 0 || rems == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_reminder_args	args;
		cms_reminder_res	*res;
		CSA_uint32	num_cmsnames;
		cms_attr_name	*cmsnames;

		if ((stat = csa2cmsnames(num_names, reminder_names,
		    &num_cmsnames, &cmsnames)) != CSA_SUCCESS)
			return (stat);

		args.cal	= cal->name;
		args.tick	= tick;
		args.num_names	= num_cmsnames;
		args.names	= cmsnames;

		res = cms_lookup_reminder_5(&args, &cal->conn);

		if (cmsnames)
			free(cmsnames);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS)
				stat = _DtCm_cms2csa_reminder_ref(res->rems,
					num_rems, rems);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else {
 
		stat = _DtCm_table_lookup_reminder(cal, num_names,
			reminder_names, num_rems, rems);
	}

        return(stat);
}

/*
 * Lookup calendar entries.
 * If it's an old server, will try to match the attributes in the library;
 * but if attrlist contains attributes not supported by the old backend,
 * will just return CSA_E_UNSUPPORTED_ATTRIBUTE.
 */
extern CSA_return_code
_DtCm_rpc_lookup_entries(
	Calendar *cal,
	CSA_uint32 num_attrs,
	CSA_attribute * attrs,
	CSA_enum *ops,
	_DtCm_libentry **elist_r)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_lookup_entries\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	*elist_r = NULL;

	if (ci->vers_out == TABLEVERS) {
		cms_lookup_entries_args	args;
		cms_entries_res		*res;
		CSA_uint32		num_cmsattrs;
		cms_attribute		*cmsattrs;
		CSA_enum		*newops;

		if ((stat = csa2cmsattrs(num_attrs, attrs, ops, &num_cmsattrs,
		    &cmsattrs, &newops)) != CSA_SUCCESS)
			return (stat);

		args.cal = cal->name;
		args.char_set = nullstr;
		args.num_attrs = num_cmsattrs;
		args.attrs = cmsattrs;
		args.ops = newops;

		res = cms_lookup_entries_5(&args, &cal->conn);

		free_cmsattrs(num_cmsattrs, cmsattrs);
		if (newops) free(newops);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				stat = _DtCmCmsentriesToLibentries(
					&cal->entry_tbl, res->entries, elist_r);
			}

			xdr_free((xdrproc_t)xdr_cms_entries_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else {
		time_t		start1, start2, end1, end2;
		long		id;
		boolean_t	no_start_time_range;
		boolean_t	no_end_time_range;
		boolean_t	no_match;
		CSA_uint32	hnum;
		cms_attribute	*hattrs;
		CSA_enum	*hops;
		cms_key		key;
		_DtCm_libentry	*entry;
 
		/* hash the matching criteria */
		if ((stat = _DtCmHashCriteria(cal->entry_tbl, num_attrs, attrs,
		    NULL, ops, &no_match, &no_start_time_range,
		    &no_end_time_range, &start1, &start2, &end1, &end2, &id,
		    &hnum, &hattrs, &hops)) == CSA_E_INVALID_ATTRIBUTE)
		{
			return (CSA_SUCCESS);
		} else if (stat != CSA_SUCCESS || no_match == B_TRUE) {
			return (stat);
		}

		if (id > 0 && no_start_time_range && no_end_time_range) {
			/* get the key for the first event */
			key.id = 0;
			if ((stat = _DtCm_table_lookup_key_range(cal, NULL, id,
			    _DtCM_BOT, _DtCM_EOT, &key, NULL)) == CSA_SUCCESS &&
			    key.id > 0) {

				/* get the detail */
				if ((stat = _DtCm_make_libentry(NULL, &entry))
				    == CSA_SUCCESS) {
					entry->e->key = key;

					if ((stat = _DtCm_table_lookup(cal,
					    entry)) == CSA_SUCCESS)
						*elist_r = entry;
					else
						_DtCm_free_libentries(entry);
				}
			}
		} else {
			stat = _DtCm_table_lookup_range(cal, start1, start2,
				no_end_time_range, end1, end2,
				id, hnum, hattrs, hops, elist_r);
		}

		_DtCmFreeHashedArrays(hnum, hattrs, hops);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_enumerate_sequence(
	Calendar *cal,
	_DtCm_libentry *entry,
	time_t start,
	time_t end,
	_DtCm_libentry **elist_r)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat = CSA_SUCCESS;
	int		i;

	DP(("rpccalls.c: _DtCm_rpc_enumerate_sequence\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	*elist_r = NULL;

	if (ci->vers_out == TABLEVERS) {
		cms_enumerate_args	args;
		cms_entries_res		*res;

		args.cal = cal->name;
		args.id = entry->e->key.id;
		args.start = start;
		args.end = end;

		res = cms_enumerate_sequence_5(&args, &cal->conn);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				stat = _DtCmCmsentriesToLibentries(
					&cal->entry_tbl, res->entries, elist_r);
			}

			xdr_free((xdrproc_t)xdr_cms_entries_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
 
		stat = _DtCm_table_lookup_key_range(cal, entry, 0, start, end,
			NULL, elist_r);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_lookup_entry_by_id(Calendar *cal, _DtCm_libentry *entry)
{
	_DtCm_Client_Info	*ci=NULL;
	CSA_return_code	stat = CSA_SUCCESS;
	int		i;

	DP(("rpccalls.c: _DtCm_rpc_lookup_entry_by_id\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location,
	    (cal->file_version < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION ?
	    TABLEVERS_4 : TABLEVERS), _DtCM_LONG_TIMEOUT, &ci))
	    != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_get_entry_attr_args	args;
		cms_get_entry_attr_res	*res;

		/*
		 * the entry reference id is filled in already
		 * otherwise, we would not be in this routine
		 */
		args.cal = cal->name;
		args.num_keys = 1;
		args.keys = &entry->e->key;
		args.num_names = 0;
		args.names = NULL;

		res = cms_get_entry_attr_5(&args, &cal->conn);

		if (res != NULL) {
			if (res->entries) {
				if ((stat = res->entries->stat) == CSA_SUCCESS)
				{
					if ((stat = _DtCmUpdateAttributes(
					    res->entries->num_attrs,
					    res->entries->attrs,
					    &entry->e->num_attrs,
					    &entry->e->attrs,
					    &cal->entry_tbl, B_FALSE,
					    NULL, B_TRUE)) == CSA_SUCCESS)
					{
						entry->filled = B_TRUE;
					}
				}
			} else
				stat = res->stat;
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

		if (res) xdr_free((xdrproc_t)xdr_cms_get_entry_attr_res, (char *)res);
	} else {
 
		stat = _DtCm_table_lookup(cal, entry);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_get_cal_attrs(Calendar *cal, int index, CSA_uint32 num_names, char **names)
{
	_DtCm_Client_Info	*ci;
	CSA_return_code		stat = CSA_SUCCESS;
	cms_attribute_value	val;

	DP(("rpccalls.c: _DtCm_rpc_get_cal_attrs\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

        if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}

	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_get_cal_attr_args	arg;
		cms_get_cal_attr_res	*res;
		CSA_uint32		num_cmsnames;
		cms_attr_name		*cmsnames;

		if ((stat = csa2cmsnames(num_names, names,
		    &num_cmsnames, &cmsnames)) != CSA_SUCCESS)
			return (stat);

		arg.cal = cal->name;
		arg.num_names = num_cmsnames;
		arg.names = cmsnames;

		res = cms_get_calendar_attr_5(&arg, &cal->conn);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				if ((stat = _DtCmUpdateAttributes(
				    res->num_attrs, res->attrs, &cal->num_attrs,
				    &cal->attrs, &cal->cal_tbl, B_TRUE, NULL,
				    B_TRUE)) == CSA_SUCCESS)
					cal->got_attrs = B_TRUE;
			}

			xdr_free((xdrproc_t)xdr_cms_get_cal_attr_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}
	} else {

		switch (index) {
		case CSA_CAL_ATTR_ACCESS_LIST_I:
			if ((stat = _DtCm_table_get_access(cal,
			    &val.item.access_list_value)) == CSA_SUCCESS) {
				val.type = CSA_VALUE_ACCESS_LIST;
				stat = _DtCmUpdateAccessListAttrVal(&val,
					&cal->attrs\
					[CSA_CAL_ATTR_ACCESS_LIST_I].value);
				_DtCm_free_cms_access_entry(val.item.\
					access_list_value);
			}
			break;

		case CSA_CAL_ATTR_NUMBER_ENTRIES_I:
			if ((stat = _DtCm_table_size(cal,
			    (int*)&val.item.uint32_value)) == CSA_SUCCESS) {
				val.type = CSA_VALUE_UINT32;
				stat = _DtCmUpdateSint32AttrVal(&val,
					&cal->attrs\
					[CSA_CAL_ATTR_NUMBER_ENTRIES_I].value);
			}
			break;

		}
	}
        return(stat);
}

extern CSA_return_code
_DtCm_rpc_set_cal_attrs(Calendar *cal, CSA_uint32 num_attrs, CSA_attribute * attrs)
{
	_DtCm_Client_Info	*ci;
	CSA_return_code	stat = CSA_SUCCESS;
	int		i;

	DP(("rpccalls.c: _DtCm_rpc_set_cal_attrs\n"));

	if (cal == NULL || attrs == NULL)
		return (CSA_E_INVALID_PARAMETER);

        if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS)
	{
		return (stat);
	}

	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		CSA_return_code 	*res;
		cms_set_cal_attr_args	args;
		CSA_uint32	num_cmsattrs;
		cms_attribute	*cmsattrs;

		if ((stat = csa2cmsattrs(num_attrs, attrs, NULL, &num_cmsattrs,
		    &cmsattrs, NULL)) != CSA_SUCCESS)
			return (stat);

		if (num_cmsattrs == 0)
			return (CSA_E_INVALID_PARAMETER);

		args.cal = cal->name;
		args.pid = getpid();
		args.num_attrs = num_cmsattrs;
		args.attrs = cmsattrs;

		res = cms_set_calendar_attr_5(&args, &cal->conn);

		free_cmsattrs(num_cmsattrs, cmsattrs);

		if (res != NULL) {
			stat = *res;
			if (stat == CSA_SUCCESS) {
				_DtCm_free_cms_attribute_values(cal->num_attrs,
					cal->attrs);
			}
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
		int	i;

		/*
		 * CSA_CAL_ATTR_ACCESS_LIST is the only settable attribute
		 */
		for (i = num_attrs - 1; i >= 0; i--) {
			if (attrs[i].name)
				break;
		}
		if (attrs[i].value == NULL)
			stat = _DtCm_table_set_access(cal, NULL);
		else
			stat = _DtCm_table_set_access(cal,
				attrs[i].value->item.access_list_value);
	}

	return (stat);
}

extern CSA_return_code
_DtCm_rpc_insert_entry(
	Calendar *cal,
	CSA_uint32 num_attrs,
	CSA_attribute * attrs,
	_DtCm_libentry **entry_r)
{
	_DtCm_Client_Info	*ci = NULL;
	CSA_return_code		stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_insert_entry;\n"));

	if (cal == NULL || num_attrs == 0 || attrs == NULL || entry_r == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS)
	{
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		cms_entry_res 	*res;
		cms_insert_args	args;
		CSA_uint32	num_cmsattrs;
		cms_attribute	*cmsattrs;

		if ((stat = csa2cmsattrs(num_attrs, attrs, NULL, &num_cmsattrs,
		    &cmsattrs, NULL)) != CSA_SUCCESS)
			return (stat);

		if (num_cmsattrs == 0)
			return (CSA_E_INVALID_PARAMETER);

		args.cal = cal->name;
		args.pid = getpid();
		args.num_attrs = num_cmsattrs;
		args.attrs = cmsattrs;

		res = cms_insert_entry_5(&args, &cal->conn);

		free_cmsattrs(num_cmsattrs, cmsattrs);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				stat = _DtCmCmsentriesToLibentries(
					&cal->entry_tbl, res->entry, entry_r);
			}

			xdr_free((xdrproc_t)xdr_cms_entry_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
 
		stat = _DtCm_table_insert(cal, num_attrs, attrs, entry_r);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_update_entry(
	Calendar *cal,
	_DtCm_libentry *oentry,
	CSA_uint32 num_attrs,
	CSA_attribute * attrs,
	CSA_enum scope,
	_DtCm_libentry **nentry)
{
	_DtCm_Client_Info	*ci = NULL;
	CSA_return_code	stat = CSA_SUCCESS;
	_DtCm_libentry	*hptr = NULL, *tptr, *cptr;
	int i;

	DP(("rpccalls.c: _DtCm_rpc_update_entry\n"));

	if (cal == NULL || oentry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS)
	{
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		cms_entry_res 	*res;
		cms_update_args	args;
		CSA_uint32	num_cmsattrs;
		cms_attribute	*cmsattrs;

		if ((stat = csa2cmsattrs(num_attrs, attrs, NULL, &num_cmsattrs,
		    &cmsattrs, NULL)) != CSA_SUCCESS)
			return (stat);

		if (num_cmsattrs == 0)
			return (CSA_E_INVALID_PARAMETER);

		args.cal = cal->name;
		args.pid = getpid();
		args.entry = oentry->e->key;
		args.scope = scope;
		args.num_attrs = num_cmsattrs;
		args.attrs = cmsattrs;

		res = cms_update_entry_5(&args, &cal->conn);

		free_cmsattrs(num_cmsattrs, cmsattrs);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS) {
				stat = _DtCmCmsentriesToLibentries(
					&cal->entry_tbl, res->entry, nentry);
			}

			xdr_free((xdrproc_t)xdr_cms_entry_res, (char *)res);
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
 
		stat = _DtCm_table_update(cal, oentry, num_attrs, attrs,
				scope, nentry);
	}

        return(stat);

}

extern CSA_return_code
_DtCm_rpc_delete_entry(
	Calendar *cal,
	_DtCm_libentry *entry,
	CSA_enum scope)
{
	_DtCm_Client_Info	*ci = NULL;
	CSA_return_code	stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_delete_entry\n"));

	if (cal == NULL || entry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS)
	{
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_FALSE;

	if (ci->vers_out == TABLEVERS) {
		CSA_return_code	*res;
		cms_delete_args	args;

		args.cal = cal->name;
		args.pid = getpid();
		args.entry = entry->e->key;
		args.scope = scope;

		res = cms_delete_entry_5(&args, &cal->conn);

		if (res != NULL) {
			stat = *res;
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
 
		stat = _DtCm_table_delete(cal, entry, scope);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_list_calendar_attributes(
	Calendar	*cal,
	CSA_uint32	*number_names,
	char		***names_r)
{
	_DtCm_Client_Info	*ci = NULL;
	CSA_return_code	stat = CSA_SUCCESS;

	DP(("rpccalls.c: _DtCm_rpc_list_calendar_attributes\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_create_tcp_client(cal->location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	cal->conn.ci = ci;
	cal->conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_enumerate_calendar_attr_res	*res;
		char				**names;
		int				i;

		*number_names = 0;
		*names_r = NULL;

		res = cms_enumerate_calendar_attr_5(&cal->name, &cal->conn);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS && res->num_names)
			{
				if (names = _DtCm_alloc_character_pointers(
				    res->num_names)) {
					for (i = 0; i < res->num_names; i++) {
						if ((names[i] = strdup(
						    res->names[i].name))
						    == NULL) {
							_DtCm_free(names);
							return (CSA_E_INSUFFICIENT_MEMORY);
						}
					}
					*number_names = res->num_names;
					*names_r = names;
				}
			}
		} else {
			stat = (cal->conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(cal->conn.stat);
		}

	} else {
 
		stat = CSA_E_NOT_SUPPORTED;
	}

        return(stat);
}

extern CSA_return_code
_DtCm_rpc_list_calendars(
	char			*location,
	CSA_uint32		*number_names,
	CSA_calendar_user	**names_r)
{
	_DtCm_Client_Info	*ci = NULL;
	CSA_return_code	stat = CSA_SUCCESS;
	_DtCm_Connection	conn;

	DP(("rpccalls.c: _DtCm_rpc_list_calendars\n"));

	if ((stat = _DtCm_create_tcp_client(location, TABLEVERS,
	    _DtCM_LONG_TIMEOUT, &ci)) != CSA_SUCCESS) {
		return (stat);
	}
	conn.ci = ci;
	conn.retry = B_TRUE;

	if (ci->vers_out == TABLEVERS) {
		cms_list_calendars_res	*res;
		CSA_calendar_user	*names;
		int			i;

		*number_names = 0;
		*names_r = NULL;

		res = cms_list_calendars_5(NULL, &conn);

		if (res != NULL) {
			if ((stat = res->stat) == CSA_SUCCESS && res->num_names)
			{
				if (names = _DtCm_alloc_calendar_users(
				    res->num_names)) {
					for (i = 0; i < res->num_names; i++) {
						if ((names[i].calendar_address =
						    strdup(res->names[i]))
						    == NULL) {
							_DtCm_free(names);
							return (CSA_E_INSUFFICIENT_MEMORY);
						}
					}
					*number_names = res->num_names;
					*names_r = names;
				}
			}
		} else {
			stat = (conn.stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(conn.stat);
		}

	} else {
 
		stat = CSA_E_NOT_SUPPORTED;
	}

        return(stat);
}

extern CSA_return_code
_DtCm_do_unregistration(
	_DtCm_Connection *conn,
	char *cal,
	unsigned long update_type)
{
	CSA_return_code	stat;

	if (conn->ci->vers_out == TABLEVERS) {
		cms_register_args args;
		CSA_return_code *res;

		args.cal = cal;
		args.update_type = update_type;
		args.prognum = _DtCm_transient;
		args.versnum = AGENTVERS_2;
		args.procnum = update_callback;
		args.pid = getpid();

		res = cms_unregister_5(&args, conn);

		if (res != NULL) {
			stat = *res;
		} else {
			stat = (conn->stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(conn->stat);
		}
	} else {
		stat = _DtCm_table_unregister_target(conn, cal);
	}

	if (stat == CSA_SUCCESS)
		_DtCm_remove_registration(conn->ci, cal);

	return (stat);
}

extern CSA_return_code
_DtCm_do_registration(
	_DtCm_Connection *conn,
	char *cal,
	unsigned long update_type)
{
	CSA_return_code	stat;

	if (conn->ci->vers_out == TABLEVERS) {
		cms_register_args args;
		CSA_return_code *res;

		args.cal = cal;
		args.update_type = update_type;
		args.prognum = _DtCm_transient;
		args.versnum = AGENTVERS_2;
		args.procnum = update_callback;
		args.pid = getpid();

		res = cms_register_5(&args, conn);

		if (res != NULL)
			stat = *res;
		else {
			stat = (conn->stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(conn->stat);
		}
	} else {

		stat = _DtCm_table_register_target(conn, cal);
	}

	if (stat == CSA_SUCCESS) {
		if ((stat = _DtCm_add_registration(conn->ci, cal, update_type))
		    != CSA_SUCCESS) {
			(void) _DtCm_do_unregistration(conn, cal, update_type);
		}
	}

	return (stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

/*
 * Newnum is initialized to 0 and newattrs to NULL when num_attrs == 0
 */
static CSA_return_code
csa2cmsattrs(
	CSA_uint32	num_attrs,
	CSA_attribute	*csaattrs,
	CSA_enum	*ops,
	CSA_uint32	*newnum,
	cms_attribute	**newattrs,
	CSA_enum	**newops)
{
	CSA_return_code	stat = CSA_SUCCESS;
	cms_attribute	*cmsattrs;
	CSA_enum	*ops_r;
	CSA_reminder	*rptr1, *rptr2;
	int		i,j;

	*newnum = 0;
	*newattrs = NULL;
	if (newops) *newops = NULL;

	if (num_attrs == 0)
		return CSA_SUCCESS;

	if ((cmsattrs = calloc(1, sizeof(cms_attribute)*num_attrs)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (newops && (ops_r = malloc(sizeof(CSA_enum)*num_attrs)) == NULL) {
		free(cmsattrs);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	for (i = 0, j = 0; i < num_attrs && stat == CSA_SUCCESS; i++) {
		if (csaattrs[i].name == NULL)
			continue;

		if (newops)
			ops_r[j] = (ops ? ops[i] : CSA_MATCH_EQUAL_TO);

		cmsattrs[j].name.name = csaattrs[i].name;
		if (csaattrs[i].value == NULL) {
			j++;
			continue;
		}

		if ((cmsattrs[j].value = (cms_attribute_value *)malloc(
		    sizeof(cms_attribute_value))) == NULL) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		}

		cmsattrs[j].value->type = csaattrs[i].value->type;

		switch (csaattrs[i].value->type) {
		case CSA_VALUE_ACCESS_LIST:
			stat = _DtCm_csa2cms_access_list(
				csaattrs[i].value->item.access_list_value,
				&cmsattrs[j].value->item.access_list_value);
			break;
		case CSA_VALUE_CALENDAR_USER:
			if (csaattrs[i].value->item.calendar_user_value &&
			    csaattrs[i].value->item.calendar_user_value->\
			    user_name)
			{
				cmsattrs[j].value->item.calendar_user_value =
			    		csaattrs[i].value->item.\
					calendar_user_value->user_name;
			} else {
				cmsattrs[j].value->item.calendar_user_value =
					nullstr;
			}
			break;
		case CSA_VALUE_STRING:
		case CSA_VALUE_DATE_TIME:
		case CSA_VALUE_DATE_TIME_RANGE:
		case CSA_VALUE_TIME_DURATION:
			if (csaattrs[i].value->item.string_value) {
				cmsattrs[j].value->item.string_value =
					csaattrs[i].value->item.string_value;
			} else {
				cmsattrs[j].value->item.string_value = nullstr;
			}
			break;
		case CSA_VALUE_REMINDER:
			if ((rptr1 = csaattrs[i].value->item.reminder_value) &&
			    (rptr2 = (CSA_reminder *)malloc(sizeof(CSA_reminder))))
			{
				rptr2->lead_time = rptr1->lead_time ?
						    rptr1->lead_time : nullstr;
				rptr2->snooze_time = rptr1->snooze_time ?
						     rptr1->snooze_time:nullstr;
				rptr2->repeat_count = rptr1->repeat_count;
				rptr2->reminder_data = rptr1->reminder_data;
				cmsattrs[j].value->item.reminder_value = rptr2;
			} else if (rptr1 && rptr2 == NULL)
				stat = CSA_E_INSUFFICIENT_MEMORY;
			break;
		default:
			/* all other value types uses the same type
			 * of data, so we just copy the value/address of
			 * of the data
			 */
			cmsattrs[j].value->item.sint32_value =
				csaattrs[i].value->item.sint32_value;
		}
		j++;
	}

	if (stat != CSA_SUCCESS) {

		free_cmsattrs(num_attrs, cmsattrs);
		if (newops) free(ops_r);

	} else if (j > 0) {
		*newnum = j;
		*newattrs = cmsattrs;
		if (newops) *newops = ops_r;
	} else {
		free(cmsattrs);
		if (newops) free(ops_r);
	}

	return (stat);
}

static void
free_cmsattrs(CSA_uint32 num_attrs, cms_attribute *attrs)
{
	int i;

	for (i = 0; i < num_attrs; i++) {
		if (attrs[i].value == NULL)
			continue;

		switch (attrs[i].value->type) {
		case CSA_VALUE_ACCESS_LIST:
			_DtCm_free_cms_access_entry(
				attrs[i].value->item.access_list_value);
			break;
		case CSA_VALUE_REMINDER:
			if (attrs[i].value->item.reminder_value)
				free(attrs[i].value->item.reminder_value);
			break;
		}

		free(attrs[i].value);
	}
	free (attrs);
}

/*
 * Newnum is initialized to 0 and newattrs to NULL when num_attrs == 0
 */
static CSA_return_code
csa2cmsnames(
	CSA_uint32	num_names,
	char		**reminder_names,
	CSA_uint32	*newnum,
	cms_attr_name	**newnames)
{
	cms_attr_name	*cmsnames;
	int		i,j;

	*newnum = 0;
	*newnames = NULL;

	if (num_names == 0)
		return (CSA_SUCCESS);

	if ((cmsnames = calloc(1, sizeof(cms_attr_name)*num_names)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	for (i = 0, j = 0; i < num_names; i++) {
		if (reminder_names[i] == NULL)
			continue;
		else {
			cmsnames[j].name = reminder_names[i];
			j++;
		}
	}

	if (j > 0) {
		*newnum = j;
		*newnames = cmsnames;
	} else
		free(cmsnames);

	return (CSA_SUCCESS);
}

/*
 * It is an old server and so we need to determine whether the user
 * is the calendar owner ourselves.
 * If the calendar name is the same as a user name, then that user
 * is the owner, and we just need to make sure the calendar is located
 * in the same domain as ours.  If the calendar is located in another
 * domain and since we can't do cross domain authentication yet, to be save
 * we assume that the user is not the owner.
 * If the calendar name is not a user name, we need to make an rpc call
 * (table_set_access) to test whether the user is the owner.
 */
static CSA_return_code
_GetV4UserAccess(Calendar *cal, cms_access_entry *alist)
{
	CSA_return_code	stat = CSA_SUCCESS;
	char	*user = _DtCmGetUserName();
	char	*localhost = _DtCmGetHostAtDomain();
	char	buf[BUFSIZ];
	char	*ptr, *dom;
	int	worldaccess = 0, useraccess = 0;
	boolean_t isowner = B_FALSE;

	/* first check if user is owner */
	if (ptr = strchr(cal->name, '@')) *ptr = '\0';

	if (_DtCmIsUserName(cal->name) == B_TRUE) {
		if (strcmp(user, cal->name) == 0) {
			/* check whether calendar location is in
			 * the same domain
			 */
			if (dom = strchr(cal->location, '.')) dom++;
			if (dom == NULL || _DtCmIsSamePath(
			    _DtCmGetLocalDomain(NULL), dom) == B_TRUE)
				isowner = B_TRUE;
		}
	} else {
		CSA_access_rights	*csalist;

		/* need to test whether the user is the owner */
		if ((stat = _DtCm_cms2csa_access_list(alist, &csalist))
		    == CSA_SUCCESS) {

			stat = _DtCm_table_set_access(cal, csalist);
			_DtCm_free_csa_access_list(csalist);

			if (stat == CSA_SUCCESS)
				isowner = B_TRUE;
			else if (stat == CSA_E_NO_AUTHORITY) {
				isowner = B_FALSE;
				stat = CSA_SUCCESS;
			}
		}
	}
	if (ptr) *ptr = '@';
	if (stat != CSA_SUCCESS)
		return (stat);

	if (isowner) {
		cal->access = CSA_OWNER_RIGHTS;
		return (CSA_SUCCESS);
	}

	snprintf(buf, sizeof buf, "%s@%s", user, localhost);
	for (; alist != NULL; alist = alist->next) {
		if (strcasecmp(alist->user, "world") == 0)
			worldaccess = alist->rights;
		else if (_DtCmIsSameUser(buf, alist->user)) {
			useraccess = alist->rights;
			break;
		}
	}

	cal->access = useraccess | worldaccess;
	return (CSA_SUCCESS);
}

