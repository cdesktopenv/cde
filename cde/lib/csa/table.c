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
/* $XConsortium: table.c /main/1 1996/04/21 19:24:46 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "entry.h"
#include "appt4.h"
#include "attr.h"
#include "rtable2.h"
#include "rtable3.h"
#include "rtable4.h" 
#include "convert4-2.h"
#include "convert2-4.h"
#include "convert4-3.h"
#include "convert3-4.h"
#include "convert4-5.h"
#include "convert5-4.h"
#include "connection.h"
#include "debug.h"
#include "misc.h"
#include "agent.h"
#include "nametbl.h"
#include "match.h"
#include "iso8601.h"

extern u_long	_DtCm_transient;

#define MAXDAY_LOOKUP		15
#define MAXDAY_LOOKUP_SECS	((DAYSEC*MAXDAY_LOOKUP)+1)

static char *errfmt = "cm: %s() unexpected return status %d.\n";

/*****************************************************************************
 * forward declaration of static functions
 *****************************************************************************/
static CSA_return_code tablestat_to_dtcmstat(
			Table_Status_4 stat,
			CSA_return_code *dtcmstat);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_return_code
_DtCm_table_create(Calendar *cal)
{
        CSA_return_code	stat;
        Table_Status_4	*res = NULL;
	_DtCm_Connection	*conn;

	DP(("table.c: _DtCm_table_create\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	conn = &cal->conn;
	conn->retry = B_TRUE;

	switch(conn->ci->vers_out) {
                Table_Op_Args_4 a4;
        case TABLEVERS_2:
        case TABLEVERS_3:
		return(CSA_E_NOT_SUPPORTED);
	case TABLEVERS_4:
                a4.target = cal->name;
		a4.new_target = calloc(1, 1);
                res = _DtCm_rtable_create_4(&a4, conn);
		free(a4.new_target);
                break;
        default:
		return(CSA_E_FAILURE);
        }
	if (res != NULL) {
		if (tablestat_to_dtcmstat(*res, &stat) != CSA_SUCCESS)
			DP((errfmt, "table.c:", "_DtCm_table_create", *res));
        } else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_table_get_access(Calendar *cal, cms_access_entry **alist)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Access_Args_4	*res = NULL;
	_DtCm_Connection	*conn;

	DP(("table.c: _DtCm_table_get_access\n"));

	if (cal == NULL || alist == NULL)
		return (CSA_E_INVALID_PARAMETER);

	*alist = NULL;

	conn = &cal->conn;

        switch(conn->ci->vers_out) {
                Access_Args_2 a2;
                Access_Args_3 a3;
                Access_Args_4 a4;
                Access_Args_2 *res2;
                Access_Args_3 *res3;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.access_list = NULL;
                res2 = _DtCm_rtable_get_access_2(&a2,  conn);
		res = _DtCm_accargs2_to_accargs4(res2);
		if (res2 != NULL)
			 xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_2, (char*)res2);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.access_list = NULL;
                res3 = _DtCm_rtable_get_access_3(&a3,  conn);
		res = _DtCm_accargs3_to_accargs4(res3);
		if (res3 != NULL)
			 xdr_free((xdrproc_t)_DtCm_xdr_Access_Args_3, (char*)res3);
                break;
        case TABLEVERS_4:
                a4.target = cal->name;
                a4.access_list = NULL;
		res = _DtCm_rtable_get_access_4(&a4, conn);
                break;
        default:
                stat = CSA_E_FAILURE;
                break;
        }

	if (res != NULL) {
		stat = _DtCm_accessentry4_to_cmsaccesslist(res->access_list, alist);
		xdr_free ((xdrproc_t)_DtCm_xdr_Access_Args_4, (char*)res);
	} else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_table_set_access(Calendar *cal, CSA_access_list alist)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Access_Status_4	s = access_other_4;
	Access_Status_4	*res;
	_DtCm_Connection	*conn;
	boolean_t	nullreturned = B_FALSE;
	Access_Entry_4	*alist4;

	DP(("table.c: _DtCm_table_set_access\n"));

	if (cal == NULL)
		return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_csaaccesslist_toaccessentry4(alist, &alist4))
	    != CSA_SUCCESS)
		return (stat);

	conn = &cal->conn;
	conn->retry = B_FALSE;

	switch(conn->ci->vers_out) {
		Access_Args_2 a2;
		Access_Args_3 a3;
		Access_Args_4 a4;
		Access_Status_2 *s2;
		Access_Status_3 *s3;
	case TABLEVERS_2:
		a2.target = cal->name;
		a2.access_list = _DtCm_acclist4_to_acclist2(alist4);
		s2 = _DtCm_rtable_set_access_2(&a2,  conn);
		if (s2 != NULL)
			s =  _DtCm_accstat2_to_accstat4(*s2);
		else
			nullreturned = B_TRUE;
		if (a2.access_list != NULL)
			_DtCm_free_access_list4((Access_Entry_4 *)a2.access_list);
		break;
	case TABLEVERS_3:
		a3.target = cal->name;
		a3.access_list = _DtCm_acclist4_to_acclist3(alist4);
		s3 = _DtCm_rtable_set_access_3(&a3,  conn);
		if (s3 != NULL)
			s =  _DtCm_accstat3_to_accstat4(*s3);
		else
			nullreturned = B_TRUE;
		if (a3.access_list != NULL)
			_DtCm_free_access_list4((Access_Entry_4 *)a3.access_list);
		break;
	case TABLEVERS_4:
		a4.target = cal->name;
		a4.access_list = alist4;
		res = _DtCm_rtable_set_access_4(&a4, conn);
		if (res != NULL)
			s = *res;
		else
			nullreturned = B_TRUE;
		break;
	default:
		stat = CSA_E_FAILURE;
		break;
	}

	if (alist4 != NULL)
		_DtCm_free_access_list4(alist4);

	if (nullreturned) {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	} else {
		switch(s) {
		case access_ok_4:
			stat = CSA_SUCCESS;
			break;
		case access_failed_4:
			stat = CSA_E_NO_AUTHORITY;
			break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
		default:
                        DP((errfmt, "table_set_access", s));
			stat = CSA_E_FAILURE;
			break;
		}
	}

        return(stat);
}

