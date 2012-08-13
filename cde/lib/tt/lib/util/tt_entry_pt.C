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
//%%  $XConsortium: tt_entry_pt.C /main/5 1996/08/29 16:11:36 barstow $ 			 				
/* @(#)tt_entry_pt.C	1.1 93/11/03
 * Files tt_entry_pt.h, tt_entry_pt.C, and tt_entry_pt_names.h must
 * be updated each time a new API call is added.  You can do this
 * by hand, or by running the "mktrcfiles" shell script which
 * will regenerate them from tt_c.h.
 */
#include "util/tt_string.h"
#include "tt_entry_pt.h"

_Tt_string _tt_entrypt_to_string(_Tt_entry_pt fun)
{
	switch (fun) {
        case TT_X_SESSION :
                return "tt_X_session";
        case TT_BCONTEXT_JOIN :
                return "tt_bcontext_join";
        case TT_BCONTEXT_QUIT :
                return "tt_bcontext_quit";
        case TT_CLOSE :
                return "tt_close";
        case TT_CONTEXT_JOIN :
                return "tt_context_join";
        case TT_CONTEXT_QUIT :
                return "tt_context_quit";
        case TT_DEFAULT_FILE :
                return "tt_default_file";
        case TT_DEFAULT_FILE_SET :
                return "tt_default_file_set";
        case TT_DEFAULT_PROCID :
                return "tt_default_procid";
        case TT_DEFAULT_PROCID_SET :
                return "tt_default_procid_set";
        case TT_THREAD_PROCID :
                return "tt_thread_procid";
        case TT_THREAD_PROCID_SET :
                return "tt_thread_procid_set";
        case TT_DEFAULT_PTYPE :
                return "tt_default_ptype";
        case TT_DEFAULT_PTYPE_SET :
                return "tt_default_ptype_set";
        case TT_DEFAULT_SESSION :
                return "tt_default_session";
        case TT_DEFAULT_SESSION_SET :
                return "tt_default_session_set";
        case TT_THREAD_SESSION :
                return "tt_thread_session";
        case TT_THREAD_SESSION_SET :
                return "tt_thread_session_set";
        case TT_ERROR_INT :
                return "tt_error_int";
        case TT_ERROR_POINTER :
                return "tt_error_pointer";
        case TT_FD :
                return "tt_fd";
        case TT_FILE_COPY :
                return "tt_file_copy";
        case TT_FILE_DESTROY :
                return "tt_file_destroy";
        case TT_FILE_JOIN :
                return "tt_file_join";
        case TT_FILE_MOVE :
                return "tt_file_move";
        case TT_FILE_OBJECTS_QUERY :
                return "tt_file_objects_query";
        case TT_FILE_QUIT :
                return "tt_file_quit";
        case TT_FREE :
                return "tt_free";
        case TT_ICONTEXT_JOIN :
                return "tt_icontext_join";
        case TT_ICONTEXT_QUIT :
                return "tt_icontext_quit";
        case TT_INITIAL_SESSION :
                return "tt_initial_session";
        case TT_INT_ERROR :
                return "tt_int_error";
        case TT_MALLOC :
                return "tt_malloc";
        case TT_MARK :
                return "tt_mark";
        case TT_MESSAGE_ACCEPT :
                return "tt_message_accept";
        case TT_MESSAGE_ADDRESS :
                return "tt_message_address";
        case TT_MESSAGE_ADDRESS_SET :
                return "tt_message_address_set";
        case TT_MESSAGE_ARG_ADD :
                return "tt_message_arg_add";
        case TT_MESSAGE_ARG_BVAL :
                return "tt_message_arg_bval";
        case TT_MESSAGE_ARG_BVAL_SET :
                return "tt_message_arg_bval_set";
        case TT_MESSAGE_ARG_IVAL :
                return "tt_message_arg_ival";
        case TT_MESSAGE_ARG_IVAL_SET :
                return "tt_message_arg_ival_set";
        case TT_MESSAGE_ARG_MODE :
                return "tt_message_arg_mode";
        case TT_MESSAGE_ARG_TYPE :
                return "tt_message_arg_type";
        case TT_MESSAGE_ARG_VAL :
                return "tt_message_arg_val";
        case TT_MESSAGE_ARG_VAL_SET :
                return "tt_message_arg_val_set";
        case TT_MESSAGE_ARG_XVAL :
                return "tt_message_arg_xval";
        case TT_MESSAGE_ARG_XVAL_SET :
                return "tt_message_arg_xval_set";
        case TT_MESSAGE_ARGS_COUNT :
                return "tt_message_args_count";
        case TT_MESSAGE_BARG_ADD :
                return "tt_message_barg_add";
        case TT_MESSAGE_BCONTEXT_SET :
                return "tt_message_bcontext_set";
        case TT_MESSAGE_CALLBACK_ADD :
                return "tt_message_callback_add";
        case TT_MESSAGE_CLASS :
                return "tt_message_class";
        case TT_MESSAGE_CLASS_SET :
                return "tt_message_class_set";
        case TT_MESSAGE_CONTEXT_BVAL :
                return "tt_message_context_bval";
        case TT_MESSAGE_CONTEXT_IVAL :
                return "tt_message_context_ival";
        case TT_MESSAGE_CONTEXT_SET :
                return "tt_message_context_set";
        case TT_MESSAGE_CONTEXT_SLOTNAME :
                return "tt_message_context_slotname";
        case TT_MESSAGE_CONTEXT_VAL :
                return "tt_message_context_val";
        case TT_MESSAGE_CONTEXT_XVAL :
                return "tt_message_context_xval";
        case TT_MESSAGE_CONTEXTS_COUNT :
                return "tt_message_contexts_count";
        case TT_MESSAGE_CREATE :
                return "tt_message_create";
        case TT_MESSAGE_CREATE_SUPER :
                return "tt_message_create_super";
        case TT_MESSAGE_DESTROY :
                return "tt_message_destroy";
        case TT_MESSAGE_DISPOSITION :
                return "tt_message_disposition";
        case TT_MESSAGE_DISPOSITION_SET :
                return "tt_message_disposition_set";
        case TT_MESSAGE_FAIL :
                return "tt_message_fail";
        case TT_MESSAGE_FILE :
                return "tt_message_file";
        case TT_MESSAGE_FILE_SET :
                return "tt_message_file_set";
        case TT_MESSAGE_GID :
                return "tt_message_gid";
        case TT_MESSAGE_HANDLER :
                return "tt_message_handler";
        case TT_MESSAGE_HANDLER_PTYPE :
                return "tt_message_handler_ptype";
        case TT_MESSAGE_HANDLER_PTYPE_SET :
                return "tt_message_handler_ptype_set";
        case TT_MESSAGE_HANDLER_SET :
                return "tt_message_handler_set";
        case TT_MESSAGE_IARG_ADD :
                return "tt_message_iarg_add";
        case TT_MESSAGE_ICONTEXT_SET :
                return "tt_message_icontext_set";
        case TT_MESSAGE_ID :
                return "tt_message_id";
        case TT_MESSAGE_OBJECT :
                return "tt_message_object";
        case TT_MESSAGE_OBJECT_SET :
                return "tt_message_object_set";
        case TT_MESSAGE_OP :
                return "tt_message_op";
        case TT_MESSAGE_OP_SET :
                return "tt_message_op_set";
        case TT_MESSAGE_OPNUM :
                return "tt_message_opnum";
        case TT_MESSAGE_OTYPE :
                return "tt_message_otype";
        case TT_MESSAGE_OTYPE_SET :
                return "tt_message_otype_set";
        case TT_MESSAGE_PATTERN :
                return "tt_message_pattern";
        case TT_MESSAGE_PRINT :
                return "tt_message_print";
        case TT_MESSAGE_RECEIVE :
                return "tt_message_receive";
        case TT_MESSAGE_REJECT :
                return "tt_message_reject";
        case TT_MESSAGE_REPLY :
                return "tt_message_reply";
        case TT_MESSAGE_SCOPE :
                return "tt_message_scope";
        case TT_MESSAGE_SCOPE_SET :
                return "tt_message_scope_set";
        case TT_MESSAGE_SEND :
                return "tt_message_send";
        case TT_MESSAGE_SEND_ON_EXIT :
                return "tt_message_send_on_exit";
        case TT_MESSAGE_SENDER :
                return "tt_message_sender";
        case TT_MESSAGE_SENDER_PTYPE :
                return "tt_message_sender_ptype";
        case TT_MESSAGE_SENDER_PTYPE_SET :
                return "tt_message_sender_ptype_set";
        case TT_MESSAGE_SESSION :
                return "tt_message_session";
        case TT_MESSAGE_SESSION_SET :
                return "tt_message_session_set";
        case TT_MESSAGE_STATE :
                return "tt_message_state";
        case TT_MESSAGE_STATUS :
                return "tt_message_status";
        case TT_MESSAGE_STATUS_SET :
                return "tt_message_status_set";
        case TT_MESSAGE_STATUS_STRING :
                return "tt_message_status_string";
        case TT_MESSAGE_STATUS_STRING_SET :
                return "tt_message_status_string_set";
        case TT_MESSAGE_UID :
                return "tt_message_uid";
        case TT_MESSAGE_USER :
                return "tt_message_user";
        case TT_MESSAGE_USER_SET :
                return "tt_message_user_set";
        case TT_MESSAGE_XARG_ADD :
                return "tt_message_xarg_add";
        case TT_MESSAGE_XCONTEXT_SET :
                return "tt_message_xcontext_set";
        case TT_OBJID_EQUAL :
                return "tt_objid_equal";
        case TT_OBJID_OBJKEY :
                return "tt_objid_objkey";
        case TT_ONOTICE_CREATE :
                return "tt_onotice_create";
        case TT_OPEN :
                return "tt_open";
        case TT_OREQUEST_CREATE :
                return "tt_orequest_create";
        case TT_OTYPE_BASE :
                return "tt_otype_base";
        case TT_OTYPE_DERIVED :
                return "tt_otype_derived";
        case TT_OTYPE_DERIVEDS_COUNT :
                return "tt_otype_deriveds_count";
        case TT_OTYPE_HSIG_ARG_MODE :
                return "tt_otype_hsig_arg_mode";
        case TT_OTYPE_HSIG_ARG_TYPE :
                return "tt_otype_hsig_arg_type";
        case TT_OTYPE_HSIG_ARGS_COUNT :
                return "tt_otype_hsig_args_count";
        case TT_OTYPE_HSIG_COUNT :
                return "tt_otype_hsig_count";
        case TT_OTYPE_HSIG_OP :
                return "tt_otype_hsig_op";
        case TT_OTYPE_IS_DERIVED :
                return "tt_otype_is_derived";
        case TT_OTYPE_OPNUM_CALLBACK_ADD :
                return "tt_otype_opnum_callback_add";
        case TT_OTYPE_OSIG_ARG_MODE :
                return "tt_otype_osig_arg_mode";
        case TT_OTYPE_OSIG_ARG_TYPE :
                return "tt_otype_osig_arg_type";
        case TT_OTYPE_OSIG_ARGS_COUNT :
                return "tt_otype_osig_args_count";
        case TT_OTYPE_OSIG_COUNT :
                return "tt_otype_osig_count";
        case TT_OTYPE_OSIG_OP :
                return "tt_otype_osig_op";
        case TT_PATTERN_ADDRESS_ADD :
                return "tt_pattern_address_add";
        case TT_PATTERN_ARG_ADD :
                return "tt_pattern_arg_add";
        case TT_PATTERN_BARG_ADD :
                return "tt_pattern_barg_add";
        case TT_PATTERN_BCONTEXT_ADD :
                return "tt_pattern_bcontext_add";
        case TT_PATTERN_CALLBACK_ADD :
                return "tt_pattern_callback_add";
        case TT_PATTERN_CATEGORY :
                return "tt_pattern_category";
        case TT_PATTERN_CATEGORY_SET :
                return "tt_pattern_category_set";
        case TT_PATTERN_CLASS_ADD :
                return "tt_pattern_class_add";
        case TT_PATTERN_CONTEXT_ADD :
                return "tt_pattern_context_add";
        case TT_PATTERN_CREATE :
                return "tt_pattern_create";
        case TT_PATTERN_DESTROY :
                return "tt_pattern_destroy";
        case TT_PATTERN_DISPOSITION_ADD :
                return "tt_pattern_disposition_add";
        case TT_PATTERN_FILE_ADD :
                return "tt_pattern_file_add";
        case TT_PATTERN_IARG_ADD :
                return "tt_pattern_iarg_add";
        case TT_PATTERN_ICONTEXT_ADD :
                return "tt_pattern_icontext_add";
        case TT_PATTERN_OBJECT_ADD :
                return "tt_pattern_object_add";
        case TT_PATTERN_OP_ADD :
                return "tt_pattern_op_add";
        case TT_PATTERN_OPNUM_ADD :
                return "tt_pattern_opnum_add";
        case TT_PATTERN_OTYPE_ADD :
                return "tt_pattern_otype_add";
        case TT_PATTERN_PRINT :
                return "tt_pattern_print";
        case TT_PATTERN_REGISTER :
                return "tt_pattern_register";
        case TT_PATTERN_SCOPE_ADD :
                return "tt_pattern_scope_add";
        case TT_PATTERN_SENDER_ADD :
                return "tt_pattern_sender_add";
        case TT_PATTERN_SENDER_PTYPE_ADD :
                return "tt_pattern_sender_ptype_add";
        case TT_PATTERN_SESSION_ADD :
                return "tt_pattern_session_add";
        case TT_PATTERN_STATE_ADD :
                return "tt_pattern_state_add";
        case TT_PATTERN_UNREGISTER :
                return "tt_pattern_unregister";
        case TT_PATTERN_USER :
                return "tt_pattern_user";
        case TT_PATTERN_USER_SET :
                return "tt_pattern_user_set";
        case TT_PATTERN_XARG_ADD :
                return "tt_pattern_xarg_add";
        case TT_PATTERN_XCONTEXT_ADD :
                return "tt_pattern_xcontext_add";
        case TT_PNOTICE_CREATE :
                return "tt_pnotice_create";
        case TT_POINTER_ERROR :
                return "tt_pointer_error";
        case TT_PREQUEST_CREATE :
                return "tt_prequest_create";
        case TT_PTYPE_DECLARE :
                return "tt_ptype_declare";
        case TT_PTYPE_EXISTS :
                return "tt_ptype_exists";
        case TT_PTYPE_OPNUM_CALLBACK_ADD :
                return "tt_ptype_opnum_callback_add";
        case TT_PTYPE_UNDECLARE :
                return "tt_ptype_undeclare";
        case TT_RELEASE :
                return "tt_release";
        case TT_SESSION_BPROP :
                return "tt_session_bprop";
        case TT_SESSION_BPROP_ADD :
                return "tt_session_bprop_add";
        case TT_SESSION_BPROP_SET :
                return "tt_session_bprop_set";
        case TT_SESSION_EQUAL :
                return "tt_session_equal";
        case TT_SESSION_JOIN :
                return "tt_session_join";
        case TT_SESSION_PROP :
                return "tt_session_prop";
        case TT_SESSION_PROP_ADD :
                return "tt_session_prop_add";
        case TT_SESSION_PROP_COUNT :
                return "tt_session_prop_count";
        case TT_SESSION_PROP_SET :
                return "tt_session_prop_set";
        case TT_SESSION_PROPNAME :
                return "tt_session_propname";
        case TT_SESSION_PROPNAMES_COUNT :
                return "tt_session_propnames_count";
        case TT_SESSION_QUIT :
                return "tt_session_quit";
        case TT_SESSION_TYPES_LOAD :
                return "tt_session_types_load";
        case TT_SPEC_BPROP :
                return "tt_spec_bprop";
        case TT_SPEC_BPROP_ADD :
                return "tt_spec_bprop_add";
        case TT_SPEC_BPROP_SET :
                return "tt_spec_bprop_set";
        case TT_SPEC_CREATE :
                return "tt_spec_create";
        case TT_SPEC_DESTROY :
                return "tt_spec_destroy";
        case TT_SPEC_FILE :
                return "tt_spec_file";
        case TT_SPEC_MOVE :
                return "tt_spec_move";
        case TT_SPEC_PROP :
                return "tt_spec_prop";
        case TT_SPEC_PROP_ADD :
                return "tt_spec_prop_add";
        case TT_SPEC_PROP_COUNT :
                return "tt_spec_prop_count";
        case TT_SPEC_PROP_SET :
                return "tt_spec_prop_set";
        case TT_SPEC_PROPNAME :
                return "tt_spec_propname";
        case TT_SPEC_PROPNAMES_COUNT :
                return "tt_spec_propnames_count";
        case TT_SPEC_TYPE :
                return "tt_spec_type";
        case TT_SPEC_TYPE_SET :
                return "tt_spec_type_set";
        case TT_SPEC_WRITE :
                return "tt_spec_write";
        case TT_STATUS_MESSAGE :
                return "tt_status_message";
        case TT_TRACE_CONTROL :
                return "tt_trace_control";
        case TT_XCONTEXT_JOIN :
                return "tt_xcontext_join";
        case TT_XCONTEXT_QUIT :
                return "tt_xcontext_quit";
        case TT_FILE_NETFILE :
		return "tt_file_netfile";
        case TT_NETFILE_FILE :
		return "tt_netfile_file";
        case TT_HOST_FILE_NETFILE :
		return "tt_host_file_netfile";
        case TT_HOST_NETFILE_FILE :
		return "tt_host_netfile_file";
        case TT_FEATURE_ENABLED :
		return "tt_feature_enabled";
        case TT_FEATURE_REQUIRED :
		return "tt_feature_required";
#if defined(__osf__) || defined(linux)
	case TT_API_CALL_LAST: return (char *) NULL; 
#elif defined(OPT_CONST_CORRECT)
	case TT_API_CALL_LAST: return (const char *) NULL; 
#else
	case TT_API_CALL_LAST: return NULL; 
#endif
	}
}
