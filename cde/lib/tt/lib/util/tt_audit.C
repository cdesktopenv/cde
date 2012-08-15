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
//%%  $XConsortium: tt_audit.C /main/13 1996/11/20 11:07:12 drk $ 			 				
/* %W 93/08/15
 *
 * tt_audit.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <unistd.h>
#include <fcntl.h>
#include "api/c/tt_c.h"
#include "util/tt_trace.h"
#include "util/tt_global_env.h"
#include "util/tt_audit.h"
#include "util/tt_port.h"
#include "util/tt_enumname.h"
#include "api/c/api_handle.h"
#include "mp/mp_c_mp.h"
#include "mp/mp_c_message.h"
#include "mp/mp_pattern.h"

extern _Tt_c_mp		*_tt_c_mp;

extern Tt_status	_tt_pointer_error(void *);

void	*_tt_error_pointer(Tt_status s);

// max length of a propename
#define TT_SPEC_PROPNAME_MAX_LEN         64

// This func does cascaded error checking, checking for some NULL
// pointers, and starts tracing. See _Tt_trace for parsing of argskey.
//
// Note carefully: in the loop which parses the args for error
// checking, the code relys on previous iterations having set
// certain variables, for example, the context slotname checking
// code relies on the msg variable having been set by a previous
// loop iteration.  All this in the name of coding efficiency.
//
Tt_status _Tt_audit::
entry(const char *argskey, _Tt_entry_pt func, ...)
{
	va_list			ap;
	char			c, *cp;
    const char *preview;
	int			num_args = strlen(argskey);
	Tt_message 		m;
	_Tt_c_message_ptr       msg;
	Tt_pattern		p;
        _Tt_pattern_ptr		pat;
#ifdef OPT_XTHREADS
	static xmutex_rec	mutex = XMUTEX_INITIALIZER;
#endif

	va_start(ap, func);
	_Tt_trace::entry(argskey, func, ap);

	// Increment the number of API calls made in this process.
#ifdef OPT_XTHREADS
	// Serialize the check for _tt_global
	xmutex_lock(&mutex);
#endif
	if (! _tt_global) {
		_tt_global = new _Tt_global();
	}
#ifdef OPT_XTHREADS
	xmutex_unlock(&mutex);
#endif

	_tt_global->grab_mutex();
	_tt_global->event_counter++;

	// init checking (formerly INITCHECK, PTR_INITCHECK, etc.
	switch (func) {
		case TT_DEFAULT_PTYPE:
		case TT_DEFAULT_PTYPE_SET:
		case TT_DEFAULT_FILE:
		case TT_DEFAULT_FILE_SET:
		case TT_DEFAULT_SESSION:
		case TT_DEFAULT_PROCID:
		case TT_DEFAULT_PROCID_SET:
		case TT_FILE_JOIN:
		case TT_FILE_QUIT:
		case TT_FILE_DESTROY:
		case TT_FILE_MOVE:
		case TT_FILE_COPY:
		case TT_FILE_OBJECTS_QUERY:
		case TT_MESSAGE_PRINT:
		case TT_MESSAGE_CREATE:
		case TT_MESSAGE_CREATE_SUPER:
		case TT_MESSAGE_DESTROY:
		case TT_MESSAGE_SEND:
		case TT_MESSAGE_SEND_ON_EXIT:
		case TT_MESSAGE_RECEIVE:
		case TT_MESSAGE_CALLBACK_ADD:
		case TT_MESSAGE_REJECT:
		case TT_MESSAGE_REPLY:
		case TT_MESSAGE_FAIL:
		case TT_MESSAGE_ACCEPT:
		case TT_MESSAGE_PATTERN:
		case TT_MESSAGE_USER:
		case TT_MESSAGE_USER_SET:
		case TT_MESSAGE_ARG_ADD:
		case TT_MESSAGE_BARG_ADD:
		case TT_MESSAGE_ARG_BVAL:
		case TT_MESSAGE_ARG_BVAL_SET:
		case TT_MESSAGE_ARG_IVAL:
		case TT_MESSAGE_ARG_IVAL_SET:
		case TT_MESSAGE_ARG_MODE:
		case TT_MESSAGE_ARG_TYPE:
		case TT_MESSAGE_ARG_VAL:
		case TT_MESSAGE_ARG_VAL_SET:
		case TT_MESSAGE_ARGS_COUNT:
		case TT_MESSAGE_XARG_ADD:
		case TT_MESSAGE_ARG_XVAL:
		case TT_MESSAGE_ARG_XVAL_SET:
		case TT_MESSAGE_CONTEXT_XVAL:
		case TT_MESSAGE_XCONTEXT_SET:
		case TT_MESSAGE_CONTEXT_VAL:
		case TT_MESSAGE_CONTEXT_IVAL:
		case TT_MESSAGE_CONTEXT_BVAL:
		case TT_MESSAGE_CONTEXT_SET:
		case TT_MESSAGE_ICONTEXT_SET:
		case TT_MESSAGE_BCONTEXT_SET:
		case TT_MESSAGE_CONTEXTS_COUNT:
		case TT_MESSAGE_CONTEXT_SLOTNAME:
		case TT_MESSAGE_CLASS_SET:
		case TT_MESSAGE_FILE:
		case TT_MESSAGE_FILE_SET:
		case TT_MESSAGE_OBJECT:
		case TT_MESSAGE_OBJECT_SET:
		case TT_MESSAGE_ID:
		case TT_MESSAGE_OP:
		case TT_MESSAGE_OP_SET:
		case TT_MESSAGE_OPNUM:
		case TT_MESSAGE_OTYPE:
		case TT_MESSAGE_OTYPE_SET:
		case TT_MESSAGE_ADDRESS:
		case TT_MESSAGE_ADDRESS_SET:
		case TT_MESSAGE_HANDLER:
		case TT_MESSAGE_HANDLER_SET:
		case TT_MESSAGE_HANDLER_PTYPE:
		case TT_MESSAGE_HANDLER_PTYPE_SET:
		case TT_MESSAGE_DISPOSITION:
		case TT_MESSAGE_DISPOSITION_SET:
		case TT_MESSAGE_SCOPE:
		case TT_MESSAGE_SCOPE_SET:
		case TT_MESSAGE_SENDER:
		case TT_MESSAGE_SENDER_PTYPE:
		case TT_MESSAGE_SENDER_PTYPE_SET:
		case TT_MESSAGE_SESSION:
		case TT_MESSAGE_SESSION_SET:
		case TT_MESSAGE_STATE:
		case TT_MESSAGE_STATUS_STRING:
		case TT_MESSAGE_STATUS_STRING_SET:
		case TT_MESSAGE_STATUS:
		case TT_MESSAGE_STATUS_SET:
		case TT_CLOSE:
		case TT_SESSION_JOIN:
		case TT_SESSION_QUIT:
		case TT_FD:
		case TT_PNOTICE_CREATE:
		case TT_PREQUEST_CREATE:
		case TT_ONOTICE_CREATE:
		case TT_OREQUEST_CREATE:
		case TT_PTYPE_OPNUM_CALLBACK_ADD:
		case TT_OTYPE_OPNUM_CALLBACK_ADD:
		case TT_SESSION_PROP_COUNT:
		case TT_SESSION_PROP:
		case TT_SESSION_PROP_ADD:
		case TT_SESSION_PROP_SET:
		case TT_SESSION_BPROP:
		case TT_SESSION_BPROP_ADD:
		case TT_SESSION_BPROP_SET:
		case TT_SESSION_PROPNAME:
		case TT_SESSION_PROPNAMES_COUNT:
		case TT_SPEC_BPROP:
		case TT_SPEC_BPROP_ADD:
		case TT_SPEC_BPROP_SET:
		case TT_SPEC_CREATE:
		case TT_SPEC_DESTROY:
		case TT_SPEC_FILE:
		case TT_SPEC_MOVE:
		case TT_SPEC_TYPE_SET:
		case TT_SPEC_PROP:
		case TT_SPEC_TYPE:
		case TT_SPEC_PROP_ADD:
		case TT_SPEC_PROP_COUNT:
		case TT_SPEC_PROP_SET:
		case TT_SPEC_PROPNAME:
		case TT_SPEC_PROPNAMES_COUNT:
		case TT_SPEC_WRITE:
		case TT_OBJID_EQUAL:
		case TT_OBJID_OBJKEY:
		case TT_PATTERN_PRINT:
		case TT_PATTERN_CREATE:
		case TT_PATTERN_DESTROY:
		case TT_PATTERN_REGISTER:
		case TT_PATTERN_UNREGISTER:
		case TT_PATTERN_CONTEXT_ADD:
		case TT_PATTERN_ICONTEXT_ADD:
		case TT_PATTERN_XCONTEXT_ADD:
		case TT_PATTERN_BCONTEXT_ADD:
		case TT_PTYPE_DECLARE:
		case TT_PTYPE_UNDECLARE:
		case TT_PTYPE_EXISTS:
		case TT_SESSION_TYPES_LOAD:
		case TT_CONTEXT_JOIN:
		case TT_ICONTEXT_JOIN:
		case TT_BCONTEXT_JOIN:
		case TT_CONTEXT_QUIT:
		case TT_ICONTEXT_QUIT:
		case TT_BCONTEXT_QUIT:
		case TT_XCONTEXT_JOIN:
		case TT_XCONTEXT_QUIT:
		case TT_OTYPE_DERIVEDS_COUNT:
		case TT_OTYPE_BASE:
		case TT_OTYPE_DERIVED:
		case TT_OTYPE_IS_DERIVED:
		case TT_OTYPE_OSIG_COUNT:
		case TT_OTYPE_HSIG_COUNT:
		case TT_OTYPE_OSIG_OP:
		case TT_OTYPE_HSIG_OP:
		case TT_OTYPE_OSIG_ARGS_COUNT:
		case TT_OTYPE_HSIG_ARGS_COUNT:
		case TT_OTYPE_OSIG_ARG_MODE:
		case TT_OTYPE_HSIG_ARG_MODE:
		case TT_OTYPE_OSIG_ARG_TYPE:
		case TT_OTYPE_HSIG_ARG_TYPE:
			if (!_tt_mp || !_tt_c_mp) {
				va_end(ap);
				return TT_ERR_NOMP;
			}
			if (_tt_c_mp && !_tt_c_mp->default_c_procid().c_pointer()) {
				va_end(ap);
				return TT_ERR_PROCID;
			}
		break;

		case TT_INITIAL_SESSION:
		case TT_PATTERN_CALLBACK_ADD:
		case TT_PATTERN_USER:
		case TT_PATTERN_USER_SET:
		case TT_PATTERN_CATEGORY:
		case TT_PATTERN_CATEGORY_SET:
		case TT_PATTERN_ARG_ADD:
		case TT_PATTERN_BARG_ADD:
		case TT_PATTERN_IARG_ADD:
		case TT_PATTERN_XARG_ADD:
		case TT_PATTERN_CLASS_ADD:
		case TT_PATTERN_FILE_ADD:
		case TT_PATTERN_OBJECT_ADD:
		case TT_PATTERN_OP_ADD:
		case TT_PATTERN_OPNUM_ADD:
		case TT_PATTERN_OTYPE_ADD:
		case TT_PATTERN_ADDRESS_ADD:
		case TT_PATTERN_DISPOSITION_ADD:
		case TT_PATTERN_SCOPE_ADD:
		case TT_PATTERN_SENDER_ADD:
		case TT_PATTERN_SENDER_PTYPE_ADD:
		case TT_PATTERN_SESSION_ADD:
		case TT_PATTERN_STATE_ADD:
			if (! _tt_mp) {
				va_end(ap);
				return(TT_ERR_NOMP);
			}
		break;
	}

	va_start(ap, func);

	// paramater checking for NULL & cascaded errors, over-long
	// propnames, etc.
	preview = argskey;
	while (num_args--) {
		switch (c = *preview++) {
			case VTYPE:
			case CHAR_PTR:
			case OBJID:
			case PROPNAME: {
				cp = va_arg(ap, char*);

				if ((cp == NULL) && (c != CHAR_PTR)) {
					va_end(ap);
					switch(func) {
					    case TT_SESSION_TYPES_LOAD:
						return TT_ERR_FILE;
					    case TT_MESSAGE_HANDLER_SET:
						return TT_ERR_PROCID;
					    default:
						return TT_ERR_POINTER;
					}
				}

				if (c == PROPNAME) {
					if (strlen(cp) > TT_SPEC_PROPNAME_MAX_LEN) {
						va_end(ap);
						return TT_ERR_PROPNAME;
					}
				}
			} break;

			case SESSION:
				cp = va_arg(ap, char*);

				if (cp == NULL) {
					va_end(ap);
					return TT_ERR_SESSION;
				}

				if (_tt_pointer_error(cp) != TT_OK) {
					va_end(ap);
					return TT_ERR_POINTER;
                                }
			break;

			case STRICT_VOID_PTR:
			case STRICT_CHAR_PTR:
				cp = va_arg(ap, char*);

				if (cp == NULL) {
					va_end(ap);
					if  (func == TT_SPEC_CREATE) {
						return TT_ERR_PATH;
					} else {
						return TT_ERR_POINTER;
					}
				}

				if (_tt_pointer_error(cp) != TT_OK) {
					va_end(ap);
					return TT_ERR_POINTER;
				}
			break;

			case STRICT_PAT_PTR:

				p = va_arg(ap, Tt_pattern);

				if (p == NULL) {
					va_end(ap);
					return TT_ERR_POINTER;
				}

				if (_tt_pointer_error(p) != TT_OK) {
					va_end(ap);
					return TT_ERR_POINTER;
				}

				switch (func) {
					case TT_PATTERN_PRINT:
					case TT_PATTERN_DESTROY:
					case TT_PATTERN_REGISTER:
					case TT_PATTERN_UNREGISTER:
					case TT_PATTERN_CATEGORY:
					case TT_PATTERN_CATEGORY_SET:
					case TT_PATTERN_ARG_ADD:
					case TT_PATTERN_BARG_ADD:
					case TT_PATTERN_IARG_ADD:
					case TT_PATTERN_XARG_ADD:
					case TT_PATTERN_CONTEXT_ADD:
					case TT_PATTERN_ICONTEXT_ADD:
					case TT_PATTERN_XCONTEXT_ADD:
					case TT_PATTERN_BCONTEXT_ADD:
					case TT_PATTERN_CLASS_ADD:
					case TT_PATTERN_FILE_ADD:
					case TT_PATTERN_OBJECT_ADD:
					case TT_PATTERN_OP_ADD:
					case TT_PATTERN_OPNUM_ADD:
					case TT_PATTERN_OTYPE_ADD:
					case TT_PATTERN_ADDRESS_ADD:
					case TT_PATTERN_DISPOSITION_ADD:
					case TT_PATTERN_SCOPE_ADD:
					case TT_PATTERN_SENDER_ADD:
					case TT_PATTERN_SENDER_PTYPE_ADD:
					case TT_PATTERN_SESSION_ADD:
					case TT_PATTERN_STATE_ADD:
					    pat = _tt_htab->lookup_pat(p);
				     
					    if (pat.is_null()) {
						    va_end(ap);
						    return TT_ERR_POINTER;
					    }
					break;
				}
			break;

			case STRICT_MESG_PTR: {

				m = va_arg(ap, Tt_message);

				if (m == NULL) {
					va_end(ap);
					return TT_ERR_POINTER;
				}

				if (_tt_pointer_error(m) != TT_OK) {
					va_end(ap);
					return TT_ERR_POINTER;
				}

				// checking specific to certain API calls
				switch (func) {
					case TT_MESSAGE_PRINT:
					case TT_MESSAGE_CREATE_SUPER:
					case TT_MESSAGE_DESTROY:
					case TT_MESSAGE_SEND:
					case TT_MESSAGE_SEND_ON_EXIT:
					case TT_MESSAGE_REJECT:
					case TT_MESSAGE_REPLY:
					case TT_MESSAGE_FAIL:
					case TT_MESSAGE_ACCEPT:
					case TT_MESSAGE_PATTERN:
					case TT_MESSAGE_ARG_ADD:
					case TT_MESSAGE_BARG_ADD:
					case TT_MESSAGE_ARG_BVAL:
					case TT_MESSAGE_ARG_BVAL_SET:
					case TT_MESSAGE_IARG_ADD:
					case TT_MESSAGE_ARG_IVAL:
					case TT_MESSAGE_ARG_IVAL_SET:
					case TT_MESSAGE_ARG_MODE:
					case TT_MESSAGE_ARG_TYPE:
					case TT_MESSAGE_ARG_VAL:
					case TT_MESSAGE_ARG_VAL_SET:
					case TT_MESSAGE_ARGS_COUNT:
					case TT_MESSAGE_XARG_ADD:
					case TT_MESSAGE_ARG_XVAL:
					case TT_MESSAGE_ARG_XVAL_SET:
					case TT_MESSAGE_XCONTEXT_SET:
					case TT_MESSAGE_CONTEXT_IVAL:
					case TT_MESSAGE_CONTEXT_VAL:
					case TT_MESSAGE_CONTEXT_XVAL:
					case TT_MESSAGE_CONTEXT_BVAL:
					case TT_MESSAGE_CONTEXT_SET:
					case TT_MESSAGE_ICONTEXT_SET:
					case TT_MESSAGE_BCONTEXT_SET:
					case TT_MESSAGE_CONTEXTS_COUNT:
					case TT_MESSAGE_CONTEXT_SLOTNAME:
					case TT_MESSAGE_CLASS:
					case TT_MESSAGE_CLASS_SET:
					case TT_MESSAGE_FILE:
					case TT_MESSAGE_FILE_SET:
					case TT_MESSAGE_OBJECT:
					case TT_MESSAGE_OBJECT_SET:
					case TT_MESSAGE_ID:
					case TT_MESSAGE_OP:
					case TT_MESSAGE_OP_SET:
					case TT_MESSAGE_OPNUM:
					case TT_MESSAGE_OTYPE:
					case TT_MESSAGE_OTYPE_SET:
					case TT_MESSAGE_ADDRESS:
					case TT_MESSAGE_ADDRESS_SET:
					case TT_MESSAGE_HANDLER:
					case TT_MESSAGE_HANDLER_SET:
					case TT_MESSAGE_HANDLER_PTYPE:
					case TT_MESSAGE_HANDLER_PTYPE_SET:
					case TT_MESSAGE_DISPOSITION:
					case TT_MESSAGE_DISPOSITION_SET:
					case TT_MESSAGE_SCOPE:
					case TT_MESSAGE_SCOPE_SET:
					case TT_MESSAGE_SENDER:
					case TT_MESSAGE_SENDER_PTYPE:
					case TT_MESSAGE_SENDER_PTYPE_SET:
					case TT_MESSAGE_SESSION:
					case TT_MESSAGE_SESSION_SET:
					case TT_MESSAGE_STATE:
					case TT_MESSAGE_STATUS_STRING:
					case TT_MESSAGE_STATUS_STRING_SET:
					case TT_MESSAGE_STATUS:
					case TT_MESSAGE_STATUS_SET:
						msg = _tt_htab->lookup_msg(m);

						if (msg.is_null()) {
							va_end(ap);
							return TT_ERR_POINTER;
						}

					break;
				}

			} break;

			// this is an index for a particular argument
			// We have a seperate case for this in order to
			// factor out the test below.
			case ARG_INTEGER: {
				int arg_index;
				_Tt_arg_list_ptr        msg_args(msg->args());

                                arg_index = va_arg(ap, int);

				if (func == TT_MESSAGE_ARG_BVAL		||
				    func == TT_MESSAGE_ARG_BVAL_SET	||
				    func == TT_MESSAGE_ARG_IVAL		||
				    func == TT_MESSAGE_ARG_IVAL_SET	||
				    func == TT_MESSAGE_ARG_MODE		||
				    func == TT_MESSAGE_ARG_TYPE		||
				    func == TT_MESSAGE_ARG_VAL		||
				    func == TT_MESSAGE_ARG_VAL_SET	||
				    func == TT_MESSAGE_ARG_XVAL		||
				    func == TT_MESSAGE_ARG_XVAL_SET) {

				    if (arg_index < 0 || msg_args.is_null() ||
					 msg_args->count() <= arg_index) {
					    return TT_ERR_NUM;
				    }
				} else {
					_tt_syslog(0, LOG_ERR,
						   "_Tt_audit::entry(): ARG_INTEGER: func==%s",
						   _tt_enumname(func));
					return TT_ERR_INTERNAL;
				}
			} break;

			case STRICT_SLOTNAME:
			case SLOTNAME: {
				char *slotname;

				slotname = va_arg(ap, char *);

				if (slotname == NULL) {
					va_end(ap);
					return TT_ERR_SLOTNAME;
				}

				if (c == STRICT_SLOTNAME) {
					if (_tt_pointer_error(slotname) != TT_OK) {
						va_end(ap);
						return _tt_pointer_error(slotname);
					}

					// if the slotname is the first
					// arg, as a STRICT_SLOTNAME implies,
					// then there won't have been a
					// message passed previously, so
					// don't check for it below.
					va_end(ap);
					return TT_OK;
				}

				switch (func) {
					case TT_MESSAGE_CONTEXT_SET:
					case TT_MESSAGE_BCONTEXT_SET:
					case TT_MESSAGE_ICONTEXT_SET:
					case TT_MESSAGE_XCONTEXT_SET:
					// context may not have been
					// created yet.
					break;

					default: {
						_Tt_msg_context_ptr cntxt;

						cntxt = msg->context(slotname);
						if (cntxt.is_null()) {
							va_end(ap);
							return TT_ERR_SLOTNAME;
						}
					} break;
				}
			} break;


			case MESG_PTR: {
				Tt_message msg;

				msg = va_arg(ap, Tt_message);

				if (msg == NULL) {
					va_end(ap);
					return TT_ERR_POINTER;
				}
			} break;

			case BYTES_WITH_LEN:
			case VOID_PTR:
#ifdef __ppc
				cp = (char *) va_arg(ap, void *);
#else
				(void) va_arg(ap, void *);
#endif
			break;

			case VOID:
#ifdef __ppc
				cp = (char *) va_arg(ap, void *);
#else
				(void) va_arg(ap, void *);
#endif
			break;

			case INTEGER:
			case AUDIT_STATUS:
			case ADDRESS:
			case DISPOSITION:
		        case STATE:
		        case FEATURE:
			case CLASS:
			case CATEGORY:
			case MODE:
			case SCOPE:
			case BYTES_LENGTH:
#ifdef __ppc
                                int  junk_int;
				junk_int = va_arg(ap, int);
#else
				(void) va_arg(ap, int);
#endif
			break;

			default:
				_tt_syslog( 0, LOG_ERR,
					    "_Tt_audit::entry(): argskey=='%c'",
					    c );
				va_end(ap);
				return TT_ERR_INTERNAL;
			// NOTREACHED
			// break;
		}
	}
	va_end(ap);

	return TT_OK;
}
