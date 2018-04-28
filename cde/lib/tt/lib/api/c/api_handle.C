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
//%%  $XConsortium: api_handle.C /main/6 1995/12/08 12:02:32 drk $ 			 				
/*
 *
 * api_handle.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Handle functions.  Messages and patterns are passed over the
 * API layer by referring to "opaque handles".  These opaque
 * handles are, abstractly, indexes into a list of "real"
 * _Tt_message_ptr and _Tt_pattern_ptr values.  
 */

#include "mp/mp_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_handle.h"
#include "util/tt_trace.h"
#include "util/tt_global_env.h"

/*
 * construct a new _Tt_api_handle_table.  This is only done once per
 * process, when tt_open() is called.
 */



_Tt_api_handle_table::
_Tt_api_handle_table()
{	
	content = new _Tt_api_handle_list;
	last_phandle = NULL;
}

/*
 * Look up a _Tt_c_message given its API handle.  Since the handle is
 * really the address of the _Tt_api_handle instance for the message,
 * this is quick, but obscure.
 *
 * Performance note: since the TT API encourages a style where
 * clusters of calls naming the same message handle appear, it's
 * actually fairly important to make this fast.  In particular note
 * that we just cast to the C pointer (_Tt_api_handle *) instead
 * of constructing and destructing a _Tt_api_handle_ptr.
 */
_Tt_c_message_ptr _Tt_api_handle_table::
lookup_msg(Tt_message h)
{
	if ((Tt_message)0 == h) {
		return 0;
	} else {
		return ((_Tt_api_handle *)h)->mptr();
	}
}


/*
 * Look up a _Tt_pattern given its API handle.  Since the handle is
 * really the address of the _Tt_api_handle instance for the pattern,
 * this is quick, but obscure.
 * See lookup_msg for a performance note.
 */
_Tt_pattern_ptr _Tt_api_handle_table::
lookup_pat(Tt_pattern h)
{
	if ((Tt_pattern)0 == h) {
		return 0;
	} else {
		return ((_Tt_api_handle *)h)->pptr();
	}
}

/*
 * Look up a _Tt_pattern given its pattern id.
 * Performance note: surprisingly, this is one of the bigger resource
 * consumers in a receive. Just to make simple benchmarks run better,
 * we cache the last one found so we can skip constructing the
 * cursor, etc.  Also it turns out that the first thing the caller of
 * lookup_pat_by_id does is turn around and look for the handle, so
 * cache and return that too.
 */
_Tt_pattern_ptr _Tt_api_handle_table::
lookup_pat_by_id(_Tt_string pat_id, Tt_pattern &phandle)
{
	if (last_pat_id == pat_id) {
		phandle = last_phandle;
		return last_pattern;
	}

	_Tt_api_handle_list_cursor c(content);
	_Tt_pattern_ptr x;
	_Tt_string x_id;

	while (c.next()) {
		if (!c->pptr().is_null()) {
			x = c->pptr();
			x_id = x->id();
			if (pat_id == x_id) {
				phandle = (Tt_pattern)((*c).c_pointer());
				last_pat_id = x_id;
				last_pattern = x;
				last_phandle = phandle;
				return x;
			}
		}
	}

	phandle = 0;
        return 0;
}


/*
 * Look up a handle for the given _Tt_c_message.  Since the MP
 * layer may give us back a different instance of its _Tt_c_message
 * for a reply, we have to compare using the _Tt_c_message::is_equal()
 * method to compare the messages by id, not address.  If the incoming
 * message_ptr is found, replace the mptr in the handle with the
 * new version, which may have updated values (e.g. out args filled
 * in.)
 */
