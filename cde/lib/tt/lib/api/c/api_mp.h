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
/*%%  $XConsortium: api_mp.h /main/4 1995/11/28 19:22:43 cde-sun $ 			 				 */
/* @(#)api_mp.h	1.14 93/07/30 SMI
 * api_mp.h
 * 
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */
#ifndef _TT_API_MP_H
#define _TT_API_MP_H
#include <api/c/tt_c.h>
#include <util/tt_string.h>


char           *_tt_default_ptype(void);
Tt_status       _tt_default_ptype_set(const char *ptid);
char           *_tt_default_file(void);
Tt_status       _tt_default_file_set(const char *docid);

Tt_status       _tt_file_join(const char *filepath);

Tt_status 	_tt_file_objects_query(const char *filepath, Tt_filter_function filter,
				     void *context, void *accumulator);
char           *_tt_file_procid(const char *filepath, int n);
Tt_status       _tt_file_quit(const char *filepath);

Tt_status       _tt_file_destroy(const char *filepath);
Tt_status       _tt_file_move(const char *oldfilepath,
			      const char *newfilepath);
Tt_status       _tt_file_copy(const char *oldfilepath,
			      const char *newfilepath);

Tt_status 	_tt_filelist_objects_query(const char **filepathlist,
					 Tt_filter_function filter,
					 void *context, void *accumulator);
Tt_status       _tt_message_accept(Tt_message m);
Tt_status 	_tt_message_arg_add(Tt_message m, Tt_mode n,
				    const char *vtype, const char *value);
char           *_tt_message_arg_val(Tt_message m, int n);
Tt_status       _tt_message_arg_val_set(Tt_message m, int n,
					const char *value);
Tt_status 	_tt_message_barg_add(Tt_message m, Tt_mode n,
				     const char *vtype, 
				     const unsigned char *value, int len);
Tt_status       _tt_message_arg_bval(Tt_message m, int n,
				     unsigned char **value, int *len);
Tt_status       _tt_message_arg_bval_set(Tt_message m, int n,
					 const unsigned char *value, int len);
Tt_status 	_tt_message_iarg_add(Tt_message m, Tt_mode n,
				     const char *vtype, int value);
Tt_status       _tt_message_arg_ival(Tt_message m, int n, int *value);
Tt_status       _tt_message_arg_ival_set(Tt_message m, int n, int value);
int             _tt_message_args_count(Tt_message m);
Tt_mode         _tt_message_arg_mode(Tt_message m, int n);
char *		_tt_message_arg_type(Tt_message m, int n);
Tt_status	_tt_message_callback_add(Tt_message m,
					    Tt_message_callback f);    
