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
//%%  $XConsortium: tt_trace_stream.C /main/3 1995/10/23 10:46:52 rswiston $ 			 				
/*
 * @(#)tt_trace_stream.C	1.13 94/11/18
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include "util/tt_enumname.h"
#include "util/tt_port.h"
#include "util/tt_trace_stream.h"
#include "mp/mp_pattern.h"
#include "mp/mp_procid.h"

const int Tt_trace_buf_len = 4096;

_Tt_trace_stream::_Tt_trace_stream()
{
}

_Tt_trace_stream::_Tt_trace_stream(
	const char *script
) :
	_Tt_trace_parser( script )
{
	_is_entered = 0;
}

_Tt_trace_stream::_Tt_trace_stream(
	int scriptfd
) :
	_Tt_trace_parser( scriptfd )
{
	_is_entered = 0;
}

_Tt_trace_stream::~_Tt_trace_stream()
{
	if (theFILE() != 0) {
		if (theFILE() != stderr) {
			fclose( theFILE() );
		}
		this->_Tt_ostream::operator=( (FILE *)0 );
	}
}

//
// Returns
//	TT_OK
//	TT_WRN_STOPPED		the script has turned off all tracing
//	TT_ERR_INVALID		syntax error in script
//	TT_ERR_FILE		could not open output file
//	
Tt_status
_Tt_trace_stream::init()
{
	int status = parse();
	if (status != 0) {
		return TT_ERR_INVALID;
	}
	if (! any_tracing()) {
		return TT_WRN_STOPPED;
	}
	if (follow_val()) {
		char buf[ 20 ];
		::sprintf( buf, "%d:\t", getpid() );
		set_indent( buf );
	}

	if (sink_val().len() <= 0) {
		this->_Tt_ostream::operator=( stderr );
	} else {
		FILE	*fp = fopen( sink_val(), mode_val());
		this->_Tt_ostream::operator=( fp );
		if (theFILE() == 0) {
			_tt_syslog( 0, LOG_ERR, "tttracefile: %s: %m",
				    sink_val().operator const char *() );
			return TT_ERR_FILE;
		}
		fcntl(fileno(fp), F_SETFD, 1);	/* Close on exec */
		// Use line-buffering, in case of many writers
		setvbuf( theFILE(), 0, _IOLBF, 0 );
	}
	return TT_OK;
}

void
_Tt_trace_stream::incr_indent() const
{
	set_indent( indent().cat("\t") );
}

void
_Tt_trace_stream::decr_indent() const
{
	set_indent( indent().left( indent().len() - 1 ) );
}

const _Tt_trace_stream &
operator <<(
	_Tt_trace_stream &os,
	_Tt_message      &msg
)
{
	if (os.attributes_val()) {
		// prevents infinite recursion
		os._Tt_ostream::operator <<( msg );
		return os;
	}
	os << _tt_enumname( msg.message_class() );
	os << " <" << msg.api_id() << "> ";
	os << _tt_enumname( msg.state() );
	const char *conjunction = " because ";
	switch (msg.state()) {
	    case TT_CREATED:
	    case TT_SENT:
		os << " by <" << msg.sender()->id() << ">";
		break;
	    case TT_HANDLED:
		conjunction = " result: ";
	    case TT_REJECTED:
	    case TT_FAILED:
		if (! msg.handler().is_null()) {
			os << " by <" << msg.handler()->id() << ">";
		}
		os << conjunction << (Tt_status)msg.status();
		break;
	    default:
		break;
	}
	os << " " << msg.op() << "()\n";
	return os;
}
