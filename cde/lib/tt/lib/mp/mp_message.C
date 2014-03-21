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
//%%  $TOG: mp_message.C /main/4 1998/04/09 17:52:22 mgreess $ 			 				
/*
 *
 * @(#)mp_message.C	1.52 93/09/07
 *
 * Tool Talk Message Passer (MP) - mp_message.cc
 * 
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */
#include <util/tt_global_env.h>
// Strictly, mp_message should not include the client-only mp structure.
// But we have just one client-only call buried in the xdr method,
// and splitting that out would be too hard (having different xdr
// methods for the server and the client would be madness!)
#include <mp/mp_c_global.h>
#include <mp/mp_c_mp.h>
#include <mp/mp_arg.h>
#include <mp/mp_msg_context.h>
#include <mp/mp_message.h>
#include <mp/mp_pattern.h>
#include <mp/mp_procid.h>
#include <mp/mp_session.h>
#include <util/tt_enumname.h>
#include <util/tt_port.h>
	
//
// Base constructor for a message.  ::xdr() relies on this
// to reset everything because _Tt_s_mp keeps a global
// _Tt_message as a buffer for reading messages off the wire,
// instead of constructing a new one each time.  XXX I refuse
// to believe that constructing a new one cannot be made cheap.
//
void _Tt_message::
base_constructor()
{
	_pattern_id = 0;
	_state = TT_CREATED;
	_status = (int)TT_OK;
	_paradigm = TT_ADDRESS_LAST;
	_scope = TT_SCOPE_NONE;
	_reliability = TT_DISCARD;
	_opnum = -1;
	_object = 0;
	_file = 0;
	_op = 0;
	_otype = 0;
	_session = 0;
	_sender = 0;
	_handler = 0;
	_abstainers = 0;
	_accepters = 0;
	_rejecters = 0;
	_sender_ptype = 0;
	_handler_ptype = 0;
	_rsessions = 0;
	_id = 0;
	_api_id = 0;
	_args = new _Tt_arg_list;
	_out_args = 0;
	_contexts = new _Tt_msg_context_list;
	_flags = 0;
	_ptr_guards = 0;
	_gid = _tt_global->gid;
	_uid = _tt_global->uid;
	// Set the default set of fields to be sent by
	// _Tt_message::xdr. Note that it is important for
	// _TT_MSK_PARADIGM and _TT_MSK_STATE to be part of this set
	// (see relevant comments in _Tt_message::xdr).
	_full_msg_guards = (_TT_MSK_ID |
			    _TT_MSK_MESSAGE_CLASS |
			    _TT_MSK_STATE |
			    _TT_MSK_PARADIGM |
			    _TT_MSK_SCOPE |
			    _TT_MSK_UID |
			    _TT_MSK_GID |
			    _TT_MSK_STATUS |
			    _TT_MSK_FLAGS);
	_message_class = TT_CLASS_UNDEFINED;
	_status_string = 0;
}



//
// Default constructor.
//

// XXX: Make sure all _Tt_strings do *not* point to
// _tt_global->universal_null_string for messages,
// because otherwise svc_getargs will clobber the
// global data

_Tt_message::
_Tt_message()
#if defined(linux) || defined(CSRG_BASED)
: _pattern_id(), _object(), _file(), _op(),
  _otype(), _sender_ptype(), _handler_ptype(),
  _api_id(), _status_string()
#else
: _pattern_id(NULL), _object(NULL), _file(NULL), _op(NULL),
  _otype(NULL), _sender_ptype(NULL), _handler_ptype(NULL),
  _api_id(NULL), _status_string(NULL)
#endif
{
	base_constructor();
}



//
// Destroys a message object.
//
_Tt_message::
~_Tt_message()
{
	// note _sender and _handler may be circular references
	// so we explicitly set them to 0 to produce any garbage
	// collection.
	_sender = (_Tt_procid *)0;
	_handler = (_Tt_procid *)0;
	_session = (_Tt_session *)0;
}



// 
// Returns 1 if m is equal to this message. Message equality can be
// determined by logically concatenating the message sender with the
// message id. 
// 
int _Tt_message::
is_equal(const _Tt_message_ptr &m)
{
	if (m->_id != _id) {
		return(0);
	}
	if (m->_sender.is_null() && _sender.is_null()) {
		return(1);
	} else if (m->_sender.is_null() || _sender.is_null()) {
		return(0);
	} else {
		return(m->_sender->id() == _sender->id());
	}
}



