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
//%%  $TOG: tt_trace.C /main/8 1998/04/09 17:53:02 mgreess $ 			 				
/*
 * @(#)tt_trace.C	1.28 95/09/20
 *
 * Copyright 1990,1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include <unistd.h>
#include <fcntl.h>
#include "api/c/tt_c.h"
#include "util/tt_global_env.h"
#include "util/tt_trace.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_trace_stream.h"
#include "util/tt_enumname.h"
#include "util/tt_iostream.h"

extern _Tt_string	_tt_entrypt_to_string(_Tt_entry_pt);
extern Tt_status	_tt_pointer_error(void *);
extern Tt_status	_tt_int_error(int);
extern void		tt_error();

#ifdef _OPT_TIMERS_SUNOS
extern "C" { int gettimeofday(struct timeval *); }
#endif

_Tt_trace_stream      **_Tt_trace::_pstream	= 0;
int			_Tt_trace::_toggled_off	= 0;

//
// Initializes tracing.
// Returns:
//	1	tracing is on
//	0	tracing is off
//	
int
_Tt_trace::init(
	int even_if_no_envariable
)
{
	//
	// States:
	//	_pstream == 0		tracing ready to be (re-)initialized
	//	*_pstream == 0		tracing initialized but off
	//	*_pstream != 0		tracing initialized and on
	//
	if (_pstream == 0) {
		_pstream = (_Tt_trace_stream **)
			malloc( sizeof( _Tt_trace_stream * ));
		*_pstream = 0;
		_toggled_off = 0;
	} else if (! even_if_no_envariable) {
		// already initialized
		return any_tracing();
	}

	if ((*_pstream != 0) && _toggled_off) {
		//
		// When tracing is toggled off, tt_trace_control(1)
		// is equivalent to tt_trace_control(-1)
		//
		toggle();
		return any_tracing();
	}
	// (re-)initialize tracing
	_Tt_string script = getenv( TRACE_SCRIPT );
	if (script.len() <= 0) {
		if (! even_if_no_envariable) {
			return any_tracing();
		}
		if (*_pstream != 0) {
			//
			// When tracing is already on and they
			// ask us to reread the envariable,
			// we do nothing if the envariable is unset.
			//
			return any_tracing();
		}
	}

	Tt_status init_status = TT_OK;
	if (    (script.len() > 0)
	     && (script[0] == '.' || script[0] == '/'))
	{
		// script holds a filename
		int script_fd = open( (char *)script, O_RDONLY );
		fcntl(script_fd, F_SETFD, 1);	/* Close on exec */
		if (script_fd < 0) {
			_tt_syslog( 0, LOG_ERR, "$%s=%s: %m",
				    TRACE_SCRIPT,
				    script.operator const char *() );
			return any_tracing();
		}
		*_pstream = new _Tt_trace_stream( script_fd );
		init_status = (*_pstream)->init();
		close( script_fd );
	} else {
		*_pstream = new _Tt_trace_stream( script );
		init_status = (*_pstream)->init();
	}
	switch (init_status) {
	    case TT_ERR_INVALID:
	    case TT_ERR_FILE:
		_tt_syslog( 0, LOG_ERR,
			    catgets( _ttcatd, 1, 22,
				     "Could not process tttracefile in $%s" ),
			    TRACE_SCRIPT );
		// fall through
	    case TT_WRN_STOPPED:
		//
		// No tracing specified by script.  Unset variable
		// so when tracing gets toggled back on something
		// useful will happen.
		//
		_tt_putenv( TRACE_SCRIPT, 0 );
		delete *_pstream;
		*_pstream = 0;
		return any_tracing();
	}
	if (! (*_pstream)->follow_val()) {
		// unset TRACE_SCRIPT so children are not traced
		_tt_putenv( TRACE_SCRIPT, 0 );
	}
	return any_tracing();
}

//
// Turn off tracing.  The next ::init() will re-read script.
//
void
_Tt_trace::fini()
{
        if (_pstream != 0) {
		if (*_pstream != 0) {
			delete *_pstream;
		}
		*_pstream = 0;
		free( _pstream );
		_pstream = 0;
	}
	_toggled_off = 0;
}

//
// Turn off tracing.  The next ::init() will re-read script.
//
void
_Tt_trace::toggle()
{
	if (_pstream == 0) {
		init( 1 );
		return;
	}
	_toggled_off = ! _toggled_off;
}