extern CSA_return_code
_DtCm_table_lookup(Calendar *cal, _DtCm_libentry *entry)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Uid_4		key;
	Appt_4		*appt;

	DP(("table.c: call to _DtCm_table_lookup\n"));

	if ((cal == NULL) || entry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	key.appt_id.tick = entry->e->key.time;
	key.appt_id.key = entry->e->key.id;
	key.next = NULL;

	conn = &cal->conn;
	conn->retry = B_TRUE;

	switch(conn->ci->vers_out) {
		Table_Args_2 a2;
		Table_Args_3 a3;
		Table_Args_4 a4;
                Table_Res_2 *res2;
		Table_Res_3 *res3;
	case TABLEVERS_2:
		a2.target = cal->name;
        	a2.args.tag = UID_2;
        	a2.args.Args_2_u.key = _DtCm_uid4_to_uid2(&key);
        	res2 = _DtCm_rtable_lookup_2(&a2, conn);
		res = _DtCm_tableres2_to_tableres4(res2);
		if (a2.args.Args_2_u.key != NULL)
			free(a2.args.Args_2_u.key);
		if (res2 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
		break;
	case TABLEVERS_3:
		a3.target = cal->name;
        	a3.args.tag = UID_3;
        	a3.args.Args_3_u.key = _DtCm_uid4_to_uid3(&key);
		a3.pid = getpid();
        	res3 = _DtCm_rtable_lookup_3(&a3, conn);
		res = _DtCm_tableres3_to_tableres4(res3);
		if (a3.args.Args_3_u.key != NULL)
			free(a3.args.Args_3_u.key);
		if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
		break;
	case TABLEVERS_4:
		a4.target = cal->name;
        	a4.args.tag = UID_4;
        	a4.args.Args_4_u.key = &key;
		a4.pid = getpid();
        	res = _DtCm_rtable_lookup_4(&a4, conn);
		break;
	default:
		stat = CSA_E_FAILURE;
		break;
	}

	if (res != NULL) {
        	switch(res->status) {
        	case access_ok_4:
		case access_failed_4:
			if ((appt = res->res.Table_Res_List_4_u.a) == NULL)
				stat = CSA_X_DT_E_ENTRY_NOT_FOUND |
					CSA_E_INVALID_ENTRY_HANDLE;
			else {
				if (stat == CSA_SUCCESS) {
					if (entry->filled == B_TRUE)
						_DtCm_free_cms_attribute_values(
						    entry->e->num_attrs,
						    entry->e->attrs);
					if ((stat = _DtCm_appt4_to_attrs(
					    cal->name, appt,
					    entry->e->num_attrs,
					    entry->e->attrs, B_FALSE))
					    == CSA_SUCCESS)
						entry->filled = B_TRUE;
				}
			}
			break;
		case access_other_4:
			stat = CSA_E_FAILURE;
			break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
        	default:
                	/* remote system error */
                       	DP((errfmt, "_DtCm_table_lookup", res->status));
			stat = CSA_E_FAILURE;
			break;
        	}
		xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);
	} else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	}

	return(stat);
}

