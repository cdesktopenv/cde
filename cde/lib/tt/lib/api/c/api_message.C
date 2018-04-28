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
//%%  $TOG: api_message.C /main/7 1999/10/14 18:39:28 mgreess $ 			 				
/* @(#)api_message.C	1.35 93/09/07
 *
 * api_message.cc
 *
 * Copyright (c) 1990-1993 by Sun Microsystems, Inc.
 */

#include "db/tt_db_file.h"
#include "mp/mp_c.h"
#include "mp/mp_trace.h"
#include "mp/mp_msg_context.h"
#include "util/tt_path.h"
#include "util/tt_audit.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_xdr.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "api/c/api_typecb.h"

/************************************************************************
* Message Functions (FSpec A.6)						*
************************************************************************/
const uid_t uid_nobody = 65534;
const gid_t gid_nobody = 65534;

// Local functions for this file
static Tt_pattern _tt_msg_pat(_Tt_c_message_ptr &msg);
static int _tt_run_opnum_callbacks(Tt_message msg, Tt_pattern pat,
				   _Tt_string &typenm, int opnum,
				   _Tt_typecb_table_ptr &tcbt);

char *		_tt_message_print(Tt_message);
Tt_status	_tt_message_xarg_add(Tt_message, Tt_mode, const char *, 
		    		     xdrproc_t, void *);
Tt_status	_tt_message_arg_xval(Tt_message, int, xdrproc_t, void *);
Tt_status	_tt_message_arg_xval_set(Tt_message, int, xdrproc_t, void *);
Tt_status	_tt_message_context_xval(Tt_message, const char *, xdrproc_t,
					 void *);
Tt_status	_tt_message_xcontext_set(Tt_message, const char *, xdrproc_t,
					 void *);
char *		_tt_message_context_val(Tt_message, const char *);
Tt_status	_tt_message_context_ival(Tt_message, const char *, int *);
Tt_status	_tt_message_context_bval(Tt_message, const char *,
					 unsigned char **, int *);
Tt_status	_tt_message_context_set(Tt_message, const char *, const char *);
Tt_status	_tt_message_icontext_set(Tt_message, const char *, int );
Tt_status	_tt_message_bcontext_set(Tt_message, const char *,
					 const unsigned char *, int);
int		_tt_message_contexts_count(Tt_message);
char *		_tt_message_context_slotname(Tt_message, int);
char *		_tt_message_id(Tt_message);
char *		_tt_message_abstainer(Tt_message m, int n);
int		_tt_message_abstainers_count(Tt_message m);
char *		_tt_message_accepter(Tt_message m, int n);
int		_tt_message_accepters_count(Tt_message m);
char *		_tt_message_rejecter(Tt_message m, int n);
int		_tt_message_rejecters_count(Tt_message m);
Tt_status	_tt_message_abstain(Tt_message m);


char *
tt_message_print(Tt_message m)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_PRINT, m);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_message_print(m);
        audit.exit(result);

        return result;
}

/* 
 * Creation and destruction (FSpec A.6.1)
 */
/* 
 * Creates a new message object and returns an opaque handle.
 */
Tt_message
tt_message_create()
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_MESSAGE_CREATE, 0);
	Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
                return (Tt_message)error_pointer(status);
        }

        result =  _tt_message_create();

        audit.exit(result);       

        return result;
}


Tt_message
tt_message_create_super(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_CREATE_SUPER, m);
	Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
                return (Tt_message)error_pointer(status);
        }

        result =  _tt_message_create_super(m);
        audit.exit(result);       

	return result;
}


Tt_status
tt_message_destroy(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_DESTROY, m);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_message_destroy(m);
	// tttk uses hint of whether destruction actually occurred
	if (status == TT_WRN_STOPPED) {
		status = TT_OK;
	}
        audit.exit(status);       

	return status;
}


Tt_status
tt_message_send(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SEND, m);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_message_send(m);
        audit.exit(status);       

	return status;
}


Tt_status
tt_message_send_on_exit(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SEND_ON_EXIT, m);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_message_send_on_exit(m);
        audit.exit(status);       

	return status;
}


Tt_message
tt_message_receive(void)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_MESSAGE_RECEIVE, 0);
	Tt_message result;

        if (status != TT_OK) {
		audit.exit((Tt_message)error_pointer(status));
                return (Tt_message)error_pointer(status);
        }

        result =  _tt_message_receive();
        audit.exit(result);       

	return result;
}


Tt_status
tt_message_callback_add(Tt_message m, Tt_message_callback f)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MX", TT_MESSAGE_CALLBACK_ADD, m,
					     f);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
	}

	status = _tt_message_callback_add(m, f);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_reject(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_REJECT, m);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
	}

	status = _tt_message_reject(m);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_reply(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_REPLY, m);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
	}

	status = _tt_message_reply(m);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_fail(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_FAIL, m);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
	}

	status = _tt_message_fail(m);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_accept(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ACCEPT, m);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
	}

	status = _tt_message_accept(m);
        audit.exit(status);

	return status;
}


Tt_pattern
tt_message_pattern(Tt_message m) 
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_PATTERN, m);
        Tt_pattern result;

        if (status != TT_OK) {
		audit.exit((Tt_pattern)error_pointer(status));
                return (Tt_pattern)error_pointer(status);
        }

        result =  _tt_message_pattern(m);
        audit.exit(result);

        return result;
}