void
_Tt_trace::entry(
	_Tt_message  &msg,
	Tt_state            old_state
)
{
	if (! _entry()) {
		return;
	}
	_tracing = (*_pstream)->is_traced( msg );
	if (! _tracing) {
		return;
	}
#ifdef _OPT_TIMERS_SUNOS
	else if ((*_pstream)->timers_val()) {
		timestamp.tv_sec = timestamp.tv_usec = (long) 0;
		gettimeofday(&timestamp);
	}
#endif
	if ((*_pstream)->get_is_entered() == 1) {
		**_pstream << "...\n";
		(*_pstream)->set_is_entered(0);
	}

	**_pstream << _tt_enumname( old_state ) << " => ";
	**_pstream << _tt_enumname( msg.state() ) << ": ";
	if ((*_pstream)->attributes_val()) {
		**_pstream << "\n";
	}
	(*_pstream)->incr_indent();
	**_pstream << msg;
}

void
_Tt_trace::entry(
	_Tt_message  &msg,
	const _Tt_procid   &recipient
)
{
	if (! _entry()) {
		return;
	}
	_tracing = (*_pstream)->is_traced( msg, Deliver );
	if (! _tracing) {
		return;
	}
#ifdef _OPT_TIMERS_SUNOS
	else if ((*_pstream)->timers_val()) {
		timestamp.tv_sec = timestamp.tv_usec = (long) 0;
		gettimeofday(&timestamp);
	}
#endif
	if ((*_pstream)->get_is_entered() == 1) {
		**_pstream << "...\n";
		(*_pstream)->set_is_entered(0);
	}

	**_pstream << "Tt_message => ";
	recipient.print( **_pstream );
	if (! _tt_mp->in_server()) {
		// This is the client's only chance to print it.
		**_pstream << msg;
	}

	(*_pstream)->incr_indent();
}

void
_Tt_trace::entry(
	_Tt_message  &msg,
	_Tt_dispatch_reason reason
)
{
	if (! _entry()) {
		return;
	}
	_tracing = (*_pstream)->is_traced( msg, Dispatch );
	if (! _tracing) {
		return;
	}
#ifdef _OPT_TIMERS_SUNOS
	else if ((*_pstream)->timers_val()) {
		timestamp.tv_sec = timestamp.tv_usec = (long) 0;
		gettimeofday(&timestamp);
	}
#endif
	if ((*_pstream)->get_is_entered() == 1) {
		**_pstream << "...\n";
		(*_pstream)->set_is_entered(0);
	}

	const char *s;
	int printmsg = 1;
	//
	// We do not print msg if it is a (often incomplete) update
	// to a message we have alread printed.
	//
	switch (reason) {
	    case TTDR_MESSAGE_SEND:
		s = _tt_enumname( TT_MESSAGE_SEND );
		break;
	    case TTDR_MESSAGE_SEND_ON_EXIT:
		s = _tt_enumname( TT_MESSAGE_SEND_ON_EXIT );
		break;
	    case TTDR_MESSAGE_REJECT:
		s = _tt_enumname( TT_MESSAGE_REJECT );
		// We print the message, so the trace will show
		// any mods the rejector made to the message.
		break;
	    case TTDR_MESSAGE_ABSTAIN:
		s = _tt_enumname( TT_MESSAGE_DESTROY );
		break;
	    case TTDR_MESSAGE_FAIL:
		s = _tt_enumname( TT_MESSAGE_FAIL );
		// state change imminent; no need to print
		printmsg = 0;
		break;
	    case TTDR_MESSAGE_REPLY:
		s = _tt_enumname( TT_MESSAGE_REPLY );
		// state change imminent; no need to print
		printmsg = 0;
		break;
	    case TTDR_SESSION_JOIN:
		s = _tt_enumname( TT_SESSION_JOIN );
		break;
	    case TTDR_FILE_JOIN:
		s = _tt_enumname( TT_FILE_JOIN );
		break;
	    case TTDR_MESSAGE_ACCEPT:
		s = _tt_enumname( TT_MESSAGE_ACCEPT );
		break;
	    case TTDR_HUPDATE:
		s = "ttsession <- ttsession";
		break;
	    case TTDR_HDISPATCH:
		s = "ttsession -> ttsession";
		break;
	    case TTDR_ERR_PTYPE_START:
		s = _tt_enumname( TT_ERR_PTYPE_START );
		// state change imminent; no need to print
		printmsg = 0;
		break;
	    case TTDR_ERR_PROCID:
		s = _tt_enumname( TT_ERR_PROCID );
		// state change imminent; no need to print
		printmsg = 0;
		break;
	}
	**_pstream << s << ": ";
	if ((*_pstream)->attributes_val()) {
		**_pstream << "\n";
	}

	(*_pstream)->incr_indent();
	if (! _tt_mp->in_server()) {
		printmsg = 1;
	}
	if (printmsg) {
		**_pstream << msg;
	}
}

