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
//%%  $XConsortium: tt_c++.C /main/3 1995/11/06 17:14:22 rswiston $

#include <string.h>
#include <Tt/tttk.h>
#include "tt_c++.h"

char *
tt_status_string(
	Tt_status status
)
{
	char *string = tt_status_message( status );
	if (string == 0) {
		return 0;
	}
	char *tab = strchr( string, '\t' );
	if (tab != 0) {
		*tab = '\0';
	}
	return string;
}

ostream &
operator<<(
	ostream & sink,
	Tt_status status2Print
)
{
	char *string = tt_status_string( status2Print );
	sink << string;
	tt_free( string );
	return sink;
}

ostream &
operator<<(
	ostream & sink,
	Tt_scope scope
)
{
	char *string = 0;
	switch (scope) {
	    case TT_SCOPE_NONE:		string = "TT_SCOPE_NONE";	break;
	    case TT_SESSION:		string = "TT_SESSION";		break;
	    case TT_FILE:		string = "TT_FILE";		break;
	    case TT_BOTH:		string = "TT_BOTH";		break;
	    case TT_FILE_IN_SESSION:	string = "TT_FILE_IN_SESSION";	break;
	}
	if (string != 0) {
		sink << string;
	} else {
		sink << "(Tt_scope)" << (int)scope;
	}
	return sink;
}

ostream &
operator<<(
	ostream & sink,
	Tt_class _class
)
{
	char *string = 0;
	switch (_class) {
	    case TT_CLASS_UNDEFINED:	string = "TT_CLASS_UNDEFINED";	break;
	    case TT_NOTICE:		string = "TT_NOTICE";		break;
	    case TT_REQUEST:		string = "TT_REQUEST";		break;
	    case TT_OFFER:		string = "TT_OFFER";		break;
	}
	if (string != 0) {
		sink << string;
	} else {
		sink << "(Tt_class)" << (int)_class;
	}
	return sink;
}

ostream &
operator<<(
	ostream & sink,
	Tt_mode mode
)
{
	char *string = 0;
	switch (mode) {
	    case TT_MODE_UNDEFINED:	string = "TT_MODE_UNDEFINED";	break;
	    case TT_IN:			string = "TT_IN";		break;
	    case TT_OUT:		string = "TT_OUT";		break;
	    case TT_INOUT:		string = "TT_INOUT";		break;
	}
	if (string != 0) {
		sink << string;
	} else {
		sink << "(Tt_mode)" << (int)mode;
	}
	return sink;
}

ostream &
operator<<(
	ostream & sink,
	Tt_address address
)
{
	char *string = 0;
	switch (address) {
	    case TT_PROCEDURE:		string = "TT_PROCEDURE";	break;
	    case TT_OBJECT:		string = "TT_OBJECT";		break;
	    case TT_HANDLER:		string = "TT_HANDLER";		break;
	    case TT_OTYPE:		string = "TT_OTYPE";		break;
	}
	if (string != 0) {
		sink << string;
	} else {
		sink << "(Tt_address)" << (int)address;
	}
	return sink;
}

ostream &
operator<<(
	ostream & sink,
	Tt_callback_action action
)
{
	char *string = 0;
	switch (action) {
	    case TT_CALLBACK_CONTINUE:	string = "TT_CALLBACK_CONTINUE";break;
	    case TT_CALLBACK_PROCESSED:	string = "TT_CALLBACK_PROCESSED";break;
	}
	if (string != 0) {
		sink << string;
	} else {
		sink << "(Tt_callback_action)" << (int)action;
	}
	return sink;
}

