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
/* $TOG: tt_c.h /main/10 1999/09/16 13:46:20 mgreess $ */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */

/*
 * ToolTalk 1.2 API C language headers.
 */

#ifndef _tt_c_h
#define _tt_c_h

#include <stdio.h>

/* ToolTalk version - Format:
 *
 * <Major Release #><Minor Release #: 2 digits><Dot Release #: 2 digits>
 *
 * For example:
 *		 10100	--> Version  1.1.0
 *		100102	--> Version 10.1.2
 */
#define TT_VERSION 10300

#if defined(_EXTERN_) && defined(__STDC__)
#	define _TT_CONST
#	define _TT_EXTERN_FUNC(type,name,list) extern XS_LVAL x##name ()
#else
#if defined(_EXTERN_)
#	define _TT_CONST
#	define _TT_EXTERN_FUNC(type,name,list) extern XS_LVAL x/**/name ()
#else
#if defined(_NAMES_)
#	define _TT_CONST
#	define _TT_EXTERN_FUNC(type,name,list)name
#else
#if defined(_XSCHEME_)
#	define _TT_CONST const
#	define _TT_EXTERN_FUNC(type,name,list)type; name; list
#else
#if defined(__cplusplus)
/* C++ 2.0 or later*/
#	define _TT_CONST const
#	define _TT_EXTERN_FUNC(type,name,list) extern "C" { type name list; }
#else
#if defined(__STDC__) 
/* ANSI C */
#	define _TT_CONST const
#	define _TT_EXTERN_FUNC(type,name,list) type name list;
#else
/* Sun C (K&R C, almost) */
#	define _TT_CONST
#	define _TT_EXTERN_FUNC(type,name,list) type name();
#endif
#endif
#endif
#endif
#endif
#endif

#if !defined(_EXTERN_) && !defined(_NAMES_) && !defined(_XSCHEME_) 

#if !defined(_XENUMS_) && !defined(_TYPES_)
#include <sys/types.h>
#endif

#include <rpc/types.h>
#include <rpc/xdr.h>