//
// trace this API entry point.  See argskey doc in header file.
//
void
_Tt_trace::entry(
	char         *argskey,
	_Tt_entry_pt  func,
	va_list       ap
)
{
	_funcname = func;

	if (! _entry()) {
		return;
	}
	_tracing = (*_pstream)->is_traced( func );
	if (! _tracing) {
		return;
	}
#ifdef _OPT_TIMERS_SUNOS
	else if ((*_pstream)->timers_val()) {
		timestamp.tv_sec = timestamp.tv_usec = (long) 0;
		gettimeofday(&timestamp);
	}
#endif

	if ((*_pstream)->get_is_entered() == 1) {
		**_pstream << "...\n";
		(*_pstream)->set_is_entered(0);
	}

        char c, *preview;
        int num_args = strlen(argskey);

	// print the API name and open paren
	**_pstream << _tt_enumname(func) << "(";

	// print the arguments
	preview = argskey;
	while (num_args--) {
		c = *preview++;
		switch (c) {
                        /* For the "type" to va_arg, we must pass int!
                         * If we try to use the real type (e.g. Tt_address, etc.)
                         * gcc will complain loudly and cause the program to abort here.
                         * This is because the size of enums can be less than that of int,
                         * and in va_args anything smaller than an int is "promoted"
                         * into an int.
                         */
			case ADDRESS : {
				Tt_address s;
				s = (Tt_address) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case STATE: {
				Tt_state s;
				s = (Tt_state) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case DISPOSITION: {
				Tt_disposition s;
				s = (Tt_disposition) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case CLASS: {
				Tt_class s;
				s = (Tt_class) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case MODE: {
				Tt_mode s;
				s = (Tt_mode) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case CATEGORY: {
				Tt_category s;
				s = (Tt_category) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

			case SCOPE: {
				Tt_scope s;
				s = (Tt_scope) va_arg(ap, int);
				**_pstream << _tt_enumname(s);
			} break;

		        case FEATURE: {
				Tt_feature f;
				f = (Tt_feature) va_arg(ap, int);
				**_pstream << _tt_enumname(f);
			} break;

			case AUDIT_STATUS: {
				Tt_status status;
				status = (Tt_status) va_arg(ap, int);
				**_pstream << status;
			} break;

			// XXX - I can't let a hack like this
			// exist w/out some level of error checking,
			// even though the error checked can only
			// be generated by my own coding error.
			// This case should never be seen since the
			// case for BYTES_WITH_LEN should swallow it up.
			case BYTES_LENGTH:
				_tt_syslog( 0, LOG_ERR, "_Tt_trace::entry(): "
					    "BYTES_LENGTH && !BYTES_WITH_LEN" );
			break;

			// a string w/ non-printable chars.
			// the next arg will be an int, which is how
			// many bytes this string is.  Get a copy of
			// the next arg now so we know how many bytes
			// to store.
			case BYTES_WITH_LEN: {
				char *cp;
				int length;

				cp = va_arg(ap, char *);

				// if this is not followed by a length
				// paramater, we are doing something
				// wrong.
				if (*preview != BYTES_LENGTH) {
					_tt_syslog( 0, LOG_ERR,
						"_Tt_trace::entry(): "
						"BYTES_WITH_LEN && !BYTES_LENGTH");
					return;
				}

				// Get the length field
				length = va_arg(ap, int);

				**_pstream << (void *)cp;
				if ((cp != 0)
				    && (_tt_pointer_error(cp) == TT_OK))
				{
					**_pstream << "==";
					_Tt_string s((const unsigned char *)cp,
						     length );
					s.print( **_pstream,
						 _Tt_string_user_width, 1 );
				}

				// use the next arg up
				preview++;
				num_args--;

				// now trace the length arg.  Note
				// that there are a couple of exceptions.
				// This is due to an optimization in
				// the implementation of the specified
				// functions, which don't interact well
				// with the tracing implementation.
				switch (func) {
					    case TT_SESSION_PROP_ADD:
					    case TT_SESSION_PROP_SET:
					    case TT_SPEC_PROP_ADD:
					    case TT_SPEC_PROP_SET:
					    break;

					    default:
					    **_pstream << length;
					    break;
				}
			} break;

			case OBJID:
			case STRICT_PAT_PTR:
			case STRICT_MESG_PTR:
			case MESG_PTR: {
                                void *vp;
                                vp = va_arg(ap, void*);
				**_pstream << vp;
			} break;

			case SLOTNAME:
			case PROPNAME:
			case SESSION:
			case VTYPE:
			case STRICT_SLOTNAME:
			case STRICT_CHAR_PTR:
			case CHAR_PTR: {
				char *cp;
				cp = va_arg(ap, char *);
				**_pstream << (void *)cp;
				if (   (cp != 0)
				    && (_tt_pointer_error(cp) == TT_OK))
				{
					**_pstream << "==";
					_Tt_string s( cp );
					s.print( **_pstream,
						 _Tt_string_user_width, 1 );
				}
			} break;

			// These things may be non-printable, for one reason
			// or another: function ptrs, caddr_t's, etc.
			case VOID_PTR:
			case STRICT_VOID_PTR: {
				void *vp;
				vp = va_arg(ap, void*);
				**_pstream << vp;
			} break;

			case ARG_INTEGER:
			case INTEGER: {
				int i;
				i = va_arg(ap, int);
				**_pstream << i;
			} break;

			case VOID:
#ifdef __ppc
                                int ii;
				ii = va_arg(ap, int);
#else
				(void) va_arg(ap, int);
#endif
			break;

			default:
				_tt_syslog( 0, LOG_ERR, "_Tt_trace::entry(): "
					    "argskey=='%c'", c );
				return;
		}

		if (num_args == 0) {
			**_pstream << ") ";
		} else {
			**_pstream << ", ";
		}
	}
	switch (func) {
	    case TT_FREE:
	    case TT_RELEASE:
		// These return void, so we are done
		**_pstream << "\n";
		break;
	}
	(*_pstream)->set_is_entered(1);
	(*_pstream)->incr_indent();
}


//
// trace this user callback function
//
void
_Tt_trace::entry(
	Tt_message_callback cb,
	Tt_message	mh,
	Tt_pattern	ph
)
{
        if (! _entry()) {
                return;
        }
	_tracing = (*_pstream)->is_traced( TT_MESSAGE_RECEIVE );
	if (! _tracing) {
		return;
	}
#ifdef _OPT_TIMERS_SUNOS
	else if ((*_pstream)->timers_val()) {
		timestamp.tv_sec = timestamp.tv_usec = (long) 0;
		gettimeofday(&timestamp);
	}
#endif

        if ((*_pstream)->get_is_entered() == 1) {
                **_pstream << "...\n";
		(*_pstream)->set_is_entered(0);
        }

	**_pstream << (void *)cb << " ("
		   << (void *)mh << ", "
		   << (void *)ph << ")";

	(*_pstream)->set_is_entered(1);
	(*_pstream)->incr_indent();
}


const _Tt_trace &
_Tt_trace::operator <<(
	_Tt_message &msg
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << msg;
	return *this;
}

const _Tt_trace &
_Tt_trace::operator <<(
	const _Tt_pattern &pat
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << pat;
	return *this;
}

const _Tt_trace &
_Tt_trace::operator <<(
	const _Tt_string &s
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << s;
	return *this;
}

const _Tt_trace &
_Tt_trace::operator <<(
	Tt_status status
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << status;
	return *this;
}

const _Tt_trace &
_Tt_trace::operator <<(
	const char *s
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << s;
	return *this;
}

const _Tt_trace &
_Tt_trace::operator <<(
	int n
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	**_pstream << n;
	return *this;
}

const _Tt_trace &
_Tt_trace::print(
	_Tt_object_printfn print_it,
	const _Tt_object  *obj
) const
{
	if ((! any_tracing()) || (! _tracing)) {
		return *this;
	}
	(*_pstream)->print( print_it, obj );
	return *this;
}

void
_Tt_trace::incr_indent() const
{
	if ((! any_tracing()) || (! _tracing)) {
		return;
	}

	(*_pstream)->incr_indent();
}

void
_Tt_trace::decr_indent() const
{
	if ((! any_tracing()) || (! _tracing)) {
		return;
	}

	(*_pstream)->decr_indent();
}

void
_Tt_trace::exitq()
{

	if ((! any_tracing()) || (! _tracing)) {
		return;
	}

	(*_pstream)->decr_indent();
	(*_pstream)->set_is_entered(0);

#ifdef _OPT_TIMERS_SUNOS
	if ((*_pstream)->timers_val()) {
		struct timeval newtime;
		char time_string[BUFSIZ];
		long temp;
		gettimeofday(&newtime);
		temp = newtime.tv_usec - timestamp.tv_usec;
		if (temp < 0) {
			temp += (long) 1000000;
			newtime.tv_sec -= 1;
		}
		sprintf(time_string, "%d.%06d",
			newtime.tv_sec - timestamp.tv_sec, temp);
	        **_pstream << " (" << time_string << " seconds)";
	}
#endif
}

void
_Tt_trace::exit()
{
        _tt_global->drop_mutex();

	exitq();
}

void _Tt_trace::
exit(
	void *val
)
{
	Tt_status val_status = _tt_pointer_error(val);

	if (any_tracing() && _tracing) {

		(*_pstream)->decr_indent();
		(*_pstream)->set_is_entered(0);

		**_pstream << "= " << val << " (";
		**_pstream << _tt_pointer_error(val) << ")";

#ifdef _OPT_TIMERS_SUNOS
		if ((*_pstream)->timers_val()) {
			struct timeval newtime;
			char time_string[BUFSIZ];
			long temp;
			gettimeofday(&newtime);
			temp = newtime.tv_usec - timestamp.tv_usec;
			if (temp < 0) {
				temp += (long) 1000000;
				newtime.tv_sec -= 1;
			}
			sprintf(time_string, "%d.%06d",
				newtime.tv_sec - timestamp.tv_sec, temp);
			**_pstream << " (" << time_string << " seconds)";
		}
#endif

		**_pstream << "\n";
	}

        if (val_status != TT_OK) {
		tt_error((char *) _tt_entrypt_to_string(_funcname), val_status);
        }

        _tt_global->drop_mutex();
}


void _Tt_trace::
exit(
	Tt_message val
)
{
	exit( (void *)val );
}

void _Tt_trace::
exit(
	Tt_pattern val
)
{
	exit( (void *)val );
}

void _Tt_trace::
exit(
	char *val
)
{
	Tt_status val_status = _tt_pointer_error(val);

	if (any_tracing() && _tracing) {

		(*_pstream)->decr_indent();
		(*_pstream)->set_is_entered(0);

		**_pstream << "= " << (void *)val;
		if ((val != 0) && (val_status == TT_OK)) {
			**_pstream << "==";
			_Tt_string s( val );
			s.print( **_pstream, _Tt_string_user_width, 1 );
		}

		**_pstream << " (" << val_status << ")";

#ifdef _OPT_TIMERS_SUNOS
		if ((*_pstream)->timers_val()) {
			struct timeval newtime;
			char time_string[BUFSIZ];
			long temp;
			gettimeofday(&newtime);
			temp = newtime.tv_usec - timestamp.tv_usec;
			if (temp < 0) {
				temp += (long) 1000000;
				newtime.tv_sec -= 1;
			}
			sprintf(time_string, "%d.%06d",
				newtime.tv_sec - timestamp.tv_sec, temp);
			**_pstream << " (" << time_string << " seconds)";
		}
#endif

		**_pstream << "\n";
	}

        if (val_status != TT_OK) {
		tt_error((char *) _tt_entrypt_to_string(_funcname), val_status);
        }

        _tt_global->drop_mutex();
}

void _Tt_trace::
exit(
	int val
)
{
	Tt_status val_status = _tt_int_error(val);

	if (any_tracing() && _tracing) {

		(*_pstream)->decr_indent();
		(*_pstream)->set_is_entered(0);

		**_pstream << "= " << val << " (";
		**_pstream << val_status << ")";

#ifdef _OPT_TIMERS_SUNOS
		if ((*_pstream)->timers_val()) {
			struct timeval newtime;
			char time_string[BUFSIZ];
			long temp;
			gettimeofday(&newtime);
			temp = newtime.tv_usec - timestamp.tv_usec;
			if (temp < 0) {
				temp += (long) 1000000;
				newtime.tv_sec -= 1;
			}
			sprintf(time_string, "%d.%06d",
				newtime.tv_sec - timestamp.tv_sec, temp);
			**_pstream << " (" << time_string << " seconds)";
		}
#endif

		**_pstream << "\n";
	}

        if (val_status != TT_OK) {
		tt_error((char *) _tt_entrypt_to_string(_funcname), val_status);
        }

        _tt_global->drop_mutex();
}

void _Tt_trace::
exit(Tt_status val)
{
	if (any_tracing() && _tracing) {

		(*_pstream)->decr_indent();
		(*_pstream)->set_is_entered(0);

		**_pstream << "= " << (int)val << " (" << val << ")";

#ifdef _OPT_TIMERS_SUNOS
		if ((*_pstream)->timers_val()) {
			struct timeval newtime;
			char time_string[BUFSIZ];
			long temp;
			gettimeofday(&newtime);
			temp = newtime.tv_usec - timestamp.tv_usec;
			if (temp < 0) {
				temp += (long) 1000000;
				newtime.tv_sec -= 1;
			}
			sprintf(time_string, "%d.%06d",
				newtime.tv_sec - timestamp.tv_sec, temp);
			**_pstream << " (" << time_string << " seconds)";
		}
#endif

		**_pstream << "\n";
	}

        if (val != TT_OK) {
		tt_error((char *) _tt_entrypt_to_string(_funcname), val);
        }

        _tt_global->drop_mutex();
}

void _Tt_trace::
_exit(int val, const char *(*enumname)(int), int drop)
{
	Tt_status val_status = _tt_int_error( val );

	if (any_tracing() && _tracing) {

		(*_pstream)->decr_indent();
		(*_pstream)->set_is_entered(0);


		**_pstream << "= " << (int)val << "==" << (*enumname)(val)
			   << " (" << val_status << ")";

#ifdef _OPT_TIMERS_SUNOS
		if ((*_pstream)->timers_val()) {
			struct timeval newtime;
			char time_string[BUFSIZ];
			long temp;
			gettimeofday(&newtime);
			temp = newtime.tv_usec - timestamp.tv_usec;
			if (temp < 0) {
				temp += (long) 1000000;
				newtime.tv_sec -= 1;
			}
			sprintf(time_string, "%d.%06d",
				newtime.tv_sec - timestamp.tv_sec, temp);
			**_pstream << " (" << time_string << " seconds)";
		}
#endif

		**_pstream << "\n";
	}

        if (val_status != TT_OK) {
		tt_error((char *) _tt_entrypt_to_string(_funcname), val_status);
        }

        if (drop) _tt_global->drop_mutex();
}

void _Tt_trace::
exit(Tt_callback_action val)
{
	const char *(*enumname)(Tt_callback_action) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname, 0);
}

void _Tt_trace::
exit(Tt_mode val)
{
	const char *(*enumname)(Tt_mode) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_scope val)
{
	const char *(*enumname)(Tt_scope) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_class val)
{
	const char *(*enumname)(Tt_class) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_category val)
{
	const char *(*enumname)(Tt_category) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_address val)
{
	const char *(*enumname)(Tt_address) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_disposition val)
{
	const char *(*enumname)(Tt_disposition) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

void _Tt_trace::
exit(Tt_state val)
{
	const char *(*enumname)(Tt_state) = &_tt_enumname;
	_exit( val, (const char *(*)(int))enumname );
}

//
// Cheap way to find out if there is any possibility that
// tracing needs to be done.
//
int _Tt_trace::
any_tracing()
{
	return (_pstream != 0) && (*_pstream != 0) && (! _toggled_off);
}

int
_Tt_trace::_entry()
{
	_tracing = 0;
	if (! init()) {
		return 0;
	}
	if (! _allowed2trace()) {
		static int warned = 0;
		if (! warned) {
			_tt_syslog( 0, LOG_WARNING,
				    catgets( _ttcatd, 1, 23,
					     "No tracing allowed if real UID "
					     "does not equal effective UID" ));
			warned = 1;
		}
		return 0;
	}
	return any_tracing();
}

//
// check that rules on UID & GID are enforced so people can't
// break security by setting trace files to binary.  If real
// and effective group/user IDs are different, and they aren't
// super user, deny permission to log.
//
int
_Tt_trace::_allowed2trace()
{
	uid_t uid = getuid(), euid = geteuid();
	gid_t gid = getgid(), egid = getegid();

        if ((euid == 0) || ((uid == euid) && (gid == egid))) {
		return 1;
	}
	return 0;
}