void *
tt_message_user(Tt_message m, int key)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_USER, m, key);
        void *result;

        if (status != TT_OK) {
		audit.exit((void *)error_pointer(status));
                return (void *)error_pointer(status);
        }

        result =  _tt_message_user(m, key);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_user_set(Tt_message m, int key, void *v)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MiA", TT_MESSAGE_USER_SET, m,
					key, v);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_user_set(m, key, v);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_add(Tt_message m, Tt_mode n, const char *vt, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MOTc", TT_MESSAGE_ARG_ADD, m,
					n, vt, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_add(m, n, vt, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_barg_add(Tt_message m, Tt_mode n, const char *vtype,
                     const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MoTnI", TT_MESSAGE_BARG_ADD, m, n,
					vtype, value, len);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_barg_add(m, n, vtype, value, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_bval(Tt_message m, int n, unsigned char **v, int *l)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MN", TT_MESSAGE_ARG_BVAL, m, n);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_bval(m, n, v, l);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_bval_set(Tt_message m, int n, const unsigned char *v, int l)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MNnI", TT_MESSAGE_ARG_BVAL_SET, m,
					n, v, l);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_bval_set(m, n, v, l);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_iarg_add(Tt_message m, Tt_mode n, const char *vtype,
                     int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MOTi", TT_MESSAGE_IARG_ADD, m, n,
					vtype, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_iarg_add(m, n, vtype, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_ival(Tt_message m, int n, int *v)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MN", TT_MESSAGE_ARG_IVAL, m, n);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_ival(m, n, v);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_ival_set(Tt_message m, int n, int v)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MNi", TT_MESSAGE_ARG_IVAL_SET, m, n, v);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_ival_set(m, n, v);
        audit.exit(status);

	return status;
}


Tt_mode
tt_message_arg_mode(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MN", TT_MESSAGE_ARG_MODE, m, n);
	Tt_mode result;

        if (status != TT_OK) {
		audit.exit((Tt_mode)error_int(status));
                return (Tt_mode)error_int(status);
        }

        result =  _tt_message_arg_mode(m, n);
        audit.exit(result);

	return result;
}


char *
tt_message_arg_type(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MN", TT_MESSAGE_ARG_TYPE, m, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_message_arg_type(m, n);
        audit.exit(result);

	return result;
}


char *
tt_message_arg_val(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MN", TT_MESSAGE_ARG_VAL, m, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_message_arg_val(m, n);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_arg_val_set(Tt_message m, int n, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MNC", TT_MESSAGE_ARG_VAL_SET, m, n,
					value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_val_set(m, n, value);
        audit.exit(status);

	return status;
}


int
tt_message_args_count(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ARGS_COUNT, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result =  _tt_message_args_count(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_xarg_add(Tt_message m, Tt_mode n, const char *vtype,
                    xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MOTXA", TT_MESSAGE_XARG_ADD, m, n, vtype,
					xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_xarg_add(m, n, vtype, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_xval(Tt_message m, int n, xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MNXX", TT_MESSAGE_ARG_XVAL, m, n,
					xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_xval(m, n, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_arg_xval_set(Tt_message m, int n, xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MNXA", TT_MESSAGE_ARG_XVAL_SET, m, n,
					xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_arg_xval_set(m, n, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_context_xval(Tt_message m, const char *slotname,
                        xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MsXX", TT_MESSAGE_CONTEXT_XVAL, m,
					    slotname, xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_context_xval(m, slotname, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_xcontext_set(Tt_message m, const char *slotname,
                        xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MsXA", TT_MESSAGE_XCONTEXT_SET, m,
					slotname, xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_xcontext_set(m, slotname, xdr_proc, value);
        audit.exit(status);

	return status;
}


char *
tt_message_context_val(Tt_message m, const char *slotname)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Ms", TT_MESSAGE_CONTEXT_VAL, m,
					    slotname);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *) error_pointer(status));
                return (char *) error_pointer(status);
        }

        result =  _tt_message_context_val(m, slotname);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_context_ival(Tt_message m, const char *slotname, int *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Ms", TT_MESSAGE_CONTEXT_IVAL, m,
 					    slotname);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_context_ival(m, slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_context_bval(Tt_message m, const char *slotname,
                        unsigned char **val, int *len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Ms", TT_MESSAGE_CONTEXT_BVAL, m,
 					    slotname);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_context_bval(m, slotname, val, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_context_set(Tt_message m, const char *slotname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MsC", TT_MESSAGE_CONTEXT_SET, m,
 					    slotname, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_context_set(m, slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_icontext_set(Tt_message m, const char *slotname, int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Msi", TT_MESSAGE_ICONTEXT_SET, m,
 					    slotname, value);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_icontext_set(m, slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_message_bcontext_set(Tt_message m, const char *slotname,
                        const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MsnI", TT_MESSAGE_BCONTEXT_SET, m,
 					    slotname, value, len);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_message_bcontext_set(m, slotname, value, len);
        audit.exit(status);

	return status;
}


int
tt_message_contexts_count(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_CONTEXTS_COUNT, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

	result = _tt_message_contexts_count(m);
        audit.exit(result);

	return result;
}


char *
tt_message_context_slotname(Tt_message m, int i)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_CONTEXT_SLOTNAME,
					    m, i);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *) error_pointer(status));
                return (char *) error_pointer(status);
        }

        result =  _tt_message_context_slotname(m, i);
        audit.exit(result);

	return result;
}

Tt_class
tt_message_class(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_CLASS, m);
	Tt_class result;

        if (status != TT_OK) {
		audit.exit((Tt_class)error_int(TT_ERR_POINTER));
		return (Tt_class)error_int(TT_ERR_POINTER);
        }

        result = _tt_message_class(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_class_set(Tt_message m, Tt_class c)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("ML", TT_MESSAGE_CLASS_SET, m, c);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_class_set(m, c);
        audit.exit(status);

	return status;
}


char *
tt_message_file(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_FILE, m);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_message_file(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_file_set(Tt_message m, const char *filepath)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mc", TT_MESSAGE_FILE_SET, m, filepath);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_file_set(m, filepath);
        audit.exit(status);

	return status;
}


char *
tt_message_object(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_OBJECT, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_object(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_object_set(Tt_message m, const char * objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mo", TT_MESSAGE_OBJECT_SET, m, objid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_object_set(m, objid);
        audit.exit(status);

	return status;
}


char *
tt_message_id(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ID, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_id(m);
        audit.exit(result);
 
        return result;
}


char *
tt_message_op(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_OP, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_op(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_op_set(Tt_message m, const char *opname)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_OP_SET, m, opname);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_op_set(m, opname);
        audit.exit(status);

	return status;
}


int
tt_message_opnum(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_OPNUM, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

	result = _tt_message_opnum(m);
        audit.exit(result);

	return result;
}


char *
tt_message_otype(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_OTYPE, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_otype(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_otype_set(Tt_message m, const char * otype)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_OTYPE_SET, m, otype);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_otype_set(m, otype);
        audit.exit(status);

	return status;
}


Tt_address
tt_message_address(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ADDRESS, m);
	Tt_address result;

        if (status != TT_OK) {
		audit.exit((Tt_address)error_int(TT_ERR_POINTER));
		return (Tt_address)error_int(TT_ERR_POINTER);
        }

        result = _tt_message_address(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_address_set(Tt_message m, Tt_address p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MD", TT_MESSAGE_ADDRESS_SET, m, p);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_address_set(m, p);
        audit.exit(status);

	return status;
}


char *
tt_message_handler(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_HANDLER, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_handler(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_handler_set(Tt_message m, const char * procid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_HANDLER_SET, m,
					procid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_handler_set(m, procid);
        audit.exit(status);

	return status;
}


char *
tt_message_handler_ptype(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_HANDLER_PTYPE, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_handler_ptype(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_handler_ptype_set(Tt_message m, const char * ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_HANDLER_PTYPE_SET, m,
					ptid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_handler_ptype_set(m, ptid);
        audit.exit(status);

	return status;
}


int
tt_message_accepters_count(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ACCEPTERS_COUNT, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result =  _tt_message_accepters_count(m);
        audit.exit(result);

	return result;
}


char *
tt_message_accepter(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_ACCEPTER, m, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_message_accepter(m, n);
        audit.exit(result);

	return result;
}


int
tt_message_abstainers_count(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_ABSTAINERS_COUNT, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result =  _tt_message_abstainers_count(m);
        audit.exit(result);

	return result;
}


char *
tt_message_abstainer(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_ABSTAINER, m, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_message_abstainer(m, n);
        audit.exit(result);

	return result;
}


int
tt_message_rejecters_count(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_REJECTERS_COUNT, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result =  _tt_message_rejecters_count(m);
        audit.exit(result);

	return result;
}


char *
tt_message_rejecter(Tt_message m, int n)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_REJECTER, m, n);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_message_rejecter(m, n);
        audit.exit(result);

	return result;
}


Tt_disposition
tt_message_disposition(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_DISPOSITION, m);
	Tt_disposition result;

        if (status != TT_OK) {
		audit.exit((Tt_disposition)error_int(TT_ERR_POINTER));
		return (Tt_disposition)error_int(TT_ERR_POINTER);
        }

        result = _tt_message_disposition(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_disposition_set(Tt_message m, Tt_disposition r)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Md", TT_MESSAGE_DISPOSITION_SET, m, r);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_disposition_set(m, r);
        audit.exit(status);

	return status;
}


Tt_scope
tt_message_scope(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SCOPE, m);
	Tt_scope result;

        if (status != TT_OK) {
		audit.exit((Tt_scope)error_int(TT_ERR_POINTER));
		return (Tt_scope)error_int(TT_ERR_POINTER);
        }

        result = _tt_message_scope(m);
        audit.exit(result);

	return result;
}


Tt_status
tt_message_scope_set(Tt_message m, Tt_scope s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MS", TT_MESSAGE_SCOPE_SET, m, s);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_scope_set(m, s);
        audit.exit(status);

	return status;
}


char *
tt_message_sender(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SENDER, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_sender(m);
        audit.exit(result);
 
        return result;
}


char *
tt_message_sender_ptype(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SENDER_PTYPE, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_sender_ptype(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_sender_ptype_set(Tt_message m, const char *ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_SENDER_PTYPE_SET, m,
					ptid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_sender_ptype_set(m, ptid);
        audit.exit(status);

	return status;
}


char *
tt_message_session(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_SESSION, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_session(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_session_set(Tt_message m, const char *sessid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_SESSION_SET, m,
					    sessid);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_session_set(m, sessid);
        audit.exit(status);

	return status;
}


Tt_state
tt_message_state(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_STATE, m);
	Tt_state result;

        if (status != TT_OK) {
		audit.exit((Tt_state)error_int(TT_ERR_POINTER));
		return (Tt_state)error_int(TT_ERR_POINTER);
        }

        result = _tt_message_state(m);
        audit.exit(result);

	return result;
}


char *
tt_message_status_string(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_STATUS_STRING, m);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }
 
        result = _tt_message_status_string(m);
        audit.exit(result);
 
        return result;
}


Tt_status
tt_message_status_string_set(Tt_message m, const char *status_str)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("MC", TT_MESSAGE_STATUS_STRING_SET, m,
					status_str);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_status_string_set(m, status_str);
        audit.exit(status);

	return status;
}


int
tt_message_status(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_STATUS, m);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

	result = _tt_message_status(m);
	// Policy is to use a Tt_status for message status
        audit.exit((Tt_status)result);

	return result;
}


Tt_status
tt_message_status_set(Tt_message m, int status_val)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Mi", TT_MESSAGE_STATUS_SET, m,
					status_val);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_message_status_set(m, status_val);
        audit.exit(status);

	return status;
}


uid_t
tt_message_uid(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_UID, m);
	uid_t result;

        if (status != TT_OK) {
		audit.exit((int) uid_nobody);
                return uid_nobody;
        }

	result = _tt_message_uid(m);
        audit.exit((int) result);

	return result;
}


gid_t
tt_message_gid(Tt_message m)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("M", TT_MESSAGE_GID, m);
	gid_t result;

        if (status != TT_OK) {
		audit.exit((int) gid_nobody);
                return gid_nobody;
        }

	result = _tt_message_gid(m);
        audit.exit((int) result);

	return result;
}


// print the message the user has passed in into a buffer and
// pass it back to the user.
char *
_tt_message_print(Tt_message m)
{
        _Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string buf;

	msg->print(buf);

	return _tt_strdup(buf);
}


Tt_message
_tt_message_create()
{
	Tt_message result;
	_Tt_c_message_ptr msg = new _Tt_c_message();
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	// NOTE: duplicate any changes in the message macro functions below

	msg->set_state(TT_CREATED);
	msg->set_sender(_tt_c_mp->default_procid());
	// sender MUST be set before set_id() is called
	msg->set_id();

	// set defaults
	msg->set_file(d_procid->default_file());
	msg->set_session(d_procid->default_session());
	msg->set_sender_ptype(d_procid->default_ptype());

	result = _tt_htab->lookup_mhandle(msg);
	return result;
}
/* 
 * Creates a message to the parent of the otype for the given message
 */
Tt_message
_tt_message_create_super(Tt_message m)
{
	_Tt_c_message_ptr	msg;

	msg = _tt_htab->lookup_msg(m);
	Tt_address mode = msg->paradigm();
	if (mode != TT_OBJECT && mode != TT_OTYPE) {
		return (Tt_message)error_pointer(TT_ERR_ADDRESS);
	}
	if (mode == TT_OBJECT) {
		if (msg->object().len() == 0) {
			return (Tt_message)error_pointer(TT_ERR_OBJID);
		}
	}
	if (msg->otype().len() == 0) {
		return (Tt_message)error_pointer(TT_ERR_OTYPE);
	}

	_Tt_c_message_ptr	super_msg = new _Tt_c_message;
	super_msg->set_state(TT_CREATED);
	super_msg->set_sender(_tt_c_mp->default_procid());
	super_msg->set_id();
	super_msg->set_message_class(msg->message_class());
	super_msg->set_paradigm(msg->paradigm());
	super_msg->set_op(msg->op());
	if (mode == TT_OBJECT) {
		super_msg->set_object(msg->object());
	}
	super_msg->set_otype(msg->otype());
	if (! msg->args().is_null()) {
		_Tt_arg_list_cursor msg_argc(msg->args());
		_Tt_arg_ptr arg;
		while (msg_argc.next()) {
			arg = new _Tt_arg(*msg_argc);
			super_msg->add_arg(arg);
		}
	}
	super_msg->set_super();
	return _tt_htab->lookup_mhandle(super_msg);
}

/* 
 * Destroys the message object associated with the handle m.
 * (Really, all we do is clear the pointer from _tt_htab, and the
 * refcount mechanism takes care of deleting the underlying _Tt_c_message.)
 */
Tt_status
_tt_message_destroy(Tt_message m)
{
	_Tt_c_message_ptr	msg;
	Tt_state		mstate;
	Tt_class		mclass;

	_tt_message_abstain(m);
	msg = _tt_htab->lookup_msg(m);

	// we only destroy the message if it's a request in a final
	// state or a notification. This is done because message
	// transmission is optimized if the server thinks that a
	// client has seen a message before. In this case only the
	// parts of the message that have changed are sent as an
	// update so if we destroy a request in a non-final state then
	// we would lose the non-changing parts of the message when
	// the update comes back.

	mstate = msg->state();
	mclass = msg->message_class();
	//
	// tttk uses hint of whether destruction actually occurred
	//
	if (msg->is_awaiting_reply()) {
		return TT_WRN_STOPPED;
	}
	if (   mclass == TT_REQUEST && mstate != TT_REJECTED
	    && mstate != TT_HANDLED && mstate != TT_FAILED)
	{
		return TT_WRN_STOPPED;
	}
	if (   mclass == TT_OFFER && mstate != TT_RETURNED
	    && mstate != TT_REJECTED && mstate != TT_ACCEPTED
	    && mstate != TT_ABSTAINED)
	{
		return TT_WRN_STOPPED;
	}
	//
	// If we get here, it means that we cannot get an update
	// to this message, so it is OK to throw away our copy of it.
	//
	_tt_htab->clear(m);
	return TT_OK;
}


/* 
 * Sending and receiving (FSpec A.6.2)
 */
/* 
 * Sends the message pointed to by the message handle m.
 */
Tt_status
_tt_message_send(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_c_procid		*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	
	if (msg->session().is_null()) {
		msg->set_session(d_procid->default_session());
	}
	PCOMMIT;
	
	return msg->dispatch();
}


/* 
 * Tells ttsession to send the message pointed to by the message handle m if
 * this process exits without calling tt_close.
 */
Tt_status
_tt_message_send_on_exit(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	Tt_address		msg_addr;
	Tt_scope		msg_scope;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	
	
	msg_addr = msg->paradigm();
	if (!(msg_addr == TT_HANDLER ||
	      msg_addr == TT_PROCEDURE)) {
		return TT_ERR_ADDRESS;
	}
	
	if (msg->state() != TT_CREATED) {
		return TT_ERR_STATE;
	}
	
	msg_scope = msg->scope();
	if (!(msg_scope == TT_SESSION ||
	      msg_scope == TT_FILE_IN_SESSION)) {
		return TT_ERR_SCOPE;
	}
	
	if (msg->session().is_null()) {
		msg->set_session(d_procid->default_session());
	}
	PCOMMIT;
	
	return msg->dispatch_on_exit();
}

/* 
* If there is a message for this process, then a handle to the message
* is returned. If the message is a reply to a message sent by this
* process then the handle will be the same as the one used to send the
* message. If there are no message, 0 is returned. The argument
* more_waiting indicates that in addition to the message returned, there
* are other messages for the process. This can be used to retrieve all
* the pending messages.
*/
Tt_message
_tt_message_receive(void)
{
	Tt_message 	result;
	Tt_pattern	pat_matched;
	_Tt_c_message_ptr	m;
	Tt_status	status;
	_Tt_c_procid	*d_procid = _tt_c_mp->default_c_procid().c_pointer();
	
	status = d_procid->next_message(m);
	
	if (status != TT_OK) {
		return (Tt_message)error_pointer(status);
	}
	if (m.is_null()) {
		return 0;
	}
	result = _tt_htab->lookup_mhandle(m);
	if (result == (Tt_message)0) {
		return 0;
	}

	// trace updated message, not incoming diffs
	_Tt_c_message_ptr msg2trace = _tt_htab->lookup_msg(result);
	_Tt_msg_trace trace( *msg2trace, *d_procid );

	pat_matched = _tt_message_pattern(result);
	
	if (TT_CALLBACK_PROCESSED==
	    _tt_htab->run_message_callbacks(result, pat_matched)) {
		return 0;
	}
	
	if (TT_CALLBACK_PROCESSED==
	    _tt_htab->run_pattern_callbacks(pat_matched, result)) {
		return 0;
	}

	_Tt_string ptype, otype;
	int opnum;
	
	ptype = m->handler_ptype();
	otype = m->otype();
	opnum = m->opnum();

	if (_tt_run_opnum_callbacks(result, pat_matched,
				    ptype, opnum,
				    d_procid->ptype_callbacks()) ||
	    _tt_run_opnum_callbacks(result, pat_matched,
				    otype, opnum,
				    d_procid->otype_callbacks())) {
		return 0;
	}
	
	// No callback volunteered to swallow the message, so
	// return it to the caller of tt_message_receive.
	return result;
}

// Run the ptype or otype opnum callbacks.  Return 1 if a callback
// returned TT_CALLBACK_PROCESSED, else 0.
static int
_tt_run_opnum_callbacks(Tt_message msg,
			Tt_pattern pat,
			_Tt_string &typenm,
			int opnum,
			_Tt_typecb_table_ptr &tcbt)
{
	_Tt_string key;
	if (typenm.len()>0 && opnum!=-1) {
		_Tt_typecb_ptr tcb;
		if (!tcbt.is_null()) {
			key = _tt_typecb_key(typenm, opnum);
			tcb = tcbt->lookup(key);
			if (!tcb.is_null()) {
				if (TT_CALLBACK_PROCESSED ==
				    tcb->run_callbacks(msg, pat)) {
					return 1;
				}
			}
		}
	}
	return 0;
}


/* 
 * Sets a callback on message m. 
 */
Tt_status
_tt_message_callback_add(Tt_message m, Tt_message_callback f)
{
	return _tt_htab->add_callback(m,f);
}


/* 
 * Informs the message server that this process is not willing to handle
 * the computation associated with this message. This causes the message
 * server to find another handler process for the message.
 */
Tt_status
_tt_message_reject(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_c_procid		*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	return d_procid->update_message(msg,TT_REJECTED);
}


/* 
 * When the process has returned any results in the message args it uses
 * this function to return the results to the sender.
 */
Tt_status
_tt_message_reply(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_c_procid		*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	return d_procid->update_message(msg, TT_HANDLED);
}


/* 
 * Informs the message server that the computation associated with a message
 * could not be completed.
 */
Tt_status
_tt_message_fail(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_c_procid		*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	return d_procid->update_message(msg, TT_FAILED);
}


Tt_status
_tt_message_abstain(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_c_procid		*d_procid = _tt_c_mp->default_c_procid().c_pointer();

	return d_procid->update_message(msg, TT_ABSTAINED);
}


/* 
 * When the process has set any patterns and is initialized,
 * it uses this function to unblock the ptype so held messages can
 * be delivered.
 */
Tt_status
_tt_message_accept(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	Tt_status status = TT_OK;

	if (msg->message_class() == TT_OFFER) {
		status = _tt_c_mp->default_c_procid()
				->update_message(msg, TT_ACCEPTED);
	}

	if (! msg->is_start_message()) {
		// if the message is not a start message, just return OK
		// as the server doesn\'t have to even know about this.
		return status;
	}
	// Oddly enough, to the server this operation is on the ptype,
	// not the message; the server doesn\'t really care which message
	// is involved, it just needs to unblock the ptype.
	Tt_status unblock_status = _tt_c_mp->default_c_procid()
					->unblock_ptype(msg->handler_ptype());
	if (status != TT_OK) {
		return status;
	} else {
		return unblock_status;
	}
}


/* 
 * Returns a handle for the registered pattern that matched message m or 0
 * if either the message hasn't matched a pattern (e.g. it's in SENT state)
 * or if the pattern isn't in the API pattern handle table (this occurs if
 * the pattern it matched was generated as a result of joining a file or
 * declaring a ptype.)
 */
Tt_pattern
_tt_message_pattern(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	Tt_pattern result;

	result = _tt_msg_pat(msg);
	return result;
}


/*
 * tt_message_receive must do the function of tt_message_pattern, but
 * can skip mapping the message handle to a _Tt_c_message_ptr since
 * it already has one.
 */
static Tt_pattern
_tt_msg_pat(_Tt_c_message_ptr &msg)
{
	Tt_pattern result;
	if (msg.is_null()) {
		result = (Tt_pattern)error_pointer(TT_ERR_POINTER);
	} else {
		_Tt_string pat_id;
		_Tt_pattern_ptr pat;

		pat_id = msg->pattern_id();
		if (pat_id.len()==0) {
			return 0;
		}
		pat = _tt_htab->lookup_pat_by_id(pat_id, result);
	}
	return result;
}


/* 
 * Returns user data cell identified with key for the message handle m.
 * (void *)0 is returned if there is no data cell associated with key.
 */
void *
_tt_message_user(Tt_message m, int key)
{
	return _tt_htab->fetch(m,key);
}


/* 
 * Sets the user data cell identified with key in message handle m to v.
 */
Tt_status
_tt_message_user_set(Tt_message m,int key, void *v)
{
	return _tt_htab->store(m,key,v);
}


/* 
 * Message Arguments (FSpec A.6.3)
 */


/* 
 * Adds a new argument to a message object. vtype is the name of a valid
 * value type. Use NULL for values of mode out.
 */
Tt_status
_tt_message_arg_add(Tt_message m, Tt_mode n, const char *vt, const char *value)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr arg;
	_Tt_string vtype;

	vtype = vt;
	arg = new _Tt_arg(n, vtype);

	arg->set_data_string(value);
	return msg->add_arg(arg);
}


/* 
 * Adds a new bytestring argument to a message object. vtype is the name of
 * a valid value type. Use NULL for values of mode out.
 */
Tt_status
_tt_message_barg_add(Tt_message m, Tt_mode n, const char *vtype, 
		     const unsigned char *value, int len)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr arg;
	_Tt_string s(value, len);

	arg = new _Tt_arg(n, vtype);
	arg->set_data_string(s);
	return msg->add_arg(arg);
}


/* 
 * Returns the value of the nth message argument as a bytestring
 */
Tt_status
_tt_message_arg_bval(Tt_message m, int n, unsigned char **v, int *l)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr	arg;
	_Tt_string	val;
	Tt_status	ttrc;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	ttrc = arg->data_string(val);
	if (ttrc != TT_OK)  {
		return ttrc;
	}
	*l = val.len();
	*v = (unsigned char *)_tt_strdup(val, *l);
	return TT_OK;
}


/* 
 * Sets the bytestring value of the nth message argument.
 */
Tt_status
_tt_message_arg_bval_set(Tt_message m, int n, const unsigned char *v, int l)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr	arg;
	_Tt_string	val(v,l);
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	return arg->set_data_string(val);
}


/* 
 * Adds a new integer argument to a message object. vtype is the name of
 * a valid value type. 
 */
Tt_status
_tt_message_iarg_add(Tt_message m, Tt_mode n, const char *vtype, 
		     int value)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr arg;

	arg = new _Tt_arg(n, vtype);
	arg->set_data_int(value);
	return msg->add_arg(arg);
}


