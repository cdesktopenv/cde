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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_trace_stream.h /main/3 1995/10/23 10:46:59 rswiston $ 			 				 */
/*
 * @(#)tt_trace_stream.h	1.6 93/08/15
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef TT_TRACE_STREAM_H
#define TT_TRACE_STREAM_H

#include "util/tt_trace_parser.h"
#include "util/tt_iostream.h"

//
// We cannot have a _Tt_trace_stream_ptr, because _Tt_object is not a
// virtual _Tt_allocated.  Making _Tt_trace_parser and _Tt_ostream be
// virtual _Tt_objects does not seem to help, because that breaks the
// ptr scheme.
//
class _Tt_trace_stream : public _Tt_trace_parser, public _Tt_ostream
{
    public:
				_Tt_trace_stream();
				_Tt_trace_stream(
					const char *script
				);
				_Tt_trace_stream(
					int         scriptfd
				);
				~_Tt_trace_stream();

	Tt_status		init();

	void			set_is_entered(int on_off)
					{ _is_entered = on_off; }
	int			get_is_entered() const
					{ return _is_entered; }

	void			incr_indent() const;
	void			decr_indent() const;

    private:
	// True iff we are in the middle of emitting a line.
	int			_is_entered;
};

//
// Redefine << _Tt_message because we print them at different
// levels of verbosity.  If we overloaded the method in
// _Tt_trace_stream, then C++ would make us redefine all the
// operator << methods.
//
const _Tt_trace_stream	       &operator <<(
					_Tt_trace_stream	&os,
					_Tt_message		&msg
				);

#endif	/* TT_TRACE_STREAM_H */
