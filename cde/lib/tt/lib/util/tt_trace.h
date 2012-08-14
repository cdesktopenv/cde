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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_trace.h /main/7 1996/08/29 16:12:38 barstow $ 			 				 */
/* -*-C++-*-
 *
 * @(#)tt_trace.h	1.26 95/09/18
 * 
 * Copyright 1990,1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef TT_TRACE_H
#define TT_TRACE_H
#include <stdio.h>
#include <stdarg.h>
#ifdef _OPT_TIMERS_SUNOS
#include <sys/time.h>
#endif
#include "api/c/tt_c.h"
#include "mp/mp_message.h"
#include "mp/mp_pattern.h"
#include "util/tt_trace_stream.h"

#define TRACE_SCRIPT    	"TT_TRACE_SCRIPT"

// These are 1 character codes passed in as the "argskey"
// argument to the entry() member functions in _Tt_audit
// and _Tt_trace.  In the case of the former, the encoding
// determines what type of error checking is done on the
// arg, in the case of the latter it determines how the arg
// is to be printed out for tracing.
enum _Tt_audit_codes {
	CATEGORY 	= 'a',	// Tt_category
	VOID_PTR 	= 'A',	// void *
	STRICT_CHAR_PTR = 'C',	// char *: ! 0 && ! tt_ptr_error
	CHAR_PTR	= 'c',	// char *: ! tt_ptr_error
	DISPOSITION	= 'd',	// Tt_disposition
	ADDRESS		= 'D',	// Tt_address
	SESSION		= 'E',	// Tt_session
	STATE		= 'e',	// Tt_state
	FEATURE		= 'F',	// Tt_feature
	INTEGER		= 'i',	// int
	BYTES_LENGTH	= 'I',	// int length of preceding unsigned char *
	STRICT_SLOTNAME = 'l',	// context slotname: ! tt_ptr_error
	CLASS 		= 'L',	// Tt_class
	STRICT_MESG_PTR = 'M',	// Tt_message: ! 0 && ! tt_ptr_error
	MESG_PTR	= 'm',	// Tt_message: ! 0
	BYTES_WITH_LEN 	= 'n',	// unsigned char * [followed by int length]
	ARG_INTEGER 	= 'N',	// index for a particular message arg
	OBJID	 	= 'o',	// object ID
	MODE	 	= 'O',	// Tt_mode
	STRICT_PAT_PTR	= 'P',	// Tt_pattern: ! 0 && ! tt_ptr_error
	PROPNAME 	= 'r',	// propname, check for NULL and for max length.
	SLOTNAME 	= 's',	// slotname, check for NULL, and context exists
	SCOPE 		= 'S',	// Tt_scope
	VTYPE		= 'T',	// char *vtype
	AUDIT_STATUS	= 't',	// Tt_status
	VOID	 	= 'v',	// check nothing, print nothing
	STRICT_VOID_PTR	= 'X'	// void *: ! 0 && ! tt_ptr_error
};

typedef enum {
	TTDR_MESSAGE_SEND,
	TTDR_MESSAGE_SEND_ON_EXIT,
	TTDR_MESSAGE_ACCEPT,
	TTDR_MESSAGE_REJECT,
	TTDR_MESSAGE_ABSTAIN,
	TTDR_MESSAGE_FAIL,
	TTDR_MESSAGE_REPLY,
	TTDR_SESSION_JOIN,
	TTDR_FILE_JOIN,
	TTDR_HDISPATCH,
	TTDR_HUPDATE,
	TTDR_ERR_PTYPE_START,
	TTDR_ERR_PROCID
} _Tt_dispatch_reason;

class _Tt_trace_stream;

class _Tt_trace : public _Tt_allocated {
    public:
	// suppresses "used but not set" warnings
				_Tt_trace() {};
	// Initializes tracing (if necessary)
	static int		init(
					int even_if_no_envariable = 0
				);

	// De-initializes tracing, turning it off
	static void		fini();
	// If tracing uninitialized: like ::init(1)
	// If tracing initialized: toggles tracing on and off
	static void		toggle();

	// Message tracing entry calls
	void			entry(
					_Tt_message &msg,
					Tt_state           old_state
				);
	void			entry(
					_Tt_message &msg,
					const _Tt_procid  &recipient
				);
	void			entry(
					_Tt_message &msg,
					_Tt_dispatch_reason reason
				);

	// API tracing entry call
	void			entry(
					const char        *argskey,
					_Tt_entry_pt       func,
					va_list            ap
				);

	// callback tracing call
	void			entry(
					Tt_message_callback cb,
					Tt_message mh,
					Tt_pattern ph
				);

	const _Tt_trace	       &operator <<(
					_Tt_message	       &msg
				) const;
	const _Tt_trace	       &operator <<(
					const _Tt_pattern      &pat
				) const;
	const _Tt_trace	       &operator <<(
					const _Tt_string       &s
				) const;
	const _Tt_trace	       &operator <<(
					Tt_status		status
				) const;
	const _Tt_trace	       &operator <<(
					const char	       *s
				) const;
	const _Tt_trace	       &operator <<(
					int			n
				) const;
	const _Tt_trace	       &print(
					_Tt_object_printfn	print_it,
					const _Tt_object       *obj
				) const;

	void			incr_indent() const;
	void			decr_indent() const;

	// Close a level of tracing

	void			exitq();	// Exit quick; no mutex drop
	void			exit();
	void			exit(
					void		       *val
				);
	void			exit(
					Tt_message		val
				);
	void			exit(
					Tt_pattern		val
				);
	void			exit(
					char		       *val
				);
	void			exit(
					int			val
				);
	void			exit(
					Tt_status		val
				);
	void			exit(
					Tt_callback_action	val
				);
	void			exit(
					Tt_mode			val
				);
	void			exit(
					Tt_scope		val
				);
	void			exit(
					Tt_class		val
				);
	void			exit(
					Tt_category		val
				);
	void			exit(
					Tt_address		val
				);
	void			exit(
					Tt_disposition		val
				);
	void			exit(
					Tt_state		val
				);

	// Is there any tracing on in this address space?
	static int		any_tracing();

    private:
	_Tt_entry_pt		_funcname;

	void			_exit(
				      int		val,
				      const char	*(*enumname)(int),
				      int		drop = 1
				);
	//
	// This should be a ptr *; see tt_trace_stream.h
	//
	static _Tt_trace_stream **_pstream;
	static int		_toggled_off;

	int			_entry();
	static int		_allowed2trace();

	int			_tracing;

#ifdef _OPT_TIMERS_SUNOS
	// This is for doing API call timers

	struct timeval timestamp;
#endif
};
#endif	/* TT_TRACE_H */
