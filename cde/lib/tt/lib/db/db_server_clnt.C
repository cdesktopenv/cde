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
//%%  $XConsortium: db_server_clnt.C /main/5 1995/11/28 19:25:18 cde-sun $ 			 				
/*
 * @(#)db_server_clnt.C	1.24 95/01/18
 *
 * Tool Talk Database Server - db_server_clnt.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * This file contains the client side interface procedure for the db server.
 */

#include <rpc/rpc.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include "tt_options.h"
#include "db/db_server.h"
#include "db/tt_db_results.h"

// DM_ERROR is a fossil from the old v1.0 database server code.
// As far as I can tell it just needs to not be zero (i.e. not TT_OK).
// rfm 8/26/94

#define DM_ERROR 1

/* Default timeout can be changed using clnt_control() */
static const struct timeval TIMEOUT = { 120, 0 };
static _Tt_isam_results res;
static clnt_stat rpc_result;

int *
_tt_min_auth_level_1(char **argp, CLIENT *clnt, clnt_stat *result)
{
	static int res;

	memset((char *)&res, 0, sizeof(res));
	*result = clnt_call(clnt, _TT_MIN_AUTH_LEVEL,
			    (xdrproc_t)xdr_wrapstring, (caddr_t) argp,
			    (xdrproc_t)xdr_int, (caddr_t) &res,
			    TIMEOUT);
	switch (*result) {
	      case RPC_SUCCESS:
		return (&res);
	      case RPC_TIMEDOUT:
		// caller handles timeouts specially.
		return (NULL);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_min_auth_level_1");
 */		
		return (NULL);
	}
}

_Tt_isam_results *
_tt_isaddindex_1(_Tt_isaddindex_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISADDINDEX,
				     (xdrproc_t)xdr_Tt_isaddindex_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isaddindex_1:");
 */
		return (NULL);
	}

}

_Tt_isam_results *
_tt_isbuild_1(_Tt_isbuild_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISBUILD,
				     (xdrproc_t)xdr_Tt_isbuild_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isbuild_1:");
 */
		return (NULL);
	}

}

_Tt_isam_results *
_tt_isclose_1(int *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISCLOSE,
				     (xdrproc_t)xdr_int, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isclose_1:");
 */
		return (NULL);
	}
}

_Tt_iscntl_results *
_tt_iscntl_1(_Tt_iscntl_args *argp, CLIENT *clnt)
{
	static _Tt_iscntl_results res;

	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISCNTL,
				     (xdrproc_t)xdr_Tt_iscntl_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_iscntl_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_iscntl_1:");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_isdelrec_1(_Tt_isdelrec_args *argp,	CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISDELREC,
				     (xdrproc_t)xdr_Tt_isdelrec_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isdelrec_1:");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_iserase_1(char **argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISERASE,
				     (xdrproc_t)xdr_wrapstring, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_iserase_1:");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_isopen_1(_Tt_isopen_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISOPEN,
				     (xdrproc_t)xdr_Tt_isopen_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isopen_1:");
 */
		return (NULL);
	}
}


_Tt_isread_results *
_tt_isread_1(_Tt_isread_args *argp, CLIENT *clnt)
{
	static _Tt_isread_results res;

	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISREAD,
				     (xdrproc_t)xdr_Tt_isread_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isread_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isread_1");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_isrewrec_1(_Tt_isrewrec_args *argp,	CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISREWREC,
				     (xdrproc_t)xdr_Tt_isrewrec_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isrewrec_1");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_isstart_1(_Tt_isstart_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISSTART,
				     (xdrproc_t)xdr_Tt_isstart_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_isstart_1");
 */
		return (NULL);
	}
}

_Tt_isam_results *
_tt_iswrite_1(_Tt_iswrite_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ISWRITE,
				     (xdrproc_t)xdr_Tt_iswrite_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_iswrite_1:");
 */
		return (NULL);
	}
}

_Tt_test_and_set_results *
_tt_test_and_set_1(_Tt_test_and_set_args *argp, CLIENT *clnt)
{
	static _Tt_test_and_set_results res;

	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_TEST_AND_SET,
				     (xdrproc_t)xdr_Tt_test_and_set_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_test_and_set_results,
				     (caddr_t) &res, TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_test_and_set_1:");
 */
		return (NULL);
	}
	
}

_Tt_isam_results *
_tt_transaction_1(_Tt_transaction_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_TRANSACTION,
				     (xdrproc_t)xdr_Tt_transaction_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_transaction_1:");
 */
		return (NULL);
	}
}


