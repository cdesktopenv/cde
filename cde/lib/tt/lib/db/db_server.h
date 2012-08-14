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
/*%%  $XConsortium: db_server.h /main/3 1995/10/23 09:58:23 rswiston $ 			 				 */
/*-*-C++-*-
 * Tool Talk Database Server - db_server.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * This file contains the declaration of structures for rpc calls to the
 * database server.
 */

#ifndef  _DB_SERVER_H
#define  _DB_SERVER_H

#include <rpc/rpc.h>

#include "util/tt_string.h"
#include "db/tt_db_results.h"
#include "tt_const.h"

/* declarations for keydesc */
struct keydesc;

typedef struct keypart keypart;
bool_t xdr_keypart(XDR*, keypart*);

typedef struct keydesc keydesc;
bool_t xdr_keydesc(XDR*, keydesc*);

/* declaration for structure containing results of NetISAM operations */

struct _Tt_isam_results {
	int result;
	int iserrno;
};
typedef struct _Tt_isam_results _Tt_isam_results;
bool_t xdr_Tt_isam_results(XDR*, _Tt_isam_results*);

/* structure containing arguments for NetISAM isaddindex operation */

struct _Tt_isaddindex_args {
	int isfd;
	keydesc *key;
};
typedef struct _Tt_isaddindex_args _Tt_isaddindex_args;
bool_t xdr_Tt_isaddindex_args(XDR*, _Tt_isaddindex_args*);

/* structure containing arguments for NetISAM isbuild operation */

struct _Tt_isbuild_args {
	char *path;
	int reclen;
	keydesc *key;
	int mode;
	int isreclen;
};
typedef struct _Tt_isbuild_args _Tt_isbuild_args;
bool_t xdr_Tt_isbuild_args(XDR*, _Tt_isbuild_args*);

/* structure containing arguments for NetISAM iscntl operation */

struct _Tt_iscntl_args {
	int isfd;
	int func;
	struct {
		u_int arg_len;
		char *arg_val;
	} arg;
};
typedef struct _Tt_iscntl_args _Tt_iscntl_args;
bool_t xdr_Tt_iscntl_args(XDR*, _Tt_iscntl_args*);

struct _Tt_iscntl_results {
	int result;
	int iserrno;
	struct {
		u_int arg_len;
		char *arg_val;
	} arg;
};
typedef struct _Tt_iscntl_results _Tt_iscntl_results;
bool_t xdr_Tt_iscntl_results(XDR*, _Tt_iscntl_results*);

/* structure containing arguments for NetISAM isdelrec operation */

struct _Tt_isdelrec_args {
	int isfd;
	long recnum;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
};
typedef struct _Tt_isdelrec_args _Tt_isdelrec_args;
bool_t xdr_Tt_isdelrec_args(XDR*, _Tt_isdelrec_args*);

/* structure containing arguments for NetISAM isopen operation */

struct _Tt_isopen_args {
	char *path;
	int mode;
};
typedef struct _Tt_isopen_args _Tt_isopen_args;
bool_t xdr_Tt_isopen_args(XDR*, _Tt_isopen_args*);

/* structure containing arguments for NetISAM isread operation */

struct _Tt_isread_args {
	int isfd;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
	int mode;
	long isrecnum;
};
typedef struct _Tt_isread_args _Tt_isread_args;
bool_t xdr_Tt_isread_args(XDR*, _Tt_isread_args*);

/* structure containing results for NetISAM isread operation */

struct _Tt_isread_results {
	_Tt_isam_results isresult;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
	int isreclen;
	long isrecnum;
};
typedef struct _Tt_isread_results _Tt_isread_results;
bool_t xdr_Tt_isread_results(XDR*, _Tt_isread_results*);

/* structure containing arguments for NetISAM isrewrec operation */

struct _Tt_isrewrec_args {
	int isfd;
	long recnum;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
};
typedef struct _Tt_isrewrec_args _Tt_isrewrec_args;
bool_t xdr_Tt_isrewrec_args(XDR*, _Tt_isrewrec_args*);

/* structure containing arguments for NetISAM isstart operation */

struct _Tt_isstart_args {
	int isfd;
	keydesc *key;
	int key_len;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
	int mode;
};
typedef struct _Tt_isstart_args _Tt_isstart_args;
bool_t xdr_Tt_isstart_args(XDR*, _Tt_isstart_args*);

/* structure containing arguments for test and set operation */

struct _Tt_test_and_set_args {
	int isfd;
	keydesc *key;
	int key_len;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
};
typedef struct _Tt_test_and_set_args _Tt_test_and_set_args;
bool_t xdr_Tt_test_and_set_args(XDR*, _Tt_test_and_set_args*);

/* structure containing results of test and set operation */

struct _Tt_test_and_set_results {
	_Tt_isam_results isresult;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
	int isreclen;
	long isrecnum;
};
typedef struct _Tt_test_and_set_results _Tt_test_and_set_results;
bool_t xdr_Tt_test_and_set_results(XDR*, _Tt_test_and_set_results*);

/* structure containing a list of transaction records */

typedef struct _Tt_trans_record *_Tt_trans_record_list;
bool_t xdr_Tt_trans_record_list(XDR*, _Tt_trans_record_list*);

struct _Tt_trans_record {
	int newp;
	long recnum;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
	_Tt_trans_record_list next;
};
typedef struct _Tt_trans_record _Tt_trans_record;
bool_t xdr_Tt_trans_record(XDR*, _Tt_trans_record*);

/* structure containing arguments for the transaction operation */

struct _Tt_transaction_args {
	int isfd;
	_Tt_trans_record_list recs;
};
typedef struct _Tt_transaction_args _Tt_transaction_args;
bool_t xdr_Tt_transaction_args(XDR*, _Tt_transaction_args*);