Tt_message _Tt_api_handle_table::
lookup_mhandle(_Tt_c_message_ptr m)
{
	// keep a cache of our last hit to avoid lookups
	if (! last_mhandle.is_null()) {
		if (m->is_equal(last_mhandle->mptr())) {
			last_mhandle->mptr()->update_message(m);
			return((Tt_message)last_mhandle.c_pointer());
		}
	}

	_Tt_api_handle_list_cursor c(content);
	_Tt_c_message_ptr x;

	while (c.next()) {
		x = c->mptr();
		if (!x.is_null()) {
			if (x->is_equal(m)) {
				x->update_message(m);
				last_mhandle = *c;
				return (Tt_message)last_mhandle.c_pointer();
			}
		}
	}
	// not found, add an entry
	if (((_Tt_c_message *)m.c_pointer())->is_a_diff()) {
		return((Tt_message)0);
	}
	_Tt_api_handle_ptr n = new _Tt_api_handle;
	n->ptr_set(m);
	content->push(n);
	last_mhandle = n;

	return (Tt_message)last_mhandle.c_pointer();
}

/*
 * Look up a handle for the given _Tt_pattern.  Since the MP
 * layer may give us back a different instance of its _Tt_pattern
 * after a match, we have to
 * compare the patterns by id, not address.
 */
Tt_pattern _Tt_api_handle_table::
lookup_phandle(_Tt_pattern_ptr p)
{
	_Tt_api_handle_list_cursor c(content);
	_Tt_pattern_ptr x;

	while (c.next()) {
		x = c->pptr();
		if (!x.is_null()) {
			if (p->id() == x->id()) {
				c->ptr_set(p);
				return (Tt_pattern)((*c).c_pointer());
			}
		}
	}

	// not found, add an entry
	
	_Tt_api_handle_ptr n = new _Tt_api_handle;
	n->ptr_set(p);
	content->push(n);

        return (Tt_pattern)n.c_pointer();

}


/*
 * Remove the handle for the given _Tt_c_message_ptr.  Use the is_equal
 * method, as in lookup.
 */
void _Tt_api_handle_table::
clear(_Tt_c_message_ptr m)
{
	_Tt_api_handle_list_cursor	c(content);
	_Tt_c_message_ptr			x;

	while (c.next()) {
		x = c->mptr();
		if (!x.is_null()) {
			if (x->is_equal(m)) {
				c.remove();
			}
		}
	}
}


/*
 * Remove the handle for the given _Tt_pattern_ptr.  Use the is_equal
 * method, as in lookup.
 */
void _Tt_api_handle_table::
clear(_Tt_pattern_ptr p)
{
	_Tt_api_handle_list_cursor 	c(content);
	_Tt_pattern_ptr			pc;

	if (last_pat_id == p->id()) {
		last_pat_id = 0;
	}
	while (c.next()) {
		pc = c->pptr();
		if (!pc.is_null()) {
			if (p->id() == pc->id()) {
				c.remove();
			}
		}
	}
}


/*
 * Remove the handle for the given Tt_message.  By hackish casting
 * we can get directly to the _Tt_api_handle element, but to delete
 * it we have to scan the whole list, as there is no cursor "GOTO".
 */
void _Tt_api_handle_table::
clear(Tt_message p)
{
	_Tt_api_handle_list_cursor c(content);
	
	while (c.next()) {
		if ((void *)((*c).c_pointer()) == (void *)p) {
			if ((*c).is_eq(last_mhandle)) {
				last_mhandle = (_Tt_api_handle *)0;
			}
			c.remove();
		}
	}
}
 
/*
 * Remove the handle for the given Tt_pattern.  By hackish casting
 * we can get directly to the _Tt_api_handle element, but to delete
 * it we have to scan the whole list, as there is no cursor "GOTO".
 */
void _Tt_api_handle_table::
clear(Tt_pattern p)
{
	_Tt_api_handle_list_cursor c(content);
	
	while (c.next()) {
		if ((void *)((*c).c_pointer()) == (void *)p) {
			c.remove();
		}
	}
}


/*
 * Store userdata for a _Tt_c_message given its API handle.  
 */