/* 
 * Returns the value of the nth message argument as an integer
 */
Tt_status
_tt_message_arg_ival(Tt_message m, int n, int *v)
{
	_Tt_c_message_ptr 	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	_Tt_string		val;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	return arg->data_int(*v);
}


/* 
 * Sets the integer value of the nth message argument.
 */
Tt_status
_tt_message_arg_ival_set(Tt_message m, int n, int v)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	return arg->set_data_int(v);
}


/* 
 * Returns the mode of the nth message argument.
 */
Tt_mode
_tt_message_arg_mode(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	return arg->mode();
}

/* 
 * Returns the type of the nth message argument.
 */
char *
_tt_message_arg_type(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg->args())[n];
	return _tt_strdup(arg->type());
}

/* 
 * Returns the value of the nth message argument as a character string.
 */
char *
_tt_message_arg_val(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	_Tt_string		v;
	char			*value;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg_args)[n];
	if (TT_OK!=arg->data_string(v)) {
		value = (char *)error_pointer(TT_ERR_POINTER);
	} else {
		value = (char *)_tt_strdup(v);
	}

	return value;
}


/* 
 * Sets the value of the nth message argument as a character string
 */
Tt_status
_tt_message_arg_val_set(Tt_message m, int n, const char *value)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_arg_ptr		arg;
	Tt_status		retval;
	_Tt_arg_list_ptr	msg_args(msg->args());

	arg = (*msg_args)[n];

	retval = arg->set_data_string((char *)value);
	return retval;
}