/* structure containing arguments for the NetISAM iswrite operation */

struct _Tt_iswrite_args {
	int isfd;
	struct {
		u_int rec_len;
		char *rec_val;
	} rec;
};
typedef struct _Tt_iswrite_args _Tt_iswrite_args;
bool_t xdr_Tt_iswrite_args(XDR*, _Tt_iswrite_args*);

/* structure containing arguments for the setoiduser operation */

struct _Tt_oidaccess_args {
	int isfd;
	struct {
		u_int key_len;
		const char *key_val;
	} key;
	short value;
};
typedef struct _Tt_oidaccess_args _Tt_oidaccess_args;
bool_t xdr_Tt_oidaccess_args(XDR*, _Tt_oidaccess_args*);

struct _Tt_oidaccess_results {
	uid_t uid;
	gid_t group;
	mode_t mode;
	int result;
	int iserrno;
};
typedef struct _Tt_oidaccess_results _Tt_oidaccess_results;
bool_t xdr_Tt_oidaccess_results(XDR*, _Tt_oidaccess_results*);

struct _Tt_prop {
	struct {
		u_int propname_len;
		const char *propname_val;
	} propname;
	long recnum;
	struct {
		u_int value_len;
		char *value_val;
	} value;
};
typedef struct _Tt_prop _Tt_prop;
bool_t xdr_Tt_prop(XDR*, _Tt_prop*);

typedef struct {
	u_int props_len;
	_Tt_prop *props_val;
} _Tt_prop_array;

struct _Tt_spec_props {
	int isfd;
	keydesc *key;
	int key_len;
	struct {
		u_int oidkey_len;
		const char *oidkey_val;
	} oidkey;
	_Tt_prop_array props;
	int result;
	int iserrno;
};
typedef struct _Tt_spec_props _Tt_spec_props;
bool_t xdr_Tt_spec_props(XDR*, _Tt_spec_props*);

struct _Tt_session_args {
	int isfd;
	keydesc *key;
	int key_len;
	struct {
		u_int oidkey_len;
		const char *oidkey_val;
	} oidkey;
	struct {
		u_int session_len;
		char *session_val;
	} session;
};
typedef struct _Tt_session_args _Tt_session_args;
bool_t xdr_Tt_session_args(XDR*, _Tt_session_args*);

typedef enum _Tt_db_results _tt_db_results;
bool_t xdr_tt_db_results(XDR *, _tt_db_results *);

struct _tt_string {
	char *value;
};
typedef struct _tt_string _tt_string;
bool_t xdr_tt_string(XDR *, _tt_string *);

struct _tt_string_list {
       u_int values_len;
      _tt_string *values_val;
};
typedef _tt_string_list _tt_string_list;

struct _tt_property_value {
	struct {
		u_int value_len;
		char *value_val;
	} value;
};
typedef struct _tt_property_value _tt_property_value;
bool_t xdr_tt_property_value(XDR *, _tt_property_value *);

struct _tt_property {
	char *name;
	struct {
		u_int values_len;
		_tt_property_value *values_val;
	} values;
};
typedef struct _tt_property _tt_property;
bool_t xdr_tt_property(XDR *, _tt_property *);

struct _tt_property_list {
	u_int properties_len;
	_tt_property *properties_val;
};
typedef struct _tt_property_list _tt_property_list;

struct _tt_access {
	uid_t user;
	gid_t group;
	mode_t mode;
};
typedef struct _tt_access _tt_access;
bool_t xdr_tt_access(XDR *, _tt_access *);

struct _tt_message {
	struct {
		u_int body_len;
		char *body_val;
	} body;
};
typedef struct _tt_message _tt_message;
bool_t xdr_tt_message(XDR *, _tt_message *);

struct _tt_message_list {
       u_int messages_len;
       _tt_message *messages_val;
};
typedef _tt_message_list _tt_message_list;

struct _tt_create_file_args {
	char *file;
	_tt_property_list properties;
	_tt_access access;
};
typedef struct _tt_create_file_args _tt_create_file_args;
bool_t xdr_tt_create_file_args(XDR *, _tt_create_file_args *);

struct _tt_create_obj_args {
	char *file;
	char *objid;
	char *otype;
	_tt_property_list properties;
	_tt_access access;
};
typedef struct _tt_create_obj_args _tt_create_obj_args;
bool_t xdr_tt_create_obj_args(XDR *, _tt_create_obj_args *);

struct _tt_remove_file_args {
        char *file;
        _tt_access access;
};
typedef struct _tt_remove_file_args _tt_remove_file_args;
bool_t xdr_tt_remove_file_args(XDR *, _tt_remove_file_args *);

struct _tt_remove_obj_args {
        char *objid;
	char *forward_pointer;
        _tt_access access;
};
typedef struct _tt_remove_obj_args _tt_remove_obj_args;
bool_t xdr_tt_remove_obj_args(XDR *, _tt_remove_obj_args *);
 
struct _tt_move_file_args {
        char *file;
	char *new_file;
        _tt_access access;
};
typedef struct _tt_move_file_args _tt_move_file_args;
bool_t xdr_tt_move_file_args(XDR *, _tt_move_file_args *);

struct _tt_set_file_prop_args {
	char *file;
	_tt_property property;
	_tt_access access;
};
typedef struct _tt_set_file_prop_args _tt_set_file_prop_args;
bool_t xdr_tt_set_file_prop_args(XDR *, _tt_set_file_prop_args *);

struct _tt_set_file_props_args {
	char *file;
	_tt_property_list properties;
	_tt_access access;
};
typedef struct _tt_set_file_props_args _tt_set_file_props_args;
bool_t xdr_tt_set_file_props_args(XDR *, _tt_set_file_props_args *);