/*
 * _tt_mfs_1()
 *	If the (char *) pointed at by the return value is non-null,
 *	the caller has to free it via
 *	xdr_free((xdrproc_t)xdr_wrapstring, *<return_val>).
 */
char **
_tt_mfs_1(char **argp, 	CLIENT *clnt)
{
	static char *res;
	res = 0;
	clnt_stat result = clnt_call(clnt, _TT_MFS,
				     (xdrproc_t)xdr_wrapstring, (caddr_t) argp,
				     (xdrproc_t)xdr_wrapstring, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_mfs_1:");
 */
		return (NULL);
	}
}

_Tt_oidaccess_results *
_tt_getoidaccess_1(_Tt_oidaccess_args *argp, CLIENT *clnt)
{
	static _Tt_oidaccess_results res;

	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_GETOIDACCESS,
				     (xdrproc_t)xdr_Tt_oidaccess_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_oidaccess_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_getoiduser:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		res.uid = 65534; // nobody.  Can't find a system define..
		return (&res);
	}
}

_Tt_isam_results *
_tt_setoiduser_1(_Tt_oidaccess_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_SETOIDUSER,
				     (xdrproc_t)xdr_Tt_oidaccess_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_setoiduser:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}

_Tt_isam_results *
_tt_setoidgroup_1(_Tt_oidaccess_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_SETOIDGROUP,
				     (xdrproc_t)xdr_Tt_oidaccess_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_setoidgroup:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}