/* 
 * Returns the number of arguments in the message.
 */
int
_tt_message_args_count(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);

	if (msg->args().is_null()) {
		return 0;
	} else {
		return msg->args()->count();
	}
}

/*
 * XDR'd Message args
 */

/* 
 * Adds a new argument with an XDR'ed value to a message object.
 */
Tt_status
_tt_message_xarg_add(Tt_message m, Tt_mode n, const char *vtype, 
		    xdrproc_t xdr_proc, void *value)
{
	_Tt_c_message_ptr	msg_p = _tt_htab->lookup_msg(m);

	// Encode the XDR arg or return failure.
	_Tt_string xdr_arg;

	if (_tt_xdr_encode(xdr_proc, value, xdr_arg) == 0) {
		return TT_ERR_XDR;
	}

	// create a new arg and set the arg value.
        _Tt_arg_ptr arg_p;

        arg_p = new _Tt_arg(n, vtype);
        arg_p->set_data_string(xdr_arg);
        return msg_p->add_arg(arg_p);
}


/* 
 * Retrieves and deserializes the data from a message argument.
 */
Tt_status
_tt_message_arg_xval(Tt_message m, int n, xdrproc_t xdr_proc, void *value)
{
	_Tt_c_message_ptr	msg_p = _tt_htab->lookup_msg(m);
        _Tt_arg_list_ptr        msg_args(msg_p->args());

	// get the specified arg and XDR decode it.
        _Tt_string	xdr_arg;
	_Tt_arg_ptr	arg;
	Tt_status	ttrc;

	arg = (*msg_p->args())[n];
	ttrc = arg->data_string(xdr_arg);
	if (ttrc != TT_OK) {
		return ttrc;
	}

	if (_tt_xdr_decode(xdr_proc, value, xdr_arg) == 0) {
		return TT_ERR_XDR;
	}

        return TT_OK;
}


