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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: api_pattern.C /main/5 1999/09/17 18:26:08 mgreess $ 			 				
/*
 *
 * @(#)api_pattern.C	1.29 07 Sep 1993
 *
 * Tool Talk API - api_pattern.cc
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */

#include "util/tt_path.h"
#include "mp/mp_c.h"
#include "mp/mp_pat_context.h"
#include "mp/mp_c_msg_context.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "util/tt_audit.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char *          _tt_pattern_print(Tt_pattern);
Tt_status	_tt_pattern_xarg_add(Tt_pattern, Tt_mode, const char *, 
		     		     xdrproc_t , void *);
Tt_status	_tt_pattern_context_add(Tt_pattern, const char *, const char *);
Tt_status	_tt_pattern_icontext_add(Tt_pattern, const char *, int );
Tt_status	_tt_pattern_xcontext_add(Tt_pattern p, const char *slotname,
			xdrproc_t xdr_proc, void *value);
Tt_status	_tt_pattern_opnum_add(Tt_pattern, int);
Tt_status	_tt_pattern_bcontext_add(Tt_pattern, const char *,
					 const unsigned char *, int);
Tt_status	_tt_session_types_load(const char *);
Tt_status	_tt_context_join(const char *, const char *);
Tt_status	_tt_icontext_join(const char *, int);
Tt_status	_tt_bcontext_join(const char *, const unsigned char *, int);
Tt_status	_tt_context_quit(const char *, const char *);
Tt_status	_tt_icontext_quit(const char *, int);
Tt_status	_tt_bcontext_quit(const char *, const unsigned char *, int);
Tt_status	_tt_xcontext_join(const char *, xdrproc_t, void *);
Tt_status	_tt_xcontext_quit(const char *, xdrproc_t, void *);


char *
tt_pattern_print(Tt_pattern p)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("P", TT_PATTERN_PRINT, p);
        char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result = _tt_pattern_print(p);
        audit.exit(result);

        return result;
}


/* 
 * extern "C" { int _tt_x_pattern_match(Tt_pattern p, Tt_message m); }
 */

Tt_pattern
tt_pattern_create(void)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_PATTERN_CREATE, 0);
	Tt_pattern result;

        if (status != TT_OK) {
		audit.exit((Tt_pattern)error_pointer(status));
                return (Tt_pattern)error_pointer(status);
        }

        result =  _tt_pattern_create();

        audit.exit(result);       

        return result;
}

Tt_status
tt_pattern_destroy(Tt_pattern p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("P", TT_PATTERN_DESTROY, p);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_pattern_destroy(p);
        audit.exit(status);       

	return status;
}


Tt_status
tt_pattern_register(Tt_pattern p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("P", TT_PATTERN_REGISTER, p);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_pattern_register(p);
        audit.exit(status);       

	return status;
}


Tt_status
tt_pattern_unregister(Tt_pattern p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("P", TT_PATTERN_UNREGISTER, p);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_pattern_unregister(p);
        audit.exit(status);       

	return status;
}


Tt_status
tt_pattern_callback_add(Tt_pattern p, Tt_message_callback f)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PX", TT_PATTERN_CALLBACK_ADD, p, f);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
	}

	status = _tt_pattern_callback_add(p, f);
        audit.exit(status);       

	return status;
}


void *
tt_pattern_user(Tt_pattern p, int key)
{
        _Tt_audit audit;
        Tt_status status = audit.entry("Pi", TT_PATTERN_USER, p, key);
        void *result;

        if (status != TT_OK) {
		audit.exit((void *)error_pointer(status));
                return (void *)error_pointer(status);
        }

        result = _tt_pattern_user(p, key);
        audit.exit(result);

        return result;
}


Tt_status
tt_pattern_user_set(Tt_pattern p, int key, void *v)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PiA", TT_PATTERN_USER_SET, p, key, v);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_user_set(p,key, v);
        audit.exit(status);

	return status;

}


Tt_category
tt_pattern_category(Tt_pattern p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("P", TT_PATTERN_CATEGORY, p);
	Tt_category result;

        if (status != TT_OK) {
		audit.exit((Tt_category)error_int(status));
                return (Tt_category)error_int(status);
        }

        result = _tt_pattern_category(p);
        audit.exit(result);

	return result;
}


