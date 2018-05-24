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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: db_server_xdr.C /main/4 1996/01/30 16:08:48 barstow $ 			 				
/*
 * @(#)db_server_xdr.C	1.19 95/01/06
 *
 * Tool Talk Database Server
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * This file contains xdr functions for the db server's rpc procedures.
 */

#include <isam.h>

#include "db/db_server.h"

bool_t
xdr_keypart(XDR *xdrs, keypart *objp)
{
	if (!xdr_u_short(xdrs, &objp->kp_start)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->kp_leng)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->kp_type)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_keydesc(XDR *xdrs, keydesc *objp)
{
	if (!xdr_short(xdrs, &objp->k_flags)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, &objp->k_nparts)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->k_part, 8, sizeof(keypart),
			(xdrproc_t) xdr_keypart)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isam_results(XDR *xdrs, _Tt_isam_results *objp)
{
	if (!xdr_int(xdrs, &objp->result)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->iserrno)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isaddindex_args(XDR *xdrs, _Tt_isaddindex_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc),
			 (xdrproc_t) xdr_keydesc)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isbuild_args(XDR *xdrs, _Tt_isbuild_args *objp)
{
	if (!xdr_string(xdrs, &objp->path, 1024)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->reclen)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc),
			 (xdrproc_t) xdr_keydesc)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->mode)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->isreclen)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_iscntl_args(XDR *xdrs, _Tt_iscntl_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->func)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->arg.arg_val, (u_int *)&objp->arg.arg_len, ISAPPLMAGICLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_iscntl_results(XDR *xdrs, _Tt_iscntl_results *objp)
{
	if (!xdr_int(xdrs, &objp->result)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->iserrno)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->arg.arg_val, (u_int *)&objp->arg.arg_len, ISAPPLMAGICLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return TRUE;
}

bool_t
xdr_Tt_isdelrec_args(XDR *xdrs, _Tt_isdelrec_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->recnum)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, 8192, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isopen_args(XDR *xdrs, _Tt_isopen_args *objp)
{
	if (!xdr_string(xdrs, &objp->path, 1024)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isread_args(XDR *xdrs, _Tt_isread_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t) xdr_char)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->mode)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->isrecnum)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isread_results(XDR *xdrs, _Tt_isread_results *objp)
{
	if (!xdr_Tt_isam_results(xdrs, &objp->isresult)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t) xdr_char)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->isreclen)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->isrecnum)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isrewrec_args(XDR *xdrs, _Tt_isrewrec_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->recnum)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_isstart_args(XDR *xdrs, _Tt_isstart_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc),
			 (xdrproc_t)
			 xdr_keydesc)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->key_len)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_test_and_set_args(XDR *xdrs, _Tt_test_and_set_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc),
			 (xdrproc_t)xdr_keydesc)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->key_len)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_test_and_set_results(XDR *xdrs, _Tt_test_and_set_results *objp)
{
	if (!xdr_Tt_isam_results(xdrs, &objp->isresult)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->isreclen)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->isrecnum)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t xdr_tt_file_netfile_args(XDR * xdrs, _tt_file_netfile_args * argp)
{
        if (!xdr_string(xdrs, &argp->file_or_netfile, ~0)) {
                return (FALSE);
        }
	return (TRUE);
}

bool_t
xdr_tt_file_netfile_results(XDR * xdrs, _tt_file_netfile_results * argp)
{
        if (!xdr_string(xdrs, &argp->result_string, ~0)) {
                return (FALSE);
        }
        if (!xdr_tt_db_results(xdrs, &argp->results)) {
                return (FALSE);
        }
	if (!xdr_int(xdrs, &argp->tt_status)) {
		return (FALSE);
	}
	return (TRUE);
}