/* 
 * Serializes and sets data into an existing message argument.
 */
Tt_status
_tt_message_arg_xval_set(Tt_message m, int n, xdrproc_t xdr_proc, void *value)
{
	_Tt_c_message_ptr	msg_p = _tt_htab->lookup_msg(m);
        _Tt_arg_list_ptr        msg_args(msg_p->args());
 
        // Encode the XDR arg or return failure.
        _Tt_string	xdr_arg;
	_Tt_arg_ptr	argp;

        if (_tt_xdr_encode(xdr_proc, value, xdr_arg) == 0) {
                return TT_ERR_XDR;
        }

        // create a new arg and set the arg value.
        argp = (*msg_p->args())[n];
        return argp->set_data_string(xdr_arg);
}


/* 
 * Adds a new argument with an XDR'ed value to a message object.
 */
Tt_status
_tt_message_context_xval(Tt_message m, const char *slotname,
			xdrproc_t xdr_proc, void *value)
{
        _Tt_c_message_ptr	msg_p = _tt_htab->lookup_msg(m);
        _Tt_msg_context_ptr cntxt = msg_p->context(slotname);

        // retrieve the specified context
        Tt_status status;
        _Tt_string context_value;

        status = cntxt->value().data_string(context_value);
        if (status != TT_OK) {
                return status;
        }

	// XDR decode the retrieved context.
        if (_tt_xdr_decode(xdr_proc, value, context_value) == 0) {
                return TT_ERR_XDR;
        }

        return TT_OK;
}