Tt_status
tt_pattern_category_set(Tt_pattern p, Tt_category c)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Pa", TT_PATTERN_CATEGORY_SET, p, c);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_category_set(p, c);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_arg_add(Tt_pattern p, Tt_mode n,
		    const char *vtype, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("POTA", TT_PATTERN_ARG_ADD, p, n,
					vtype, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_arg_add(p, n, vtype, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_barg_add(Tt_pattern p, Tt_mode n, const char *vtype, 
		     const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("POTnI", TT_PATTERN_BARG_ADD, p, n,
					vtype, value, len);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_barg_add(p, n, vtype, value, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_iarg_add(Tt_pattern p, Tt_mode n, const char *vtype, int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("POTi", TT_PATTERN_IARG_ADD, p, n,
					vtype, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status =  _tt_pattern_iarg_add(p, n, vtype, value);
        audit.exit(status);

	return status;

}


Tt_status
tt_pattern_xarg_add(Tt_pattern p, Tt_mode n, const char *vtype, 
		     xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("POTXA", TT_PATTERN_XARG_ADD, p,
					n, vtype, xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_xarg_add(p, n, vtype, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_context_add(Tt_pattern p, const char *slotname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PSC", TT_PATTERN_CONTEXT_ADD, p,
 					    slotname, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_context_add(p, slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_icontext_add(Tt_pattern p, const char *slotname, int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PSi", TT_PATTERN_ICONTEXT_ADD, p,
 					    slotname, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_icontext_add(p, slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_xcontext_add(Tt_pattern p, const char *slotname,
			xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PSXA", TT_PATTERN_XCONTEXT_ADD, p,
 					    slotname, xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_xcontext_add(p, slotname, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_bcontext_add(Tt_pattern p, const char *slotname,
			const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PlnI", TT_PATTERN_BCONTEXT_ADD, p,
 					    slotname, (void *) value, len);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_bcontext_add(p, slotname, value, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_class_add(Tt_pattern p, Tt_class c)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PL", TT_PATTERN_CLASS_ADD, p, c);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_class_add(p, c);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_file_add(Tt_pattern p, const char *filepath)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_FILE_ADD, p, filepath);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_file_add(p, filepath);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_object_add(Tt_pattern  p, const char * objid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Po", TT_PATTERN_OBJECT_ADD, p,
					 objid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_object_add(p, objid);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_op_add(Tt_pattern p, const char *opname)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_OP_ADD, p,
					 opname);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_op_add(p, opname);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_opnum_add(Tt_pattern p, int opnum)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Pi", TT_PATTERN_OPNUM_ADD, p, opnum);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_opnum_add(p, opnum);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_otype_add(Tt_pattern  p, const char * otype)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_OTYPE_ADD, p, otype);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_otype_add(p, otype);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_address_add(Tt_pattern p, Tt_address d)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PD", TT_PATTERN_ADDRESS_ADD, p, d);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_address_add(p, d);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_disposition_add(Tt_pattern p, Tt_disposition r)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Pd", TT_PATTERN_DISPOSITION_ADD, p, r);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_disposition_add(p, r);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_scope_add(Tt_pattern p, Tt_scope s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PS", TT_PATTERN_SCOPE_ADD, p, s);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_scope_add(p, s);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_sender_add(Tt_pattern p, const char *procid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_SENDER_ADD, p, procid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_sender_add(p, procid);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_sender_ptype_add(Tt_pattern p, const char *ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_SENDER_PTYPE_ADD, p,
					ptid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_sender_ptype_add(p, ptid);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_session_add(Tt_pattern p, const char *sessid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("PC", TT_PATTERN_SESSION_ADD, p,
					sessid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_session_add(p, sessid);
        audit.exit(status);

	return status;
}


Tt_status
tt_pattern_state_add(Tt_pattern p, Tt_state s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("Pe", TT_PATTERN_STATE_ADD, p, s);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_pattern_state_add(p, s);
        audit.exit(status);

	return status;
}


Tt_status
tt_ptype_declare(const char * ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_PTYPE_DECLARE, ptid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_ptype_declare(ptid);
        audit.exit(status);

	return status;
}


Tt_status
tt_ptype_undeclare(const char * ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_PTYPE_UNDECLARE, ptid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_ptype_undeclare(ptid);
        audit.exit(status);

	return status;
}


Tt_status
tt_ptype_exists(const char * ptid)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("C", TT_PTYPE_EXISTS, ptid);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_ptype_exists(ptid);
        audit.exit(status);

	return status;
}


Tt_status
tt_session_types_load(const char * sessid, const char *filename)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("EC", TT_SESSION_TYPES_LOAD, sessid,
					filename);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

	// XXX: Should check that the supplied sessid is the session that the
	// default procid is talking to, but we don't have a tt_session_equal.

	// Really, things like
	// tt_ptype_exists that don\'t really require a procid should
	// all take a sessid and do this, to pave the way for a possible
	// implementation that allows multiple sessions per procid.

        status = _tt_session_types_load(filename);
        audit.exit(status);

	return status;
}


Tt_status
tt_context_join(const char *slotname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lC", TT_CONTEXT_JOIN, slotname,
					value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_context_join(slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_icontext_join(const char *slotname, int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("li", TT_ICONTEXT_JOIN, slotname, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_icontext_join(slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_bcontext_join(const char *slotname, const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lnI", TT_BCONTEXT_JOIN, slotname,
					value, len);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_bcontext_join(slotname, value, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_context_quit(const char *slotname, const char *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lC", TT_CONTEXT_QUIT, slotname,
					value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_context_quit(slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_icontext_quit(const char *slotname, int value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("li", TT_ICONTEXT_QUIT, slotname, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_icontext_quit(slotname, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_bcontext_quit(const char *slotname, const unsigned char *value, int len)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lnI", TT_BCONTEXT_QUIT, slotname,
					value, len);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_bcontext_quit(slotname, value, len);
        audit.exit(status);

	return status;
}


Tt_status
tt_xcontext_join(const char *slotname, xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lXA", TT_XCONTEXT_JOIN, slotname,
					xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_xcontext_join(slotname, xdr_proc, value);
        audit.exit(status);

	return status;
}


Tt_status
tt_xcontext_quit(const char *slotname, xdrproc_t xdr_proc, void *value)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("lXA", TT_XCONTEXT_QUIT, slotname,
					xdr_proc, value);

        if (status != TT_OK) {
		audit.exit(status);       
                return status;
        }

        status = _tt_xcontext_quit(slotname, xdr_proc, value);
        audit.exit(status);

	return status;
}


/***************************************************************************
  Patterns (FSpec A.9)
***************************************************************************/


// Print the pattern the user has passed in into a buffer & return
// that buffer to him.
char *
_tt_pattern_print(Tt_pattern p)
{
        _Tt_pattern_ptr	pat = _tt_htab->lookup_pat(p);
        _Tt_string	buf;

        pat->print(buf);

        return _tt_strdup(buf);
}


/* 
 * Creation, alteration, and destruction (FSpec A.9.1)
 */

/* 
 * Create a new pattern object and returns a handle for it; use this
 * handle in future calls to manipulate the pattern object.
 */
Tt_pattern
_tt_pattern_create(void)
{
	Tt_pattern result;
	_Tt_pattern_ptr pat;

	pat = new _Tt_pattern();
	result = _tt_htab->lookup_phandle(pat);
	return result;
}

/* 
 * Destroys a pattern object. Destroying a pattern object implicitly
 * unregisters the pattern.
 */
Tt_status
_tt_pattern_destroy(Tt_pattern p)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	Tt_status	status;

	PCOMMIT;

	status = _tt_c_mp->default_c_procid()->del_pattern(pat->id());
	_tt_htab->clear(p);

	//
	// Ignore errors about pattern not found. This prevents
	// tt_pattern_unregister(pat) followed by tt_pattern_destroy(pat)
	// from returning a warning. (bug #1158125)
	//
	if (status == TT_WRN_NOTFOUND) {
	  status = TT_OK;
	}
	return status;
}


/* 
 * Registers the pattern constructed in the pattern object with the
 * message server, so that the process will start receiving messages that
 * match the pattern. Once a pattern is registered, no further changes
 * can be made in the pattern (except for the implicit changes performed
 * by joining files and sessions)
 */
Tt_status
_tt_pattern_register(Tt_pattern p)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	Tt_status	st;

	if (pat->is_registered()) {
		return TT_ERR_INVALID;
	}
	PCOMMIT;
	st = _tt_c_mp->default_c_procid()->add_pattern(pat);
	if (st == TT_OK) {
		pat->set_registered();
	}
	return st;
}


/* 
 * Unregisters the pattern so that the process stops receiving messages
 * that match it.
 */
Tt_status
_tt_pattern_unregister(Tt_pattern p)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	Tt_status	st;

	if (! pat->is_registered()) {
		return TT_OK;
	}
	PCOMMIT;
	st = _tt_c_mp->default_c_procid()->del_pattern(pat->id());
	if (st == TT_OK) {
		pat->clr_registered();
	}
	return st;
}


/* 
 * Sets a callback on messages retrieved through pattern p
 */
Tt_status
_tt_pattern_callback_add(Tt_pattern p, Tt_message_callback f)
{
	if (! _tt_mp) {
		return(TT_ERR_NOMP);
	}

	if (_tt_pointer_error(p)!=TT_OK) return TT_ERR_POINTER;
	return _tt_htab->add_callback(p,f);
}


void *
_tt_pattern_user(Tt_pattern p, int key)
{
	return _tt_htab->fetch(p,key);
}


Tt_status
_tt_pattern_user_set(Tt_pattern p,int key, void *v)
{
	return _tt_htab->store(p,key,v);
}


/* 
 * Category (FSpec A.9.3)
 */

/* 
 * Returns the category of the pattern object associated with p.
 */
Tt_category
_tt_pattern_category(Tt_pattern p)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	// _Tt_pattern::category seems not to exist.
	return pat->category();
}


/* 
 * Sets the category of the pattern object associated with p.
 */
Tt_status
_tt_pattern_category_set(Tt_pattern p, Tt_category c)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->set_category(c));
}


/* 
 * Pattern Attributes (FSpec A.9.4)
 */


/* 
 * Adds a new argument to a message object. vtype is the name of a valid
 * value type. Use NULL for values of mode out.
 */
Tt_status
_tt_pattern_arg_add(Tt_pattern p, Tt_mode n,
		    const char *vtype, const char *value)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	_Tt_arg_ptr	arg;

	arg = new _Tt_arg(n, vtype);
	// NULL arg value means leave value unset (which matches everything)
	if (value != (char *)0) {
		arg->set_data_string(value);
	}

	return pat->add_arg(arg);
}


/* 
 * Adds a new opaque argument to a pattern.
 */
Tt_status
_tt_pattern_barg_add(Tt_pattern p, Tt_mode n, const char *vtype, 
		     const unsigned char *value, int len)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	_Tt_arg_ptr	arg;


	arg = new _Tt_arg(n, vtype);

	// NULL arg value means leave value unset (which matches everything)
	if (value != (unsigned char *) 0)  {
		arg->set_data_string( _Tt_string(value, len) );
	}

	return pat->add_arg(arg);
}


/* 
 * Adds a new integer argument to a pattern object. vtype is the name of
 * a valid value type. 
 */
Tt_status
_tt_pattern_iarg_add(Tt_pattern p, Tt_mode n, const char *vtype, 
		     int value)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	_Tt_arg_ptr	arg;

	arg = new _Tt_arg(n, vtype);
	arg->set_data_int(value);
	return pat->add_arg(arg);
}


/* 
 * Adds a new argument w/ an XDR'ed value to a pattern object.
 */
Tt_status
_tt_pattern_xarg_add(Tt_pattern p, Tt_mode n, const char *vtype, 
		     xdrproc_t xdr_proc, void *value)
{
        _Tt_pattern_ptr pat_p = _tt_htab->lookup_pat(p);
        _Tt_arg_ptr	arg;


        // Encode the XDR arg or return failure.
        _Tt_string xdr_arg;

        if (_tt_xdr_encode(xdr_proc, value, xdr_arg) == 0) {
                return TT_ERR_XDR;
        }

	// Set the argument.
        arg = new _Tt_arg(n, vtype);

	_Tt_string s(xdr_arg);
        if (!s.is_null()) {
                arg->set_data_string(s);
        }
        return pat_p->add_arg(arg);
}


/* 
 * Adds a string value to a context slot.
 */
Tt_status
_tt_pattern_context_add(Tt_pattern p, const char *slotname, const char *value)
{
	_Tt_pattern_ptr		pat = _tt_htab->lookup_pat(p);
	_Tt_pat_context_ptr	cntxt;
	Tt_status		status = TT_OK;

	int add = 0;
	cntxt = pat->context(slotname);
	if (cntxt.is_null()) {
		cntxt = new _Tt_pat_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName(slotname);
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	// NULL value means leave value unset (which matches everything)
	if (value != NULL) {
		_Tt_string valString(value);
		status = cntxt->addValue(valString);
		if (status != TT_OK) {
			return status;
		}
	}
	if (add) {
		return pat->add_context( cntxt );
	}
	return status;
}


/* 
 * Adds an integer value to a context slot.
 */
Tt_status
_tt_pattern_icontext_add(Tt_pattern p, const char *slotname, int value)
{
	_Tt_pattern_ptr		pat = _tt_htab->lookup_pat(p);
	_Tt_pat_context_ptr	cntxt;
	Tt_status		status;

	int add = 0;
	cntxt = pat->context(slotname);
	if (cntxt.is_null()) {
		cntxt = new _Tt_pat_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName(slotname);
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	status = cntxt->addValue(value);
	if (status != TT_OK) {
		return status;
	}
	if (add) {
		return pat->add_context(cntxt);
	}
	return status;
}


/* 
 * XDR equivalent of tt_pattern_bcontext_add.
 */
Tt_status
_tt_pattern_xcontext_add(Tt_pattern p, const char *slotname,
			xdrproc_t xdr_proc, void *value)
{
        int			add = 0;
        _Tt_pattern_ptr		pat_p = _tt_htab->lookup_pat(p);
        _Tt_pat_context_ptr	cntxt_p;
        Tt_status		status;
 

        cntxt_p = pat_p->context(slotname);
        if (cntxt_p.is_null()) {
                cntxt_p = new _Tt_pat_context;
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
 
        // add the XDR'ed context
        status = cntxt_p->addValue(xdr_context);
        if (status != TT_OK) {
                return status;
        }
        if (add) {
                return pat_p->add_context(cntxt_p);
        }
        return status;
}


/* 
 * Adds a byte-string value to a context slot.
 */
Tt_status
_tt_pattern_bcontext_add(Tt_pattern p, const char *slotname,
			const unsigned char *value, int len)
{
	_Tt_pattern_ptr		pat = _tt_htab->lookup_pat(p);
	_Tt_pat_context_ptr	cntxt;
	Tt_status		status = TT_OK;


	int add = 0;
	cntxt = pat->context( slotname );
	if (cntxt.is_null()) {
		cntxt = new _Tt_pat_context;
		if (cntxt.is_null()) {
			return TT_ERR_NOMEM;
		}
		status = cntxt->setName( slotname );
		if (status != TT_OK) {
			return status;
		}
		add = 1;
	}
	// NULL value means leave value unset (which matches everything)
	if (value != NULL) {
		_Tt_string valString( value, len );

		status = cntxt->addValue( valString );
		if (status != TT_OK) {
			return status;
		}
	}
	if (add) {
		return pat->add_context( cntxt );
	}
	return status;
}


Tt_status
_tt_pattern_class_add(Tt_pattern p, Tt_class c)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_message_class(c));
}


Tt_status
_tt_pattern_file_add(Tt_pattern p, const char *filepath)
{
	_Tt_pattern_ptr	pat = _tt_htab->lookup_pat(p);

	return(pat->add_netfile(filepath));
}


Tt_status
_tt_pattern_object_add(Tt_pattern  p, const char * objid)
{
	_Tt_string	oid = objid;
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_object(oid.unquote_nulls()));
}


Tt_status
_tt_pattern_op_add(Tt_pattern p, const char *opname)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_op(opname));
}


Tt_status
_tt_pattern_opnum_add(Tt_pattern p, int opnum)
{

	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_opnum(opnum));
}


Tt_status
_tt_pattern_otype_add(Tt_pattern  p, const char * otype)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	Tt_status rc;

	if (TT_WRN_LAST < (rc = _tt_valid_otype(otype))) {
		return rc;
	}
	return(pat->add_otype(otype));
}


Tt_status
_tt_pattern_address_add(Tt_pattern p, Tt_address d)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_paradigm(d));
}


Tt_status
_tt_pattern_disposition_add(Tt_pattern p,Tt_disposition r)
{

	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_reliability(r));
}


Tt_status
_tt_pattern_scope_add(Tt_pattern p, Tt_scope s)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_scope(s));
}


Tt_status
_tt_pattern_sender_add(Tt_pattern p, const char *procid)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_sender(procid));
}


Tt_status
_tt_pattern_sender_ptype_add(Tt_pattern p, const char *ptid)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_sender_ptype(ptid));
}