// 
// XDR encodes/decodes a message.
// 
// Because xdr'ing a message is crucial to the message-passing
// performance the way messages are xdr'ed is a little more complicated
// than other xdr methods.
// 
// The main optimization that is done is to have an integer field,
// _ptr_guards,  is a collection of bit fields. If a particular bit is
// turned on then this means that the corresponding message field is
// being sent/received from the xdr stream. These bit fields obey the
// following naming convention, if _ptr_guards&_TT_MSK_<field> is true
// then <field> is being sent/received from the xdr stream. Note that
// this means that this optimization can only apply to at most 32
// fields.
// 
// This scheme is an optimization if it allows us to send less
// information for a given message. The way it does this is by allowing
// "default" values to not be sent. For example, if a typical value of
// the _scope field is TT_SESSION then rather than send this value every
// time, we just leave the bit turned off and have the other end just use
// the default value if it notices the bit is turned off.  Another way
// we can avoid sending the entire message is if it is known that
// certain fields won't be required. For example, when a handler
// replies to a message, it is unnecessary to send back the entire
// list of arguments since, by definition,  only the TT_OUT and
// TT_INOUT arguments can change. 
// 
// There are certain methods that are called just before a message is
// going to be xdr'ed that set up the _ptr_guards field to turn on the
// proper set of flags. These methods are:
// 
// _Tt_c_message::set_return_handler_flags
// 
//	Optimizes message xdr for the case of a handler replying to a
//	message. 
// 
// _Tt_s_message::set_return_sender_flags
//
//	Optimizes message xdr for the case of a message being returned
//	to its original sender. 
// 
// _Tt_s_message::set_send_handler_flags
//
//	Optimizes message xdr for the case of sending a message to a
//	handler. 
// 
// The way these methods work is by setting the _ptr_guards field to some
// value other than 0 that has the proper flag bits turned on. Then this
// method checks for _ptr_guards being 0. If it is then it just uses the
// value of _full_msg_guards which is an always up-to-date set of flags
// of message fields that need to get xdr'ed in the default case. If
// _ptr_guards is not 0 then that value gets used instead of the default.
// Another detail of this mechanism is that the _flags field may
// sometimes have the _TT_MSG_UPDATE_XDR_MODE flag turned on. If this is
// the case then this indicates that when encoding the message only the
// arguments contained in the _out_args list are to be used rather than
// the ones in the _args list. This is because the args in _out_args are
// only the TT_OUT and TT_INOUT arguments.
//
bool_t _Tt_message::
xdr(XDR *xdrs)
{
	if (xdrs->x_op == XDR_DECODE) {
		base_constructor();
	}
	if (xdrs->x_op == XDR_ENCODE && _ptr_guards==0) {
		_flags &= ~(1<<_TT_MSG_UPDATE_XDR_MODE);	
 		_ptr_guards = _full_msg_guards;
 	}

	
	if (! xdr_int(xdrs, &_ptr_guards)) {
		return(0);
	}
	int id_2_set;
	int need_2_set_id = 0;
	if (_ptr_guards&_TT_MSK_ID) {
		if (xdrs->x_op == XDR_DECODE) {
			need_2_set_id = 1;
			if (! xdr_int(xdrs, &id_2_set)) {
				return(0);
			}
		} else {
			if (! xdr_int(xdrs, &_id)) {
				return(0);
			}
		}
	}
	if (_ptr_guards&_TT_MSK_MESSAGE_CLASS) {
		if (! xdr_int(xdrs, (int *)&_message_class)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_STATE) {
		if (! xdr_int(xdrs, (int *)&_state)) {
			return(0);
		}
	} else { // XXXX only if XDR_DECODE, right?  et al!
		_state = TT_SENT;
	}
	if (_ptr_guards&_TT_MSK_PARADIGM) {
		if (! xdr_int(xdrs, (int *)&_paradigm)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_SCOPE) {
		if (! xdr_int(xdrs, (int *)&_scope)) {
			return(0);
		}
	} else if (_tt_global->xdr_version() > 1) {
		_scope = TT_SESSION;
	}
	if (_ptr_guards&_TT_MSK_RELIABILITY) {
		if (! xdr_int(xdrs, (int *)&_reliability)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_OPNUM) {
		if (! xdr_int(xdrs, &_opnum)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_STATUS) {
		if (! xdr_int(xdrs,&_status)) {
			return(0);
		}
	}
	// Even though in Solaris 2.x uid_t and gid_t are typedef'd as
	// u_long, the compiler complains if they are not explicitly
	// coerced.

	/*
	 * Obviously there is no xdr_u_uid_t, so we have to hack around this.
	 * At least by using sizeof checks, we aren't manually specifying
	 * the size of uid_t depending on the platform (which broke terribly
	 * when we tried to port to 64-bit linux at first).
	 * 
	 * A side-effect of testing this way is that we have to spell out the pointer
	 * casts, since one of them is actually wrong for any given platform (but
	 * is also a dead code branch)
	 */
	if (_ptr_guards&_TT_MSK_UID) {
		if (xdrs->x_op == XDR_DECODE) {
			_uid = 0;
		}
		if (sizeof (int) == sizeof(uid_t)) {
			if (! xdr_u_int(xdrs,(u_int *) &_uid)) return 0;
		} else if (sizeof (long) == sizeof(uid_t)) {
			if (! xdr_u_long(xdrs,(u_long *) &_gid)) return 0;
		} else {
			_tt_syslog( 0, LOG_ERR, "_Tt_message::xdr(XDR*): uid_t size is not equal to int or long; TOOLTALK RPC WILL NOT WORK!" );
		}
	}
	if (_ptr_guards&_TT_MSK_GID) {
		if (xdrs->x_op == XDR_DECODE) {
			_gid = 0;
		}
		if (sizeof (int) == sizeof(gid_t)) {
			if (! xdr_u_int(xdrs,(u_int *) &_gid)) {
				return(0);
			}
		} else if (sizeof (long) == sizeof(gid_t)) {
			if (! xdr_u_long(xdrs,(u_long *) &_gid)) {
				return(0);
			}
		} else {
			_tt_syslog( 0, LOG_ERR, "_Tt_message::xdr(XDR*): gid_t size is not equal to int or long; TOOLTALK RPC WILL NOT WORK!" );
		}
	}

 	if (_ptr_guards&_TT_MSK_SESSION) {
 		if (xdrs->x_op == XDR_DECODE) {
 			_session = new _Tt_session();
 		}
 		if (! _session->xdr(xdrs)) {
 			return(0);
 		}
 	} else if (xdrs->x_op == XDR_DECODE) {
		_session = _tt_mp->initial_session;
	}

 	if (_ptr_guards&_TT_MSK_SENDER) {
 		if (xdrs->x_op == XDR_DECODE) {
 			_sender = new _Tt_procid();
 		}
 		if (! _sender->xdr(xdrs)) {
 			return(0);
 		}
 	} else if (! _tt_mp->in_server()) {
		// Since we know we're not in the server, we know that
		// _tt_c_mp is valid.
 		_sender = _tt_c_mp->default_procid();
 	}
	// _set_id() only works if _handler is set
	if (need_2_set_id) {
		_set_id(id_2_set);
	}
 	
	
	switch (_tt_global->xdr_version()) {
	      case 1:
		// version 1 tooltalk always xdr's a _Tt_procid_ptr
		if (! _handler.xdr(xdrs)) {
			return(0);
		}
		break;
	      case 2:
	      default:
		if (xdrs->x_op == XDR_DECODE) {
			//
			// XXX holtz: Also do this for other fields?
			// If not, *this is a hybrid message that
			// is dangerous to look at until you use it
			// to update_message().
			//
			_handler = 0;
		}
		if (_ptr_guards&_TT_MSK_HANDLER) {
			if (xdrs->x_op == XDR_DECODE) {
				_handler = new _Tt_procid();
			}
			if (! _handler->xdr(xdrs)) {
				return(0);
			}
		}
		break;
	}
	if (_ptr_guards&_TT_MSK_FILE) {
		if (! _file->xdr(xdrs)) {
			return(0);
		}
	}

	if (_ptr_guards&_TT_MSK_OBJECT) {
		if (! _object->xdr(xdrs)) {
			return(0);
		}
	}

	if (_ptr_guards&_TT_MSK_OP) {
		if (! _op->xdr(xdrs)) {
			return(0);
		}
	}

	if (_ptr_guards&_TT_MSK_ARGS) {
		if (xdrs->x_op == XDR_ENCODE) {
			if (_flags&(1<<_TT_MSG_UPDATE_XDR_MODE)) {

				// if this flag is turned on then only
				// send arguments from the _out_args
				// field rather than the _args field.

				if (_out_args.is_null()) {
					_out_args = new _Tt_arg_list();
				}
				if (! _out_args->xdr(xdrs)) {
					return(0);
				}
			} else {
				if (! _args->xdr(xdrs)) {
					return(0);
				}
			}
		} else {
			_args->flush();
			SET_GUARD(_full_msg_guards, 0, _TT_MSK_ARGS);

			if (! _args->xdr(xdrs)) {
				return(0);
			}
		}
	}
	if (_ptr_guards&_TT_MSK_FLAGS) {
		if (! xdr_int(xdrs, &_flags)) {
			return(0);
		}
		if (xdrs->x_op == XDR_DECODE) {
			// Assume non-remote, until we learn otherwise
			_flags &= ~(1<<_TT_MSG_IS_REMOTE);
		}
	}
	if (_ptr_guards&_TT_MSK_OTYPE) {
		if (! _otype->xdr(xdrs)) {
			return(0);
		}
	}

	if (_ptr_guards&_TT_MSK_SENDER_PTYPE) {
		if (! _sender_ptype->xdr(xdrs)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_HANDLER_PTYPE) {
		if (! _handler_ptype->xdr(xdrs)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_PATTERN_ID) {
		if (! _pattern_id->xdr(xdrs)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_RSESSIONS) {
		if (xdrs->x_op == XDR_DECODE) {
			_rsessions = new _Tt_string_list();
		}
		if (! _rsessions->xdr(xdrs)) {
			return(0);
		}
	}
	if (_ptr_guards&_TT_MSK_STATUS_STRING) {
		if (! _status_string->xdr(xdrs)) {
			return(0);
		}
	}
	//
	// Because of the bitmask, we don't worry about
	// xdr versioning here, and in effect pretend that
	// _TT_MSK_CONTEXTS has been defined from day 1.
	//
	if (_ptr_guards&_TT_MSK_CONTEXTS) {
		if (! _contexts->xdr(xdrs)) {
			return(0);
		}
	}
	//
	// Ditto.
	//
	if (_ptr_guards&_TT_MSK_OFFEREES) {
		//
		// TT_OFFERS are more rare than contexts, so
		// we allocate the lists lazily to save memory.
		//
		if (_abstainers.is_null()) {
			_abstainers = new _Tt_procid_list;
		}
		if (! _abstainers->xdr(xdrs)) {
			return(0);
		}
		if (_accepters.is_null()) {
			_accepters = new _Tt_procid_list;
		}
		if (! _accepters->xdr(xdrs)) {
			return(0);
		}
		if (_rejecters.is_null()) {
			_rejecters = new _Tt_procid_list;
		}
		if (! _rejecters->xdr(xdrs)) {
			return(0);
		}
	}

	if (xdrs->x_op == XDR_DECODE) {
		// Now we want to update _full_msg_guards from
		// _ptr_guards appropiately. We must choose to do this
		// either by assignment or by or'ing the fields.
		// Assignment is required if this is a new message
		// because _full_msg_guards may contain fields that
		// are 1 when they need to be 0 (see
		// _Tt_message::_Tt_message). (and or'ing won't
		// help). We can tell that the message coming in is a
		// new message by checking whether the paradigm and
		// state field is being sent. This is a gross hack but
		// unfortunately when the need for it was discovered
		// there was no other way to test for this in a
		// binary-compatible way with previous versions. Note
		// that the paradigm (known as "address" in the
		// documentation) doesn't ever change once its been
		// created so it should never be the case that sending
		// the message to a recipient in other than the
		// defualt xdr mode should require sending the
		// paradigm field.

		if (_ptr_guards&(_TT_MSK_PARADIGM|_TT_MSK_STATE)) {
			_full_msg_guards = _ptr_guards;
		} else {
			_full_msg_guards |= _ptr_guards;
		}
	}
	
	_ptr_guards = 0;
	return(1);
}


// 
// m represents the same message as this but perhaps with updated
// information. This method handles updating this message with the
// appropiate values in m. 
// 
void _Tt_message::
update_message(const _Tt_message_ptr &m)
{
	int	margc;
	int	mguards = m->_full_msg_guards;

	_full_msg_guards |= mguards;
	if (!_tt_mp->in_server()) {
		if (mguards&_TT_MSK_STATE) {
			set_state(m->_state);
		}
		if (mguards&_TT_MSK_RELIABILITY) {
			set_reliability(m->_reliability);
		}
		switch (_paradigm) {
		      case TT_OBJECT:
		      case TT_OTYPE:
			if (mguards&_TT_MSK_SCOPE) {
				_scope = m->_scope;
			}
			break;
		      default:
			break;
		}
		if (mguards&_TT_MSK_PATTERN_ID) {
			if (m->_pattern_id.len()) {
				_pattern_id = m->_pattern_id;
			}
		}
		if (mguards&_TT_MSK_HANDLER) {
			_handler = m->_handler;
		}
		if (mguards&_TT_MSK_HANDLER_PTYPE) {
			_handler_ptype = m->_handler_ptype;
		}
		if (mguards&_TT_MSK_OPNUM) {
			set_opnum(m->opnum());
		}
	}
	if (mguards&_TT_MSK_STATUS) {
		_status = m->_status;
	}
	if (mguards & _TT_MSK_STATUS_STRING) {
		set_status_string(m->_status_string);
	}
	if (mguards & _TT_MSK_OFFEREES) {
		//
		// We could just snare references and share the lists,
		// but if the given message is ttsession's static
		// wire buffer, then the lists will probably be changing.
		//
		_abstainers = new _Tt_procid_list( *m->_abstainers );
		_accepters  = new _Tt_procid_list( *m->_accepters );
		_rejecters  = new _Tt_procid_list( *m->_rejecters );
	}
	switch (m->_state) {
	    case TT_HANDLED:
	    case TT_FAILED:
		//
		// update object and file
		//

		if (mguards&_TT_MSK_OBJECT) {
			_object = m->_object;
		}
		if (mguards&_TT_MSK_FILE) {
			_file = m->_file;
		}
		if (mguards&_TT_MSK_CONTEXTS) {
			// All contexts are INOUT, and extra can
			// be added by the handler.
			_contexts = new _Tt_msg_context_list( *m->_contexts );
		}
		if (mguards&_TT_MSK_ARGS) {
			margc = _args->count();
			if (margc && m->_args->count()) { 

				//
				// update arguments with new values
				//

				_Tt_arg_list_cursor	argc;
				_Tt_arg_list_cursor	nargc;

				argc.reset(_args);
				nargc.reset(m->_args);
				while (argc.next()) {
					switch (argc->mode()) {
					      case TT_OUT:
					      case TT_INOUT:
						if (nargc.next()) {
							argc->update_value(*nargc);
							nargc.remove();
						}
						break;
					      default:
						if (!_tt_mp->in_server() &&
						    _message_class==TT_NOTICE &&
						    !nargc.next()) { 
							m->_full_msg_guards = 0;
							m->_args->flush();
							return;
						}
						break;
					}
				}
			}
		}
		// fall through
	    case TT_RETURNED:
		if (is_awaiting_reply()) {
			set_awaiting_reply( 0 );
		}
		break;
	}
	m->_full_msg_guards = 0;
	// XXX holtz 20 Jul 94  Why change given message?
	m->_args->flush();
}

_Tt_msg_context_ptr _Tt_message::
context(const char *slotname) const
{
	_Tt_msg_context_list_cursor contextC( _contexts );
	while (contextC.next()) {
		if (contextC->slotName() == slotname) {
			return *contextC;
		}
	}
	return 0;
}


_Tt_msg_context_ptr _Tt_message::
context(int i) const
{
	if ((i >= 0) && (i < _contexts->count())) {
		return (*_contexts)[ i ];
	}
	return 0;
}


int _Tt_message::
contextsCount() const
{
	if (_contexts.is_null()) {
		return 0;
	}
	return _contexts->count();
}


// 
// Returns the id of the pattern that matched this message (ie. that
// caused it to be delivered). This id will be null if the message
// matched a ptype pattern rather than a dynamic pattern.
// 
_Tt_string & _Tt_message::
pattern_id()
{
	return(_pattern_id);
}


// 
// Sets the pattern id of a messsage and turns on the corresponding bit
// field in _full_msg_guards if non-empty.
// 
void _Tt_message::
set_pattern_id(_Tt_string id)
{
	_pattern_id = id;
	SET_GUARD(_full_msg_guards, _pattern_id.len(), _TT_MSK_PATTERN_ID);

}


// 
// Function wrapper used to invoke the xdr method on a message when
// interfacing to the C RPC routines.
// 
bool_t
tt_xdr_message(XDR *xdrs, _Tt_message_ptr *msgp)
{
	return((*msgp).xdr(xdrs));
}


uid_t _Tt_message::
uid() const
{
	return _uid;
}


gid_t _Tt_message::
gid() const
{
	return _gid;
}


int _Tt_message::
id() const
{
	return(_id);
}

const _Tt_string & _Tt_message::
api_id() const
{
	return _api_id;
}


Tt_status _Tt_message::
set_message_class(Tt_class mclass)
{
	_message_class = mclass;
	return(TT_OK);
}


Tt_status _Tt_message::
set_state(Tt_state state)
{
	_state = state;

	return(TT_OK);
}


Tt_status _Tt_message::
set_paradigm(Tt_address p)
{
	_paradigm = p;
	
	return(TT_OK);
}


// 
// Sets the scope of a message. Note that if we're a post-version-1
// client then we also turn on the corresponding flag in _full_msg_guards
// if this is a non-TT_SESSION scope.
// 
Tt_status _Tt_message::
set_scope(Tt_scope s)
{

// XXX: The xdr_version() only shadows to the sessions xdr version when an 
//      RPC call is in progress. set_scope() can be called by
//      tt_message_scope_set() at which time the xdr version is not setup
//      correctly.

	_scope = s;
	SET_GUARD(_full_msg_guards,1,_TT_MSK_SCOPE);
	return(TT_OK);
}


// 
// Sets the file field of a message and turn on the corresponding bit in
// _full_msg_guards if non-empty.
// 
Tt_status _Tt_message::
set_file(_Tt_string f)
{
	if (f.len() == 0) {
		return(TT_OK);
	}
	_file = f;
	SET_GUARD(_full_msg_guards, 1, _TT_MSK_FILE);
	return(TT_OK);
}


// 
// Sets the session field in a message and turns on the corresponding bit
// field in _full_msg_guards if non-null.
// 
Tt_status _Tt_message::
set_session(_Tt_session_ptr &s)
{
	_session = s;
	SET_GUARD(_full_msg_guards,(! _session.is_null()), _TT_MSK_SESSION);
	
	return(TT_OK);
}


//
// Sets the op field in a message and turns on the corresponding bit
// field in _full_msg_guards if non-null.
//
Tt_status _Tt_message::
set_op(_Tt_string o)
{
	_op = o;
	SET_GUARD(_full_msg_guards, _op.len(), _TT_MSK_OP);
	
	return(TT_OK);
}


//
// Sets the opnum field in a message and turns on the corresponding
// bit field in _full_msg_guards if not equal to -1 (the default).
//
Tt_status _Tt_message::
set_opnum(int o)
{
	_opnum = o;
	SET_GUARD(_full_msg_guards, _opnum != -1, _TT_MSK_OPNUM);
	
	return(TT_OK);
}


// 
// Adds an argument to a message. Turns on the bit field for _args in
// _full_msg_guards since we know the list is non-empty.
// 
Tt_status _Tt_message::
add_arg(_Tt_arg_ptr &arg)
{
	_args->append(arg);
	
	SET_GUARD(_full_msg_guards, 1, _TT_MSK_ARGS);
	return(TT_OK);
}


// 
// Adds a TT_OUT or TT_INOUT argument to the _out_args list. This is used
// by _Tt_message::xdr when the _TT_MSG_UPDATE_XDR flag is turned on.
// 
void _Tt_message::
add_out_arg(_Tt_arg_ptr &arg)
{
	if (_out_args.is_null()) {
		_out_args = new _Tt_arg_list();
	}
	switch (arg->mode()) {
	      case TT_OUT:
	      case TT_INOUT:
		// a parallel list of just out arguments is kept since
		// whenever it is known that the receiver of a message
		// is only seeing an update of the message, only the
		// out arguments need to be sent.
		_out_args->append(arg);
		SET_GUARD(_full_msg_guards, 1, _TT_MSK_ARGS);
		break;
	      case TT_IN:
	      default:
		break;
	}
}


// 
// Adds a context to a message. Turns on the bit field for _contexts in
// _full_msg_guards since we know the list is non-empty.
// 
Tt_status _Tt_message::
add_context(_Tt_msg_context_ptr &context)
{
	_contexts->append_ordered(context);
	
	SET_GUARD(_full_msg_guards, 1, _TT_MSK_CONTEXTS);
	return(TT_OK);
}


// 
// Sets the object field in a message and turns on the corresponding bit
// field in _full_msg_guards.
// 
Tt_status _Tt_message::
set_object(_Tt_string oid)
{
	_object = oid;
	SET_GUARD(_full_msg_guards, _object.len(), _TT_MSK_OBJECT);
	return(TT_OK);
}


Tt_status _Tt_message::
set_otype(_Tt_string ot)
{
	_otype = ot;
	SET_GUARD(_full_msg_guards, _otype.len(), _TT_MSK_OTYPE);
	
	return(TT_OK);
}


Tt_status _Tt_message::
set_sender(_Tt_procid_ptr &s)
{
	_sender = s;
	SET_GUARD(_full_msg_guards, (! _sender.is_null()), _TT_MSK_SENDER);
	
	return(TT_OK);
}


Tt_status _Tt_message::
unset_handler_procid(void)
{
	_handler = (_Tt_procid *) 0;
	SET_GUARD(_full_msg_guards, 0, _TT_MSK_HANDLER);
	
	return(TT_OK);
}

Tt_status _Tt_message::
set_handler_procid(const _Tt_procid_ptr &h)
{
	_handler = h;
	SET_GUARD(_full_msg_guards, (! _handler.is_null()), _TT_MSK_HANDLER);
	
	return(TT_OK);
}


Tt_status _Tt_message::
add_voter(const _Tt_procid_ptr &voter, Tt_state vote)
{
	if (voter.is_null() || (_message_class != TT_OFFER)) {
		return TT_OK;
	}
	if (_abstainers.is_null()) {
		_abstainers = new _Tt_procid_list;
	}
	if (_abstainers.is_null()) {
		return TT_ERR_NOMEM;
	}
	if (_accepters.is_null()) {
		_accepters = new _Tt_procid_list;
	}
	if (_accepters.is_null()) {
		return TT_ERR_NOMEM;
	}
	if (_rejecters.is_null()) {
		_rejecters = new _Tt_procid_list;
	}
	if (_rejecters.is_null()) {
		return TT_ERR_NOMEM;
	}
	switch (vote) {
	    case TT_ACCEPTED:
		_accepters->append( voter );
		break;
	    case TT_REJECTED:
		_rejecters->append( voter );
		break;
	    case TT_ABSTAINED:
		_abstainers->append( voter );
		break;
	    default:
		// Not a vote; bail out.
		return TT_OK;
	}
	SET_GUARD(_full_msg_guards, 1, _TT_MSK_OFFEREES);
	return TT_OK;
}


Tt_status _Tt_message::
set_sender_ptype(_Tt_string s)
{
	_sender_ptype = s;
	SET_GUARD(_full_msg_guards,_sender_ptype.len(), _TT_MSK_SENDER_PTYPE);
	
	return(TT_OK);
}


Tt_status _Tt_message::
set_handler_ptype(_Tt_string h)
{
	_handler_ptype = h;
	SET_GUARD(_full_msg_guards,_handler_ptype.len(),_TT_MSK_HANDLER_PTYPE);
	
	return(TT_OK);
}


Tt_status _Tt_message::
set_reliability(Tt_disposition r)
{
	_reliability = r;
	SET_GUARD(_full_msg_guards, _reliability != TT_DISCARD,
		  _TT_MSK_RELIABILITY);		
	
	return(TT_OK);
}


// 
// Sets the id of a message which is used to uniquely identify the
// message within the sending process. The id is also used when queueing
// the message.  This implies that the message contains enough
// information that is unique so that the queued message will never be
// misinterpreted to be another message. This uniqueness is achieved by
// combining the sender procid with the message id. This works because
// the prefix of the sender procid is unique and the message id then
// distinguishes the message among other messages the process has sent.
// 
Tt_status _Tt_message::
set_id()
{
	return _set_id(_tt_mp->generate_message_id());
}

Tt_status _Tt_message::
_set_id(int id)
{
	_id = id;
	char buf[BUFSIZ];
	sprintf( buf, "%d ", _id );
	_api_id = buf;
	if (sender().is_null()) {	
		abort();
	}
	_api_id = _api_id.cat( sender()->id() );
	return(TT_OK);
}

/* 
 * Sets this to be a super message (send to parent type)
 */
void _Tt_message::
set_super()
{
	_flags |= (1<<_TT_MSG_IS_SUPER);
}

_Tt_string & _Tt_message::
status_string()
{
	return(_status_string);
}


Tt_status _Tt_message::
set_status_string(_Tt_string st)
{
	_status_string = st;
	SET_GUARD(_full_msg_guards,_status_string.len(),_TT_MSK_STATUS_STRING);
	
	return(TT_OK);
}


// 
// This method will identify this message as a "start" message if flag is
// 1. Otherwise it clears the flag associated with this. When turning the
// flag on we set the status field to TT_WRN_START_MESSAGE because the
// tooltalk client needs to check for this value to know that this is a
// start message. When turning the flag off we have to set the status to
// TT_OK but only if it is still TT_WRN_START_MESSAGE (otherwise we end
// up stomping on a status code set by the client.).
// 
void _Tt_message::
set_start_message(int flag)
{
	if (flag) {
		set_status((int)TT_WRN_START_MESSAGE);
		_flags |= (1<<_TT_MSG_IS_START_MSG);
	} else {
		if (_status == (int)TT_WRN_START_MESSAGE) {
			SET_GUARD(_full_msg_guards,1,_TT_MSK_STATUS);
			_status = TT_OK;
		}
		_flags &= ~(1<<_TT_MSG_IS_START_MSG);
	}
}


// 
// Returns 1 if this is a start message. Version 1 clients didn't
// implement the _TT_MSG_IS_START_MSG flag so the only way to tell with
// them is if the _status field is TT_WRN_START_MESSAGE.
// 
int _Tt_message::
is_start_message() const
{
	return(_status == TT_WRN_START_MESSAGE ||
	       (_flags&(1<<_TT_MSG_IS_START_MSG)));
}

void _Tt_message::
set_awaiting_reply(int flag)
{
	if (flag) {
		_flags |= (1<<_TT_MSG_AWAITING_REPLY);
	} else {
		_flags &= ~(1<<_TT_MSG_AWAITING_REPLY);
	}
}

int _Tt_message::
is_awaiting_reply() const
{
	return (_flags&(1<<_TT_MSG_AWAITING_REPLY));
}


Tt_status _Tt_message::
set_status(int st)
{
	// _status is used in the special case of a start message
	if (st == (int)TT_OK && _status == TT_WRN_START_MESSAGE) {
		return(TT_OK);
	}
	_status = st;
	SET_GUARD(_full_msg_guards,1,_TT_MSK_STATUS);
	return(TT_OK);
}


// 
// Sets the _handler field of a message for the special case of an
// observer replying to a start message. This special case is
// distinguished by turning on the _TT_MSG_OBSERVER flag.
// 
void _Tt_message::
set_observer_procid(const _Tt_procid_ptr &p)
{
	_flags |= (1<<_TT_MSG_OBSERVER);
	_handler = p;
}


// 
// Clears the _TT_MSG_OBSERVER flag and sets the _handler field back to
// null. 
// 
void _Tt_message::
clr_observer_procid()
{
	_flags &= ~(1<<_TT_MSG_OBSERVER);
	_handler = (_Tt_procid *)0;
}


static void
_tt_procid_list_print(const _Tt_ostream &os,
		      const _Tt_procid_list_ptr &procs, const char *label)
{
	if ((! procs.is_null()) && (procs->count() > 0)) {
		os << label << ":\n";
		_Tt_string indent = os.indent();
		os.set_indent( indent.cat( "\t" ));
		_Tt_procid_list_cursor procC( procs );
		while (procC.next()) {	
			procC->print( os );
		}
		os.set_indent( indent );
	}
}


//
// Prints out a message to os.
//
void _Tt_message::
print(const _Tt_ostream &os) const
{
	_Tt_arg_list_cursor		arg_cursor;

	os << _tt_enumname(state()) << " ";
	os << _tt_enumname(_paradigm) << " ";
	os << _tt_enumname(_message_class) << " ";
	os << "(" << _tt_enumname(reliability()) << " in ";
	os << _tt_enumname(scope()) << "): ";
	os << (int)_status << " == " << (Tt_status)_status << "\n";

	os << "id:\t\t" << _api_id << "\n";
	os << "op:\t\t" << _op << "\n";
	if (_args->count() > 0) {
		os << "args:\n";
		_Tt_string indent = os.indent();
		os.set_indent( indent.cat( "\t" ));
		_Tt_arg_list_cursor argC( _args );
		while (argC.next()) {	
			argC->print( os );
		}
		os.set_indent( indent );
	}
	if (_contexts->count() > 0) {
		os << "contexts:\n";
		_Tt_string indent = os.indent();
		os.set_indent( indent.cat( "\t" ));
		_Tt_msg_context_list_cursor contextC( _contexts );
		while (contextC.next()) {	
			contextC->print( os );
		}
		os.set_indent( indent );
	}
	if (_status_string.len() > 0) {
		os << "status_string:\t" << _status_string << "\n";
	}
	if (! _session.is_null()) {
		os << "session:\t" << _session->address_string() << "\n";
	}
	if (_file.len() > 0) {
		os << "file:\t\t" << _tt_network_path_to_local_path(_file)
		   << "\n";
	}
	if (_object.len() > 0) {
		os << "object:\t\t" << _object << "\n";
	}
	if (_otype.len() > 0) {
		os << "otype:\t\t" << _otype << "\n";
	}
	if (! _sender.is_null()) {
		os << "sender:\t\t[" << (unsigned long)_uid << "/" << (unsigned long)_gid << "] ";
		_sender->print( os );
	}
	if (_sender_ptype.len() > 0) {
		os << "sender_ptype:\t" << _sender_ptype << "\n";
	}
	if (_pattern_id.len() > 0) {
		os << "pattern:\t" << _pattern_id << "\n";
	}
	if (opnum() != -1) {
		os << "opnum:\t\t" << opnum() << "\n";
	}
	if (! _handler.is_null()) {
		os << "handler:\t";
		_handler->print( os );
	}
	if (_handler_ptype.len() > 0) {
		os << "handler_ptype:\t" << _handler_ptype << "\n";
	}
	_tt_procid_list_print( os, _accepters, "accepters" );
	_tt_procid_list_print( os, _rejecters, "rejecters" );
	_tt_procid_list_print( os, _abstainers, "abstainers" );
}
