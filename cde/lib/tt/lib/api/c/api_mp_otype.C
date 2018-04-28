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
//%%  $XConsortium: api_mp_otype.C /main/4 1995/11/21 19:34:39 cde-sun $ 			 				
/*
 *
 * mp_api_otype.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains API functions related to the MP. For
 * each API function named tt_<name> there is a _tt_<name> function in
 * some file named api_mp_*.cc.
 */

#include "mp/mp_c.h"
#include "mp/mp_rpc_interface.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "util/tt_audit.h"

/*
 * These functions differ from the usual layering in the API/MP interface.
 * Usually, we would realize the _Tt_otype and _Tt_otype_table objects on
 * the client side, with the methods split up so the client side makes
 * RPC calls and the server side does the work.  In this case, this would
 * end up caching most of the otype information in the client side, which
 * would give us cache-coherency problems we don't have time to deal with
 * right now.  So we have a simpler interface of the API routines just
 * making RPC calls to the server. RFM 11/7/90.
 */

extern _Tt_c_mp *_tt_c_mp;


int
tt_otype_deriveds_count(const char *otype)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("o", TT_OTYPE_DERIVEDS_COUNT, otype);
	int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_otype_deriveds_count(otype);
        audit.exit(result);

	return result;
}


char *
tt_otype_derived(const char *otype, int i)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_OTYPE_DERIVED, otype, i);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_otype_derived(otype, i);
        audit.exit(result);

	return result;
}


char *
tt_otype_base(const char *otype)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("o", TT_OTYPE_BASE, otype);
        char *result;
 
        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_otype_base(otype);
        audit.exit(result);

        return result;
}


int
tt_otype_is_derived(const char *derivedotype, const char *baseotype)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oo", TT_OTYPE_IS_DERIVED,
				       derivedotype, baseotype);
        int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_otype_is_derived(derivedotype, baseotype);
        audit.exit(result);

        return result;
}


int
tt_otype_osig_count(const char *otype)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("o", TT_OTYPE_OSIG_COUNT, otype);
        int result;
 
        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }
 
        result = _tt_otype_osig_count(otype);
        audit.exit(result);
 
        return result;
}


int
tt_otype_hsig_count(const char *otype)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("o", TT_OTYPE_HSIG_COUNT, otype);
        int result; 
  
        if (status != TT_OK) { 
		audit.exit(error_int(status));
                return error_int(status);
        } 
  
        result = _tt_otype_hsig_count(otype);
        audit.exit(result); 
  
        return result;
}


char *
tt_otype_osig_op(const char *otype, int sig)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_OTYPE_OSIG_OP, otype, sig);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_otype_osig_op(otype, sig);
        audit.exit(result);

        return result;
}


char *
tt_otype_hsig_op(const char *otype, int sig)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_OTYPE_HSIG_OP, otype, sig); 
        char *result;
 
        if (status != TT_OK) { 
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status); 
        } 
 
        result = _tt_otype_hsig_op(otype, sig);
        audit.exit(result); 
 
        return result; 
}


int
tt_otype_osig_args_count(const char *otype, int sig)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_OTYPE_OSIG_ARGS_COUNT, otype, sig);
        int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_otype_osig_args_count(otype, sig);
        audit.exit(result);

        return result;
}


int
tt_otype_hsig_args_count(const char *otype, int sig)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oi", TT_OTYPE_HSIG_ARGS_COUNT, otype, sig);
        int result; 
 
        if (status != TT_OK) { 
		audit.exit(error_int(status));
                return error_int(status); 
        } 
 
        result = _tt_otype_hsig_args_count(otype, sig);
        audit.exit(result); 
 
        return result; 
}


Tt_mode
tt_otype_osig_arg_mode(const char *otype, int sig, int arg)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oii", TT_OTYPE_OSIG_ARG_MODE, otype, sig, arg);
        Tt_mode result;

        if (status != TT_OK) {
		audit.exit((Tt_mode)error_int(status));
		return (Tt_mode) error_int(status);
        }

        result = _tt_otype_osig_arg_mode(otype, sig, arg);
        audit.exit(result);

        return result;
}


Tt_mode
tt_otype_hsig_arg_mode(const char *otype, int sig, int arg)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oii", TT_OTYPE_HSIG_ARG_MODE, otype, sig, arg);
        Tt_mode result;

        if (status != TT_OK) {
		audit.exit((Tt_mode)error_int(status));
                return (Tt_mode)error_int(status);
        }

        result = _tt_otype_hsig_arg_mode(otype, sig, arg);
        audit.exit(result);

        return result;
}


char *
tt_otype_osig_arg_type(const char *otype, int sig, int arg)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oii", TT_OTYPE_OSIG_ARG_TYPE, otype, sig, arg);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_otype_osig_arg_type(otype, sig, arg);
        audit.exit(result);

        return result;
}