/* former status 1030 removed, but not used, for backward compatibility. */
typedef enum tt_status {
        TT_OK 			= 0,
        TT_WRN_NOTFOUND		= 1,
	TT_WRN_STALE_OBJID	= 2,
        TT_WRN_STOPPED		= 3,
	TT_WRN_SAME_OBJID	= 4,
	TT_WRN_START_MESSAGE	= 5,
	TT_WRN_NOT_ENABLED	= 6,
	TT_WRN_APPFIRST 	= 512,
	TT_WRN_LAST 		= 1024,
        TT_ERR_CLASS 		= 1025,
        TT_ERR_DBAVAIL 		= 1026,
        TT_ERR_DBEXIST		= 1027,
	TT_ERR_FILE		= 1028,
	TT_ERR_INVALID		= 1029,
        TT_ERR_MODE		= 1031,
	TT_ERR_ACCESS		= 1032,
        TT_ERR_NOMP		= 1033,
        TT_ERR_NOTHANDLER	= 1034,
        TT_ERR_NUM		= 1035,
        TT_ERR_OBJID		= 1036,
        TT_ERR_OP		= 1037,
        TT_ERR_OTYPE		= 1038,
        TT_ERR_ADDRESS		= 1039,
        TT_ERR_PATH		= 1040,
        TT_ERR_POINTER		= 1041,
        TT_ERR_PROCID		= 1042,
        TT_ERR_PROPLEN		= 1043,
        TT_ERR_PROPNAME		= 1044,
        TT_ERR_PTYPE		= 1045,
        TT_ERR_DISPOSITION	= 1046,
        TT_ERR_SCOPE		= 1047,
        TT_ERR_SESSION		= 1048,
        TT_ERR_VTYPE		= 1049,
	TT_ERR_NO_VALUE		= 1050,
	TT_ERR_INTERNAL		= 1051,
	TT_ERR_READONLY		= 1052,
	TT_ERR_NO_MATCH		= 1053,
	TT_ERR_UNIMP		= 1054,
	TT_ERR_OVERFLOW		= 1055,
	TT_ERR_PTYPE_START	= 1056,
	TT_ERR_CATEGORY		= 1057,
	TT_ERR_DBUPDATE         = 1058,
	TT_ERR_DBFULL           = 1059,
	TT_ERR_DBCONSIST	= 1060,
	TT_ERR_STATE		= 1061,
	TT_ERR_NOMEM		= 1062,
	TT_ERR_SLOTNAME		= 1063,
	TT_ERR_XDR		= 1064,
	TT_ERR_NETFILE		= 1065,
	TT_ERR_TOOLATE		= 1066,
	TT_ERR_AUTHORIZATION	= 1067,
	TT_ERR_VERSION_MISMATCH	= 1068,
	TT_DESKTOP_		= 1100,
	TT_DESKTOP_EPERM	= 1101,
	TT_DESKTOP_ENOENT	= 1102,
	TT_DESKTOP_EINTR	= 1104,
	TT_DESKTOP_EIO		= 1105,
	TT_DESKTOP_EAGAIN	= 1111,
	TT_DESKTOP_ENOMEM	= 1112,
	TT_DESKTOP_EACCES	= 1113,
	TT_DESKTOP_EFAULT	= 1114,
	TT_DESKTOP_EEXIST	= 1117,
	TT_DESKTOP_ENODEV	= 1119,
	TT_DESKTOP_ENOTDIR	= 1120,
	TT_DESKTOP_EISDIR	= 1121,
	TT_DESKTOP_EINVAL	= 1122,
	TT_DESKTOP_ENFILE	= 1123,
	TT_DESKTOP_EMFILE	= 1124,
	TT_DESKTOP_ETXTBSY	= 1126,
	TT_DESKTOP_EFBIG	= 1127,
	TT_DESKTOP_ENOSPC	= 1128,
	TT_DESKTOP_EROFS	= 1130,
	TT_DESKTOP_EMLINK	= 1131,
	TT_DESKTOP_EPIPE	= 1132,
	TT_DESKTOP_ENOMSG	= 1135,
	TT_DESKTOP_EDEADLK	= 1145,
	TT_DESKTOP_ECANCELED	= 1147,
	TT_DESKTOP_ENOTSUP	= 1148,
	TT_DESKTOP_ENODATA	= 1161,
	TT_DESKTOP_EPROTO	= 1171,
	TT_DESKTOP_ENOTEMPTY	= 1193,
	TT_DESKTOP_ETIMEDOUT	= 1245,
	TT_DESKTOP_EALREADY	= 1249,
	TT_DESKTOP_UNMODIFIED	= 1299,
	TT_MEDIA_ERR_SIZE	= 1300,
	TT_MEDIA_ERR_FORMAT	= 1301,
	TT_AUTHFILE_ACCESS	= 1400,
	TT_AUTHFILE_LOCK	= 1401,
	TT_AUTHFILE_LOCK_TIMEOUT = 1402,
	TT_AUTHFILE_UNLOCK	= 1403,
	TT_AUTHFILE_MISSING	= 1404,
	TT_AUTHFILE_ENTRY_MISSING = 1405,
	TT_AUTHFILE_WRITE	= 1406,
	TT_ERR_APPFIRST		= 1536,
	TT_ERR_LAST 		= 2047,
        TT_STATUS_LAST 		= 2048} Tt_status;


typedef enum tt_filter_action {
	TT_FILTER_CONTINUE	= 0, 
	TT_FILTER_STOP		= 1, 
	TT_FILTER_LAST		= 2} Tt_filter_action;

typedef enum tt_callback_action {
	TT_CALLBACK_CONTINUE	= 0,
	TT_CALLBACK_PROCESSED	= 1,
	TT_CALLBACK_LAST	= 2} Tt_callback_action;

typedef enum tt_mode {
	TT_MODE_UNDEFINED	= 0,
        TT_IN			= 1,
        TT_OUT			= 2,
        TT_INOUT		= 3,
        TT_MODE_LAST		= 4} Tt_mode;

typedef enum tt_scope {
	TT_SCOPE_NONE		= 0,
        TT_SESSION		= 1,
        TT_FILE			= 2,
        TT_BOTH			= 3,
	TT_FILE_IN_SESSION	= 4} Tt_scope;

typedef enum tt_class {
	TT_CLASS_UNDEFINED	= 0,
        TT_NOTICE		= 1,
        TT_REQUEST		= 2,
	TT_OFFER		= 3,
        TT_CLASS_LAST		= 4} Tt_class;