Tt_class        _tt_message_class(Tt_message m);
Tt_status       _tt_message_class_set(Tt_message m, Tt_class c);
Tt_message      _tt_message_create(void);
Tt_message      _tt_message_create_super(Tt_message m);
Tt_status       _tt_message_destroy(Tt_message m);
Tt_status       _tt_message_fail(Tt_message m);
char           *_tt_message_file(Tt_message m);
Tt_status       _tt_message_file_set(Tt_message m, const char *filepath);
char           *_tt_message_object(Tt_message m);
Tt_status       _tt_message_object_set(Tt_message m, const char *objid);
char           *_tt_message_observer(Tt_message m);
char           *_tt_message_op(Tt_message m);
Tt_status       _tt_message_op_set(Tt_message m, const char *opname);
int             _tt_message_opnum(Tt_message m);
char           *_tt_message_otype(Tt_message m);
Tt_status       _tt_message_otype_set(Tt_message m, const char *otype);
Tt_address     _tt_message_address(Tt_message m);
Tt_status       _tt_message_address_set(Tt_message m, Tt_address p);
Tt_pattern      _tt_message_pattern(Tt_message m);
Tt_message      _tt_message_receive(void);
char           *_tt_message_handler(Tt_message m);
Tt_status       _tt_message_handler_set(Tt_message m, const char *procid);
char           *_tt_message_handler_ptype(Tt_message m);
Tt_status       _tt_message_handler_ptype_set(Tt_message m, const char *ptid);
Tt_status       _tt_message_reject(Tt_message m);
Tt_disposition  _tt_message_disposition(Tt_message m);
Tt_status       _tt_message_disposition_set(Tt_message m, Tt_disposition r);
Tt_status       _tt_message_reply(Tt_message m);
Tt_scope        _tt_message_scope(Tt_message m);
Tt_status       _tt_message_scope_set(Tt_message m, Tt_scope s);
Tt_status       _tt_message_send(Tt_message m);
Tt_status       _tt_message_send_on_exit(Tt_message m);
char           *_tt_message_sender(Tt_message m);
char           *_tt_message_sender_ptype(Tt_message m);
Tt_status       _tt_message_sender_ptype_set(Tt_message m, const char *ptid);
char           *_tt_message_session(Tt_message m);
Tt_status       _tt_message_session_set(Tt_message m, const char *sessid);
Tt_state        _tt_message_state(Tt_message m);
int	        _tt_message_status(Tt_message m);
Tt_status       _tt_message_status_set(Tt_message m, int status);
char	        *_tt_message_status_string(Tt_message m);
Tt_status       _tt_message_status_string_set(Tt_message m, const char *status_str);
uid_t	        _tt_message_uid(Tt_message m);
gid_t	        _tt_message_gid(Tt_message m);
void           *_tt_message_user(Tt_message m, int key);
Tt_status       _tt_message_user_set(Tt_message m, int key, void *v);

int		_tt_otype_deriveds_count(const char *otype);
char *		_tt_otype_derived(const char *otype, int i);
char *		_tt_otype_base(const char *otype);
int		_tt_otype_is_derived(const char *derivedotype,
				     const char *baseotype);
int		_tt_otype_osig_count(const char * otype);
int		_tt_otype_hsig_count(const char * otype);
char *		_tt_otype_osig_op(const char * otype, int sig);
char *		_tt_otype_hsig_op(const char * otype, int sig);
int		_tt_otype_osig_args_count(const char * otype, int sig);
int		_tt_otype_hsig_args_count(const char * otype, int sig);
Tt_mode		_tt_otype_osig_arg_mode(const char * otype,
					int sig, int arg);
Tt_mode		_tt_otype_hsig_arg_mode(const char * otype,
					int sig, int arg);
char *		_tt_otype_osig_arg_type(const char * otype,
					int sig, int arg);
char *		_tt_otype_hsig_arg_type(const char * otype,
					int sig, int arg);
Tt_status	_tt_valid_otype(const char *otype);

Tt_status 	_tt_spec_bprop(const char *nodeid, const char *propname, int i,
			       unsigned char **value, int *length);
Tt_status 	_tt_spec_bprop_add(const char *nodeid, const char *propname,
				   unsigned const char *value, int length);
Tt_status 	_tt_spec_bprop_set(const char *nodeid, const char *propname,
				   unsigned const char *value, int length);
char           *_tt_spec_create(const char *filepath);
Tt_status       _tt_spec_destroy(const char *nodeid);
char           *_tt_spec_file(const char *nodeid);
char           *_tt_spec_move(const char *nodeid, const char *newfilepath);
Tt_status	_tt_spec_type_set(const char * nodeid, const char * otid);
char	       *_tt_spec_type(const char * nodeid);
char           *_tt_spec_prop(const char *nodeid, const char *propname, int i);
Tt_status       _tt_spec_prop_add(const char *nodeid, const char *propname, const char *value);
int             _tt_spec_prop_count(const char *nodeid, const char *propname);
Tt_status       _tt_spec_prop_set(const char *nodeid, const char *propname, const char *value);
char           *_tt_spec_propname(const char *nodeid, int n);
int             _tt_spec_propnames_count(const char *nodeid);
Tt_status       _tt_spec_write(const char *nodeid);