Tt_message
tt_message_copy(
	Tt_message msg1
)
{
	if (tt_ptr_error( msg1 ) != TT_OK) {
		return msg1;
	}
	Tt_message msg2 = tt_message_create();
	Tt_status status = tt_ptr_error( msg2 );
	if (tt_ptr_error( msg2 ) != TT_OK) {
		return msg2;
	}
	int contexts = tt_message_contexts_count( msg1 );
	status = tt_int_error( contexts );
	if (status != TT_OK) {
		tt_message_destroy( msg2 );
		return (Tt_message)tt_error_pointer( status );
	}
	for (int context = 0; context < contexts; context++) {
		char *slot = tt_message_context_slotname( msg1, context );
		int ival;
		status = tt_message_context_ival( msg1, slot, &ival );
		switch (status) {
			unsigned char *bval;
			int len;
		    case TT_OK:
			tt_message_icontext_set( msg2, slot, ival );
			break;
		    case TT_ERR_NUM:
			tt_message_context_bval( msg1, slot, &bval, &len );
			tt_message_bcontext_set( msg2, slot, bval, len );
			tt_free( (caddr_t)bval );
			break;
		}
		tt_free( slot );
	}
	int args = tt_message_args_count( msg1 );
	status = tt_int_error( args );
	if (status != TT_OK) {
		tt_message_destroy( msg2 );
		return (Tt_message)tt_error_pointer( status );
	}
	for (int arg = 0; arg < args; arg++) {
		Tt_mode mode = tt_message_arg_mode( msg1, arg );
		char *vtype = tt_message_arg_type( msg1, arg );
		int ival;
		status = tt_message_arg_ival( msg1, arg, &ival );
		switch (status) {
			unsigned char *bval;
			int len;
		    case TT_OK:
			tt_message_iarg_add( msg2, mode, vtype, ival );
			break;
		    case TT_ERR_NUM:
			tt_message_arg_bval( msg1, arg, &bval, &len );
			tt_message_barg_add( msg2, mode, vtype, bval, len );
			tt_free( (caddr_t)bval );
			break;
		}
		tt_free( vtype );
	}

	tt_message_class_set( msg2, tt_message_class( msg1 ));

	char *file = tt_message_file( msg1 );
	if ((tt_ptr_error( file ) == TT_OK) && (file != 0)) {
		tt_message_file_set( msg2, file );
		tt_free( file );
	}

	char *object = tt_message_object( msg1 );
	if ((tt_ptr_error( object ) == TT_OK) && (object != 0)) {
		tt_message_object_set( msg2, object );
		tt_free( object );
	}

	char *op = tt_message_op( msg1 );
	if ((tt_ptr_error( op ) == TT_OK) && (op != 0)) {
		tt_message_op_set( msg2, op );
		tt_free( op );
	}

	char *otype = tt_message_otype( msg1 );
	if ((tt_ptr_error( otype ) == TT_OK) && (otype != 0)) {
		tt_message_otype_set( msg2, otype );
		tt_free( otype );
	}

	tt_message_address_set( msg2, tt_message_address( msg1 ));

	// XXX skip handler, handler_ptype, sender

	tt_message_scope_set( msg2, tt_message_scope( msg1 ));

	char *sender_ptype = tt_message_sender_ptype( msg1 );
	if ((tt_ptr_error( sender_ptype ) == TT_OK) && (sender_ptype != 0)) {
		tt_message_sender_ptype_set( msg2, sender_ptype );
		tt_free( sender_ptype );
	}

	char *session = tt_message_session( msg1 );
	if ((tt_ptr_error( session ) == TT_OK) && (session != 0)) {
		tt_message_session_set( msg2, session );
		tt_free( session );
	}

	tt_message_status_set( msg2, tt_message_status( msg1 ));

	char *status_string = tt_message_status_string( msg1 );
	if ((tt_ptr_error( status_string ) == TT_OK) && (status_string != 0)) {
		tt_message_status_string_set( msg2, status_string );
		tt_free( status_string );
	}

	return msg2;
}