Tt_status
_tt_pattern_session_add(Tt_pattern p, const char *sessid)
{

	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);
	_Tt_session_ptr sptr;
	_Tt_string	strsessid = (char *)0;
	Tt_status	status;

	// Add the P on if isn't already.
	_prepend_P_to_sessid(sessid, strsessid);

	status = _tt_c_mp->find_session(strsessid, sptr, 1);
	if (TT_OK != status) return status;

	return(pat->add_session(sptr->id()));
}


Tt_status
_tt_pattern_state_add(Tt_pattern p, Tt_state s)
{
	_Tt_pattern_ptr pat = _tt_htab->lookup_pat(p);

	return(pat->add_state(s));
}


/* 
 * --> Experimental function for PE internal dispatch. Not to be advertised
 * in the header files yet.
 */
/* 
 * int
 * _tt_x_pattern_match(Tt_pattern p, Tt_message m)
 * {
 * 	_Tt_message_ptr	msg;
 * 	_Tt_pattern_ptr pat;
 * 
 * 	if (tt_ptr_error(m) != TT_OK || tt_ptr_error(p) != TT_OK) {
 * 		return(0);
 * 	}
 * 	msg = _tt_htab->lookup_msg(m);
 * 	pat = _tt_htab->lookup_pat(p);
 * 	if (msg.is_null() || pat.is_null()) {
 * 		return(0);
 * 	}
 * 	return(pat->match(msg));
 * }
 */