_Tt_isam_results *
_tt_setoidmode_1(_Tt_oidaccess_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_SETOIDMODE,
				     (xdrproc_t)xdr_Tt_oidaccess_args,
				     (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results,
				     (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_setoidmode:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}

_Tt_isam_results *
_tt_addsession_1(_Tt_session_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_ADDSESSION,
				     (xdrproc_t)xdr_Tt_session_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_addsession:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}

_Tt_isam_results *
_tt_delsession_1(_Tt_session_args *argp, CLIENT *clnt)
{
	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_DELSESSION,
				     (xdrproc_t)xdr_Tt_session_args, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_isam_results, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_delsession:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}


_Tt_spec_props *
_tt_gettype_1(_Tt_spec_props *argp, CLIENT *clnt)
{
	static _Tt_spec_props res;

	memset((char *)&res, 0, sizeof(res));
	clnt_stat result = clnt_call(clnt, _TT_GETTYPE,
				     (xdrproc_t)xdr_Tt_spec_props, (caddr_t) argp,
				     (xdrproc_t)xdr_Tt_spec_props, (caddr_t) &res,
				     TIMEOUT);
	switch (result) {
	      case RPC_SUCCESS:
		return (&res);
	      default:
/* 
 * 		clnt_perror(clnt,"_tt_gettype:");
 */
		res.result = DM_ERROR;
		res.iserrno = 0;
		return (&res);
	}
}

clnt_stat _tt_get_rpc_result()
{
	return rpc_result;
}

_tt_auth_level_results *
_tt_get_min_auth_level_1(void *argp, CLIENT *clnt)
{
	static _tt_auth_level_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_MIN_AUTH_LEVEL,
				     (xdrproc_t) xdr_void, (caddr_t) argp,
				     (xdrproc_t) xdr_tt_auth_level_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		rpc_result = result;
		return (NULL);
	}
	return (&res);
}

_tt_file_partition_results *
_tt_get_file_partition_1(char **argp, CLIENT *clnt)
{
	static _tt_file_partition_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_FILE_PARTITION,
				     (xdrproc_t) xdr_wrapstring,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_partition_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_create_file_1(_tt_create_file_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_CREATE_FILE,
				     (xdrproc_t) xdr_tt_create_file_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_create_obj_1(_tt_create_obj_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_CREATE_OBJ,
				     (xdrproc_t) xdr_tt_create_obj_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_remove_file_1(_tt_remove_file_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_REMOVE_FILE,
				     (xdrproc_t) xdr_tt_remove_file_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_remove_obj_1(_tt_remove_obj_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_REMOVE_OBJ,
				     (xdrproc_t) xdr_tt_remove_obj_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_move_file_1(_tt_move_file_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_REMOVE_FILE,
				     (xdrproc_t) xdr_tt_move_file_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_set_file_props_1(_tt_set_file_props_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_FILE_PROPS,
				     (xdrproc_t) xdr_tt_set_file_props_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_set_file_prop_1(_tt_set_file_prop_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_FILE_PROP,
				     (xdrproc_t) xdr_tt_set_file_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_add_file_prop_1(_tt_add_file_prop_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_ADD_FILE_PROP,
				     (xdrproc_t) xdr_tt_add_file_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_cache_results *
_tt_delete_file_prop_1(_tt_del_file_prop_args *argp, CLIENT *clnt)
{
	static _tt_db_cache_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_DELETE_FILE_PROP,
				     (xdrproc_t) xdr_tt_del_file_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_cache_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_file_prop_results *
_tt_get_file_prop_1(_tt_get_file_prop_args *argp, CLIENT *clnt)
{
	static _tt_file_prop_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_FILE_PROP,
				     (xdrproc_t) xdr_tt_get_file_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_prop_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_file_props_results *
_tt_get_file_props_1(_tt_get_file_props_args *argp, CLIENT *clnt)
{
	static _tt_file_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_FILE_PROPS,
				     (xdrproc_t) xdr_tt_get_file_props_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_file_objs_results *
_tt_get_file_objs_1(_tt_get_file_objs_args *argp, CLIENT *clnt)
{
	static _tt_file_objs_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_FILE_OBJS,
				     (xdrproc_t) xdr_tt_get_file_objs_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_objs_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_set_file_access_1(_tt_set_file_access_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_FILE_ACCESS,
				     (xdrproc_t) xdr_tt_set_file_access_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_file_access_results *
_tt_get_file_access_1(_tt_get_file_access_args *argp, CLIENT *clnt)
{
	static _tt_file_access_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_FILE_ACCESS,
				     (xdrproc_t) xdr_tt_get_file_access_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_access_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_props_results *
_tt_set_obj_props_1(_tt_set_obj_props_args *argp, CLIENT *clnt)
{
	static _tt_obj_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_OBJ_PROPS,
				     (xdrproc_t) xdr_tt_set_obj_props_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_props_results *
_tt_set_obj_prop_1(_tt_set_obj_prop_args *argp, CLIENT *clnt)
{
	static _tt_obj_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_OBJ_PROP,
				     (xdrproc_t) xdr_tt_set_obj_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_props_results *
_tt_add_obj_prop_1(_tt_add_obj_prop_args *argp, CLIENT *clnt)
{
	static _tt_obj_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_ADD_OBJ_PROP,
				     (xdrproc_t) xdr_tt_add_obj_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_props_results *
_tt_delete_obj_prop_1(_tt_del_obj_prop_args *argp, CLIENT *clnt)
{
	static _tt_obj_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_DELETE_OBJ_PROP,
				     (xdrproc_t) xdr_tt_del_obj_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_prop_results *
_tt_get_obj_prop_1(_tt_get_obj_prop_args *argp, CLIENT *clnt)
{
	static _tt_obj_prop_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_OBJ_PROP,
				     (xdrproc_t) xdr_tt_get_obj_prop_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_prop_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_props_results *
_tt_get_obj_props_1(_tt_get_obj_props_args *argp, CLIENT *clnt)
{
	static _tt_obj_props_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_OBJ_PROPS,
				     (xdrproc_t) xdr_tt_get_obj_props_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_props_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_set_obj_type_1(_tt_set_obj_type_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_OBJ_TYPE,
				     (xdrproc_t) xdr_tt_set_obj_type_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_type_results *
_tt_get_obj_type_1(_tt_get_obj_type_args *argp, CLIENT *clnt)
{
	static _tt_obj_type_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_OBJ_TYPE,
				     (xdrproc_t) xdr_wrapstring,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_type_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_set_obj_file_1(_tt_set_obj_file_args *argp, CLIENT *clnt)
{
        static _tt_db_results res;
 
        memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_OBJ_FILE,
				     (xdrproc_t) xdr_tt_set_obj_file_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
                return (NULL);
        }
        return (&res);
}
 
_tt_obj_file_results *
_tt_get_obj_file_1(_tt_get_obj_file_args *argp, CLIENT *clnt)
{
	static _tt_obj_file_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_OBJ_FILE,
				     (xdrproc_t) xdr_tt_get_obj_file_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_file_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_set_obj_access_1(_tt_set_obj_access_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_SET_OBJ_ACCESS,
				     (xdrproc_t) xdr_tt_set_obj_access_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_obj_access_results *
_tt_get_obj_access_1(_tt_get_obj_access_args *argp, CLIENT *clnt)
{
	static _tt_obj_access_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_GET_OBJ_ACCESS,
				     (xdrproc_t) xdr_tt_get_obj_access_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_obj_access_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_is_file_in_db_results *
_tt_is_file_in_db_1(_tt_is_file_in_db_args *argp, CLIENT *clnt)
{
	static _tt_is_file_in_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_IS_FILE_IN_DB,
				     (xdrproc_t) xdr_tt_is_file_in_db_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_is_file_in_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_is_obj_in_db_results *
_tt_is_obj_in_db_1(_tt_is_obj_in_db_args *argp, CLIENT *clnt)
{
	static _tt_is_obj_in_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_IS_OBJ_IN_DB,
				     (xdrproc_t) xdr_tt_is_obj_in_db_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_is_obj_in_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_db_results *
_tt_queue_message_1(_tt_queue_msg_args *argp, CLIENT *clnt)
{
	static _tt_db_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_QUEUE_MESSAGE,
				     (xdrproc_t) xdr_tt_queue_msg_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_db_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}

_tt_dequeue_msgs_results *
_tt_dequeue_messages_1(_tt_dequeue_msgs_args *argp, CLIENT *clnt)
{
	static _tt_dequeue_msgs_results res;

	memset((void *)&res, '\0', sizeof(res));
	clnt_stat result = clnt_call(clnt, TT_DEQUEUE_MESSAGES,
				     (xdrproc_t) xdr_tt_dequeue_msgs_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_dequeue_msgs_results,
				     (caddr_t) &res, TIMEOUT);
	if (result != RPC_SUCCESS) {
		return (NULL);
	}
	return (&res);
}


// used for API call tt_host_file_netfile()
_tt_file_netfile_results *
_tt_file_netfile_1(_tt_file_netfile_args *argp, CLIENT *clnt)
{
        static _tt_file_netfile_results res;
 
        memset((void *)&res, '\0', sizeof(res));
        clnt_stat result = clnt_call(clnt, TTDB_FILE_NETFILE,
				     (xdrproc_t) xdr_tt_file_netfile_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_netfile_results,
				     (caddr_t) &res,
				     TIMEOUT);
        if (result == RPC_PROCUNAVAIL) {
                res.results = TT_DB_ERR_RPC_UNIMP;
        } else {
                if (result != RPC_SUCCESS) {
                        return (NULL);
                }
        }

        return (&res);
}


// used for API call tt_host_netfile_file()
_tt_file_netfile_results *
_tt_netfile_file_1(_tt_file_netfile_args *argp, CLIENT *clnt)
{
        static _tt_file_netfile_results res;
 
        memset((void *)&res, '\0', sizeof(res));
        clnt_stat result = clnt_call(clnt, TTDB_NETFILE_FILE,
				     (xdrproc_t) xdr_tt_file_netfile_args,
				     (caddr_t) argp,
				     (xdrproc_t) xdr_tt_file_netfile_results,
				     (caddr_t) &res,
				     TIMEOUT);
        if (result == RPC_PROCUNAVAIL) {
                res.results = TT_DB_ERR_RPC_UNIMP;
        } else {
                if (result != RPC_SUCCESS) {
                        return (NULL);
                }
        }

        return (&res);
}

_tt_get_all_sessions_results	*
_tt_get_all_sessions_1(_tt_get_all_sessions_args *argp, CLIENT * clnt)
{
    static _tt_get_all_sessions_results	res;

    clnt_call(clnt, TT_GET_ALL_SESSIONS,
				 (xdrproc_t) xdr_tt_get_all_sessions_args,
				 (caddr_t) argp,
				 (xdrproc_t) xdr_tt_get_all_sessions_results,
				 (caddr_t) &res,
				 TIMEOUT);

    return(&res);
}

_tt_garbage_collect_results	*
_tt_garbage_collect_1(void * /*NOTUSED*/, CLIENT *clnt)
{
	static	_tt_garbage_collect_results	res;

	clnt_call(clnt, TT_GARBAGE_COLLECT,
				     (xdrproc_t) xdr_void, (caddr_t) NULL,
				     (xdrproc_t) xdr_tt_garbage_collect_results,
				     (caddr_t) &res,
				     TIMEOUT);
	
	return(&res);
}

_tt_delete_session_results *
_tt_delete_session_1(_tt_delete_session_args * args, CLIENT *clnt)
{
	static	_tt_delete_session_results	res;

	clnt_call(clnt, TT_DELETE_SESSION,
				     (xdrproc_t) xdr_tt_delete_session_args,
				     (caddr_t) args,
				     (xdrproc_t) xdr_tt_delete_session_results,
				     (caddr_t) &res,
				     TIMEOUT);
	
	return(&res);
}