extern CSA_return_code
_DtCm_table_lookup_range(
	Calendar	*cal,
	long		start1,
	long		start2,
	boolean_t	no_end_time_range,
	long		end1,
	long		end2,
	long		id,
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_enum	*ops,
	_DtCm_libentry	**entries)
{
        CSA_return_code	stat = CSA_SUCCESS;
        Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Range_4		new_range, old_range;
	Appt_4		*appts = NULL, *lptr = NULL, *tptr = NULL, *rptr = NULL;

	DP(("table.c: _DtCm_table_lookup_range\n"));

        if (cal == NULL || entries == NULL)
                return (CSA_E_INVALID_PARAMETER);

	*entries = NULL;

	old_range.key1 = start1;
	old_range.key2 = start2;
 
	new_range.key1 = old_range.key1;
        new_range.key2 = old_range.key2;
        new_range.next = NULL;

	conn = &cal->conn;
	conn->retry = B_TRUE;

        if (conn->use == udp_transport)
                if ((new_range.key1 + MAXDAY_LOOKUP_SECS) < old_range.key2)
                        new_range.key2 = new_range.key1 + MAXDAY_LOOKUP_SECS;

	do {
		switch(conn->ci->vers_out) {
			Table_Args_2 a2;
			Table_Args_3 a3;
			Table_Args_4 a4;
			Table_Res_2 *res2;
			Table_Res_3 *res3;
		case TABLEVERS_2:
			a2.target = cal->name;
			a2.args.tag = RANGE_2;
			a2.args.Args_2_u.range = _DtCm_range4_to_range2(&new_range);
			res2 = _DtCm_rtable_lookup_range_2(&a2, conn);
			res = _DtCm_tableres2_to_tableres4(res2);
			if (a2.args.Args_2_u.range != NULL)
				free(a2.args.Args_2_u.range);
			if (res2 != NULL)
				xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
			break;
		case TABLEVERS_3:
			a3.target = cal->name;
			a3.args.tag = RANGE_3;
			a3.args.Args_3_u.range = _DtCm_range4_to_range3(&new_range);
			a3.pid = getpid();
			res3 = _DtCm_rtable_lookup_range_3(&a3, conn);
			res = _DtCm_tableres3_to_tableres4(res3);
			if (a3.args.Args_3_u.range != NULL)
				free(a3.args.Args_3_u.range);
			if (res3 != NULL)
				xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
			break;
		case TABLEVERS_4:
			a4.target = cal->name;
			a4.args.tag = RANGE_4;
			a4.args.Args_4_u.range = &new_range;
			a4.pid = getpid();
			res = _DtCm_rtable_lookup_range_4(&a4, conn);
			break;
		default:
			stat = CSA_E_FAILURE;
			break;
		}

		if (res != NULL) {
			switch(res->status) {
			case access_ok_4:
			case access_failed_4:
				tptr = _DtCm_copy_appt4(res->res.Table_Res_List_4_u.a);
				if (stat == CSA_SUCCESS) {
					if (conn->use == tcp_transport)
                                        	appts = tptr;
                                	else { /* udp transport */
                                        	if (appts == NULL)
                                                	appts = lptr = tptr;
                                        	else {
                                                	/* traverse from the last res ptr */
                                                	for (rptr = lptr; rptr != NULL && 
								rptr->next != NULL; rptr = rptr->next);
							if (tptr != NULL)
                                                		rptr->next = lptr = tptr;
							else
								lptr = rptr;
                                        	}
                                	}
				}
				break;
			case access_other_4:
				stat = CSA_E_INVALID_PARAMETER;
				break;
			case access_notable_4:
				stat = CSA_E_CALENDAR_NOT_EXIST;
				break;
			default:
				/* remote system error */
				DP ((errfmt, "table_lookup_range", res->status));
				stat = CSA_E_FAILURE;
				break;
			}
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res); res = NULL;

			/* range must encompass interested ticks */
			new_range.key1 = new_range.key2-1;
                        if ((new_range.key2 = 
				(new_range.key1+MAXDAY_LOOKUP_SECS)) >
                                        old_range.key2)
                                new_range.key2 = old_range.key2;
		} else { /* res == NULL */
			stat = (conn->stat == RPC_SUCCESS) ?
				CSA_E_SERVICE_UNAVAILABLE :
				_DtCm_clntstat_to_csastat(conn->stat);
		}
	} while (stat == CSA_SUCCESS && new_range.key1 < old_range.key2 && 
		(new_range.key1+1) != old_range.key2);

	if (stat == CSA_SUCCESS) {
		/*
		 * if attrs are specified, return only those
		 * appts that matches the values
		 */
		if (id > 0 || num_attrs > 0 || !no_end_time_range)
			appts = _DtCm_match_appts(appts, id, no_end_time_range,
				end1, end2, num_attrs, attrs, ops);

		stat = _DtCm_appt4_to_libentries(cal->name, appts, entries);
		_DtCm_free_appt4(appts);

	} else if (appts != NULL) {
		/* this happens if udp is used and some rpc calls succeeded */
                _DtCm_free_appt4(appts);
	}

        return(stat);
}

/*
 * This routine looks up events of a repeating sequence.
 * The caller either specifies entry and elist_r or
 * id and key. 
 * If entry is specified, events are converted to cms_entry structures
 * and returned in elist_r, otherwise; the caller is interested only
 * in the key of the first entry which will be returned in the cms_key
 * structure pointed to by key.
 */