/* 
 * XDR equivalent of tt_message_bcontext_set.
 */
Tt_status
_tt_message_xcontext_set(Tt_message m, const char *slotname,
			xdrproc_t xdr_proc, void *value)
{
	_Tt_c_message_ptr	msg_p = _tt_htab->lookup_msg(m);
	Tt_status		status;
        int			add = 0;


        _Tt_msg_context_ptr cntxt_p = msg_p->context(slotname);

        if (cntxt_p.is_null()) {
                cntxt_p = new _Tt_msg_context;
                if (cntxt_p.is_null()) {
                        return TT_ERR_NOMEM;
                }
                status = cntxt_p->setName(slotname);
                if (status != TT_OK) {
                        return status;
                }
                add = 1;
        }

        // Encode the XDR context or return failure.
        _Tt_string      xdr_context;

        if (_tt_xdr_encode(xdr_proc, value, xdr_context) == 0) {
                return TT_ERR_XDR;
        }

	// add/store the XDR'ed context
        status = cntxt_p->setValue(xdr_context);
        if (status != TT_OK) {
                return status;
        }
        if (add) {
                return msg_p->add_context(cntxt_p);
        }
        return status;
}


/*
 * Contexts
 */

/*
 * Returns the string value associated with the slot name in this message.
 */