typedef enum tt_category {
	TT_CATEGORY_UNDEFINED	= 0,
        TT_OBSERVE		= 1,
        TT_HANDLE		= 2,
        TT_HANDLE_PUSH		= 3,
        TT_HANDLE_ROTATE	= 4,
        TT_CATEGORY_LAST	= 5} Tt_category;

typedef enum tt_address {
        TT_PROCEDURE		= 0,
        TT_OBJECT		= 1,
	TT_HANDLER		= 2,
	TT_OTYPE		= 3,
        TT_ADDRESS_LAST		= 4} Tt_address;

typedef enum tt_disposition {
        /* Flag bits, not enumerated values */
	TT_DISCARD 		= 0, /* for resetting value */
        TT_QUEUE 		= 1,
        TT_START 		= 2}  Tt_disposition;

typedef enum tt_state {
        TT_CREATED		= 0,
        TT_SENT			= 1,
        TT_HANDLED		= 2,
	TT_FAILED		= 3,
	TT_QUEUED		= 4,
	TT_STARTED		= 5,
	TT_REJECTED		= 6,
	TT_RETURNED		= 7,
	TT_ACCEPTED		= 8,
	TT_ABSTAINED		= 9,
        TT_STATE_LAST		= 10} Tt_state;

typedef enum tt_feature {
	_TT_FEATURE_MULTITHREADED	= 1,
	_TT_FEATURE_LAST		= 2} Tt_feature;

#define TT_FEATURE_MULTITHREADED	_TT_FEATURE_MULTITHREADED
#define TT_FEATURE_LAST			_TT_FEATURE_LAST

#ifndef _XENUMS_

/*
 * Official properties of ToolTalk objects.
 *
 * ToolTalk should prevent non-root processes from setting the values
 * of properties whose name begins with '_'.
 *
 * Integrators should prevent users from assigning arbitrary strings
 * as values of properties whose name begins with '.'.  That is, "dot"
 * properties are like "dot" files: their existence should normally be
 * hidden from users, and the displaying and setting of their contents
 * should be mediated programatically, sort of like the OpenWindows
 * "props" application mediates .Xdefaults.
 */
#define TT_OBJECT_NAME_PROPERTY			".Name"
#define TT_OBJECT_OWNER_PROPERTY		"_Owner"
#define TT_OBJECT_GROUP_PROPERTY		"_Group"
#define TT_OBJECT_MODE_PROPERTY			"_Mode"
#define TT_OBJECT_CREATION_DATE_PROPERTY	"_Creation_Date"

/* 
 * The following define opaque handles which can be type checked (as compared
 * to void *).  Don't bother looking for the definitions of the structs, there
 * aren't any.
 */
typedef struct _Tt_message_handle *Tt_message;
typedef struct _Tt_pattern_handle *Tt_pattern;
typedef struct _tt_AuthFileEntry  *Tt_AuthFileEntry;


#if defined(__cplusplus) || defined(__STDC__)
typedef Tt_filter_action (*Tt_filter_function)(_TT_CONST char *nodeid,
					       void *context,
					       void *accumulator);
typedef Tt_callback_action (*Tt_message_callback) (Tt_message m,
						   Tt_pattern p);
#else
typedef Tt_filter_action (*Tt_filter_function)();
typedef Tt_callback_action (*Tt_message_callback)();
#endif

#endif /* _XENUMS_ */
#endif /* _XSCHEME_  _NAMES_  _EXTERN_ */

#ifndef _XENUMS_

_TT_EXTERN_FUNC(char *,tt_open,(void))
_TT_EXTERN_FUNC(Tt_status,tt_close,(void))
_TT_EXTERN_FUNC(char *,tt_X_session,(_TT_CONST char *xdisplay))

_TT_EXTERN_FUNC(char *,tt_default_ptype,(void))
_TT_EXTERN_FUNC(Tt_status,tt_default_ptype_set,(_TT_CONST char * ptid))
_TT_EXTERN_FUNC(char *,tt_default_file,(void))
_TT_EXTERN_FUNC(Tt_status,tt_default_file_set,(_TT_CONST char * docid))
_TT_EXTERN_FUNC(char *,tt_default_session,(void))
_TT_EXTERN_FUNC(Tt_status,tt_default_session_set,(_TT_CONST char *sessid))
_TT_EXTERN_FUNC(char *,tt_thread_session,(void))
_TT_EXTERN_FUNC(Tt_status,tt_thread_session_set,(_TT_CONST char *sessid))
_TT_EXTERN_FUNC(char *,tt_default_procid,(void))
_TT_EXTERN_FUNC(Tt_status,tt_default_procid_set,(_TT_CONST char *procid))
_TT_EXTERN_FUNC(char *,tt_thread_procid,(void))
_TT_EXTERN_FUNC(Tt_status,tt_thread_procid_set,(_TT_CONST char *procid))
_TT_EXTERN_FUNC(char *,tt_procid_session,(_TT_CONST char *procid))