extern CSA_return_code
_DtCm_table_lookup_key_range(
	Calendar	*cal,
	_DtCm_libentry	*entry,
	long		id,
	time_t		start,
	time_t		end,
	cms_key		*key,
	_DtCm_libentry	**elist_r)
{
	CSA_return_code		stat = CSA_SUCCESS;
	Table_Res_4		*res=NULL;
	_DtCm_Connection	*conn;
	Keyrange_4		keyrange;
	Abb_Appt_4		*abbs;
	_DtCm_libentry		*head, *prev, *eptr;
 
	DP(("table.c: _DtCm_table_lookup_key_range\n"));

	if (cal == NULL || (entry == NULL && key == NULL) ||
	   (entry && elist_r == NULL))
                return (CSA_E_INVALID_PARAMETER);
 
	keyrange.next = NULL;
	keyrange.key = (entry ? entry->e->key.id : id);
	keyrange.tick1 = start;
	keyrange.tick2 = end;

	conn = &cal->conn;
	conn->retry = B_TRUE;

        switch(conn->ci->vers_out) {
		Table_Args_3 a3;
		Table_Args_4 a4;
		Table_Res_3 *res3;
        case TABLEVERS_2:
		return (CSA_E_NOT_SUPPORTED);

	case TABLEVERS_3:
		a3.target = cal->name;
		a3.args.tag = KEYRANGE_3;
		a3.args.Args_3_u.keyrange = _DtCm_keyrange4_to_keyrange3(&keyrange);
		a3.pid = getpid();
		res3 = _DtCm_rtable_abbreviated_lookup_key_range_3(&a3, conn);
		res = _DtCm_tableres3_to_tableres4(res3);
		if (a3.args.Args_3_u.keyrange != NULL)
			free(a3.args.Args_3_u.keyrange);
		if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
		break;
        case TABLEVERS_4:
                a4.target = cal->name;
                a4.args.tag = KEYRANGE_4;
                a4.args.Args_4_u.keyrange = &keyrange;
		a4.pid = getpid();
                res = _DtCm_rtable_abbreviated_lookup_key_range_4(&a4, conn);
                break;
        default:
		return (CSA_E_FAILURE);
        }

        if (res != NULL) {
                switch(res->status) {
                case access_ok_4:
                case access_failed_4:
			abbs = res->res.Table_Res_List_4_u.b;
			if (entry == NULL) {
				/* caller is only interested in the first key */
				if (abbs != NULL) {
					key->id = abbs->appt_id.key;
					key->time = abbs->appt_id.tick;
				} else
					key->id = 0;
				break;
			}

			/* we just copy the time from the returned data */
			head = prev = NULL;
			while (abbs != NULL) {
				if ((stat = _DtCm_make_libentry(entry->e,
				    &eptr)) != CSA_SUCCESS)
					break;

				/* change time to the time of the instance */
				eptr->e->key.time = abbs->appt_id.tick;
				_csa_tick_to_iso8601(eptr->e->key.time,
					eptr->e->\
					attrs[CSA_ENTRY_ATTR_START_DATE_I].\
					value->item.string_value);

				if (head == NULL)
					head = eptr;
				else {
					prev->next = eptr;
					eptr->prev = prev;
				}

				prev = eptr;
				abbs = abbs->next;
			}
			if (stat == CSA_SUCCESS)
				*elist_r = head;
			else if (head != NULL)
				_DtCm_free_libentries(head);

                        break;
                case access_other_4:
                        stat = CSA_E_FAILURE;
                        break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
		default:
			/* remote system error */
			DP((errfmt, "_DtCm_table_lookup_key_range", res->status));
                        stat = CSA_E_FAILURE;
                        break;
                }
                xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);
        } else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
        }

        return(stat);
}

extern CSA_return_code
_DtCm_table_insert(
	Calendar *cal,
	uint num_attrs,
	CSA_attribute * attrs,
	_DtCm_libentry **entry_r)
{
        CSA_return_code	stat = CSA_SUCCESS;
        Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Appt_4		*appt4;

	DP(("table.c: _DtCm_table_insert\n"));

        if (cal == NULL || num_attrs == 0 || attrs == NULL || entry_r == NULL)
                return (CSA_E_INVALID_PARAMETER);

	if ((stat = _DtCm_attrs_to_appt4(num_attrs, attrs, &appt4)) != CSA_SUCCESS)
		return (stat);

	/* if the backend's version is less than 4, then it
	 * does not support repeating event types beyond yearly
	 */
	conn = &cal->conn;
	if (conn->ci->vers_out < TABLEVERS_4 && appt4->period.period > yearly_4)	{
		_DtCm_free_appt4(appt4);
		return (CSA_E_NOT_SUPPORTED);
	}

	/* for old backend, insert one appt at a time
	 * in order to match appointments with the new id
	 */

	conn->retry = B_FALSE;

        switch(conn->ci->vers_out) {
                Table_Args_2 a2;
                Table_Args_3 a3;
                Table_Args_4 a4;
                Table_Res_2 *res2;
                Table_Res_3 *res3;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.args.tag = APPT_2;
                a2.args.Args_2_u.appt = _DtCm_appt4_to_appt2(appt4);
                res2 = _DtCm_rtable_insert_2(&a2, conn);
                res = _DtCm_tableres2_to_tableres4(res2);
		if (a2.args.Args_2_u.appt != NULL)
			_DtCm_free_appt2(a2.args.Args_2_u.appt);
                if (res2 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.args.tag = APPT_3;
                a3.args.Args_3_u.appt = _DtCm_appt4_to_appt3(appt4);
		a3.pid = getpid();
                res3 = _DtCm_rtable_insert_3(&a3, conn);
                res = _DtCm_tableres3_to_tableres4(res3);
		if (a3.args.Args_3_u.appt != NULL)
			_DtCm_free_appt3(a3.args.Args_3_u.appt);
                if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
                break;
        case TABLEVERS_4:
                a4.target = cal->name;
                a4.args.tag = APPT_4;
                a4.args.Args_4_u.appt = appt4;
		a4.pid = getpid();
                res = _DtCm_rtable_insert_4(&a4, conn);
                break;
        default:
		stat = CSA_E_FAILURE;
		break;
	}

        if (res != NULL) {
                switch(res->status) {
                case access_ok_4:
			if (res->res.Table_Res_List_4_u.a) {
				stat = _DtCm_appt4_to_libentries(cal->name,
					res->res.Table_Res_List_4_u.a, entry_r);
			}
                        break;
		case access_failed_4:
			stat = CSA_E_NO_AUTHORITY;
			break;
                case access_other_4:
                        stat = CSA_E_INVALID_PARAMETER;
                        break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
                case access_partial_4:
		case access_incomplete_4:
			stat = CSA_E_FAILURE;
			break;
                default:
                        /* remote system error */
                        DP((errfmt, "table_insert_one", res->status));
                        stat = CSA_E_FAILURE;
                        break;
                }
                xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);
        } else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
        }

	_DtCm_free_appt4(appt4);

        return(stat);
}