char *
_tt_message_context_val(Tt_message m, const char *slotname)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string		value;
	char		       *returnVal;
	_Tt_msg_context_ptr cntxt = msg->context( slotname );

	Tt_status status = cntxt->value().data_string( value );

	if (status != TT_OK) {
		returnVal = (char *)error_pointer(TT_ERR_POINTER);
	} else {
		returnVal = (char *)_tt_strdup( value );
	}
	return returnVal;
}


/*
 * Returns the integer value associated with the slot name in this message.
 */
Tt_status
_tt_message_context_ival(Tt_message m, const char *slotname, int *value)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_msg_context_ptr cntxt = msg->context( slotname );

	return cntxt->value().data_int( *value );
}


/*
 * Returns the byte-vector value associated with the slot name in this message.
 */
Tt_status
_tt_message_context_bval(Tt_message m, const char *slotname,
			unsigned char **val, int *len)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string		value;
	_Tt_msg_context_ptr	cntxt = msg->context(slotname);
	Tt_status		status = cntxt->value().data_string(value);

	if (status != TT_OK) {
		return status;
	}
	*len = value.len();
	*val = (unsigned char *)_tt_strdup( value, *len );
	if (*val == NULL) {
		return TT_ERR_NOMEM;
	}
	return TT_OK;
}


/*
 * Set a context to a string value.
 */
Tt_status
_tt_message_context_set(Tt_message m, const char *slotname, const char *value)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	Tt_status		status;

	int add = 0;
	_Tt_msg_context_ptr cntxt = msg->context( slotname );

	if (cntxt.is_null()) {
		cntxt = new _Tt_msg_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName( slotname );
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	_Tt_string valString( value );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}
	if (add) {
		return msg->add_context( cntxt );
	}
	return status;
}


/*
 * Set a context to an integer value.
 */
Tt_status
_tt_message_icontext_set(Tt_message m, const char *slotname, int value)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	Tt_status		status;


	int add = 0;
	_Tt_msg_context_ptr cntxt = msg->context( slotname );
	if (cntxt.is_null()) {
		cntxt = new _Tt_msg_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName( slotname );
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	status = cntxt->setValue( value );
	if (status != TT_OK) {
		return status;
	}
	if (add) {
		return msg->add_context( cntxt );
	}
	return status;
}


/*
 * Set a context to a byte-array value.
 */
Tt_status
_tt_message_bcontext_set(Tt_message m, const char *slotname,
			const unsigned char *value, int len)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	Tt_status		status;

	int add = 0;
	_Tt_msg_context_ptr cntxt = msg->context( slotname );
	if (cntxt.is_null()) {
		cntxt = new _Tt_msg_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName( slotname );
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	_Tt_string valString( value, len );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}
	if (add) {
		return msg->add_context( cntxt );
	}
	return status;
}


/*
 * Return the number of contexts.
 */
int
_tt_message_contexts_count(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->contextsCount();
}


/*
 * Return the slotname of the nth context.
 */
char *
_tt_message_context_slotname(Tt_message m, int i)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	char		       *returnVal;
	_Tt_msg_context_ptr	cntxt = msg->context( i );

	if (cntxt.is_null()) {
		return (char *)error_pointer(TT_ERR_NUM);

	}

	returnVal = (char *)_tt_strdup( cntxt->slotName() );

	if (returnVal == 0) {
		return (char *)error_pointer(TT_ERR_NOMEM);
	}
	return returnVal;
}


/* 
 * Attributes (FSpec A.6.4)
 */

/* 
 * Returns the message class associated with m.
 */
Tt_class
_tt_message_class(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->message_class();
}


/* 
 * Sets the class of message pointed to by m.
 */
Tt_status
_tt_message_class_set(Tt_message m, Tt_class c)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_message_class(c);
}


/* 
 * Returns the file field of message m.
 */
char *
_tt_message_file(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	return _tt_strdup(_tt_network_path_to_local_path(msg->file()));
}


/* 
 * Sets the file field of message m.
 */
Tt_status
_tt_message_file_set(Tt_message m, const char *filepath)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string		abspath;
	Tt_scope		scope = msg->scope();

	if ((scope == TT_FILE) || (scope == TT_BOTH)) {
 		abspath = _Tt_db_file::getNetworkPath(filepath);
	}
	else {
 		abspath = _tt_local_network_path(filepath);
	}

        if ((filepath != 0) && (abspath.len() <= 0)) {
		return TT_ERR_FILE;
	}
	return msg->set_file(abspath);
}


/* 
 * Returns the object field for message m
 */
char *
_tt_message_object(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string		obj = msg->object();

	if (obj.len() == 0) {
		return (char *)error_pointer(TT_ERR_OBJID);
	}
	return _tt_strdup(obj.quote_nulls());
}


/* 
 * Sets the object field for message m
 */
Tt_status
_tt_message_object_set(Tt_message m, const char * objid)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_string		oid;

	oid = objid;
	return(msg->set_object(oid.unquote_nulls()));
}


/* 
 * Returns the id of m.
 */
char *
_tt_message_id(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->api_id());
}


/* 
 * Returns the op associated with m.
 */
char *
_tt_message_op(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->op());
}


/* 
 * Set the message op to op.
 */
Tt_status
_tt_message_op_set(Tt_message m, const char *opname)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->set_op(opname));
}


/* 
 * Return the opnum associated with m.
 */
int
_tt_message_opnum(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->opnum());
}