Tt_message
tttk_message_create(
	const char *optext
)
{
	Tttk_op op = tttk_string_op( optext );
	Tt_class theClass;
	switch (op) {
	    default:
		theClass = TT_REQUEST;
		break;
	    case TTDT_CREATED:
	    case TTDT_DELETED:
	    case TTDT_MODIFIED:
	    case TTDT_REVERTED:
	    case TTDT_MOVED:
	    case TTDT_SAVED:
	    case TTDT_STARTED:
	    case TTDT_STOPPED:
	    case TTDT_STATUS:
		theClass = TT_NOTICE;
		break;
	}
	Tt_scope scope;
	switch (op) {
	    default:
		scope = TT_SESSION;
		break;
	    case TTDT_CREATED:
	    case TTDT_DELETED:
	    case TTDT_MODIFIED:
	    case TTDT_REVERTED:
	    case TTDT_MOVED:
	    case TTDT_SAVED:
	    case TTDT_GET_MODIFIED:
	    case TTDT_SAVE:
	    case TTDT_REVERT:
		scope = TT_BOTH;
		break;
	}
	Tt_message msg = tttk_message_create( 0, theClass, scope,
					      0, optext, 0 );
	Tt_mode mode = TT_INOUT;
	switch (op) {
	    default:
		break;
	    case TTDT_MOVED:
		tt_message_arg_add( msg, TT_IN, Tttk_file, 0 );
		// fall through
	    case TTDT_CREATED:
	    case TTDT_DELETED:
	    case TTDT_MODIFIED:
	    case TTDT_REVERTED:
	    case TTDT_SAVED:
	    case TTDT_GET_MODIFIED:
	    case TTDT_SAVE:
	    case TTDT_REVERT:
		tt_message_arg_add( msg, TT_IN, Tttk_file, 0 );
		break;
	    case TTDT_GET_SYSINFO:
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		// fall through
	    case TTDT_GET_STATUS:
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		break;
	    case TTDT_STATUS:
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		// fall through
	    case TTDT_STARTED:
	    case TTDT_STOPPED:
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		break;
	    case TTDT_GET_ICONIFIED:
	    case TTDT_GET_MAPPED:
                tt_message_arg_add( msg, TT_OUT, Tttk_boolean, 0 );
		break;
	    case TTDT_QUIT:
                tt_message_arg_add( msg, TT_IN, Tttk_boolean, 0 );
		// fall through
	    case TTDT_SET_MAPPED:
	    case TTDT_SET_ICONIFIED:
                tt_message_arg_add( msg, TT_IN, Tttk_boolean, 0 );
		break;
	    case TTDT_GET_GEOMETRY:
		mode = TT_OUT;
	    case TTDT_SET_GEOMETRY:
                tt_message_arg_add( msg, mode, Tttk_width, 0 );
                tt_message_arg_add( msg, mode, Tttk_height, 0 );
                tt_message_arg_add( msg, mode, Tttk_xoffset, 0 );
                tt_message_arg_add( msg, mode, Tttk_yoffset, 0 );
		break;
	    case TTDT_GET_XINFO:
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_integer, 0 );
		break;
	    case TTDT_GET_SITUATION:
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		break;
	    case TTDT_SIGNAL:
	    case TTDT_SET_SITUATION:
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		break;
	    case TTDT_DO_COMMAND:
	    case TTDT_GET_ENVIRONMENT:
	    case TTDT_GET_LOCALE:
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_OUT, Tttk_string, 0 );
		break;
	    case TTDT_SET_LOCALE:
	    case TTDT_SET_ENVIRONMENT:
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		tt_message_arg_add( msg, TT_IN, Tttk_string, 0 );
		break;
	}
	return msg;
}

int
tt_procid_fd(
	const char *procid
)
{
	char *defaultProcid = tt_default_procid();
	tt_default_procid_set( procid );
	int val2return = tt_fd();
	if (! tt_is_err( tt_ptr_error( defaultProcid ))) {
		tt_default_procid_set( defaultProcid );
	}
	return val2return;
}
