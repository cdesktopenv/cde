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
//%%  $TOG: mp_pattern.C /main/5 1999/10/14 18:48:24 mgreess $ 			 				
/*
 *
 * @(#)mp_pattern.C	1.28 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_pattern.cc
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */
#include "api/c/api_spec_map_ref.h"
#include "mp/mp_global.h"
#include "mp/mp_arg.h"
#include "mp/mp_pat_context.h"
#include "mp/mp_mp.h"
#include "mp/mp_pattern.h"
#include "mp/mp_message.h"
#include "mp/mp_session.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_enumname.h"
#include "util/tt_global_env.h"
#include "util/tt_port.h"

//
// Base constructor for _Tt_pattern objects. Should be called by all
// other _Tt_pattern constructors.
//
void _Tt_pattern::
base_constructor()
{
	server_callback	= 0;
	_category	= TT_CATEGORY_LAST;
	_classes	= 0;
	_paradigms	= 0;
	_reliabilities	= 0;
	_scopes		= 0;
	_states		= 0;
	_files		= new _Tt_string_list();
	_handler_ptypes = new _Tt_string_list();
	_handlers	= new _Tt_string_list();
	_objects	= new _Tt_string_list();
	_observer_ptypes = new _Tt_string_list();
	_opnums		= new _Tt_int_rec_list();
	_ops		= new _Tt_string_list();
	_args		= new _Tt_arg_list();
	_contexts	= new _Tt_pat_context_list;
	_otypes		= new _Tt_string_list();
	_sender_ptypes	= new _Tt_string_list();
	_senders	= new _Tt_string_list();
	_sessions	= new _Tt_string_list();
	_flags		= 0;
}

_Tt_pattern::
~_Tt_pattern()
{
}


//
// Deletes the given argument from the _files field of this object.
//
Tt_status _Tt_pattern::
del_file(const _Tt_string &f)
{
	_Tt_string_list_cursor	fcursor(_files);

	while (fcursor.next()) {
		if (*fcursor == f) {
			fcursor.remove();
		}
	}
	return(TT_OK);
}


//
// Deletes the given argument from the _sessions field of this object.
//
Tt_status _Tt_pattern::
del_session(const _Tt_string &s)
{
	_Tt_string_list_cursor	scursor(_sessions);

	while (scursor.next()) {
		if (*scursor == s) {
			scursor.remove();
		}
	}
	return(TT_OK);
}


//
// Adds the given object id to the _objects field. Will return
// TT_WRN_STALE_OBJID if the oid is a stale object id.
//
Tt_status _Tt_pattern::
add_object(const _Tt_string &oid)
{
	_Tt_api_spec_map_ref spec_map;
	_Tt_objid_spec_ptr   spec = spec_map.getSpec(oid);
	Tt_status	     status;

	if (spec.is_null()) {
		return(TT_ERR_OBJID);
	}

	switch (spec->getDBResults()) {
	      case TT_DB_OK:
		status = TT_OK;
		break;
	      case TT_DB_WRN_FORWARD_POINTER:
		status = TT_WRN_STALE_OBJID;
		break;
	      default:
		return(TT_ERR_OBJID);
	}
	add_field(oid, _objects);

	return(status);
}


// 
// Generic function for adding an integer to a _Tt_int_rec_list if it
// isn't already there.
// 
void _Tt_pattern::
add_field(int v, _Tt_int_rec_list_ptr &vlist)
{
	_Tt_int_rec_list_cursor	c(vlist);

	while (c.next()) {
		if (c->val == v) {
			return;
		}
	}
	vlist->push(new _Tt_int_rec((int)v));
}


// 
// Generic function for adding a string to a _Tt_string_list if it
// isn't already there.
// 
void _Tt_pattern::
add_field(const _Tt_string &v, _Tt_string_list_ptr &vlist)
{
	_Tt_string_list_cursor	c(vlist);

	while (c.next()) {
		if (*c == v) {
			return;
		}
	}
	vlist->push(v);
}


_Tt_pat_context_ptr _Tt_pattern::
context(const char *slotname) const
{
	_Tt_pat_context_list_cursor contextC( _contexts );
	while (contextC.next()) {
		if (contextC->slotName() == slotname) {
			return *contextC;
		}
	}
	return 0;
}


_Tt_pat_context_ptr _Tt_pattern::
context(int i) const
{
	if ((i >= 0) && (i < _contexts->count())) {
		return (*_contexts)[ i ];
	}
	return 0;
}


int _Tt_pattern::
contextsCount() const
{
	if (_contexts.is_null()) {
		return 0;
	}
	return _contexts->count();
}

Tt_status
_Tt_pattern::add_netfile(
	const _Tt_string &filepath,
	int fallback_2_local_netfile
)
{
        _Tt_string abspath;
	int        __scopes = scopes();

	if ((__scopes&(1<<TT_FILE)) || (__scopes&(1<<TT_BOTH))) {
		abspath = _Tt_db_file::getNetworkPath(filepath);
		if ((abspath.len() == 0) && (! fallback_2_local_netfile)) {
			return TT_ERR_FILE;
		}
	}
	if (abspath.len() == 0) {
		abspath = _tt_local_network_path(filepath);
		if (abspath.len() == 0) {
			return TT_ERR_FILE;
		}
	}
	return(add_file(abspath));
}