extern CSA_return_code
_DtCm_table_update(
	Calendar *cal,
	_DtCm_libentry *oentry,
	uint num_attrs,
	CSA_attribute * attrs,
	CSA_enum scope,
	_DtCm_libentry **nentry)
{
        CSA_return_code	stat = CSA_SUCCESS;
        Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Id_4		key;
	Options_4	opt;
	Appt_4		*appt4;
	_DtCm_libentry	*temp;

	DP(("table.c: _DtCm_table_update_one\n"));

        if (cal == NULL || oentry == NULL || nentry == NULL)
                return (CSA_E_INVALID_PARAMETER);

	/* convert scope to option */
	if ((stat = _DtCm_scope_to_options4(scope, &opt)) != CSA_SUCCESS)
		return (stat);

	_csa_iso8601_to_tick(oentry->e->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
		value->item.string_value, &key.tick);
	key.key	= oentry->e->key.id;

	/* create new appt4 from old entry and new attribute values */
	temp = oentry->next;
	oentry->next = NULL;
	stat = _DtCm_libentries_to_appt4(oentry, &appt4);
	oentry->next = temp;

	if (stat != CSA_SUCCESS)
		return (stat);

	/* use do_all for old backends */
	if (appt4->period.period == single_4)
		opt = do_all_4;

	/* apply the new attribute values to the appointment */
	if ((stat = _DtCm_attrs_to_apptdata(num_attrs, attrs, appt4))
	    != CSA_SUCCESS) {
		_DtCm_free_appt4(appt4);
		return (stat);
	}


	/* if the backend's version is less than 4, then it does
	 * not support repeating event types beyond yearly
	 * nor does it support do_forward
	 */
	conn = &cal->conn;
	if (conn->ci->vers_out < TABLEVERS_4 &&
	    (appt4->period.period > yearly_4 || scope >= CSA_SCOPE_FORWARD)) {
		_DtCm_free_appt4(appt4);
		return (CSA_E_NOT_SUPPORTED);
	}

	conn->retry = B_FALSE;
        switch(conn->ci->vers_out) {
                Table_Args_2 a2;
                Table_Args_3 a3;
                Table_Args_4 a4;
                Table_Res_2 *res2;
                Table_Res_3 *res3;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.args.tag = APPTID_2;
		a2.args.Args_2_u.apptid.oid = (Id_2 *)calloc(1, sizeof(Id_2));
                _DtCm_id4_to_id2(&key, a2.args.Args_2_u.apptid.oid);
		a2.args.Args_2_u.apptid.new_appt = _DtCm_appt4_to_appt2(appt4);

		if (opt == do_all_4)
                    res2 = _DtCm_rtable_change_2(&a2, conn);
		else
		    res2 = _DtCm_rtable_change_instance_2(&a2, conn);

                res = _DtCm_tableres2_to_tableres4(res2);
		free(a2.args.Args_2_u.apptid.oid);
		if (a2.args.Args_2_u.apptid.new_appt != NULL)
			_DtCm_free_appt2(a2.args.Args_2_u.apptid.new_appt);
                if (res2 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.args.tag = APPTID_3;
		a3.args.Args_3_u.apptid.oid = (Id_3 *)calloc(1, sizeof(Id_3));
                _DtCm_id4_to_id3(&key, a3.args.Args_3_u.apptid.oid);
		a3.args.Args_3_u.apptid.new_appt = _DtCm_appt4_to_appt3(appt4);
		a3.pid = getpid();

		if (opt == do_all_4)
                    res3 = _DtCm_rtable_change_3(&a3, conn);
		else
		    res3 = _DtCm_rtable_change_instance_3(&a3, conn);

                res = _DtCm_tableres3_to_tableres4(res3);
		free(a3.args.Args_3_u.apptid.oid);
		if (a3.args.Args_3_u.apptid.new_appt != NULL)
			_DtCm_free_appt3(a3.args.Args_3_u.apptid.new_appt);
                if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
                break;
        case TABLEVERS_4:
                a4.target = cal->name;
                a4.args.tag = APPTID_4;
                a4.args.Args_4_u.apptid.oid = &key;
		a4.args.Args_4_u.apptid.new_appt = appt4;
		a4.args.Args_4_u.apptid.option = opt;
		a4.pid = getpid();
                res = _DtCm_rtable_change_4(&a4, conn);
                break;
        default:
		stat = CSA_E_FAILURE;
		break;
        }
	_DtCm_free_appt4(appt4);

        if (res != NULL) {
                switch(res->status) {
                case access_ok_4:
                        if (res->res.Table_Res_List_4_u.a) {
				stat = _DtCm_appt4_to_libentries(cal->name,
					res->res.Table_Res_List_4_u.a, nentry);
			}
                        break;
		case access_failed_4:
			stat = CSA_E_NO_AUTHORITY;
			break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
                case access_other_4:
                case access_partial_4:
		case access_incomplete_4:
			stat = CSA_E_FAILURE;
			break;
                default:
                        /* remote system error */
                        DP((errfmt, "_DtCm_table_update", res->status));
                        stat = CSA_E_FAILURE;
                        break;
		}
		xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);
	} else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
        }

        return(stat);
}