_TT_EXTERN_FUNC(Tt_status,tt_file_join,(_TT_CONST char * filepath))
_TT_EXTERN_FUNC(Tt_status,tt_file_quit,(_TT_CONST char * filepath))
_TT_EXTERN_FUNC(Tt_status,tt_file_objects_query,
                (_TT_CONST char * filepath,
                 Tt_filter_function filter,
                 void *context,void *accumulator))
_TT_EXTERN_FUNC(Tt_status,tt_file_move,
		(_TT_CONST char *oldfilepath, _TT_CONST char *newfilepath))
_TT_EXTERN_FUNC(Tt_status,tt_file_copy,
		(_TT_CONST char *oldfilepath, _TT_CONST char *newfilepath))
_TT_EXTERN_FUNC(Tt_status,tt_file_destroy,(_TT_CONST char * filepath))

_TT_EXTERN_FUNC(char *,tt_message_context_val,
		(Tt_message m, _TT_CONST char *slotname))
_TT_EXTERN_FUNC(Tt_status,tt_message_context_ival,
		(Tt_message m, _TT_CONST char *slotname, int *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_context_bval,
		(Tt_message m, _TT_CONST char *slotname,
		 unsigned char **value, int *len))
_TT_EXTERN_FUNC(Tt_status,tt_message_context_xval,
		(Tt_message m, _TT_CONST char *slotname,
		 xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_context_set,
		(Tt_message m, _TT_CONST char *slotname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_icontext_set,
		(Tt_message m, _TT_CONST char *slotname, int value))
_TT_EXTERN_FUNC(Tt_status,tt_message_bcontext_set,
		(Tt_message m, _TT_CONST char *slotname,
		 _TT_CONST unsigned char *value, int len))
_TT_EXTERN_FUNC(Tt_status,tt_message_xcontext_set,
		(Tt_message m, _TT_CONST char *slotname,
		 xdrproc_t xdr_proc, void *value))

_TT_EXTERN_FUNC(Tt_status,tt_context_join,
		(_TT_CONST char *slotname, _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_icontext_join,
		(_TT_CONST char *slotname, int value))
_TT_EXTERN_FUNC(Tt_status,tt_bcontext_join,
		(_TT_CONST char *slotname,
		 _TT_CONST unsigned char *value, int len))
_TT_EXTERN_FUNC(Tt_status,tt_xcontext_join,
		(_TT_CONST char *slotname,
		 xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(Tt_status,tt_context_quit,
		(_TT_CONST char *slotname, _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_icontext_quit,
		(_TT_CONST char *slotname, int value))
_TT_EXTERN_FUNC(Tt_status,tt_bcontext_quit,
		(_TT_CONST char *slotname,
		 _TT_CONST unsigned char *value, int len))
_TT_EXTERN_FUNC(Tt_status,tt_xcontext_quit,
		(_TT_CONST char *slotname,
		 xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(int,tt_message_contexts_count,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_context_slotname,
		(Tt_message m, int i))

_TT_EXTERN_FUNC(Tt_message,tt_message_create,(void))
_TT_EXTERN_FUNC(Tt_message,tt_message_create_super,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_destroy,(Tt_message m))

_TT_EXTERN_FUNC(Tt_status,tt_message_send,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_send_on_exit,(Tt_message m))
_TT_EXTERN_FUNC(Tt_message,tt_message_receive,(void))
_TT_EXTERN_FUNC(Tt_status,tt_message_reply,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_reject,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_accept,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_fail,(Tt_message m))
_TT_EXTERN_FUNC(Tt_pattern,tt_message_pattern,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_callback_add,
		(Tt_message m,Tt_message_callback f))

_TT_EXTERN_FUNC(void *,tt_message_user,(Tt_message m,int key))
_TT_EXTERN_FUNC(Tt_status,tt_message_user_set,(Tt_message m,int key,void *v))

_TT_EXTERN_FUNC(Tt_status,tt_message_arg_add,
                (Tt_message m,Tt_mode n,_TT_CONST char *vtype,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_mode,tt_message_arg_mode,(Tt_message m,int n))
_TT_EXTERN_FUNC(char *,tt_message_arg_type,(Tt_message m,int n))
_TT_EXTERN_FUNC(char *,tt_message_arg_val,(Tt_message m,int n))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_val_set,
                (Tt_message m,int n,_TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_bval,
		(Tt_message m,int n,unsigned char **value,int *len))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_bval_set,
                (Tt_message m,int n,_TT_CONST unsigned char *value,int len))
_TT_EXTERN_FUNC(Tt_status,tt_message_barg_add,
                (Tt_message m,Tt_mode n,_TT_CONST char *vtype,
		 _TT_CONST unsigned char *value,int len))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_ival,
		(Tt_message m,int n,int *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_ival_set,
                (Tt_message m,int n,int value))
_TT_EXTERN_FUNC(Tt_status,tt_message_iarg_add,
                (Tt_message m,Tt_mode n,_TT_CONST char *vtype,int value))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_xval,
		(Tt_message m,int n,xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_arg_xval_set,
                (Tt_message m,int n,xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(Tt_status,tt_message_xarg_add,
                (Tt_message m,Tt_mode n,_TT_CONST char *vtype,
		 xdrproc_t xdr_proc, void *value))
_TT_EXTERN_FUNC(int,tt_message_args_count,(Tt_message m))
_TT_EXTERN_FUNC(Tt_class,tt_message_class,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_class_set,
                (Tt_message m,Tt_class c))
_TT_EXTERN_FUNC(char *,tt_message_file,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_file_set,
		(Tt_message m,_TT_CONST char *file))
_TT_EXTERN_FUNC(char *,tt_message_object,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_object_set,
		(Tt_message m,_TT_CONST char *objid))
_TT_EXTERN_FUNC(char *,tt_message_id,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_op,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_op_set,
		(Tt_message m,_TT_CONST char *opname))
_TT_EXTERN_FUNC(int,tt_message_opnum,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_otype,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_otype_set,
		(Tt_message m,_TT_CONST char *otype))
_TT_EXTERN_FUNC(Tt_address,tt_message_address,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_address_set,
                (Tt_message m,Tt_address p))
_TT_EXTERN_FUNC(char *,tt_message_handler,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_handler_set,
		(Tt_message m,_TT_CONST char *procid))
_TT_EXTERN_FUNC(char *,tt_message_handler_ptype,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_handler_ptype_set,
                (Tt_message m,_TT_CONST char *ptid))
_TT_EXTERN_FUNC(int,tt_message_accepters_count,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_accepter,(Tt_message m,int n))
_TT_EXTERN_FUNC(int,tt_message_rejecters_count,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_rejecter,(Tt_message m,int n))
_TT_EXTERN_FUNC(int,tt_message_abstainers_count,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_abstainer,(Tt_message m,int n))
_TT_EXTERN_FUNC(Tt_disposition,tt_message_disposition,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_disposition_set,
                (Tt_message m,Tt_disposition r))
_TT_EXTERN_FUNC(Tt_scope,tt_message_scope,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_scope_set,(Tt_message m,Tt_scope s))
_TT_EXTERN_FUNC(char *,tt_message_sender,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_message_sender_ptype,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_sender_ptype_set,
                (Tt_message m,_TT_CONST char *ptid))
_TT_EXTERN_FUNC(char *,tt_message_session,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_session_set,
                (Tt_message m,_TT_CONST char *sessid))
_TT_EXTERN_FUNC(Tt_state,tt_message_state,(Tt_message m))
_TT_EXTERN_FUNC(int,tt_message_status,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_status_set,(Tt_message m,int status))
_TT_EXTERN_FUNC(char *,tt_message_status_string,(Tt_message m))
_TT_EXTERN_FUNC(Tt_status,tt_message_status_string_set,
		(Tt_message m,_TT_CONST char *status_str))
_TT_EXTERN_FUNC(uid_t,tt_message_uid,(Tt_message m))
_TT_EXTERN_FUNC(gid_t,tt_message_gid,(Tt_message m))

/* 
 * The following are "macros" which provide a simpler high level
 * interface to the above low level calls.
 */
_TT_EXTERN_FUNC(Tt_message,tt_pnotice_create,
		(Tt_scope scope, _TT_CONST char *op))
_TT_EXTERN_FUNC(Tt_message,tt_prequest_create,
		(Tt_scope scope, _TT_CONST char *op))
_TT_EXTERN_FUNC(Tt_message,tt_onotice_create,
		(_TT_CONST char *objid, _TT_CONST char *op))
_TT_EXTERN_FUNC(Tt_message,tt_orequest_create,
		(_TT_CONST char *objid, _TT_CONST char *op))

_TT_EXTERN_FUNC(char *,tt_spec_create,(_TT_CONST char * filepath))
_TT_EXTERN_FUNC(Tt_status,tt_spec_write,(_TT_CONST char * nodeid))
_TT_EXTERN_FUNC(Tt_status,tt_spec_destroy,(_TT_CONST char * nodeid))

_TT_EXTERN_FUNC(char *,tt_spec_file,(_TT_CONST char * nodeid))
_TT_EXTERN_FUNC(char *,tt_spec_type,(_TT_CONST char * nodeid))
_TT_EXTERN_FUNC(Tt_status,tt_spec_type_set,
		(_TT_CONST char * nodeid,_TT_CONST char * otid))
_TT_EXTERN_FUNC(char *,tt_spec_move,
		(_TT_CONST char * nodeid,_TT_CONST char * newfilepath))
_TT_EXTERN_FUNC(char *,tt_spec_propname,(_TT_CONST char *nodeid,int n))
_TT_EXTERN_FUNC(int,tt_spec_propnames_count,(_TT_CONST char *nodeid))


_TT_EXTERN_FUNC(char *,tt_spec_prop,
		(_TT_CONST char *nodeid,_TT_CONST char *propname,int i))
_TT_EXTERN_FUNC(Tt_status,tt_spec_prop_add,
                (_TT_CONST char *nodeid,_TT_CONST char *propname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(int,tt_spec_prop_count,
		(_TT_CONST char *nodeid,_TT_CONST char *propname))
_TT_EXTERN_FUNC(Tt_status,tt_spec_prop_set,
                (_TT_CONST char *nodeid,_TT_CONST char *propname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_spec_bprop,
                (_TT_CONST char *nodeid,_TT_CONST char *propname,int i,
                 unsigned char **value,int *length))
_TT_EXTERN_FUNC(Tt_status,tt_spec_bprop_add,
                (_TT_CONST char *nodeid,_TT_CONST char *propname,
                  _TT_CONST unsigned char *value,int length))
_TT_EXTERN_FUNC(Tt_status,tt_spec_bprop_set,
                (_TT_CONST char *nodeid,_TT_CONST char *propname,
		 _TT_CONST unsigned char *value,int length))

_TT_EXTERN_FUNC(int,tt_objid_equal,
		(_TT_CONST char * nodeid1,_TT_CONST char * nodeid2))
_TT_EXTERN_FUNC(char *,tt_objid_objkey,(_TT_CONST char * nodeid))

_TT_EXTERN_FUNC(int,tt_otype_deriveds_count,(_TT_CONST char *otype))
_TT_EXTERN_FUNC(char *,tt_otype_derived,(_TT_CONST char *otype, int i))
_TT_EXTERN_FUNC(char *,tt_otype_base,(_TT_CONST char *otype))
_TT_EXTERN_FUNC(int,tt_otype_is_derived,
		(_TT_CONST char *derivedotype, _TT_CONST char *baseotype))
_TT_EXTERN_FUNC(int,tt_otype_osig_count,(_TT_CONST char * otype))
_TT_EXTERN_FUNC(int,tt_otype_hsig_count,(_TT_CONST char * otype))
_TT_EXTERN_FUNC(char *,tt_otype_osig_op,(_TT_CONST char * otype, int sig))
_TT_EXTERN_FUNC(char *,tt_otype_hsig_op,(_TT_CONST char * otype, int sig))
_TT_EXTERN_FUNC(int,tt_otype_osig_args_count,(_TT_CONST char * otype, int sig))
_TT_EXTERN_FUNC(int,tt_otype_hsig_args_count,(_TT_CONST char * otype, int sig))
_TT_EXTERN_FUNC(Tt_mode,tt_otype_osig_arg_mode,(_TT_CONST char * otype,
						int sig, int arg))
_TT_EXTERN_FUNC(Tt_mode,tt_otype_hsig_arg_mode,(_TT_CONST char * otype,
						int sig, int arg))
_TT_EXTERN_FUNC(char *,tt_otype_osig_arg_type,(_TT_CONST char * otype,
						int sig, int arg))
_TT_EXTERN_FUNC(char *,tt_otype_hsig_arg_type,(_TT_CONST char * otype,
						int sig, int arg))

_TT_EXTERN_FUNC(Tt_pattern,tt_pattern_create,(void))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_destroy,(Tt_pattern p))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_register,(Tt_pattern p))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_unregister,(Tt_pattern p))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_callback_add,
		(Tt_pattern m,Tt_message_callback f))

_TT_EXTERN_FUNC(void *,tt_pattern_user,(Tt_pattern p,int key))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_user_set,(Tt_pattern p,int key,void *v))

_TT_EXTERN_FUNC(Tt_category,tt_pattern_category,(Tt_pattern p))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_category_set,
                (Tt_pattern p,Tt_category c))

_TT_EXTERN_FUNC(Tt_status,tt_pattern_arg_add,
                (Tt_pattern p,Tt_mode n,_TT_CONST char *vtype,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_barg_add,
                (Tt_pattern m,Tt_mode n,_TT_CONST char *vtype,
		 _TT_CONST unsigned char *value,int len))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_iarg_add,
                (Tt_pattern m,Tt_mode n,_TT_CONST char *vtype, int value))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_xarg_add,
                (Tt_pattern m,Tt_mode n,_TT_CONST char *vtype,
		 xdrproc_t xdr_proc, void *value))

_TT_EXTERN_FUNC(Tt_status,tt_pattern_class_add,
                (Tt_pattern p,Tt_class c))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_file_add,
		(Tt_pattern p,_TT_CONST char *file))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_object_add,
		(Tt_pattern p,_TT_CONST char *objid))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_op_add,
		(Tt_pattern p,_TT_CONST char *opname))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_opnum_add,(Tt_pattern p,int opnum))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_otype_add,
		(Tt_pattern p,_TT_CONST char *otype))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_address_add,
                (Tt_pattern p,Tt_address d))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_disposition_add,
                (Tt_pattern p,Tt_disposition r))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_scope_add,(Tt_pattern p,Tt_scope s))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_sender_add,
		(Tt_pattern p,_TT_CONST char *procid))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_sender_ptype_add,
		(Tt_pattern p,_TT_CONST char *ptid))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_session_add,
		(Tt_pattern p,_TT_CONST char *sessid))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_state_add,(Tt_pattern p,Tt_state s))