int             _tt_objid_equal(const char *nodeid1, const char *nodeid2);
char           *_tt_objid_objkey(const char *nodeid);
Tt_status 	_tt_pattern_arg_add(Tt_pattern p, Tt_mode n,
				    const char *vtype, const char *value);
Tt_status 	_tt_pattern_barg_add(Tt_pattern p, Tt_mode n,
				     const char *vtype, 
				     const unsigned char *value, int len);
Tt_status 	_tt_pattern_iarg_add(Tt_pattern p, Tt_mode n,
				     const char *vtype, int value);

Tt_status	_tt_pattern_callback_add(Tt_pattern m,
					   Tt_message_callback f);    
Tt_category     _tt_pattern_category(Tt_pattern p);
Tt_status       _tt_pattern_category_set(Tt_pattern p, Tt_category c);
Tt_status       _tt_pattern_class_add(Tt_pattern p, Tt_class c);
Tt_pattern      _tt_pattern_create(void);
Tt_status       _tt_pattern_destroy(Tt_pattern p);
Tt_status       _tt_pattern_file_add(Tt_pattern p, const char *filepath);
Tt_status       _tt_pattern_object_add(Tt_pattern p, const char *objid);
Tt_status       _tt_pattern_op_add(Tt_pattern p, const char *opname);
Tt_status       _tt_pattern_otype_add(Tt_pattern p, const char *otype);
Tt_status       _tt_pattern_address_add(Tt_pattern p, Tt_address d);
Tt_status       _tt_pattern_register(Tt_pattern p);
Tt_status       _tt_pattern_unregister(Tt_pattern p);
Tt_status       _tt_pattern_disposition_add(Tt_pattern p, Tt_disposition r);
Tt_status       _tt_pattern_scope_add(Tt_pattern p, Tt_scope s);
Tt_status       _tt_pattern_sender_add(Tt_pattern p, const char *procid);
Tt_status       _tt_pattern_sender_ptype_add(Tt_pattern p, const char *ptid);
Tt_status       _tt_pattern_session_add(Tt_pattern p, const char *sessid);
Tt_status	_tt_pattern_state_add(Tt_pattern p, Tt_state s);
void           *_tt_pattern_user(Tt_pattern p, int key);
Tt_status       _tt_pattern_user_set(Tt_pattern p, int key, void *v);

Tt_status       _tt_ptype_declare(const char *ptid);
Tt_status       _tt_ptype_undeclare(const char *ptid);
Tt_status       _tt_ptype_exists(const char *ptid);


char 		*_tt_session_prop(const char *sessid, const char *propname,
				  int i);
Tt_status	_tt_session_prop_add(const char *sessid, const char *propname,
				     const char *value);
int		_tt_session_prop_count(const char *sessid,
				       const char *propname);
Tt_status	_tt_session_prop_set(const char *sessid, const char *propname,
				     const char *value);
Tt_status	_tt_session_bprop(const char *sessid, const char *propname,
				  int i,
				  unsigned char **value, int *length);
Tt_status	_tt_session_bprop_add(const char *sessid, const char *propname,
				      unsigned const char *value, int length);
Tt_status	_tt_session_bprop_set(const char *sessid, const char *propname,
				      unsigned const char *value, int length);
char	       *_tt_session_propname(const char *sessid, int n);
int		_tt_session_propnames_count(const char *sessid);

char           *_tt_default_session(void);
Tt_status       _tt_default_session_set(const char *sessid);

char           *_tt_default_procid(void);
Tt_status       _tt_default_procid_set(const char *procid);

char           *_tt_thread_session(void);
Tt_status       _tt_thread_session_set(const char *sessid);

char           *_tt_thread_procid(void);
Tt_status       _tt_thread_procid_set(const char *procid);

char	       *_tt_procid_session(const char *);
Tt_status	_tt_feature_enabled(Tt_feature f);
Tt_status	_tt_feature_required(Tt_feature f);

void		_tt_internal_init();
#endif		