extern CSA_return_code
_DtCm_table_delete(
	Calendar *cal,
	_DtCm_libentry *entry,
	CSA_enum scope)
{
        CSA_return_code	stat = CSA_SUCCESS;
        Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Uidopt_4	key;
	cms_attribute	*rtype;

	DP(("table.c: _DtCm_table_delete\n"));

        if (cal == NULL || entry == NULL)
                return (CSA_E_INVALID_PARAMETER);

	_csa_iso8601_to_tick(entry->e->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
		value->item.string_value, &key.appt_id.tick);

	/* if this is an one-time appt, make sure the scope is do_all */
	rtype = &entry->e->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I];
	if (rtype->value == NULL ||
	    rtype->value->item.sint32_value == CSA_X_DT_REPEAT_ONETIME)
		scope = CSA_SCOPE_ALL;

	if ((stat = _DtCm_scope_to_options4(scope, &key.option)) != CSA_SUCCESS)
		return (stat);

	key.appt_id.key		= entry->e->key.id;
	key.next		= NULL;

	conn = &cal->conn;
	if ((key.option == do_forward_4) && (conn->ci->vers_out < TABLEVERS_4))
		return (CSA_E_NOT_SUPPORTED);

	conn->retry = B_FALSE;

        switch(conn->ci->vers_out) {
                Table_Args_2 a2;
                Table_Args_3 a3;
                Table_Args_4 a4;
                Table_Res_2 *res2;
                Table_Res_3 *res3;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.args.tag = UID_2;
                a2.args.Args_2_u.key = _DtCm_uidopt4_to_uid2(&key);

		if (key.option == do_all_4)
                    res2 = _DtCm_rtable_delete_2(&a2, conn);
		else
		    res2 = _DtCm_rtable_delete_instance_2(&a2, conn);

                res = _DtCm_tableres2_to_tableres4(res2);
		if (a2.args.Args_2_u.key != NULL)
			free(a2.args.Args_2_u.key);
                if (res2 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.args.tag = UID_3;
                a3.args.Args_3_u.key = _DtCm_uidopt4_to_uid3(&key);
		a3.pid = getpid();

		if (key.option == do_all_4)
                    res3 = _DtCm_rtable_delete_3(&a3, conn);
		else
		    res3 = _DtCm_rtable_delete_instance_3(&a3, conn);

                res = _DtCm_tableres3_to_tableres4(res3);
		if (a3.args.Args_3_u.key != NULL)
			free(a3.args.Args_3_u.key);
                if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
		break;
	case TABLEVERS_4:
                a4.target = cal->name;
                a4.args.tag = UIDOPT_4;
                a4.args.Args_4_u.uidopt = &key;
		a4.pid = getpid();
                res = _DtCm_rtable_delete_4(&a4, conn);
                break;
        default:
		stat = CSA_E_FAILURE;
		break;
        }

        if (res!=NULL) {
		switch(res->status) {
		case access_ok_4:
                        if (res->res.Table_Res_List_4_u.a == NULL)
				stat = CSA_X_DT_E_ENTRY_NOT_FOUND |
					CSA_E_INVALID_ENTRY_HANDLE;
			break;
		case access_failed_4:
			stat = CSA_E_NO_AUTHORITY;
			break;
		case access_other_4:
			stat = CSA_E_INVALID_PARAMETER;
			break;
		case access_notable_4:
		case access_partial_4:
		case access_incomplete_4:
			stat = CSA_E_FAILURE;
			break;
		default:
		/* remote system error */
			DP((errfmt, "table_delete", res->status));
                        stat = CSA_E_FAILURE;
                        break;
                }
                xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);

        } else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
        }

        return(stat);
}