bool_t
xdr_Tt_trans_record_list(XDR *xdrs, _Tt_trans_record_list *objp)
{
	if (!xdr_pointer(xdrs, (char **)objp, sizeof(struct _Tt_trans_record),
			 (xdrproc_t)xdr_Tt_trans_record)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_trans_record(XDR *xdrs, _Tt_trans_record *objp)
{
	if (!xdr_int(xdrs, &objp->newp)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->recnum)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_Tt_trans_record_list(xdrs, &objp->next)) {
		return (FALSE);
	}
	return TRUE;
}

bool_t
xdr_Tt_transaction_args(XDR *xdrs, _Tt_transaction_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_Tt_trans_record_list(xdrs, &objp->recs)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_iswrite_args(XDR *xdrs,	_Tt_iswrite_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->rec.rec_val, (u_int *)&objp->rec.rec_len, ISMAXRECLEN, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_oidaccess_args(XDR *xdrs, _Tt_oidaccess_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->key.key_val, (u_int *)&objp->key.key_len, OID_KEY_LENGTH, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, (short *)&objp->value)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_oidaccess_results(XDR *xdrs, _Tt_oidaccess_results *objp)
{
	if (!xdr_long(xdrs, (long *)&objp->uid)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, (long *)&objp->group)) {
		return (FALSE);
	}
	if (!xdr_short(xdrs, (short *)&objp->mode)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->result)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->iserrno)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_prop(XDR *xdrs, _Tt_prop *objp)
{
	if (!xdr_array(xdrs, (char **)&objp->propname.propname_val, (u_int *)&objp->propname.propname_len, ~0, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->recnum)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->value.value_val, (u_int *)&objp->value.value_len, 8192, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_spec_props(XDR *xdrs, _Tt_spec_props *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc), (xdrproc_t)xdr_keydesc)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->key_len)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->oidkey.oidkey_val, (u_int *)&objp->oidkey.oidkey_len, OID_KEY_LENGTH, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->props.props_val, (u_int *)&objp->props.props_len, ~0, sizeof(_Tt_prop), (xdrproc_t)xdr_Tt_prop)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->result)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->iserrno)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_Tt_session_args(XDR *xdrs, _Tt_session_args *objp)
{
	if (!xdr_int(xdrs, &objp->isfd)) {
		return (FALSE);
	}
	if (!xdr_pointer(xdrs, (char **)&objp->key, sizeof(keydesc), (xdrproc_t)xdr_keydesc)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->key_len)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->oidkey.oidkey_val, (u_int *)&objp->oidkey.oidkey_len, OID_KEY_LENGTH, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->session.session_val, (u_int *)&objp->session.session_len, ~0, sizeof(char), (xdrproc_t)xdr_char)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_db_results(XDR *xdrs, _tt_db_results *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_string(XDR *xdrs, _tt_string *objp)
{
	if (!xdr_string(xdrs, &objp->value, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_property_value(XDR *xdrs, _tt_property_value *objp)
{
	if (!xdr_bytes(xdrs, (char **)&objp->value.value_val, (u_int *)&objp->value.value_len, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_property(XDR *xdrs, _tt_property *objp)
{
	if (!xdr_string(xdrs, &objp->name, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->values.values_val, (u_int *)&objp->values.values_len, ~0, sizeof(_tt_property_value), (xdrproc_t)xdr_tt_property_value)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_access(XDR *xdrs, _tt_access *objp)
{
	if (!xdr_long(xdrs, (long *)&objp->user)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, (long *)&objp->group)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, (unsigned long *)&objp->mode)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_message(XDR *xdrs, _tt_message *objp)
{
	if (!xdr_bytes(xdrs, (char **)&objp->body.body_val, (u_int *)&objp->body.body_len, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_create_file_args(XDR *xdrs, _tt_create_file_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_create_obj_args(XDR *xdrs, _tt_create_obj_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->otype, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t   
xdr_tt_remove_file_args(XDR *xdrs, _tt_remove_file_args *objp)
{
        if (!xdr_string(xdrs, &objp->file, ~0)) {
                return (FALSE);
        }
        if (!xdr_tt_access(xdrs, &objp->access)) {
                return (FALSE);
        }
        return (TRUE);
}
 
bool_t
xdr_tt_remove_obj_args(XDR *xdrs, _tt_remove_obj_args *objp)
{
        if (!xdr_string(xdrs, &objp->objid, ~0)) {
                return (FALSE);
        }
        if (!xdr_string(xdrs, &objp->forward_pointer, ~0)) {
                return (FALSE);
        }
        if (!xdr_tt_access(xdrs, &objp->access)) {
                return (FALSE);
        }
        return (TRUE);
}

bool_t
xdr_tt_move_file_args(XDR *xdrs, _tt_move_file_args *objp)
{
        if (!xdr_string(xdrs, &objp->file, ~0)) {
                return (FALSE);
        }
        if (!xdr_string(xdrs, &objp->new_file, ~0)) {
                return (FALSE);
        }
        if (!xdr_tt_access(xdrs, &objp->access)) {
                return (FALSE);
        }
        return (TRUE);
}

bool_t
xdr_tt_set_file_prop_args(XDR *xdrs, _tt_set_file_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_file_props_args(XDR *xdrs, _tt_set_file_props_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_add_file_prop_args(XDR *xdrs, _tt_add_file_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->unique)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_del_file_prop_args(XDR *xdrs, _tt_del_file_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_file_prop_args(XDR *xdrs, _tt_get_file_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->name, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_file_props_args(XDR *xdrs, _tt_get_file_props_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_file_objs_args(XDR *xdrs, _tt_get_file_objs_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_file_access_args(XDR *xdrs, _tt_set_file_access_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->new_access)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_file_access_args(XDR *xdrs, _tt_get_file_access_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_obj_prop_args(XDR *xdrs, _tt_set_obj_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_obj_props_args(XDR *xdrs, _tt_set_obj_props_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_add_obj_prop_args(XDR *xdrs, _tt_add_obj_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->unique)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_del_obj_prop_args(XDR *xdrs, _tt_del_obj_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_obj_prop_args(XDR *xdrs, _tt_get_obj_prop_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->name, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_obj_props_args(XDR *xdrs, _tt_get_obj_props_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_obj_type_args(XDR *xdrs, _tt_set_obj_type_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->otype, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_obj_type_args(XDR *xdrs, _tt_get_obj_type_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_obj_file_args(XDR *xdrs, _tt_set_obj_file_args *objp)
{
        if (!xdr_string(xdrs, &objp->objid, ~0)) {
                return (FALSE);
        }
        if (!xdr_string(xdrs, &objp->file, ~0)) {
                return (FALSE);
        }
        if (!xdr_tt_access(xdrs, &objp->access)) {
                return (FALSE);
        }
        return (TRUE);
}

bool_t
xdr_tt_get_obj_file_args(XDR *xdrs, _tt_get_obj_file_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_set_obj_access_args(XDR *xdrs, _tt_set_obj_access_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->new_access)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_obj_access_args(XDR *xdrs, _tt_get_obj_access_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_is_file_in_db_args(XDR *xdrs, _tt_is_file_in_db_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_is_obj_in_db_args(XDR *xdrs, _tt_is_obj_in_db_args *objp)
{
	if (!xdr_string(xdrs, &objp->objid, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_queue_msg_args(XDR *xdrs, _tt_queue_msg_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->ptypes.values_val, (u_int *)&objp->ptypes.values_len, ~0, sizeof(_tt_string), (xdrproc_t)xdr_tt_string)) {
		return (FALSE);
	}
	if (!xdr_tt_message(xdrs, &objp->message)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_dequeue_msgs_args(XDR *xdrs, _tt_dequeue_msgs_args *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->ptypes.values_val, (u_int *)&objp->ptypes.values_len, ~0, sizeof(_tt_string), (xdrproc_t)xdr_tt_string)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_db_cache_results(XDR *xdrs, _tt_db_cache_results *objp)
{
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_auth_level_results(XDR *xdrs, _tt_auth_level_results *objp)
{
	if (!xdr_int(xdrs, &objp->auth_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_file_partition_results(XDR *xdrs, _tt_file_partition_results *objp)
{
	if (!xdr_string(xdrs, &objp->partition, ~0)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->network_path, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_file_prop_results(XDR *xdrs, _tt_file_prop_results *objp)
{
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_file_props_results(XDR *xdrs, _tt_file_props_results *objp)
{
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_file_objs_results(XDR *xdrs, _tt_file_objs_results *objp)
{
	if (!xdr_array(xdrs, (char **)&objp->objids.values_val, (u_int *)&objp->objids.values_len, ~0, sizeof(_tt_string), (xdrproc_t)xdr_tt_string)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_file_access_results(XDR *xdrs, _tt_file_access_results *objp)
{
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_obj_prop_results(XDR *xdrs, _tt_obj_prop_results *objp)
{
	if (!xdr_tt_property(xdrs, &objp->property)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_obj_props_results(XDR *xdrs, _tt_obj_props_results *objp)
{
	if (!xdr_array(xdrs, (char **)&objp->properties.properties_val, (u_int *)&objp->properties.properties_len, ~0, sizeof(_tt_property), (xdrproc_t)xdr_tt_property)) {
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->cache_level)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_obj_type_results(XDR *xdrs, _tt_obj_type_results *objp)
{
	if (!xdr_string(xdrs, &objp->otype, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_obj_file_results(XDR *xdrs, _tt_obj_file_results *objp)
{
	if (!xdr_string(xdrs, &objp->file, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_obj_access_results(XDR *xdrs, _tt_obj_access_results *objp)
{
	if (!xdr_tt_access(xdrs, &objp->access)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_is_file_in_db_results(XDR *xdrs, _tt_is_file_in_db_results *objp)
{
	if (!xdr_bool(xdrs, &objp->directory_flag)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_is_obj_in_db_results(XDR *xdrs, _tt_is_obj_in_db_results *objp)
{
	if (!xdr_string(xdrs, &objp->forward_pointer, ~0)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_dequeue_msgs_results(XDR *xdrs, _tt_dequeue_msgs_results *objp)
{
	if (!xdr_array(xdrs, (char **)&objp->messages.messages_val, (u_int *)&objp->messages.messages_len, ~0, sizeof(_tt_message), (xdrproc_t)xdr_tt_message)) {
		return (FALSE);
	}
	if (!xdr_tt_db_results(xdrs, &objp->results)) {
		return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_tt_get_all_sessions_args(XDR *xdrs, _tt_get_all_sessions_args *objp)
{
	return(xdr_array(xdrs, (char **)&objp->oidkey.oidkey_val, (u_int *)&objp->oidkey.oidkey_len, OID_KEY_LENGTH, sizeof(char), (xdrproc_t)xdr_char));
}

bool_t
xdr_tt_get_all_sessions_results(XDR *xdrs, _tt_get_all_sessions_results *objp)
{
	if (!xdr_array(xdrs,
		       (char **)&objp->oidkey.oidkey_val,
		       (u_int *)&objp->oidkey.oidkey_len,
		       MAXKEYSIZE,
		       sizeof(char),
		       (xdrproc_t)xdr_char)) {
		return(FALSE);
	}

	if (!xdr_array(xdrs,
		       (char **)&objp->session_list.values_val,
		       (u_int *)&objp->session_list.values_len,
		       OPT_MAX_GET_SESSIONS,
		       sizeof(char *),
		       (xdrproc_t)xdr_tt_string)) {
		return (FALSE);
	}
	return(TRUE);
}

bool_t
xdr_tt_garbage_collect_results(XDR *xdrs, _tt_garbage_collect_results *objp)
{
	return(xdr_int(xdrs, &objp->tt_status));
}

bool_t
xdr_tt_delete_session_results(XDR *xdrs, _tt_delete_session_results *objp)
{
	return(xdr_int(xdrs, &objp->tt_status));
}


bool_t
xdr_tt_delete_session_args(XDR *xdrs, _tt_delete_session_args *objp)
{
	return(xdr_string(xdrs, &objp->session.value, ~0));
}