_TT_EXTERN_FUNC(Tt_status,tt_pattern_context_add,
		(Tt_pattern p, _TT_CONST char *slotname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_icontext_add,
		(Tt_pattern p, _TT_CONST char *slotname,
		 int value))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_bcontext_add,
		(Tt_pattern p, _TT_CONST char *slotname,
		 _TT_CONST unsigned char *value, int length))
_TT_EXTERN_FUNC(Tt_status,tt_pattern_xcontext_add,
		(Tt_pattern p, _TT_CONST char *slotname,
		 xdrproc_t xdr_proc, void *value))

_TT_EXTERN_FUNC(Tt_status,tt_ptype_declare,(_TT_CONST char * ptid))
_TT_EXTERN_FUNC(Tt_status,tt_ptype_undeclare,(_TT_CONST char * ptid))
_TT_EXTERN_FUNC(Tt_status,tt_ptype_exists,(_TT_CONST char * ptid))
_TT_EXTERN_FUNC(Tt_status,tt_ptype_opnum_callback_add,
		(_TT_CONST char * ptid, int opnum, Tt_message_callback f))
_TT_EXTERN_FUNC(Tt_status,tt_otype_opnum_callback_add,
		(_TT_CONST char * otid, int opnum, Tt_message_callback f))
	

_TT_EXTERN_FUNC(Tt_status,tt_session_join,(_TT_CONST char * sessid))
_TT_EXTERN_FUNC(Tt_status,tt_session_quit,(_TT_CONST char * sessid))
_TT_EXTERN_FUNC(char *,tt_initial_session,(void))