Tt_status _Tt_api_handle_table::
store(Tt_message h, int key, void *userdata)
{
	if ((Tt_message)0 == h) {
                return TT_ERR_POINTER;
	}

	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	t->store(key, userdata);
	return TT_OK;
}

/*
 * Store userdata for a _Tt_pattern given its API handle.  
 */
Tt_status _Tt_api_handle_table::
store(Tt_pattern h, int key, void *userdata)
{
	if ((Tt_pattern)0 == h) {
                return TT_ERR_POINTER;
	}
	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	t->store(key, userdata);
	return TT_OK;
}


/*
 * Fetch userdata for a _Tt_c_message given its API handle.  
 */
void *  _Tt_api_handle_table::
fetch(Tt_message h, int key)
{
	if ((Tt_message)0 == h) {
		return error_pointer(TT_ERR_POINTER);
	}

	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	return t->fetch(key);
}

/*
 * Fetch userdata for a _Tt_pattern given its API handle.  
 */
void *  _Tt_api_handle_table::
fetch(Tt_pattern h, int key)
{
	if ((Tt_pattern)0 == h) {
		return error_pointer(TT_ERR_POINTER);
	}

	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	return t->fetch(key);
}

/*
 * Store callback for a _Tt_c_message given its API handle.  
 */
Tt_status _Tt_api_handle_table::
add_callback(Tt_message h, Tt_message_callback cb)
{
	if ((Tt_message)0 == h || (Tt_message_callback)0 == cb) {
		return TT_ERR_POINTER;
	}
	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	t->add_callback(cb);
	return TT_OK;
}

/*
 * Store callback for a _Tt_pattern given its API handle.  
 */
Tt_status _Tt_api_handle_table::
add_callback(Tt_pattern h, Tt_message_callback cb)
{
	if ((Tt_pattern)0 == h || (Tt_message_callback)0 == cb) {
		return TT_ERR_POINTER;
	}
	_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
	t->add_callback(cb);
	return TT_OK;
}

/*
 * Run callbacks for a Tt_message given its API handle and the handle
 * of the pattern it matched
 */
Tt_callback_action _Tt_api_handle_table::
run_message_callbacks(Tt_message h, Tt_pattern ph)
{
	if ((Tt_message)0 == h) {
		return TT_CALLBACK_CONTINUE;
	} else {
		_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
		return t->run_callbacks(h,ph);
	}
}

/*
 * Run callbacks for a Tt_pattern given its API handle and the API handle
 * of the message that matched it.
 */
Tt_callback_action _Tt_api_handle_table::
run_pattern_callbacks(Tt_pattern h, Tt_message mh)
{
	if ((Tt_pattern)0 == h) {
		return TT_CALLBACK_CONTINUE;
	} else {
		_Tt_api_handle_ptr	t = (_Tt_api_handle *)h;
		return t->run_callbacks(mh,h);
	}
}


void 
_tt_api_handle_print(FILE *fs, const _Tt_object *obj)
{
	((_Tt_api_handle *)obj)->print(fs);
}

/*
 * Print the whole table
void _Tt_api_handle_table::
print(FILE *f) const
{
	(void)fprintf(f,"_Tt_api_handle_table at %#lx <\n",(long)this);
	content->print(_tt_api_handle_print, f);
	(void)fprintf(f,">\n");
}
 */


//----------------------------------------------------------------------
//
// _Tt_api_handle methods
//
//----------------------------------------------------------------------

/*
 * Construct a _Tt_api_handle
 */
_Tt_api_handle::
_Tt_api_handle()
{
	udlp = new _Tt_api_userdata_list;
	cblp = new _Tt_api_callback_list;
}


_Tt_api_handle::
~_Tt_api_handle()
{
}


/*
 * Set to point to a message
 */
void _Tt_api_handle::
ptr_set(_Tt_c_message_ptr p)
{
	_mptr = p;
	_pptr = (_Tt_pattern *)0;
}