extern CSA_return_code 
_DtCm_table_size(Calendar *cal, int *size)
{
	int *res;
	CSA_return_code	stat = CSA_SUCCESS;
	_DtCm_Connection	*conn;

	DP(("table.c: _DtCm_table_size\n"));

	if (cal == NULL || size == NULL)
		return (CSA_E_INVALID_PARAMETER);

	conn = &cal->conn;
	conn->retry = B_TRUE;

        switch(conn->ci->vers_out) {
                Table_Args_2 a2;
                Table_Args_3 a3;
                Table_Args_4 a4;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.args.tag = TICK_2;
                a2.args.Args_2_u.tick = 0;
                res = _DtCm_rtable_size_2(&a2, conn);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.args.tag = TICK_3;
                a3.args.Args_3_u.tick = 0;
		a3.pid = getpid();
                res = _DtCm_rtable_size_3(&a3, conn);
                break;
        case TABLEVERS_4:
                a4.target = cal->name;
                a4.args.tag = TICK_4;
                a4.args.Args_4_u.tick = 0;
		a4.pid = getpid();
                res = _DtCm_rtable_size_4(&a4, conn);
                break;
        default:
		stat = CSA_E_FAILURE;
                break;
        }

	if (res != NULL)
		*size = *res;
	else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	}

	return (stat);
}

extern CSA_return_code
_DtCm_table_lookup_reminder(
	Calendar *cal,
	CSA_uint32 num_names,
	char **reminder_names,
	CSA_uint32 *num_rems,
	CSA_reminder_reference **rems)
{
        CSA_return_code	stat = CSA_SUCCESS;
        Table_Res_4	*res = NULL;
	_DtCm_Connection	*conn;
	Reminder_4	*rptr;
	time_t		tick;

	DP(("table.c: _DtCm_table_lookup_reminder\n"));

	if (cal == NULL || num_rems == 0 || rems == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/*
	 * Due to the implementation of existing backends which
	 * will unmanage any reminders that happens before the
	 * the given tick, the user specified tick is ignore and
	 * we will pass in the current time.
	 */
	tick = time(0);

	conn = &cal->conn;
	conn->retry = B_TRUE;

	switch(conn->ci->vers_out) {
                Table_Args_2 a2;
                Table_Args_3 a3;
                Table_Args_4 a4;
                Table_Res_2 *res2;
                Table_Res_3 *res3;
        case TABLEVERS_2:
                a2.target = cal->name;
                a2.args.tag = TICK_2;
                a2.args.Args_2_u.tick = tick;
                res2 = _DtCm_rtable_lookup_next_reminder_2(&a2, conn);
                res = _DtCm_tableres2_to_tableres4(res2);
                if (res2 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_2, (char*)res2);
                break;
	case TABLEVERS_3:
                a3.target = cal->name;
                a3.args.tag = TICK_3;
                a3.args.Args_3_u.tick = tick;
		a3.pid = getpid();
                res3 = _DtCm_rtable_lookup_next_reminder_3(&a3, conn);
                res = _DtCm_tableres3_to_tableres4(res3);
                if (res3 != NULL)
			xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_3, (char*)res3);
                break;
        case TABLEVERS_4:
		a4.target = cal->name;
                a4.args.tag = TICK_4;
                a4.args.Args_4_u.tick = tick;
		a4.pid = getpid();
                res = _DtCm_rtable_lookup_next_reminder_4(&a4, conn);
                break;
        default:
		stat = CSA_E_FAILURE;
		break;
        }

        if (res != NULL) {
                switch(res->status) {
                case access_ok_4:
			/*
			 * if reminder names are specified,
			 * return only those that match
			 */
			rptr = _DtCm_copy_reminder4(res->res.Table_Res_List_4_u.r);
                	xdr_free((xdrproc_t)_DtCm_xdr_Table_Res_4, (char*)res);

			if (num_names > 0 && reminder_names != NULL) {
				rptr = _DtCm_match_reminders(rptr, num_names,
					reminder_names);
			}

			stat = _DtCm_reminder4_to_csareminder(rptr, num_rems,
				rems);

			if (rptr != NULL)
				_DtCm_free_reminder4(rptr);
                        break;
		case access_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;
                case access_failed_4:
                case access_other_4:
                        stat = CSA_E_FAILURE;
			break;
		default:
			/* remote system error */
			DP ((errfmt, "_DtCm_table_lookup_reminder", res->status));
			stat = CSA_E_FAILURE;
			break;
                }
        } else {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	}

        return(stat);
}