/* 
 * Ptype functions (FSpec A.10)
 */


/* 
 * Used to declare a ptype for a process.
 */
Tt_status
_tt_ptype_declare(const char * ptid)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	status;
	_Tt_string	sptid;

	PCOMMIT;
	sptid = ptid;
	status = _tt_c_mp->default_c_procid()->declare_ptype(sptid);
	if (status < TT_WRN_LAST) {
		_tt_c_mp->default_c_procid()->set_default_ptype(sptid);
	}

	return status;
}

/* 
 * Used to undeclare a ptype for a process.
 */
Tt_status
_tt_ptype_undeclare(const char * ptid)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	status;
	_Tt_string	sptid;

	PCOMMIT;
	sptid = ptid;
	status = _tt_c_mp->default_c_procid()->undeclare_ptype(sptid);
	return status;
}

/* 
 * Used to check if a ptype is known by the default session.
 */
Tt_status
_tt_ptype_exists(const char * ptid)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	status;
	_Tt_string	sptid;

	PCOMMIT;
	sptid = ptid;
	status = _tt_c_mp->default_c_procid()->exists_ptype(sptid);
	return status;
}

/* 
 * Used to load new types into the default session.
 */
Tt_status
_tt_session_types_load(const char *filename)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	status;
	    
	PCOMMIT;

	int fd;
	struct stat typefile_stat;

	if (-1==(fd=open(filename, O_RDONLY))) {
		return TT_ERR_FILE;
	}
	fcntl(fd, F_SETFD, 1);	/* Close on exec */

	if (-1==fstat(fd, &typefile_stat)) {
		close(fd);
		return TT_ERR_FILE;
	}
	_Tt_string typebuffer((int)typefile_stat.st_size);
	if (typefile_stat.st_size !=
	    read(fd, (void *)(char *)typebuffer,
		 (unsigned int)typefile_stat.st_size)) {
		close(fd);
		return TT_ERR_FILE;
	}
	if (-1==close(fd)) {
		return TT_ERR_FILE;
	}
	status = _tt_c_mp->default_c_procid()->load_types(typebuffer);
	return status;
}