char *
tt_otype_hsig_arg_type(const char *otype, int sig, int arg)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("oii", TT_OTYPE_HSIG_ARG_TYPE, otype, sig, arg);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_otype_hsig_arg_type(otype, sig, arg);
        audit.exit(result);

        return result;
}


/*
 * otype retrieval functions
 */

int
_tt_otype_deriveds_count(const char * otype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	d_session->call(TT_RPC_OTYPE_DERIVEDS_COUNT,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return error_int(result.status);
	} else {
		return result.int_val;
	}
}


char *
_tt_otype_derived(const char * otype, int i)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session   *d_session;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();

	args.base_otid = otype;
	args.num = i;
	d_session->call(TT_RPC_OTYPE_DERIVED,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {
		return _tt_strdup(result.str_val);
	}
}

char *
_tt_otype_base(const char *otype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.derived_otid = otype;
	d_session->call(TT_RPC_OTYPE_BASE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {	
		return _tt_strdup(result.str_val);
	}
}


int
_tt_otype_is_derived(const char *derivedotype,
		     const char *baseotype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();


	args.derived_otid = derivedotype;
	args.base_otid = baseotype;
	
	d_session->call(TT_RPC_OTYPE_IS_DERIVED,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return 0;	// Predicates that return failure are a pain
	} else {
		return result.int_val;
	}
}


int
_tt_otype_osig_count(const char *otype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	
	d_session->call(TT_RPC_OTYPE_OSIG_COUNT,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return error_int(result.status);
	} else {
		return result.int_val;
	}
}


int
_tt_otype_hsig_count(const char *otype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	
	d_session->call(TT_RPC_OTYPE_HSIG_COUNT,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return error_int(result.status);
	} else {
		return result.int_val;
	}
}


char *
_tt_otype_osig_op(const char *otype, int sig)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	
	d_session->call(TT_RPC_OTYPE_OSIG_OP,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {
		return _tt_strdup(result.str_val);
	}
}


char *
_tt_otype_hsig_op(const char *otype, int sig)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	
	d_session->call(TT_RPC_OTYPE_HSIG_OP,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {
		return _tt_strdup(result.str_val);
	}
}


int
_tt_otype_osig_args_count(const char *otype, int sig)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	
	d_session->call(TT_RPC_OTYPE_OSIG_ARGS_COUNT,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return error_int(result.status);
	} else {
		return result.int_val;
	}
}


int
_tt_otype_hsig_args_count(const char *otype, int sig)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	
	d_session->call(TT_RPC_OTYPE_HSIG_ARGS_COUNT,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return error_int(result.status);
	} else {
		return result.int_val;
	}
}


Tt_mode
_tt_otype_osig_arg_mode(const char *otype, int sig, int arg)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	args.num2 = arg;
	
	d_session->call(TT_RPC_OTYPE_OSIG_ARG_MODE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (Tt_mode)error_int(result.status);
	} else {
		return (Tt_mode)result.int_val;
	}
}


Tt_mode
_tt_otype_hsig_arg_mode(const char *otype, int sig, int arg)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	args.num2 = arg;
	
	d_session->call(TT_RPC_OTYPE_HSIG_ARG_MODE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (Tt_mode)error_int(result.status);
	} else {
		return (Tt_mode)result.int_val;
	}
}


char *
_tt_otype_osig_arg_type(const char *otype, int sig, int arg)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	args.num2 = arg;
	
	d_session->call(TT_RPC_OTYPE_OSIG_ARG_TYPE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {
		return _tt_strdup(result.str_val);
	}
}


char *
_tt_otype_hsig_arg_type(const char *otype, int sig, int arg)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.base_otid = otype;
	args.num = sig;
	args.num2 = arg;
	
	d_session->call(TT_RPC_OTYPE_HSIG_ARG_TYPE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	if (result.status!=TT_OK) {
		return (char *)error_pointer(result.status);
	} else {
		return _tt_strdup(result.str_val);
	}
}

/* 
 * Check to see if the given typename is a valid otype by getting its
 * base class.  If the otype doesn't exist, an error will be returned.
 */
Tt_status
_tt_valid_otype(const char *otype)
{
	_Tt_otype_args args;
	_Tt_rpc_result result;
	_Tt_c_session *d_session;

	d_session = _tt_c_mp->default_c_procid()->default_session().c_pointer();
	
	args.derived_otid = otype;
	d_session->call(TT_RPC_OTYPE_BASE,
			 (xdrproc_t)tt_xdr_otype_args,
			 (char *)&args,
			 (xdrproc_t)tt_xdr_rpc_result,
			 (char *)&result);
	return result.status;
}