/*
 * Set to point to a pattern
 */
void _Tt_api_handle::
ptr_set(_Tt_pattern_ptr p)
{

	_pptr = p;
	_mptr = (_Tt_c_message *)0;
}

/*
 * Store userdata under a key
 */
void _Tt_api_handle::
store(int key, void *userdata)
{
	_Tt_api_userdata_list_cursor c(udlp);

	while (c.next()) {
		if (c->key == key) {
			c->userdata = userdata;
			return;
		}
	}
	// No userdata list element exists for this key.
	_Tt_api_userdata_ptr n = new _Tt_api_userdata;
	n->key = key;
	n->userdata = userdata;
	udlp->push(n);
}

/*
 * Fetch userdata previously stored under a key
 */
void * _Tt_api_handle::
fetch(int key)
{
	_Tt_api_userdata_list_cursor c(udlp);

	while (c.next()) {
		if (c->key == key) {
			return c->userdata;
		}
	}

	// No userdata list element exists for this key.
	return (void *) 0;
}

/*
 * Store callback
 */
void _Tt_api_handle::
add_callback(Tt_message_callback f)
{
	// No userdata list element exists for this key.
	_Tt_api_callback_ptr n = new _Tt_api_callback;
	n->callback = f;
	cblp->push(n);
}


/*
 * Run the callbacks stored on this handle, passing the message and
 * pattern handles to each
 */
Tt_callback_action _Tt_api_handle::
run_callbacks(Tt_message mh, Tt_pattern ph)
{
	Tt_callback_action result = TT_CALLBACK_CONTINUE;
	_Tt_api_callback_list_cursor c(cblp);
	_Tt_trace trace;

	// Mutexes must be dropped around callbacks.  Note that
	// _Tt_trace::entry does *not* grab a mutex for the callback
	// version, and exit does not drop one for the callback version.
	// See tt_trace.C

	// XXX: The only reason we can get away with this here is because
	// of the global libtt lock, and the fact that the cursor used
	// here is a local variable.

	_tt_global->drop_mutex();
	
	while (result==TT_CALLBACK_CONTINUE && c.next()) {
		trace.entry(c->callback, mh, ph);
		result = (c->callback)(mh,ph);
		trace.exit(result);
	}

	_tt_global->grab_mutex();
	
	return result;
}

// These access functions are prime candidates for inlining, but
// for now they're broken out since I need to set breakpoints
// in them.

_Tt_c_message_ptr _Tt_api_handle::
mptr()
{
	return _mptr;
}

_Tt_pattern_ptr _Tt_api_handle::
pptr()
{
	return _pptr;
}

void
_tt_api_userdata_print(FILE *fs, const _Tt_object *obj)
{
	((_Tt_api_userdata *)obj)->print(fs);
}


/*
 * Print a _Tt_api_handle
 */
void _Tt_api_handle::
print(FILE *f) const
{
	(void)fprintf(f,"_Tt_api_handle at %lx\n",(long)this);
}

/*
 * Print userdata pairs
 */
void _Tt_api_userdata::
print(FILE *f) const
{
	(void)fprintf(f,"_Tt_api_userdata at %#lx < Key: %d, Value %#lx>\n",
		      (long)this, key, (long)userdata);
}

/*
 * Print callbacks
 */
void _Tt_api_callback::
print(FILE *f) const
{
	(void)fprintf(f,"_Tt_api_callback at %#lx < Callback at: %#lx>\n",
		      (long)this, (long)callback);
}

_Tt_api_callback::
_Tt_api_callback()
{
	callback = NULL;
}

_Tt_api_callback::
~_Tt_api_callback()
{
}


_Tt_api_handle_table::
~_Tt_api_handle_table()
{
}

_Tt_api_userdata::
_Tt_api_userdata()
{
	key = 0;
	userdata = NULL;
}

_Tt_api_userdata::
~_Tt_api_userdata()
{
}