/*
 * Contexts and static patterns
 */

/*
 * Add a string context to all patterns.
 */
Tt_status
_tt_context_join(const char *slotname, const char *value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 	status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;

	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	_Tt_string valString( value );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_join( *_tt_c_mp->default_c_procid()->default_session(),
			      _tt_c_mp->default_c_procid() );
}


/*
 * Add an integer context to all patterns.
 */
Tt_status
_tt_icontext_join(const char *slotname, int value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 	status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;
	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	status = cntxt->setValue( value );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_join( *_tt_c_mp->default_c_procid()->default_session(),
			      _tt_c_mp->default_c_procid() );
}


/*
 * Add a byte-array context to all patterns.
 */
Tt_status
_tt_bcontext_join(const char *slotname, const unsigned char *value, int len)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 	status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;
	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	_Tt_string valString( value, len );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_join( *_tt_c_mp->default_c_procid()->default_session(),
			     _tt_c_mp->default_c_procid() );
}


/*
 * Remove a string context from all patterns.
 */
Tt_status
_tt_context_quit(const char *slotname, const char *value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 	status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;
	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	_Tt_string valString( value );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_quit( *_tt_c_mp->default_c_procid()->default_session(),
			     _tt_c_mp->default_c_procid() );
}


/*
 * Remove an integer context from all patterns.
 */