/* 
 * Return the otype of the object field associated with m.
 */
char *
_tt_message_otype(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	if (msg->otype().len() == 0) {
		return((char *)0);
	}	

	return _tt_strdup(msg->otype());
}


/* 
 * Set the otype of message m.
 */
Tt_status
_tt_message_otype_set(Tt_message m, const char * otype)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	Tt_status rc;

	if (TT_WRN_LAST < (rc = _tt_valid_otype(otype))) {
		return rc;
	}
	return(msg->set_otype(otype));
}


/* 
 * Return the paradigm of a message.
 */
Tt_address
_tt_message_address(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->paradigm());
}


/* 
 * Set the paradigm of a message.
 */
Tt_status
_tt_message_address_set(Tt_message m, Tt_address p)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->set_paradigm(p));
}


/* 
 * Return the handler of message m
 */
char *
_tt_message_handler(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	if (msg->handler().is_null()) {
		return((char *)0);
	}

	return _tt_strdup(msg->handler()->id());
}


/* 
 * Set the handler procid for message m.
 */
Tt_status
_tt_message_handler_set(Tt_message m,const char * procid)
{
	_Tt_string sprocid(procid);

	if (sprocid.len() == 0)	{
		return TT_ERR_PROCID;
	}

	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_procid_ptr		p = new _Tt_c_procid(sprocid);

	return(msg->set_handler_procid(p));
}


/* 
 * Return the handler ptype for message m.
 */
char *
_tt_message_handler_ptype(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->handler_ptype());
}


/* 
 * Set the handler ptype for message m.
 */
Tt_status
_tt_message_handler_ptype_set(Tt_message m, const char * ptid)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_handler_ptype(ptid);
}


int
_tt_message_accepters_count(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);

	if (msg->accepters().is_null()) {
		return 0;
	} else {
		return msg->accepters()->count();
	}
}


char *
_tt_message_accepter(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_procid_ptr		accepter;

	if (msg->accepters()->count() <= n) {
		return (char *)error_pointer(TT_ERR_NUM);
	}
	accepter = (*msg->accepters())[n];
	return _tt_strdup(accepter->id());
}


int
_tt_message_abstainers_count(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);

	if (msg->abstainers().is_null()) {
		return 0;
	} else {
		return msg->abstainers()->count();
	}
}


char *
_tt_message_abstainer(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_procid_ptr		abstainer;

	if (msg->abstainers()->count() <= n) {
		return (char *)error_pointer(TT_ERR_NUM);
	}
	abstainer = (*msg->abstainers())[n];
	return _tt_strdup(abstainer->id());
}


int
_tt_message_rejecters_count(Tt_message m)
{
	_Tt_c_message_ptr msg = _tt_htab->lookup_msg(m);

	if (msg->rejecters().is_null()) {
		return 0;
	} else {
		return msg->rejecters()->count();
	}
}


char *
_tt_message_rejecter(Tt_message m, int n)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_procid_ptr		rejecter;

	if (msg->rejecters()->count() <= n) {
		return (char *)error_pointer(TT_ERR_NUM);
	}
	rejecter = (*msg->rejecters())[n];
	return _tt_strdup(rejecter->id());
}


/* 
 * Return the reliability flags for message m.
 */
Tt_disposition
_tt_message_disposition(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->reliability());
}


/* 
 * Set the reliability flags for message m.
 */
Tt_status
_tt_message_disposition_set(Tt_message m,Tt_disposition r)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->set_reliability(r));
}


/* 
 * Return the scope field for message m.
 */
Tt_scope
_tt_message_scope(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->scope());
}


/* 
 * Set the scope for message m.
 */
Tt_status
_tt_message_scope_set(Tt_message m, Tt_scope s)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_scope(s);
}


/* 
 * Return the sender procid for message m.
 */
char *
_tt_message_sender(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->sender()->id());
}


/* 
 * Return the sender ptype for message m.
 */
char *
_tt_message_sender_ptype(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->sender_ptype());
}


/* 
 * Set the sender ptype for message m.
 */
Tt_status
_tt_message_sender_ptype_set(Tt_message m, const char *ptid)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_sender_ptype(ptid);
}


/* 
 * Return the session procid for message m.
 */
char *
_tt_message_session(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return _tt_strdup(msg->session()->id());
}


/* 
 * Set the session procid for message m.
 */
Tt_status
_tt_message_session_set(Tt_message m, const char *sessid)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);
	_Tt_session_ptr		sptr;
	_Tt_string		strsessid = (char *)0;
	Tt_status		status;

	// Add the P on if isn't already.
	_prepend_P_to_sessid(sessid, strsessid);

	status = _tt_c_mp->find_session(strsessid, sptr, 1);
	if (TT_OK != status) return TT_ERR_SESSION;

	return(msg->set_session(sptr));
}


/* 
 * Return the state of message m.
 */
Tt_state
_tt_message_state(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->state());
}


/* 
 * Return the status of message m.
 */
char *
_tt_message_status_string(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(_tt_strdup(msg->status_string()));
}


/* 
 * Set the status for message m.
 */
Tt_status
_tt_message_status_string_set(Tt_message m, const char *status_str)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_status_string(status_str);
}


/* 
 * Return the status of message m.
 */
int
_tt_message_status(Tt_message m)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return(msg->status());
}


/* 
 * Set the status for message m.
 */
Tt_status
_tt_message_status_set(Tt_message m, int status)
{
	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	return msg->set_status(status);
}


/* 
 * Return the uid of the sender of message m, or "nobody" if error.
 */
uid_t
_tt_message_uid(Tt_message m)
{
	if (_tt_pointer_error(m)!=TT_OK) {
		return uid_nobody;
	}

	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	if (msg.is_null()) {
		return uid_nobody;
	}

	return(msg->uid());
}


/* 
 * Return the gid of the sender of message m, or "nobody" if error.
 */
gid_t
_tt_message_gid(Tt_message m)
{
	if (_tt_pointer_error(m)!=TT_OK) {
		return gid_nobody;
	}

	_Tt_c_message_ptr	msg = _tt_htab->lookup_msg(m);

	if (msg.is_null()) {
		return gid_nobody;
	}

	return(msg->gid());
}