//
// XDR encodes/decodes a pattern. 
//
bool_t _Tt_pattern::
xdr(XDR *xdrs)
{
	if (! tt_xdr_string(xdrs, &_pattern_id)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_category)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_classes)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_states)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_paradigms)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_scopes)) {
		return(0);
	}
	if (! xdr_int(xdrs, (int *)&_reliabilities)) {
		return(0);
	}
	if (! _files.xdr(xdrs)) {
		return(0);
	}
	if (! _sessions.xdr(xdrs)) {
		return(0);
	}
	if (! _ops.xdr(xdrs)) {
		return(0);
	}
	if (! _opnums.xdr(xdrs)) {
		return(0);
	}
	if (! _objects.xdr(xdrs)) {
		return(0);
	}
	if (! _otypes.xdr(xdrs)) {
		return(0);
	}
	if (! _senders.xdr(xdrs)) {
		return(0);
	}
	if (! _handlers.xdr(xdrs)) {
		return(0);
	}
	if (! _sender_ptypes.xdr(xdrs)) {
		return(0);
	}
	if (! _handler_ptypes.xdr(xdrs)) {
		return(0);
	}
	if (! _args.xdr(xdrs)) {
		return(0);
	}
	if (_tt_global->xdr_version() >= TT_CONTEXTS_XDR_VERSION) {
		if (! _contexts.xdr(xdrs)) {
			return(0);
		}
	}

	return(1);
}


//
// Function wrapper so that the xdr method for a pattern can be invoked
// from the C RPC interface.
//
bool_t tt_xdr_pattern(XDR *xdrs, _Tt_pattern_ptr *pat)
{
	return((*pat)->xdr(xdrs));
}

// 
// methods and functions associated with printing patterns
//

static const char *
int_to_class(int r)
{
	return(_tt_enumname((Tt_class)r));
}

static const char *
int_to_paradigm(int r)
{
	return(_tt_enumname((Tt_address) r));
}

static const char *
int_to_state(int r)
{
	return(_tt_enumname((Tt_state) r));
}

static const char *
int_to_scope(int r)
{
	return(_tt_enumname((Tt_scope) r));
}


// 
// Used to print out a generic list of integers. The given conversion
// function ppfn returns an appropiate string to print for each integer.
// See all the int_to_* functions above.
// 
static void
print_enum_mask(const _Tt_ostream &os, int start, int end, int mask,
		const char *(*ppfn)(int a))
{
	int	i;

	for (i=start;i <= end;i++) {
		if (mask&(1<<i)) {
			os << (*ppfn)(i) << " ";
		}
	}
}


// 
// Used by the generic list and table packages to print out elements.
// Should not be used otherwise.
// 
void
_tt_int_rec_print(const _Tt_ostream &os, const _Tt_object *obj)
{
	((_Tt_int_rec *)obj)->print(os);
}


// 
// Prints out a pattern object. Used for debugging and for printing out
// patterns being considered when tracing is turned on. 
// 
void _Tt_pattern::
print(const _Tt_ostream &os) const
{
	os << "id:\t\t";
	os << _pattern_id << "\n";
	os << "category:\t" << _tt_enumname(_category) << "\n";
	if (_classes != 0) {
		os << "classes:\t";
		print_enum_mask(os,(int)TT_CLASS_UNDEFINED,(int)TT_CLASS_LAST,
				_classes, int_to_class);
		os << "\n";
	}
	if (_states != 0) {
		os << "states:\t\t";
		print_enum_mask(os,(int)TT_CREATED, (int)TT_STATE_LAST,
				_states, int_to_state);
		os << "\n";
	}
	if (_paradigms != 0) {
		os << "addresses:\t";
		print_enum_mask(os,(int)TT_PROCEDURE,(int)TT_HANDLER,
				_paradigms, int_to_paradigm);
		os << "\n";
	}
	if (_scopes != 0) {
		os << "scopes:\t\t";
		print_enum_mask(os,(int)TT_SCOPE_NONE,(int)TT_FILE_IN_SESSION,
				_scopes, int_to_scope);
		os << "\n";
	}
	if (_files->count()) {
		os << "files:\t\t";
		_files->print(_tt_string_print, os);
		os << "\n";
	}
	if (_sessions->count()) {
		os << "sessions:\t";
		_sessions->print(_tt_string_print, os);
		os << "\n";
	}
	if (_ops->count()) {
		os << "ops:\t\t";
		_ops->print(_tt_string_print, os);
		os << "\n";
	}
	if (_args->count()) {
		os << "args:\n";
		_Tt_string indent = os.indent();
		os.set_indent( indent.cat( "\t" ));
		_Tt_arg_list_cursor argC( _args );
		while (argC.next()) {	
			argC->print( os );
		}
		os.set_indent( indent );
	}
	if (_contexts->count()) {
		os << "contexts:\n";
		_Tt_string indent = os.indent();
		os.set_indent( indent.cat( "\t" ));
		_Tt_pat_context_list_cursor contextC( _contexts );
		while (contextC.next()) {	
			contextC->print( os );
		}
		os.set_indent( indent );
	}
	if (_opnums->count()) {
		os << "opnums:\t";
		_opnums->print(_tt_int_rec_print, os);
		os << "\n";
	}
	if (_otypes->count()) {
		os << "otypes:\t";
		_otypes->print(_tt_string_print, os);
		os << "\n";
	}
	if (_senders->count()) {
		os << "senders:\t";
		_senders->print(_tt_string_print, os);
		os << "\n";
	}
	if (_handlers->count()) {
		os << "handlers:\t";
		_handlers->print(_tt_string_print, os);
		os << "\n";
	}
	if (_sender_ptypes->count()) {
		os << "sender_ptypes:\t";
		_sender_ptypes->print(_tt_string_print, os);
		os << "\n";
	}
	if (_handler_ptypes->count()) {
		os << "handler_ptypes:\t";
		_handler_ptypes->print(_tt_string_print, os);
		os << "\n";
	}
}