struct _tt_add_file_prop_args {
	char *file;
	_tt_property property;
	int unique;
	_tt_access access;
};
typedef struct _tt_add_file_prop_args _tt_add_file_prop_args;
bool_t xdr_tt_add_file_prop_args(XDR *, _tt_add_file_prop_args *);

struct _tt_del_file_prop_args {
	char *file;
	_tt_property property;
	_tt_access access;
};
typedef struct _tt_del_file_prop_args _tt_del_file_prop_args;
bool_t xdr_tt_del_file_prop_args(XDR *, _tt_del_file_prop_args *);

struct _tt_get_file_prop_args {
	char *file;
	char *name;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_get_file_prop_args _tt_get_file_prop_args;
bool_t xdr_tt_get_file_prop_args(XDR *, _tt_get_file_prop_args *);

struct _tt_get_file_props_args {
	char *file;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_get_file_props_args _tt_get_file_props_args;
bool_t xdr_tt_get_file_props_args(XDR *, _tt_get_file_props_args *);

struct _tt_get_file_objs_args {
        char *file;
	_tt_access access;
        int cache_level;
};
typedef struct _tt_get_file_objs_args _tt_get_file_objs_args;
bool_t xdr_tt_get_file_objs_args(XDR *, _tt_get_file_objs_args *);
 
struct _tt_set_file_access_args {
	char *file;
	_tt_access new_access;
	_tt_access access;
};
typedef struct _tt_set_file_access_args _tt_set_file_access_args;
bool_t xdr_tt_set_file_access_args(XDR *, _tt_set_file_access_args *);

struct _tt_get_file_access_args {
        char *file;
	_tt_access access;
};
typedef struct _tt_get_file_access_args _tt_get_file_access_args;
bool_t xdr_tt_get_file_access_args(XDR *, _tt_get_file_access_args *);
 
struct _tt_set_obj_prop_args {
	char *objid;
	_tt_property property;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_set_obj_prop_args _tt_set_obj_prop_args;
bool_t xdr_tt_set_obj_prop_args(XDR *, _tt_set_obj_prop_args *);

struct _tt_set_obj_props_args {
	char *objid;
	_tt_property_list properties;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_set_obj_props_args _tt_set_obj_props_args;
bool_t xdr_tt_set_obj_props_args(XDR *, _tt_set_obj_props_args *);

struct _tt_add_obj_prop_args {
	char *objid;
	_tt_property property;
	int unique;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_add_obj_prop_args _tt_add_obj_prop_args;
bool_t xdr_tt_add_obj_prop_args(XDR *, _tt_add_obj_prop_args *);

struct _tt_del_obj_prop_args {
	char *objid;
	_tt_property property;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_del_obj_prop_args _tt_del_obj_prop_args;
bool_t xdr_tt_del_obj_prop_args(XDR *, _tt_del_obj_prop_args *);

struct _tt_get_obj_prop_args {
	char *objid;
	char *name;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_get_obj_prop_args _tt_get_obj_prop_args;
bool_t xdr_tt_get_obj_prop_args(XDR *, _tt_get_obj_prop_args *);

struct _tt_get_obj_props_args {
	char *objid;
	_tt_access access;
	int cache_level;
};
typedef struct _tt_get_obj_props_args _tt_get_obj_props_args;
bool_t xdr_tt_get_obj_props_args(XDR *, _tt_get_obj_props_args *);

struct _tt_set_obj_type_args {
	char *objid;
	char *otype;
	_tt_access access;
};
typedef struct _tt_set_obj_type_args _tt_set_obj_type_args;
bool_t xdr_tt_set_obj_type_args(XDR *, _tt_set_obj_type_args *);

struct _tt_get_obj_type_args {
        char *objid;
	_tt_access access;
};
typedef struct _tt_get_obj_type_args _tt_get_obj_type_args;
bool_t xdr_tt_get_obj_type_args(XDR *, _tt_get_obj_type_args *);
 
struct _tt_set_obj_file_args {
        char *objid;
        char *file;
        _tt_access access;
};
typedef struct _tt_set_obj_file_args _tt_set_obj_file_args;
bool_t xdr_tt_set_obj_file_args(XDR *, _tt_set_obj_file_args *);

struct _tt_get_obj_file_args {
        char *objid;
	_tt_access access;
};
typedef struct _tt_get_obj_file_args _tt_get_obj_file_args;
bool_t xdr_tt_get_obj_file_args(XDR *, _tt_get_obj_file_args *);
 
struct _tt_set_obj_access_args {
	char *objid;
	_tt_access new_access;
	_tt_access access;
};
typedef struct _tt_set_obj_access_args _tt_set_obj_access_args;
bool_t xdr_tt_set_obj_access_args(XDR *, _tt_set_obj_access_args *);

struct _tt_get_obj_access_args {
        char *objid;
	_tt_access access;
};
typedef struct _tt_get_obj_access_args _tt_get_obj_access_args;
bool_t xdr_tt_get_obj_access_args(XDR *, _tt_get_obj_access_args *);
 
struct _tt_is_file_in_db_args {
        char *file;
	_tt_access access;
};
typedef struct _tt_is_file_in_db_args _tt_is_file_in_db_args;
bool_t xdr_tt_is_file_in_db_args(XDR *, _tt_is_file_in_db_args *);
 
struct _tt_is_obj_in_db_args {
        char *objid;
	_tt_access access;
};
typedef struct _tt_is_obj_in_db_args _tt_is_obj_in_db_args;
bool_t xdr_tt_is_obj_in_db_args(XDR *, _tt_is_obj_in_db_args *);
 
struct _tt_queue_msg_args {
	char *file;
	_tt_string_list ptypes;
	_tt_message message;
};
typedef struct _tt_queue_msg_args _tt_queue_msg_args;
bool_t xdr_tt_queue_msg_args(XDR *, _tt_queue_msg_args *);

struct _tt_dequeue_msgs_args {
	char *file;
	_tt_string_list ptypes;
};
typedef struct _tt_dequeue_msgs_args _tt_dequeue_msgs_args;
bool_t xdr_tt_dequeue_msgs_args(XDR *, _tt_dequeue_msgs_args *);

struct _tt_file_netfile_args {
	char *file_or_netfile;
};
typedef struct _tt_file_netfile_args _tt_file_netfile_args;
bool_t xdr_tt_file_netfile_args(XDR *, _tt_file_netfile_args *);

struct _tt_get_all_sessions_args {
	struct {
		u_int oidkey_len;
		const char *oidkey_val;
	} oidkey;
};
typedef struct _tt_get_all_sessions_args _tt_get_all_sessions_args;
bool_t	xdr_tt_get_all_sessions_args(XDR *, _tt_get_all_sessions_args *);

struct _tt_delete_session_args {
    _tt_string		session;
};
typedef struct _tt_delete_session_args _tt_delete_session_args;
bool_t	xdr_tt_delete_session_args(XDR *, _tt_delete_session_args *);

struct _tt_db_cache_results {
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_db_cache_results _tt_db_cache_results;
bool_t xdr_tt_db_cache_results(XDR *, _tt_db_cache_results *);

struct _tt_auth_level_results {
	int auth_level;
	_tt_db_results results;
};
typedef struct _tt_auth_level_results _tt_auth_level_results;
bool_t xdr_tt_auth_level_results(XDR *, _tt_auth_level_results *);

struct _tt_file_partition_results {
	char *partition;
	char *network_path;
	_tt_db_results results;
};
typedef struct _tt_file_partition_results _tt_file_partition_results;
bool_t xdr_tt_file_partition_results(XDR *, _tt_file_partition_results *);

struct _tt_file_prop_results {
	_tt_property property;
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_file_prop_results _tt_file_prop_results;
bool_t xdr_tt_file_prop_results(XDR *, _tt_file_prop_results *);

struct _tt_file_props_results {
	_tt_property_list properties;
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_file_props_results _tt_file_props_results;
bool_t xdr_tt_file_props_results(XDR *, _tt_file_props_results *);

struct _tt_file_objs_results {
	_tt_string_list objids;
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_file_objs_results _tt_file_objs_results;
bool_t xdr_tt_file_objs_results(XDR *, _tt_file_objs_results *);

struct _tt_file_access_results {
	_tt_access access;
	_tt_db_results results;
};
typedef struct _tt_file_access_results _tt_file_access_results;
bool_t xdr_tt_file_access_results(XDR *, _tt_file_access_results *);

struct _tt_obj_prop_results {
	_tt_property property;
	char *file;
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_obj_prop_results _tt_obj_prop_results;
bool_t xdr_tt_obj_prop_results(XDR *, _tt_obj_prop_results *);

struct _tt_obj_props_results {
	_tt_property_list properties;
	char *file;
	int cache_level;
	_tt_db_results results;
};
typedef struct _tt_obj_props_results _tt_obj_props_results;
bool_t xdr_tt_obj_props_results(XDR *, _tt_obj_props_results *);

struct _tt_obj_type_results {
	char *otype;
	_tt_db_results results;
};
typedef struct _tt_obj_type_results _tt_obj_type_results;
bool_t xdr_tt_obj_type_results(XDR *, _tt_obj_type_results *);

struct _tt_obj_file_results {
	char *file;
	_tt_db_results results;
};
typedef struct _tt_obj_file_results _tt_obj_file_results;
bool_t xdr_tt_obj_file_results(XDR *, _tt_obj_file_results *);

struct _tt_obj_access_results {
	_tt_access access;
	_tt_db_results results;
};
typedef struct _tt_obj_access_results _tt_obj_access_results;
bool_t xdr_tt_obj_access_results(XDR *, _tt_obj_access_results *);

struct _tt_is_file_in_db_results {
	bool_t directory_flag;
	_tt_db_results results;
};
typedef struct _tt_is_file_in_db_results _tt_is_file_in_db_results;
bool_t xdr_tt_is_file_in_db_results(XDR *, _tt_is_file_in_db_results *);

struct _tt_is_obj_in_db_results {
	char *forward_pointer;
	_tt_db_results results;
};
typedef struct _tt_is_obj_in_db_results _tt_is_obj_in_db_results;
bool_t xdr_tt_is_obj_in_db_results(XDR *, _tt_is_obj_in_db_results *);

struct _tt_dequeue_msgs_results {
        _tt_message_list messages;
	_tt_db_results results;
};
typedef struct _tt_dequeue_msgs_results _tt_dequeue_msgs_results;
bool_t xdr_tt_dequeue_msgs_results(XDR *, _tt_dequeue_msgs_results *);

struct _tt_file_netfile_results {
	char *result_string;
	_tt_db_results results;
	int tt_status;
};
typedef struct _tt_file_netfile_results _tt_file_netfile_results;
bool_t xdr_tt_file_netfile_results(XDR *, _tt_file_netfile_results *);

struct _tt_get_all_sessions_results {
	struct {
		u_int oidkey_len;
		const char *oidkey_val;
	} oidkey;
	_tt_string_list		session_list;
};
typedef struct _tt_get_all_sessions_results _tt_get_all_sessions_results;
bool_t	xdr_tt_get_all_sessions_results(XDR *,_tt_get_all_sessions_results *);

struct _tt_delete_session_results {
	int	tt_status;
};
typedef struct _tt_delete_session_results _tt_delete_session_results;
bool_t	xdr_tt_delete_session_results(XDR *,_tt_delete_session_results *);

struct _tt_garbage_collect_results {
	int	tt_status;
};
bool_t	xdr_tt_garbage_collect_results(XDR *, _tt_garbage_collect_results *);

#define TT_DBSERVER_PROG 	((u_long)100083)
#define TT_DBSERVER_VERS 	((u_long)1)

#define _TT_MIN_AUTH_LEVEL 	((u_long)1)
#define _TT_ISADDINDEX 		((u_long)2)
#define _TT_ISBUILD 		((u_long)3)
#define _TT_ISCLOSE 		((u_long)4)
#define _TT_ISCNTL 		((u_long)5)
#define _TT_ISDELREC 		((u_long)6)
#define _TT_ISERASE 		((u_long)7)
#define _TT_ISOPEN 		((u_long)8)
#define _TT_ISREAD 		((u_long)9)
#define _TT_ISREWREC 		((u_long)10)
#define _TT_ISSTART 		((u_long)11)
#define _TT_ISWRITE 		((u_long)12)
#define _TT_TEST_AND_SET 	((u_long)13)
#define _TT_TRANSACTION 	((u_long)14)
#define _TT_MFS 		((u_long)15)
#define _TT_GETOIDACCESS	((u_long)16)
#define _TT_SETOIDUSER 		((u_long)17)
#define _TT_SETOIDGROUP		((u_long)18)
#define _TT_SETOIDMODE 		((u_long)19)
#define _TT_READSPEC		((u_long)20)
#define _TT_WRITESPEC		((u_long)21)
#define _TT_ADDSESSION		((u_long)22)
#define _TT_DELSESSION		((u_long)23)
#define _TT_GETTYPE		((u_long)24)

#define TT_GET_MIN_AUTH_LEVEL   ((u_long)101)
#define TT_GET_FILE_PARTITION   ((u_long)102)
#define TT_CREATE_FILE          ((u_long)103)
#define TT_CREATE_OBJ           ((u_long)104)
#define TT_REMOVE_FILE          ((u_long)105)
#define TT_REMOVE_OBJ           ((u_long)106)
#define TT_MOVE_FILE            ((u_long)107)
#define TT_SET_FILE_PROPS       ((u_long)108)
#define TT_SET_FILE_PROP        ((u_long)109)
#define TT_ADD_FILE_PROP        ((u_long)110)
#define TT_DELETE_FILE_PROP     ((u_long)111)
#define TT_GET_FILE_PROP        ((u_long)112)
#define TT_GET_FILE_PROPS       ((u_long)113)
#define TT_GET_FILE_OBJS        ((u_long)114)
#define TT_SET_FILE_ACCESS      ((u_long)115)
#define TT_GET_FILE_ACCESS      ((u_long)116)
#define TT_SET_OBJ_PROPS        ((u_long)117)
#define TT_SET_OBJ_PROP         ((u_long)118)
#define TT_ADD_OBJ_PROP         ((u_long)119)
#define TT_DELETE_OBJ_PROP      ((u_long)120)
#define TT_GET_OBJ_PROP         ((u_long)121)
#define TT_GET_OBJ_PROPS        ((u_long)122)
#define TT_SET_OBJ_TYPE         ((u_long)123)
#define TT_GET_OBJ_TYPE         ((u_long)124)
#define TT_SET_OBJ_FILE         ((u_long)125)
#define TT_GET_OBJ_FILE         ((u_long)126)
#define TT_SET_OBJ_ACCESS       ((u_long)127)
#define TT_GET_OBJ_ACCESS       ((u_long)128)
#define TT_IS_FILE_IN_DB        ((u_long)129)
#define TT_IS_OBJ_IN_DB         ((u_long)130)
#define TT_QUEUE_MESSAGE        ((u_long)131)
#define TT_DEQUEUE_MESSAGES     ((u_long)132)
#define TTDB_FILE_NETFILE	((u_long)133)
#define TTDB_NETFILE_FILE	((u_long)134)

#define TT_GET_ALL_SESSIONS	((u_long)135)
#define TT_GARBAGE_COLLECT	((u_long)136)	/* Perform garbage cleanup */
#define TT_DELETE_SESSION	((u_long)137)	/* Delete named session */

#ifdef _TT_DBCLIENT_SIDE

#include "db/tt_db_client_utils.h"

extern int *_tt_min_auth_level_1(char**, CLIENT*, clnt_stat *status);
extern _Tt_isam_results *_tt_isaddindex_1(_Tt_isaddindex_args*, CLIENT*);
extern _Tt_isam_results *_tt_isbuild_1(_Tt_isbuild_args*, CLIENT*);
extern _Tt_isam_results *_tt_isclose_1(int*, CLIENT*);
extern _Tt_iscntl_results *_tt_iscntl_1(_Tt_iscntl_args*, CLIENT*);
extern _Tt_isam_results *_tt_isdelrec_1(_Tt_isdelrec_args*, CLIENT*);
extern _Tt_isam_results *_tt_iserase_1(char**, CLIENT*);
extern _Tt_isam_results *_tt_isopen_1(_Tt_isopen_args*, CLIENT*);
extern _Tt_isread_results *_tt_isread_1(_Tt_isread_args*, CLIENT*);
extern _Tt_isam_results *_tt_isrewrec_1(_Tt_isrewrec_args*, CLIENT*);
extern _Tt_isam_results *_tt_isstart_1(_Tt_isstart_args*, CLIENT*);
extern _Tt_isam_results *_tt_iswrite_1(_Tt_iswrite_args*, CLIENT*);
extern _Tt_test_and_set_results *_tt_test_and_set_1(_Tt_test_and_set_args*,
						    CLIENT*);
extern _Tt_isam_results *_tt_transaction_1(_Tt_transaction_args*, CLIENT*);
extern char **_tt_mfs_1(char**, CLIENT*);
extern _Tt_oidaccess_results *_tt_getoidaccess_1(_Tt_oidaccess_args*, CLIENT*);
extern _Tt_isam_results *_tt_setoiduser_1(_Tt_oidaccess_args*, CLIENT*);
extern _Tt_isam_results *_tt_setoidgroup_1(_Tt_oidaccess_args*, CLIENT*);
extern _Tt_isam_results *_tt_setoidmode_1(_Tt_oidaccess_args*, CLIENT*);
#ifdef UNUSED
// These are no longer used by the client library, but must
// remain in the server for compatiblity
extern _Tt_spec_props *_tt_readspec_1(_Tt_spec_props*, CLIENT*);
extern _Tt_isam_results *_tt_writespec_1(_Tt_spec_props*, CLIENT*);
#endif /* UNUSED */
extern _Tt_isam_results *_tt_addsession_1(_Tt_session_args*, CLIENT*);
extern _Tt_isam_results *_tt_delsession_1(_Tt_session_args*, CLIENT*);
extern _Tt_spec_props *_tt_gettype_1(_Tt_spec_props*, CLIENT*);

extern clnt_stat _tt_get_rpc_result();
extern _tt_auth_level_results *_tt_get_min_auth_level_1(void *, CLIENT *);
extern _tt_file_partition_results *_tt_get_file_partition_1(char **, CLIENT *);
extern _tt_db_cache_results *_tt_create_file_1(_tt_create_file_args *, CLIENT *);
extern _tt_db_cache_results *_tt_create_obj_1(_tt_create_obj_args *, CLIENT *);
extern _tt_db_results *_tt_remove_file_1(_tt_remove_file_args *, CLIENT *);
extern _tt_db_results *_tt_remove_obj_1(_tt_remove_obj_args *, CLIENT *);
extern _tt_db_results *_tt_move_file_1(_tt_move_file_args *, CLIENT *);
extern _tt_db_cache_results *_tt_set_file_props_1(_tt_set_file_props_args *,
						  CLIENT *);
extern _tt_db_cache_results *_tt_set_file_prop_1(_tt_set_file_prop_args *,
						 CLIENT *);
extern _tt_db_cache_results *_tt_add_file_prop_1(_tt_add_file_prop_args *,
						 CLIENT *);
extern _tt_db_cache_results *_tt_delete_file_prop_1(_tt_del_file_prop_args *,
						    CLIENT *);
extern _tt_file_prop_results *_tt_get_file_prop_1(_tt_get_file_prop_args *,
						  CLIENT *);
extern _tt_file_props_results *_tt_get_file_props_1(_tt_get_file_props_args *,
						    CLIENT *);
extern _tt_file_objs_results *_tt_get_file_objs_1(_tt_get_file_objs_args *,
						  CLIENT *);
extern _tt_db_results *_tt_set_file_access_1(_tt_set_file_access_args *,
					     CLIENT *);
extern _tt_file_access_results *_tt_get_file_access_1(_tt_get_file_access_args *,
						      CLIENT *);
extern _tt_obj_props_results *_tt_set_obj_props_1(_tt_set_obj_props_args *,
						  CLIENT *);
extern _tt_obj_props_results *_tt_set_obj_prop_1(_tt_set_obj_prop_args *,
						 CLIENT *);
extern _tt_obj_props_results *_tt_add_obj_prop_1(_tt_add_obj_prop_args *,
						 CLIENT *);
extern _tt_obj_props_results *_tt_delete_obj_prop_1(_tt_del_obj_prop_args *,
						    CLIENT *);
extern _tt_obj_prop_results *_tt_get_obj_prop_1(_tt_get_obj_prop_args *,
						CLIENT *);
extern _tt_obj_props_results *_tt_get_obj_props_1(_tt_get_obj_props_args *,
						  CLIENT *);
extern _tt_db_results *_tt_set_obj_type_1(_tt_set_obj_type_args *,
					  CLIENT *);
extern _tt_obj_type_results *_tt_get_obj_type_1(_tt_get_obj_type_args *,
						CLIENT *);
extern _tt_db_results *_tt_set_obj_file_1(_tt_set_obj_file_args *,
					  CLIENT *);
extern _tt_obj_file_results *_tt_get_obj_file_1(_tt_get_obj_file_args *,
						CLIENT *);
extern _tt_db_results *_tt_set_obj_access_1(_tt_set_obj_access_args *,
					    CLIENT *);
extern _tt_obj_access_results *_tt_get_obj_access_1(_tt_get_obj_access_args *,
						    CLIENT *);
extern _tt_is_file_in_db_results *_tt_is_file_in_db_1(_tt_is_file_in_db_args *,
						      CLIENT *);
extern _tt_is_obj_in_db_results *_tt_is_obj_in_db_1(_tt_is_obj_in_db_args *,
						    CLIENT *);
extern _tt_db_results *_tt_queue_message_1(_tt_queue_msg_args *, CLIENT *);
extern _tt_dequeue_msgs_results *_tt_dequeue_messages_1(_tt_dequeue_msgs_args *,
							CLIENT *);
extern _tt_file_netfile_results * _tt_file_netfile_1(_tt_file_netfile_args *, CLIENT *);
extern _tt_file_netfile_results * _tt_netfile_file_1(_tt_file_netfile_args *, CLIENT *);

extern _tt_file_partition_results *_tt_get_file_partition_1(char **, const _Tt_db_client_ptr &);
extern _tt_db_cache_results *_tt_create_file_1(_tt_create_file_args *, const _Tt_db_client_ptr &); 
extern _tt_db_cache_results *_tt_create_obj_1(_tt_create_obj_args *, const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_remove_file_1(_tt_remove_file_args *, const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_remove_obj_1(_tt_remove_obj_args *, const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_move_file_1(_tt_move_file_args *, const _Tt_db_client_ptr &);
extern _tt_db_cache_results *_tt_set_file_props_1(_tt_set_file_props_args *,
                                                  const _Tt_db_client_ptr &);
extern _tt_db_cache_results *_tt_set_file_prop_1(_tt_set_file_prop_args *,
                                                 const _Tt_db_client_ptr &);
extern _tt_db_cache_results *_tt_add_file_prop_1(_tt_add_file_prop_args *,
                                                 const _Tt_db_client_ptr &);
extern _tt_db_cache_results *_tt_delete_file_prop_1(_tt_del_file_prop_args *,
                                                    const _Tt_db_client_ptr &);
extern _tt_file_prop_results *_tt_get_file_prop_1(_tt_get_file_prop_args *,
                                                  const _Tt_db_client_ptr &);
extern _tt_file_props_results *_tt_get_file_props_1(_tt_get_file_props_args *,
                                                    const _Tt_db_client_ptr &);
extern _tt_file_objs_results *_tt_get_file_objs_1(_tt_get_file_objs_args *,
                                                  const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_set_file_access_1(_tt_set_file_access_args *,
                                             const _Tt_db_client_ptr &);
extern _tt_file_access_results *_tt_get_file_access_1(_tt_get_file_access_args *,
                                                      const _Tt_db_client_ptr &);
extern _tt_obj_props_results *_tt_set_obj_props_1(_tt_set_obj_props_args *,
                                                  const _Tt_db_client_ptr &);
extern _tt_obj_props_results *_tt_set_obj_prop_1(_tt_set_obj_prop_args *,
                                                 const _Tt_db_client_ptr &);
extern _tt_obj_props_results *_tt_add_obj_prop_1(_tt_add_obj_prop_args *,
                                                 const _Tt_db_client_ptr &);
extern _tt_obj_props_results *_tt_delete_obj_prop_1(_tt_del_obj_prop_args *,
                                                    const _Tt_db_client_ptr &);
extern _tt_obj_prop_results *_tt_get_obj_prop_1(_tt_get_obj_prop_args *,
                                                const _Tt_db_client_ptr &);
extern _tt_obj_props_results *_tt_get_obj_props_1(_tt_get_obj_props_args *,
                                                  const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_set_obj_type_1(_tt_set_obj_type_args *,
                                          const _Tt_db_client_ptr &);
extern _tt_obj_type_results *_tt_get_obj_type_1(_tt_get_obj_type_args *,
                                                const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_set_obj_file_1(_tt_set_obj_file_args *,
                                          const _Tt_db_client_ptr &);
extern _tt_obj_file_results *_tt_get_obj_file_1(_tt_get_obj_file_args *,
                                                const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_set_obj_access_1(_tt_set_obj_access_args *,
                                            const _Tt_db_client_ptr &);
extern _tt_obj_access_results *_tt_get_obj_access_1(_tt_get_obj_access_args *,
                                                    const _Tt_db_client_ptr &);
extern _tt_is_file_in_db_results *_tt_is_file_in_db_1(_tt_is_file_in_db_args *,
                                                      const _Tt_db_client_ptr &);
extern _tt_is_obj_in_db_results *_tt_is_obj_in_db_1(_tt_is_obj_in_db_args *,
                                                    const _Tt_db_client_ptr &);
extern _tt_db_results *_tt_queue_message_1(_tt_queue_msg_args *, const _Tt_db_client_ptr &);
extern _tt_dequeue_msgs_results *_tt_dequeue_messages_1(_tt_dequeue_msgs_args *, const _Tt_db_client_ptr &);
extern _tt_get_all_sessions_results *_tt_get_all_sessions_1(_tt_get_all_sessions_args *, CLIENT *);
extern _tt_garbage_collect_results *_tt_garbage_collect_1(void *, CLIENT *);
extern _tt_delete_session_results * _tt_delete_session_1(_tt_delete_session_args *, CLIENT *);
#else

extern int *_tt_min_auth_level_1(char**, SVCXPRT*);
extern _Tt_isam_results *_tt_isaddindex_1(_Tt_isaddindex_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_isbuild_1(_Tt_isbuild_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_isclose_1(int*, SVCXPRT*);
extern _Tt_iscntl_results *_tt_iscntl_1(_Tt_iscntl_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_isdelrec_1(_Tt_isdelrec_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_iserase_1(char**, SVCXPRT*);
extern _Tt_isam_results *_tt_isopen_1(_Tt_isopen_args*, SVCXPRT*);
extern _Tt_isread_results *_tt_isread_1(_Tt_isread_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_isrewrec_1(_Tt_isrewrec_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_isstart_1(_Tt_isstart_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_iswrite_1(_Tt_iswrite_args*, SVCXPRT*);
extern _Tt_test_and_set_results *_tt_test_and_set_1(_Tt_test_and_set_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_transaction_1(_Tt_transaction_args*, SVCXPRT*);
extern char **_tt_mfs_1(char**, SVCXPRT*);
extern _Tt_oidaccess_results *_tt_getoidaccess_1(_Tt_oidaccess_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_setoiduser_1(_Tt_oidaccess_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_setoidgroup_1(_Tt_oidaccess_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_setoidmode_1(_Tt_oidaccess_args*, SVCXPRT*);
extern _Tt_spec_props *_tt_readspec_1(_Tt_spec_props*, SVCXPRT*);
extern _Tt_isam_results *_tt_writespec_1(_Tt_spec_props*, SVCXPRT*);
extern _Tt_isam_results *_tt_addsession_1(_Tt_session_args*, SVCXPRT*);
extern _Tt_isam_results *_tt_delsession_1(_Tt_session_args*, SVCXPRT*);
extern _Tt_spec_props *_tt_gettype_1(_Tt_spec_props*, SVCXPRT*);

extern _tt_auth_level_results *_tt_get_min_auth_level_1(void *,
							SVCXPRT*);
extern _tt_file_partition_results *_tt_get_file_partition_1(char **,
							    SVCXPRT*);
extern _tt_db_cache_results *_tt_create_file_1(_tt_create_file_args *,
					       SVCXPRT*);
extern _tt_db_cache_results *_tt_create_obj_1(_tt_create_obj_args *,
					      SVCXPRT*);
extern _tt_db_results *_tt_remove_file_1(_tt_remove_file_args *,
					 SVCXPRT*);
extern _tt_db_results *_tt_remove_obj_1(_tt_remove_obj_args *,
					SVCXPRT*);
extern _tt_db_results *_tt_move_file_1(_tt_move_file_args *,
				       SVCXPRT*);
extern _tt_db_cache_results *_tt_set_file_props_1(_tt_set_file_props_args *,
						  SVCXPRT*);
extern _tt_db_cache_results *_tt_set_file_prop_1(_tt_set_file_prop_args *,
						 SVCXPRT*);
extern _tt_db_cache_results *_tt_add_file_prop_1(_tt_add_file_prop_args *,
						 SVCXPRT*);
extern _tt_db_cache_results *_tt_delete_file_prop_1(_tt_del_file_prop_args *,
						    SVCXPRT*);
extern _tt_file_prop_results *_tt_get_file_prop_1(_tt_get_file_prop_args *,
						  SVCXPRT*);
extern _tt_file_props_results *_tt_get_file_props_1(_tt_get_file_props_args *,
						    SVCXPRT*);
extern _tt_file_objs_results *_tt_get_file_objs_1(_tt_get_file_objs_args *,
						  SVCXPRT*);
extern _tt_db_results *_tt_set_file_access_1(_tt_set_file_access_args *,
					     SVCXPRT*);
extern _tt_file_access_results *
       _tt_get_file_access_1(_tt_get_file_access_args *, SVCXPRT*);
extern _tt_obj_props_results *_tt_set_obj_props_1(_tt_set_obj_props_args *,
						  SVCXPRT*);
extern _tt_obj_props_results *_tt_set_obj_prop_1(_tt_set_obj_prop_args *,
						 SVCXPRT*);
extern _tt_obj_props_results *_tt_add_obj_prop_1(_tt_add_obj_prop_args *,
						 SVCXPRT*);
extern _tt_obj_props_results *_tt_delete_obj_prop_1(_tt_del_obj_prop_args *,
						    SVCXPRT*);
extern _tt_obj_prop_results *_tt_get_obj_prop_1(_tt_get_obj_prop_args *,
						SVCXPRT*);
extern _tt_obj_props_results *_tt_get_obj_props_1(_tt_get_obj_props_args *,
						  SVCXPRT*);
extern _tt_db_results *_tt_set_obj_type_1(_tt_set_obj_type_args *,
					  SVCXPRT*);
extern _tt_obj_type_results *_tt_get_obj_type_1(_tt_get_obj_type_args *,
						SVCXPRT*);
extern _tt_db_results *_tt_set_obj_file_1(_tt_set_obj_file_args *,
					  SVCXPRT *);
extern _tt_obj_file_results *_tt_get_obj_file_1(_tt_get_obj_file_args *,
						SVCXPRT*);
extern _tt_db_results *_tt_set_obj_access_1(_tt_set_obj_access_args *,
					    SVCXPRT*);
extern _tt_obj_access_results *_tt_get_obj_access_1(_tt_get_obj_access_args *,
						    SVCXPRT*);
extern _tt_is_file_in_db_results *_tt_is_file_in_db_1(_tt_is_file_in_db_args *,
						      SVCXPRT *);
extern _tt_is_obj_in_db_results *_tt_is_obj_in_db_1(_tt_is_obj_in_db_args *,
						    SVCXPRT*);
extern _tt_db_results *_tt_queue_message_1(_tt_queue_msg_args *,
					   SVCXPRT*);
extern _tt_dequeue_msgs_results *
       _tt_dequeue_messages_1(_tt_dequeue_msgs_args *, SVCXPRT*);

extern _tt_file_netfile_results * _tt_file_netfile_1(_tt_file_netfile_args *,
						SVCXPRT*);
extern _tt_file_netfile_results * _tt_netfile_file_1(_tt_file_netfile_args *,
						SVCXPRT*);


extern _tt_get_all_sessions_results *
_tt_get_all_sessions_1(_tt_get_all_sessions_args *    /*NOTUSED*/,
		       SVCXPRT 			 * /*NOTUSED*/);

extern _tt_delete_session_results *
_tt_delete_session_1(_tt_delete_session_args *    /*NOTUSED*/,
		     SVCXPRT 			 * /*NOTUSED*/);

extern _tt_garbage_collect_results *
_tt_garbage_collect_1(void    * /*NOTUSED*/,
		      SVCXPRT * /*NOTUSED*/);

extern const char *_TT_LOG_FILE;

#endif /* _TT_DBCLIENT_SIDE */
#endif /* _DB_SERVER_H */