extern CSA_return_code
_DtCm_table_unregister_target(
	_DtCm_Connection *conn,
	char *calendar)
{
	CSA_return_code	stat;
	Registration_Status_4 res;
	boolean_t	nullreturned = B_FALSE;

	DP(("table.c: _DtCm_table_unregister_target\n"));

	switch(conn->ci->vers_out) {
		Registration_2 r2;
		Registration_3 r3;
		Registration_4 r4;
		Registration_Status_2 *stat2;
		Registration_Status_3 *stat3;
		Registration_Status_4 *stat4;

	case TABLEVERS_2:
		r2.target = calendar;
        	r2.prognum = _DtCm_transient;
        	r2.versnum = AGENTVERS;
        	r2.procnum = update_callback;
		r2.next = NULL;
		stat2 = _DtCm_deregister_callback_2(&r2, conn);
		if (stat2 != NULL)
			res = _DtCm_regstat2_to_regstat4(*stat2);
		else
			nullreturned = B_TRUE;
		break;

	case TABLEVERS_3:
		r3.target = calendar;
        	r3.prognum = _DtCm_transient;
        	r3.versnum = AGENTVERS;
        	r3.procnum = update_callback;
		r3.next = NULL;
		r3.pid = getpid();
		stat3 = _DtCm_deregister_callback_3(&r3, conn);
		if (stat3 != NULL)
			res = _DtCm_regstat3_to_regstat4(*stat3);
		else
			nullreturned = B_TRUE;
		break;

	case TABLEVERS_4:
		r4.target = calendar;
        	r4.prognum = _DtCm_transient;
        	r4.versnum = AGENTVERS;
        	r4.procnum = update_callback;
		r4.next = NULL;
		r4.pid = getpid();
		stat4 = _DtCm_deregister_callback_4(&r4, conn);
		if (stat4 != NULL)
			res = *stat4;
		else
			nullreturned = B_TRUE;
		break;
	default:
		stat = CSA_E_FAILURE;
	}

	if (nullreturned) {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	} else {
		switch (res) {
		case deregistered_4:
			stat = CSA_SUCCESS;
			break; 

		case reg_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;

		default:
			stat = CSA_E_FAILURE;
			break;
		}
	}

	return (stat);
}

extern CSA_return_code
_DtCm_table_register_target(
	_DtCm_Connection *conn,
	char *calendar)
{
	CSA_return_code	stat;
	Registration_Status_4 res;
	boolean_t	nullreturned = B_FALSE;

	DP(("table.c: _DtCm_table_register_target\n"));

	switch(conn->ci->vers_out) {
		Registration_2 r2;
		Registration_3 r3;
		Registration_4 r4;
		Registration_Status_2 *stat2;
		Registration_Status_3 *stat3;
		Registration_Status_4 *stat4;

	case TABLEVERS_2:
		r2.target = calendar;
        	r2.prognum = _DtCm_transient;
        	r2.versnum = AGENTVERS;
        	r2.procnum = update_callback;
		r2.next = NULL;
		stat2 = _DtCm_register_callback_2(&r2, conn);
		if (stat2 != NULL)
			res = _DtCm_regstat2_to_regstat4(*stat2);
		else
			nullreturned = B_TRUE;
		break;

	case TABLEVERS_3:
		r3.target = calendar;
        	r3.prognum = _DtCm_transient;
        	r3.versnum = AGENTVERS;
        	r3.procnum = update_callback;
		r3.next = NULL;
		r3.pid = getpid();
		stat3 = _DtCm_register_callback_3(&r3, conn);
		if (stat3 != NULL)
			res = _DtCm_regstat3_to_regstat4(*stat3);
		else
			nullreturned = B_TRUE;
		break;

	case TABLEVERS_4:
		r4.target = calendar;
        	r4.prognum = _DtCm_transient;
        	r4.versnum = AGENTVERS;
        	r4.procnum = update_callback;
		r4.next = NULL;
		r4.pid = getpid();
		stat4 = _DtCm_register_callback_4(&r4, conn);
		if (stat4 != NULL)
			res = *stat4;
		else
			nullreturned = B_TRUE;
		break;
	default:
		stat = CSA_E_FAILURE;
	}

	if (nullreturned) {
		stat = (conn->stat == RPC_SUCCESS) ? CSA_E_SERVICE_UNAVAILABLE :
			_DtCm_clntstat_to_csastat(conn->stat);
	} else {
		switch (res) {
		case registered_4:
			stat = CSA_SUCCESS;
			break;

		case reg_notable_4:
			stat = CSA_E_CALENDAR_NOT_EXIST;
			break;

		default:
			stat = CSA_E_FAILURE;
			break;
		}
	}
	return (stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

/*
 * Returns 1 if stat contains an invalid status
 */
static CSA_return_code
tablestat_to_dtcmstat(Table_Status_4 stat, CSA_return_code *dtcmstat)
{
	switch (stat) {
		case ok_4:
			*dtcmstat = CSA_SUCCESS;
			break;
		case duplicate_4:
			*dtcmstat = CSA_E_CALENDAR_EXISTS;
			break;
		case notable_4:
			*dtcmstat = CSA_E_CALENDAR_NOT_EXIST;
			break;
		case denied_4:
			*dtcmstat = CSA_E_NO_AUTHORITY;
			break;
		case badtable_4:
		case other_4:
			*dtcmstat = CSA_E_FAILURE;
			break;
		case tbl_not_owner_4:
			*dtcmstat = CSA_E_NO_AUTHORITY;
			break;
		case tbl_exist_4:
			*dtcmstat = CSA_E_CALENDAR_EXISTS;
			break;
		case tbl_notsupported_4:
			*dtcmstat = CSA_E_NOT_SUPPORTED;
			break;
		default:
			*dtcmstat = CSA_E_FAILURE;
			return (CSA_E_FAILURE);
	}

	return (CSA_SUCCESS);
}