_TT_EXTERN_FUNC(char *,tt_session_propname,(_TT_CONST char *sessid,int n))
_TT_EXTERN_FUNC(int,tt_session_propnames_count,(_TT_CONST char *sessid))
_TT_EXTERN_FUNC(char *,tt_session_prop,
		(_TT_CONST char *sessid,_TT_CONST char *propname,int i))
_TT_EXTERN_FUNC(Tt_status,tt_session_prop_add,
                (_TT_CONST char *sessid,_TT_CONST char *propname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(int,tt_session_prop_count,
		(_TT_CONST char *sessid,_TT_CONST char *propname))
_TT_EXTERN_FUNC(Tt_status,tt_session_prop_set,
		(_TT_CONST char *sessid, _TT_CONST char *propname,
		 _TT_CONST char *value))
_TT_EXTERN_FUNC(Tt_status,tt_session_bprop,
		(_TT_CONST char *sessid,_TT_CONST char *propname,int i,
		 unsigned char **value,int *length))
_TT_EXTERN_FUNC(Tt_status,tt_session_bprop_add,
                (_TT_CONST char *sessid,_TT_CONST char *propname,
		 _TT_CONST unsigned char *value,int length))
_TT_EXTERN_FUNC(Tt_status,tt_session_bprop_set,
                (_TT_CONST char *sessid,_TT_CONST char *propname,
		 _TT_CONST unsigned char *value,int length))
_TT_EXTERN_FUNC(Tt_status,tt_session_types_load,
                (_TT_CONST char *sessid, _TT_CONST char *filename))


_TT_EXTERN_FUNC(int,tt_fd,(void))

_TT_EXTERN_FUNC(int,tt_mark,(void))
_TT_EXTERN_FUNC(void,tt_release,(int mark))
_TT_EXTERN_FUNC(caddr_t,tt_malloc,(size_t s))
_TT_EXTERN_FUNC(void,tt_free,(caddr_t p))

_TT_EXTERN_FUNC(char *,tt_status_message,(Tt_status ttrc))
_TT_EXTERN_FUNC(Tt_status,tt_pointer_error,(void *pointer))
_TT_EXTERN_FUNC(Tt_status,tt_int_error,(int return_val))

_TT_EXTERN_FUNC(void,tt_error,(const char *func,Tt_status ttrc))
_TT_EXTERN_FUNC(void *,tt_error_pointer,(Tt_status ttrc))
_TT_EXTERN_FUNC(int,tt_error_int,(Tt_status ttrc))

_TT_EXTERN_FUNC(int,tt_trace_control,(int onoff))

_TT_EXTERN_FUNC(char *,tt_message_print,(Tt_message m))
_TT_EXTERN_FUNC(char *,tt_pattern_print,(Tt_pattern p))

_TT_EXTERN_FUNC(char *,tt_file_netfile, (const char *filename))
_TT_EXTERN_FUNC(char *,tt_netfile_file, (const char *netfilename))

_TT_EXTERN_FUNC(char *,tt_host_file_netfile, (const char * host, const char * filename))
_TT_EXTERN_FUNC(char *,tt_host_netfile_file, (const char * host, const char * netfilename))
_TT_EXTERN_FUNC(Tt_status,tt_feature_enabled, (Tt_feature f))
_TT_EXTERN_FUNC(Tt_status,tt_feature_required, (Tt_feature f))

_TT_EXTERN_FUNC(char*, tt_AuthFileName, (void))
_TT_EXTERN_FUNC(int, tt_LockAuthFile, (char*, int, int, long))
_TT_EXTERN_FUNC(void, tt_UnlockAuthFile, (char*))
_TT_EXTERN_FUNC(Tt_AuthFileEntry, tt_ReadAuthFileEntry, (FILE*))
_TT_EXTERN_FUNC(void, tt_FreeAuthFileEntry, (Tt_AuthFileEntry))
_TT_EXTERN_FUNC(int, tt_WriteAuthFileEntry, (FILE*, Tt_AuthFileEntry))
_TT_EXTERN_FUNC(Tt_AuthFileEntry, tt_GetAuthFileEntry, (char*, char*, char*))
_TT_EXTERN_FUNC(char*, tt_GenerateMagicCookie, (int))

#endif /* _XENUMS_ */

#define tt_ptr_error(p) tt_pointer_error((void *)(p))
#define tt_is_err(p) (TT_WRN_LAST < (p))
#undef _TT_EXTERN_FUNC
#undef _TT_CONST
#endif