Tt_status
_tt_icontext_quit(const char *slotname, int value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 	status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;
	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	status = cntxt->setValue( value );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_quit( *_tt_c_mp->default_c_procid()->default_session(),
			     _tt_c_mp->default_c_procid() );
}


/*
 * Remove a byte-array context from all patterns.
 */
Tt_status
_tt_bcontext_quit(const char *slotname, const unsigned char *value, int len)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
	Tt_status	 status;

	PCOMMIT;

	_Tt_c_msg_context_ptr cntxt = new _Tt_c_msg_context;
	status = cntxt->setName( slotname );
	if (status != TT_OK) {
		return status;
	}
	_Tt_string valString( value, len );
	status = cntxt->setValue( valString );
	if (status != TT_OK) {
		return status;
	}

	return cntxt->c_quit(*_tt_c_mp->default_c_procid()->default_session(),
			     _tt_c_mp->default_c_procid());
}


/*
 * XDR equivalent of tt_bcontext_join.
 */
Tt_status
_tt_xcontext_join(const char *slotname, xdrproc_t xdr_proc, void *value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
        Tt_status	status;

        PCOMMIT;

        _Tt_c_msg_context_ptr cntxt_p = new _Tt_c_msg_context;

        status = cntxt_p->setName(slotname);
        if (status != TT_OK) {
                return status;
        }

        // Encode the XDR context or return failure.
        _Tt_string xdr_context;

        if (_tt_xdr_encode(xdr_proc, value, xdr_context) == 0) {
                return TT_ERR_XDR;
        }

	// join the new context to the existing patterns
        _Tt_string valString(xdr_context);

        status = cntxt_p->setValue(valString);
        if (status != TT_OK) {
                return status;
        }
 
        return cntxt_p->c_join( *_tt_c_mp->default_c_procid()->default_session(),
                              _tt_c_mp->default_c_procid() );
}


/*
 * XDR equivalent of tt_bcontext_quit.
 */
Tt_status
_tt_xcontext_quit(const char *slotname, xdrproc_t xdr_proc, void *value)
{
	_Tt_c_procid    *d_procid = _tt_c_mp->default_c_procid().c_pointer();
        Tt_status	status;

        PCOMMIT;

	// setup target context.
        _Tt_c_msg_context_ptr cntxt_p;

        cntxt_p = new _Tt_c_msg_context;
        status = cntxt_p->setName(slotname);
        if (status != TT_OK) {
                return status;
        }

        // Encode the XDR context or return failure.
        _Tt_string xdr_context;
 
        if (_tt_xdr_encode(xdr_proc, value, xdr_context) == 0) {
                return TT_ERR_XDR;
        }

	// try to quit the specified, now encoded, context.
        _Tt_string valString(xdr_context);

        status = cntxt_p->setValue(valString);
        if (status != TT_OK) {
                return status;
        }

        return cntxt_p->c_quit(*_tt_c_mp->default_c_procid()->default_session(),
                               _tt_c_mp->default_c_procid());
}
