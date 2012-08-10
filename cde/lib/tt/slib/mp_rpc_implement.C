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
//%%  $TOG: mp_rpc_implement.C /main/9 1999/10/14 18:42:58 mgreess $ 			 				
/*
 * mp_rpc_implement.C 1.40 29 Jul 1993
 *
 * mp_rpc_implement.cc -- implementation of RPC functions
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 * 
 * This file implements the RPC wrapper functions for each RPC
 * procedure number. The responsibility of each wrapper is to decode and
 * encode arguments and results and invoke the right methods
 * corresponding to each activity.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#if defined(linux) || defined(CSRG_BASED)
/*# include <g++/minmax.h>*/
#else
# include <macros.h>
#endif
#include "tt_options.h"
#include "util/tt_xdr_version.h"
#include <rpc/rpc.h>
#include <limits.h>
#include "mp/mp_global.h"
#include "mp_s_file.h"
#include "mp_s_file_utils.h"
#include "mp/mp_mp.h"
#include "mp_s_mp.h"
#include "mp_s_message.h"
#include "mp_otype.h"
#include "mp_signature.h"
#include "mp/mp_arg.h"
#include "mp_s_msg_context.h"
#include "mp_s_pattern.h"
#include "mp_s_procid.h"
#include "mp/mp_rpc_fns.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "mp/mp_xdr_functions.h"
#include "mp_s_xdr_functions.h"
#include "util/tt_trace.h"
#include "mp_typedb.h"

#if defined(OPT_SECURE_RPC)

static	uid_t	req_auth_uid;
static	gid_t	req_auth_gid;
static	short	req_auth_gidlen;

// authdes_getucred() in 4.x is declared to take int* instead of gid_t*
#if defined(OPT_BUG_SUNOS_4)
static	int	req_auth_gids[10];
#else
static	gid_t	req_auth_gids[NGROUPS_MAX];
#endif /* OPT_BUG_SUNOS_4 */

#endif /* OPT_SECURE_RPC */

#if defined(OPT_BUG_SUNOS_4)
#	define RPC_ARG_T	char *
#else
#	define RPC_ARG_T	caddr_t
#endif /* OPT_BUG_SUNOS_4 */

#if defined(OPT_BUG_SUNOS_5)
	typedef (*Hack_getucred)(const struct authdes_cred *,
				 uid_t *,
				 gid_t *,
				 short *,
				 gid_t *);
#endif /* OPT_BUG_SUNOS_5 */

void _tt_rpc_nullproc(SVCXPRT *); 
void _tt_rpc_dispatch(SVCXPRT *);
void _tt_rpc_dispatch_with_context(SVCXPRT *);
void _tt_rpc_update_msg(SVCXPRT *);
void _tt_rpc_join_file(SVCXPRT *);
void _tt_rpc_quit_file(SVCXPRT *);
void _tt_rpc_join_session(SVCXPRT *);
void _tt_rpc_quit_session(SVCXPRT *);
void _tt_rpc_join_context(SVCXPRT *);
void _tt_rpc_quit_context(SVCXPRT *);
void _tt_rpc_next_message(SVCXPRT *);
void _tt_rpc_add_pattern(SVCXPRT *);
void _tt_rpc_add_pattern_with_context(SVCXPRT *);
void _tt_rpc_del_pattern(SVCXPRT *);
void _tt_rpc_declare_ptype(SVCXPRT *);
void _tt_rpc_set_fd_channel(SVCXPRT *);
void _tt_rpc_set_prop(SVCXPRT *);
void _tt_rpc_add_prop(SVCXPRT *);
void _tt_rpc_get_prop(SVCXPRT *);
void _tt_rpc_prop_count(SVCXPRT *);
void _tt_rpc_prop_name(SVCXPRT *);
void _tt_rpc_prop_names_count(SVCXPRT *);
void _tt_rpc_alloc_procid_key(SVCXPRT *);
void _tt_rpc_close_procid(SVCXPRT *);
void _tt_rpc_otype_deriveds_count(SVCXPRT *);
void _tt_rpc_otype_derived(SVCXPRT *);
void _tt_rpc_otype_base(SVCXPRT *);
void _tt_rpc_otype_is_derived(SVCXPRT *);
void _tt_rpc_otype_osig_count(SVCXPRT *);
void _tt_rpc_otype_hsig_count(SVCXPRT *);
void _tt_rpc_otype_osig_op(SVCXPRT *);
void _tt_rpc_otype_hsig_op(SVCXPRT *);
void _tt_rpc_otype_osig_args_count(SVCXPRT *);
void _tt_rpc_otype_hsig_args_count(SVCXPRT *);
void _tt_rpc_otype_osig_arg_mode(SVCXPRT *);
void _tt_rpc_otype_hsig_arg_mode(SVCXPRT *);
void _tt_rpc_otype_osig_arg_type(SVCXPRT *);
void _tt_rpc_otype_hsig_arg_type(SVCXPRT *);
void _tt_rpc_hdispatch(SVCXPRT *);
void _tt_rpc_hupdate_msg(SVCXPRT *);
void _tt_rpc_get_omap(SVCXPRT *);
void _tt_rpc_add_omap(SVCXPRT *);
void _tt_rpc_dispatch_2(SVCXPRT *);
void _tt_rpc_dispatch_2_with_context(SVCXPRT *);
void _tt_rpc_update_msg_2(SVCXPRT *);
void _tt_rpc_msgread_2(SVCXPRT *);
void _tt_rpc_vrfy_session(SVCXPRT *);
void _tt_rpc_dispatch_on_exit(SVCXPRT *);
void _tt_rpc_undeclare_ptype(SVCXPRT *);
void _tt_rpc_exists_ptype(SVCXPRT *);
void _tt_rpc_unblock_ptype(SVCXPRT *);
void _tt_rpc_load_types(SVCXPRT *);


typedef	void (*_Tt_rpc_stub)(SVCXPRT *);
/* 
 * Dispatch table for rpc procedures. Note that the position of the
 * dispatch functions must correspond to the _Tt_rpc_procedure number
 * enum. 
 */
const _Tt_rpc_stub	_tt_rpc_dispatch_table[TT_RPC_LAST] = {
	0, 				/* 0 -	TT_RPC_NULLPROC */
	_tt_rpc_dispatch,		/* 1 -	TT_RPC_DISPATCH */
	_tt_rpc_update_msg,		/* 2 -	TT_RPC_UPDATE_MSG */
	_tt_rpc_join_file,		/* 3 -	TT_RPC_JOIN_FILE */
	_tt_rpc_quit_file,		/* 4 -	TT_RPC_QUIT_FILE */
	_tt_rpc_join_session,		/* 5 - 	TT_RPC_JOIN_SESSION */
	_tt_rpc_quit_session,		/* 6 -	TT_RPC_QUIT_SESSION */
	_tt_rpc_next_message,		/* 7 -	TT_RPC_NEXT_MESSAGE */
	_tt_rpc_add_pattern,		/* 8 -	TT_RPC_ADD_PATTERN */
	_tt_rpc_del_pattern,		/* 9 -	TT_RPC_DEL_PATTERN */
	_tt_rpc_declare_ptype,		/* 10 -	TT_RPC_DECLARE_PTYPE */
	_tt_rpc_set_fd_channel,		/* 11 -	TT_RPC_SET_FD_CHANNEL */
	_tt_rpc_set_prop,		/* 12 -	TT_RPC_SET_PROP */
	_tt_rpc_add_prop,		/* 13 - TT_RPC_ADD_PROP */
	_tt_rpc_get_prop,		/* 14 -	TT_RPC_GET_PROP */
	_tt_rpc_prop_count,		/* 15 -	TT_RPC_PROP_COUNT */
	_tt_rpc_prop_name,		/* 16 -	TT_RPC_PROP_NAME */
	_tt_rpc_prop_names_count,	/* 17 - TT_RPC_PROP_NAMES_COUNT */
	_tt_rpc_alloc_procid_key,	/* 18 - TT_RPC_ALLOC_PROCID_KEY */
	_tt_rpc_close_procid,		/* 19 -	TT_RPC_CLOSE_PROCID */
	_tt_rpc_otype_deriveds_count,	/* 20 - TT_RPC_OTYPE_DERIVEDS_COUNT */
	_tt_rpc_otype_derived,		/* 21 - TT_RPC_OTYPE_DERIVED */
	_tt_rpc_otype_base,		/* 22 - TT_RPC_OTYPE_BASE */
	_tt_rpc_otype_is_derived,	/* 23 - TT_RPC_OTYPE_IS_DERIVED */
	_tt_rpc_otype_osig_count,	/* 24 - TT_RPC_OTYPE_OSIG_COUNT */
	_tt_rpc_otype_hsig_count,	/* 25 - TT_RPC_OTYPE_HSIG_COUNT */
	_tt_rpc_otype_osig_op,		/* 26 - TT_RPC_OTYPE_OSIG_OP */
	_tt_rpc_otype_hsig_op,		/* 27 - TT_RPC_OTYPE_HSIG_OP */
	_tt_rpc_otype_osig_args_count,	/* 28 - TT_RPC_OTYPE_OSIG_ARGS_COUNT */
	_tt_rpc_otype_hsig_args_count,	/* 29 - TT_RPC_OTYPE_HSIG_ARGS_COUNT */
	_tt_rpc_otype_osig_arg_mode,	/* 30 - TT_RPC_OTYPE_OSIG_ARG_MODE */
	_tt_rpc_otype_hsig_arg_mode,	/* 31 - TT_RPC_OTYPE_HSIG_ARG_MODE */
	_tt_rpc_otype_osig_arg_type,	/* 32 - TT_RPC_OTYPE_OSIG_ARG_TYPE */
	_tt_rpc_otype_hsig_arg_type,	/* 33 - TT_RPC_OTYPE_HSIG_ARG_TYPE */
	_tt_rpc_hdispatch,		/* 34 - TT_RPC_HDISPATCH */
	_tt_rpc_hupdate_msg,		/* 35 - TT_RPC_HUPDATE_MSG */
	_tt_rpc_dispatch_2,		/* 36 - TT_RPC_DISPATCH_2 */
	_tt_rpc_update_msg_2,		/* 37 - TT_RPC_UPDATE_MSG_2 */
	_tt_rpc_msgread_2,		/* 38 - TT_RPC_MSGREAD_2 */
	_tt_rpc_dispatch_on_exit,	/* 39 - TT_RPC_DISPATCH_ON_EXIT */
	_tt_rpc_undeclare_ptype,	/* 40 -	TT_RPC_UNDECLARE_PTYPE */
	_tt_rpc_exists_ptype,		/* 41 -	TT_RPC_EXISTS_PTYPE */
	_tt_rpc_unblock_ptype,		/* 42 -	TT_RPC_UNBLOCK_PTYPE */
	_tt_rpc_join_context,		/* 43 -	TT_RPC_JOIN_CONTEXT */
	_tt_rpc_quit_context,		/* 44 -	TT_RPC_QUIT_CONTEXT */
	_tt_rpc_dispatch_with_context,	/* 45 - TT_RPC_DISPATCH_WITH_CONTEXT */
	_tt_rpc_dispatch_2_with_context,/* 46 - TT_RPC_DISPATCH_2_WITH_CONTEXT */
	_tt_rpc_add_pattern_with_context,/* 47 - TT_RPC_ADD_PATTERN_WITH_CONTEXT */
	_tt_rpc_load_types		/* 48 - TT_RPC_LOAD_TYPES */
};


/* 
 * Dispatch function invoked whenever an RPC request comes in. 
 */
void
_tt_service_rpc(svc_req *rqstp, SVCXPRT *transp)
{
	_Tt_auth_level		session_auth;
#if defined(OPT_SECURE_RPC)
	authdes_cred		*des_cred;
#endif

        // Increment the counter for the number of RPC calls
        // handled during the life of this process. 
        _tt_global->event_counter++; 
 
	//
	// authenticate request
	//
	session_auth = _tt_s_mp->initial_s_session->auth_level();
	switch (rqstp->rq_cred.oa_flavor) {
	    case AUTH_UNIX:
	    case AUTH_SHORT:
		if (session_auth == _TT_AUTH_DES ||
		    session_auth == _TT_AUTH_ICEAUTH) {
			svcerr_auth(transp, AUTH_TOOWEAK);
			return;
		}
		else if (session_auth == _TT_AUTH_UNIX) {
			if (_tt_s_mp->unix_cred_chk_flag) {

				// "authunix_parms" is now deprecated in
				// favor of "authsys_parms", but
				// "authsys_parms" does not exist on
				// all platforms yet.
				
				authunix_parms* sys_cred =
					(authunix_parms *) rqstp->rq_clntcred;
				if (sys_cred->aup_uid != geteuid()) {
					svcerr_auth(transp, AUTH_BADCRED);
					return;
				}
			}
			// else, we are not doing auth checking
		}
					
		break;
#if defined(OPT_SECURE_RPC)
	    case AUTH_DES:
		if (session_auth != _TT_AUTH_DES) {
			svcerr_auth(transp, AUTH_TOOWEAK);
			return;
		}
		des_cred = (authdes_cred *)rqstp->rq_clntcred;
#if !defined(OPT_BUG_SUNOS_5)
		if (! authdes_getucred(des_cred,
				       &req_auth_uid,
				       &req_auth_gid,
				       &req_auth_gidlen,
				       req_auth_gids)) {
			svcerr_auth(transp, AUTH_BADCRED);
		}
#else 		
		Hack_getucred hack_getucred_p;
			
		hack_getucred_p = (Hack_getucred)authdes_getucred;
		if (! (*hack_getucred_p)(des_cred,
					 &req_auth_uid,
					 &req_auth_gid,
					 &req_auth_gidlen,
					 req_auth_gids)) {
			svcerr_auth(transp, AUTH_BADCRED);
		}
#endif			/* OPT_BUG_SUNOS_5 */
			
		// Now that we have the credentials, make sure they
		// are the right ones
					
		if (req_auth_uid == geteuid() &&
		    req_auth_gid == getegid()) {
						
			break;	// else fall through to svcerr_weakauth
		}
#endif				/* OPT_SECURE_RPC */
	    default:
		if (session_auth == _TT_AUTH_ICEAUTH) {
		    _Tt_string	clnt_cookie;
	            if (!_tt_svc_getargs(transp,
					     (xdrproc_t) tt_xdr_string,
					     (char *) &clnt_cookie)) {
		        svcerr_auth(transp, AUTH_BADCRED);
		        return;
		    }

		    const _Tt_string	&svr_cookie =
	            	_tt_s_mp->initial_s_session->auth_cookie();
	            if (clnt_cookie != svr_cookie) {
		        svcerr_auth(transp, AUTH_BADCRED);
		        return;
		    }
	        }
	}
	
	//
	// dispatch request to appropiate dispatch function
	//
				
	//
	// Set and reset xdr version on entry and exit.  See tt_xdr_version.h
	//
	_Tt_xdr_version		xvers((int)rqstp->rq_vers);
	
	switch (rqstp->rq_proc) {
	    case NULLPROC:
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, 0);
		break;
	    case TT_RPC_VRFY_SESSION:
		_tt_rpc_vrfy_session(transp);
		break;
	    default:
		if (rqstp->rq_proc >= 1 && rqstp->rq_proc < TT_RPC_LAST) {
			(*_tt_rpc_dispatch_table[rqstp->rq_proc])(transp);
		} else {
			svcerr_noproc(transp);
		}
		break;
	}
}

/* 
 * Called when a sender sends a message to the server.
 * This original version of _tt_rpc_dispatch() is a synchronous RPC
 * call, and it returns a _Tt_dispatch_reply_args, which contains
 * dispatch status and file-scoped queuing info.  The new version,
 * _tt_rpc_dispatch_2(), is asynchronous and thus faster.
 */
void
_tt_rpc_dispatch(SVCXPRT *transp)
{
	_Tt_s_message_ptr		msg;
	_Tt_dispatch_reply_args		args;

	msg = new _Tt_s_message();
	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_xdr_message,
			     (char *)&msg)) {
		svcerr_decode(transp);
		return;
	}

	args.status = TT_OK;
	_Tt_msg_trace trace( *msg, TTDR_MESSAGE_SEND );
	//
	// XXX Turn this off until we do TT_ERR_OVERFLOW right
	//
	//if (_tt_s_mp->active_messages > _tt_s_mp->max_active_messages)
	if (0)
	{
		args.status = TT_ERR_OVERFLOW;
	} else {
		// dispatch the message so that any information regarding
		// scope, and queueing can be relayed back to the sender.
		msg->dispatch( trace );
		msg->qmsg_info(args.qmsg_info);
	}
	if (msg->scope() != TT_SESSION) {
		// if no file-scope patterns exist for this file then
		// return an error to allow the client to remove this
		// session from the file's scope.
		if (! _tt_s_mp->in_file_scope(msg->file())) {
			args.status = TT_ERR_FILE;
		}
	}
	if (!svc_sendreply(transp,
			   (xdrproc_t)tt_xdr_dispatch_reply_args,
			   (RPC_ARG_T)&args)) {
		return;
	}

	if (args.status == TT_OK || args.status == TT_ERR_FILE) {
		// now deliver the message
		msg->deliver_to_observers_and_handlers( trace );
	}
}


/* 
 * Called when a sender sends a contextful message
 * to the server. 
 */
void
_tt_rpc_dispatch_with_context(SVCXPRT *transp)
{
	//
	// In messages, xdr versioning for contexts isn't necessary,
	// since a bitmask controls what fields of a _Tt_message get
	// xdr'd.  This separate RPC call exists only so that
	// context-aware clients can tell whether a server understands
	// contexts.
	//
	_tt_rpc_dispatch(transp);
}


/* 
 * High-performance version of _tt_rpc_dispatch().  No information
 * is relayed back to the calling client.
 */
void
_tt_rpc_dispatch_2(SVCXPRT *transp)
{
	_Tt_s_message_ptr		msg;

	msg = new _Tt_s_message();
	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_xdr_message,
			     (char *)&msg)) {
		svcerr_decode(transp);
		return;
	}
	_Tt_msg_trace trace( *msg, TTDR_MESSAGE_SEND );
	msg->dispatch( trace );
	// XXX holtz 93/09/17 Should we really deliver() if dispatch() fails?
	// Note: this applies to every dispatch() call in this file.
	msg->deliver_to_observers_and_handlers( trace );
}


/* 
 * Called when a sender sends a contextful session-scoped message
 * to the server. 
 */
void
_tt_rpc_dispatch_2_with_context(SVCXPRT *transp)
{
	//
	// In messages, xdr versioning for contexts isn't necessary,
	// since a bitmask controls what fields of a _Tt_message get
	// xdr'd.  This separate RPC call exists only so that
	// context-aware clients can tell whether a server understands
	// contexts.
	//
	_tt_rpc_dispatch_2(transp);
}



/* 
 * Called when a handler or voter updates a message in any way (ie. state change)
 */
void
_tt_update_msg(SVCXPRT *transp, int reply)
{
	_Tt_s_procid_ptr		proc;
	Tt_status			status;

	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_xdr_update_args,
			     (char *)&(_tt_s_mp->update_args))) {
		svcerr_decode(transp);
		return;
	}

	int found = 0;
	_Tt_message_ptr &msg = _tt_s_mp->update_args.message;
	//
	// We have to rummage around in the message to see which
	// procid is talking to us.
	//
	if (! msg->handler().is_null()) {
		//
		// Assume the handler is the updater.
		//
		found = _tt_s_mp->find_proc(msg->handler(), proc, 0);
	} else if (msg->message_class() == TT_OFFER) {
		//
		// Assume the last voter is the updater.
		//
		_Tt_procid_list_ptr proc_list;
		switch (_tt_s_mp->update_args.newstate) {
		    case TT_REJECTED:
			proc_list = msg->rejecters();
			break;
		    case TT_ACCEPTED:
			proc_list = msg->accepters();
			break;
		    case TT_ABSTAINED:
			proc_list = msg->abstainers();
			break;
		}
		if ((! proc_list.is_null()) && (proc_list->count() > 0)) {
			found = _tt_s_mp->find_proc(proc_list->bot(), proc, 0);
		}
	}
	if (found) {
		status = TT_OK;
	} else {
		status = TT_ERR_PROCID;
	}

	if (reply && (! svc_sendreply(transp, (xdrproc_t)xdr_int,
				      (RPC_ARG_T)&status)))
	{
		return;
	}

	if (status != TT_OK) {
		return;
	}
	if (msg->is_observer()) {
		// Clears _handler when observer replies to START_MESSAGE
		// XXX _handler is used to tell us which procid is talking
		// to us, because nothing in the message tells us who
		// is observing it.  Ugly.
		msg->clr_observer_procid();
	}
	proc->update_message(msg, _tt_s_mp->update_args.newstate);
}


/* 
 * Called when a handler updates a message in any way (ie. state change)
 */
void
_tt_rpc_update_msg(SVCXPRT *transp)
{
	_tt_update_msg(transp, 1);
}

/* 
 * Asynchronous, higher-performance version of _tt_rpc_update_msg()
 */
void
_tt_rpc_update_msg_2(SVCXPRT *transp)
{
	_tt_update_msg(transp, 0);
}


/* 
 * Called when a process joins a file.
 */
void
_tt_rpc_join_file(SVCXPRT *transp)
{
	_Tt_file_join_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;
	_Tt_s_file_ptr		fptr;

	if (! _tt_svc_getargs(transp, (xdrproc_t)tt_xdr_file_join_args,
			      (char *)&args)) {
		svcerr_decode(transp);
	}
	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		fptr = new _Tt_s_file(args.path);
		status = fptr->s_join(proc);
	} else {
		status = TT_ERR_PROCID;
	}
	(void)svc_sendreply(transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status);
}


/* 
 * Called when a process quits a file.
 */
void
_tt_rpc_quit_file(SVCXPRT *transp)
{
	_Tt_file_join_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;
	_Tt_s_file_ptr		fptr;

	if (! _tt_svc_getargs(transp, (xdrproc_t)tt_xdr_file_join_args,
			      (char *)&args)) {
		svcerr_decode(transp);
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		fptr = new _Tt_s_file(args.path);
		status = fptr->s_quit(proc);
	} else {
		status = TT_ERR_PROCID;
	}
	(void)svc_sendreply(transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status);
}

/* 
 * Called when a procid joins a session.
 */
void
_tt_rpc_join_session(SVCXPRT *transp)
{
	_Tt_procid_ptr		cproc;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_procid,
			      (char *)&cproc)) {
		svcerr_decode(transp);
		return;
	}
	if (_tt_s_mp->find_proc(cproc, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		if (cproc->start_token().len() != 0) {
			sp->set_start_token(cproc->start_token());
		}
		status = _tt_s_mp->initial_s_session->s_join(proc);
	} else {
		status = TT_ERR_PROCID;
	}

	if (svc_sendreply(transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status) == 0) {
		return;
	}
}


/* 
 * Called when a procid quits a session.
 */
void
_tt_rpc_quit_session(SVCXPRT *transp)
{
	_Tt_procid_ptr		cproc;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_procid,
			      (char *)&cproc)) {
		svcerr_decode(transp);
		return;
	}
	if (_tt_s_mp->find_proc(cproc, proc, 1)) {
		status = _tt_s_mp->initial_s_session->s_quit(proc);
	} else {
		status = TT_ERR_PROCID;
	}

	if (svc_sendreply(transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status) == 0) {
		return;
	}
}

/*
 * Called when a procid wants to join a context.
 */
void
_tt_rpc_join_context(SVCXPRT *transp)
{
	_Tt_context_join_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs( transp, (xdrproc_t)tt_xdr_context_join_args,
			      (char *)&args))
	{
		svcerr_decode(transp);
	}
	if (_tt_s_mp->find_proc( args.procid, proc, 1 )) {
		status = ((_Tt_s_msg_context &)*args.context).s_join( proc );
	} else {
		status = TT_ERR_PROCID;
	}
	svc_sendreply( transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status );
}

/*
 * Called when a procid wants to quit a context.
 */
void
_tt_rpc_quit_context(SVCXPRT *transp)
{
	_Tt_context_join_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs( transp, (xdrproc_t)tt_xdr_context_join_args,
			      (char *)&args))
	{
		svcerr_decode(transp);
	}
	if (_tt_s_mp->find_proc( args.procid, proc, 1 )) {
		status = ((_Tt_s_msg_context &)*args.context).s_quit( proc );
	} else {
		status = TT_ERR_PROCID;
	}
	svc_sendreply( transp, (xdrproc_t)xdr_int, (RPC_ARG_T)&status );
}


/* 
 * Called when a procid wants to retrieve the next undelivered message
 * for it.
 */
void
_tt_rpc_next_message(SVCXPRT *transp)
{
	_Tt_procid_ptr		cproc;
	_Tt_next_message_args	args;
	_Tt_s_procid_ptr	proc;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_procid,
			      (char *)&cproc)) {
		svcerr_decode(transp);
		return;
	}
	// default is to tell client to clear fd, that way in case of
	// any errors, at least the client won't loop.
	args.clear_signal = 1;
	args.msgs = (_Tt_message_list *)0;
	if (_tt_s_mp->find_proc(cproc, proc, 0)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		sp->next_message(args);
	}

	if (svc_sendreply(transp,
			  (xdrproc_t)tt_xdr_next_message_args,
			  (RPC_ARG_T)&args) == 0) {
		return;
	}
}


/* 
 * Called when a procid wants to register a new pattern.
 */
void
_tt_rpc_add_pattern(SVCXPRT *transp)
{
	_Tt_s_procid_ptr	proc;
	_Tt_s_add_pattern_args	args;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_s_xdr_add_pattern_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->add_pattern(args.pattern);
	} else {
		status = TT_ERR_PROCID;
	}

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}


/* 
 * Called when a procid wants to register a new context-ful pattern.
 */
void
_tt_rpc_add_pattern_with_context(SVCXPRT *transp)
{
	//
	// Contexts are only xdr'd in _Tt_patterns in xdr version 3 or
	// later.  The rpc protocol version number represents a floor
	// for the xdr version number.  Even though this rpc call was
	// introduced into protocol version 2, it can be assumed that
	// the caller is using at least xdr version 3.
	//
//	_Tt_xdr_version	xvers( max(_tt_global->xdr_version(),
//				   TT_CONTEXTS_XDR_VERSION) );
	if(_tt_global->xdr_version() > TT_CONTEXTS_XDR_VERSION) {
		_Tt_xdr_version xvers(_tt_global->xdr_version() );
	} else {
		_Tt_xdr_version xvers(TT_CONTEXTS_XDR_VERSION);
	}

	_tt_rpc_add_pattern(transp);
}


/* 
 * Called when a procid wants to unregister a pattern with the server.
 */
void
_tt_rpc_del_pattern(SVCXPRT *transp)
{
	_Tt_s_procid_ptr	proc;
	_Tt_s_del_pattern_args	args;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_s_xdr_del_pattern_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->del_pattern(args.pattern_id);
	} else {
		status = TT_ERR_PROCID;
	}

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}



/* 
 * Called by a procid to declare a ptype.
 */
void
_tt_rpc_declare_ptype(SVCXPRT *transp)
{
	_Tt_declare_ptype_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_declare_ptype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->declare_ptype(args.ptid);
	} else {
		status = TT_ERR_PROCID;
	}	

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}

/* 
 * Called by a procid to undeclare a ptype.
 */
void
_tt_rpc_undeclare_ptype(SVCXPRT *transp)
{
	_Tt_declare_ptype_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_declare_ptype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->undeclare_ptype(args.ptid);
	} else {
		status = TT_ERR_PROCID;
	}	

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}

/* 
 * Called by a procid to test existence of a ptype.
 */
void
_tt_rpc_exists_ptype(SVCXPRT *transp)
{
	_Tt_declare_ptype_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_declare_ptype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->exists_ptype(args.ptid);
	} else {
		status = TT_ERR_PROCID;
	}	

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}


/* 
 * Called by a procid to unblock queued messages for its ptype.
 */
void
_tt_rpc_unblock_ptype(SVCXPRT *transp)
{
	_Tt_declare_ptype_args	args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_declare_ptype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(args.procid, proc, 1)) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		status = sp->unblock_ptype(args.ptid);
	} else {
		status = TT_ERR_PROCID;
	}	

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}

/* 
 * Called by a procid to load new types into its associated ttsession.
 */
void
_tt_rpc_load_types(SVCXPRT *transp)
{
	_Tt_load_types_args	args;
	Tt_status		status;
	XDR			xdrs;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_load_types_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	xdrmem_create(&xdrs,
		      (char *)args.xdrtypes,
		      (u_int)args.xdrtypes.len(),
		      XDR_DECODE);

	int junk;
	status = _Tt_typedb::merge_from(&xdrs,_tt_s_mp->tdb, junk);

	if (status==TT_OK) {
		_tt_s_mp->install_ptable(_tt_s_mp->tdb->ptable);
		_tt_s_mp->install_otable(_tt_s_mp->tdb->otable);
	}

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}



/* 
 * Called by a procid to notify the server of its fd signalling channel. 
 */
void
_tt_rpc_set_fd_channel(SVCXPRT *transp)
{
	_Tt_fd_args		fd_args;
	_Tt_s_procid_ptr	proc;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_fd_args,
			      (char *)&fd_args)) {
		svcerr_decode(transp);
		return;
	}

	if (_tt_s_mp->find_proc(fd_args.procid, proc, 1)) {
		status = TT_OK;
	} else {
		status = TT_ERR_PROCID;
	}

	//
	// Note: it's important to reply immediately here because
	// setting up the channel involves connecting to the socket
	// on the client side so if we attempted to reply after
	// the operation then we would get deadlock.
	//
	if (svc_sendreply(transp,
			  (xdrproc_t)xdr_int,
			  (RPC_ARG_T)&status) == 0) {
		return;
	}

	if (status == TT_OK) {
		_Tt_s_procid	*sp = (_Tt_s_procid *)proc.c_pointer();
		sp->set_start_token(fd_args.start_token);
		sp->set_fd_channel(fd_args.fd);
	}
}


/* 
 * Set the value of a session property
 */
void
_tt_rpc_set_prop(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	status = _tt_s_mp->initial_s_session->s_setprop(args.prop, args.value);

	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}

/* 
 * Add a value to a session property
 */
void
_tt_rpc_add_prop(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	status = _tt_s_mp->initial_s_session->s_addprop(args.prop, args.value);
	if (svc_sendreply(transp,(xdrproc_t)xdr_int,(RPC_ARG_T)&status) == 0) {
		return;
	}
}

/* 
 * Get a value from a session property
 */
void
_tt_rpc_get_prop(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	Tt_status		status;
	_Tt_string		value;
	_Tt_rpc_result		result;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	
	if ((status = _tt_s_mp->initial_s_session->s_getprop(args.prop, args.num, value))
	    != TT_OK) {
		result.status = status;
		result.str_val = "";
		result.int_val = 0;
	} else {
		result.status = TT_OK;
		result.str_val = value;
		result.int_val = 0;
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of values for a session property
 */
void
_tt_rpc_prop_count(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	int			cnt;
	_Tt_rpc_result		result;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	
	result.status = _tt_s_mp->initial_s_session->s_propcount(args.prop, cnt);
	result.str_val = "";
	if (result.status == TT_OK) {
		result.int_val = cnt;
	} else {
		result.int_val = 0;
	}

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return a session property name
 */
void
_tt_rpc_prop_name(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	Tt_status		status;
	_Tt_string		prop;
	_Tt_rpc_result		result;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	if ((status = _tt_s_mp->initial_s_session->s_propname(args.num, prop))
	    != TT_OK) {
		result.status = status;
		result.str_val = "";
		result.int_val = 0;
	} else {
		result.status = TT_OK;
		result.str_val = prop;
		result.int_val = 0;
	}

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of session properties
 */
void
_tt_rpc_prop_names_count(SVCXPRT *transp)
{
	_Tt_prop_args		args;
	int			cnt;
	_Tt_rpc_result		result;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_prop_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = _tt_s_mp->initial_s_session->s_propnames_count(cnt);
	result.str_val = "";
	if (result.status == TT_OK) {
		result.int_val = cnt;
	} else {
		result.int_val = 0;
	}

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}


void
_tt_rpc_alloc_procid_key(SVCXPRT *transp)
{
	_Tt_string		result;

	if (! _tt_svc_getargs(transp,(xdrproc_t)xdr_void,0)) {
		svcerr_decode(transp);
		return;
	}

	result = _tt_s_mp->alloc_procid_key();

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_string,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

void
_tt_rpc_close_procid(SVCXPRT *transp)
{
	_Tt_procid_ptr		p;
	_Tt_s_procid_ptr	rp;
	Tt_status		status;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_procid,
			      (char *)&p)) {
		svcerr_decode(transp);
	}
	if (_tt_s_mp->find_proc(p, rp, 0)) {
		status = _tt_s_mp->s_remove_procid(*rp);
	} else {
		status = TT_ERR_PROCID;
	}
	(void)svc_sendreply(transp,(xdrproc_t)xdr_int, (RPC_ARG_T)&status);
}

void
_tt_rpc_vrfy_session(SVCXPRT *transp)
{
	_Tt_string		id;

	if (! _tt_svc_getargs(transp, (xdrproc_t)xdr_void, 0)) {
		svcerr_decode(transp);
		return;
	}
	if (_tt_global->xdr_version() == 1) {
		id = _tt_s_mp->initial_s_session->id();
	} else {
		id = _tt_s_mp->initial_s_session->process_tree_id();
	}
	if (!svc_sendreply(transp,
			   (xdrproc_t)tt_xdr_string,
			   (RPC_ARG_T)&id)) {
		svcerr_decode(transp);
	}
}
/* 
 * Return the number of otypes derived from a base otype
 */
void
_tt_rpc_otype_deriveds_count(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		base_otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.str_val = "";
	base_otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (base_otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		result.int_val = base_otype->children()->count();
		result.status = TT_OK;
	}

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the n'th otype derived from a base otype
 */
void
_tt_rpc_otype_derived(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_string_list_ptr	children;
	_Tt_otype_ptr		base_otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.str_val = "";
	base_otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (base_otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		children =  base_otype->children();
		if (args.num<0 || children->count()<args.num) {
			result.status = TT_ERR_NUM;
			result.str_val = "";
		} else if (children->count()==args.num) {
			result.status = TT_OK;
			result.str_val = (char *)0;
		} else {
			result.status = TT_OK;
			result.str_val = (*children)[args.num];
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the otype that is the base of a derived otype
 */
void
_tt_rpc_otype_base(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_string_list_ptr	parents;
	_Tt_otype_ptr		derived_otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	derived_otype = _tt_s_mp->otable->lookup(args.derived_otid);
	if (derived_otype.is_null()) {
		result.str_val = (char *)0;
		result.status = TT_ERR_OTYPE;
	} else {
		parents =  derived_otype->parents();
		if (parents.is_null() || parents->is_empty()) {
			result.status = TT_OK;
			result.str_val = (char *)0;
		} else {
			result.status = TT_OK;
			result.str_val = parents->top();
		}
	}

	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return 1 iff the derived otype derives directly or indirectly from the
 * base otype.
 */
void
_tt_rpc_otype_is_derived(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_string_list_ptr	parents;
	_Tt_otype_ptr		derived_otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	if (args.derived_otid==args.base_otid) {
		result.int_val = 1;
	} else {
		derived_otype = _tt_s_mp->otable->lookup(args.derived_otid);
		if (derived_otype.is_null()) {
			result.status = TT_ERR_OTYPE;
		} else {
			parents =  derived_otype->parents();
			result.int_val = 0;
			_Tt_otype_ptr parent;
			while (!parents->is_empty()) {
				parent = _tt_s_mp->otable->
					lookup(parents->top());
				if (parent.is_null()) {
					break;
				} else if (parent->otid()==args.base_otid) {
					result.int_val = 1;
					break;
				} else {
					parents = parent->parents();
				}
			}
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of observer signatures for the specified otype
 */
void
_tt_rpc_otype_osig_count(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->osigs();
		result.int_val = sigs->count();
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of handler signatures for the specified otype
 */
void
_tt_rpc_otype_hsig_count(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->hsigs();
		result.int_val = sigs->count();
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the op for an observer signature of an otype
 */
void
_tt_rpc_otype_osig_op(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->osigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			result.str_val = sig->op();
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the op for a handler signature of an otype
 */
void
_tt_rpc_otype_hsig_op(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->hsigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			result.str_val = sig->op();
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of arguments of an observer signature of an otype
 */
void
_tt_rpc_otype_osig_args_count(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->osigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			result.int_val = sargs->count();
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the number of arguments of a handler signature of an otype
 */
void
_tt_rpc_otype_hsig_args_count(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->hsigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			result.int_val = sargs->count();
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the mode of the specified arg of an observer signature of an otype
 */
void
_tt_rpc_otype_osig_arg_mode(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->osigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			if (args.num2 < 0 || args.num2 >= sargs->count()) {
				result.status = TT_ERR_NUM;
			} else {
				_Tt_arg_ptr sarg = (*sargs)[args.num2];
				result.int_val = (int)sarg->mode();
			}
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the mode of the specified arg of a handler signature of an otype
 */
void
_tt_rpc_otype_hsig_arg_mode(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->hsigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			if (args.num2 < 0 || args.num2 >= sargs->count()) {
				result.status = TT_ERR_NUM;
			} else {
				_Tt_arg_ptr sarg = (*sargs)[args.num2];
				result.int_val = (int)sarg->mode();
			}
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the type of the specified arg of an observer signature of an otype
 */
void
_tt_rpc_otype_osig_arg_type(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->osigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			if (args.num2 < 0 || args.num2 >= sargs->count()) {
				result.status = TT_ERR_NUM;
			} else {
				_Tt_arg_ptr sarg = (*sargs)[args.num2];
				result.str_val = sarg->type();
			}
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

/* 
 * Return the type of the specified arg of a handler signature of an otype
 */
void
_tt_rpc_otype_hsig_arg_type(SVCXPRT *transp)
{
	_Tt_otype_args		args;
	_Tt_rpc_result		result;
	_Tt_otype_ptr		otype;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_otype_args,
			      (char *)&args)) {
		svcerr_decode(transp);
		return;
	}

	result.status = TT_OK;
	result.str_val = "";
	result.int_val = 0;
	otype =  _tt_s_mp->otable->lookup(args.base_otid);
	if (otype.is_null()) {
		result.status = TT_ERR_OTYPE;
	} else {
		_Tt_signature_list_ptr sigs = otype->hsigs();
		if (args.num < 0 || args.num >= sigs->count()) {
			result.status = TT_ERR_NUM;
		} else {
			_Tt_signature_ptr sig = (*sigs)[args.num];
			_Tt_arg_list_ptr sargs = sig->args();
			if (args.num2 < 0 || args.num2 >= sargs->count()) {
				result.status = TT_ERR_NUM;
			} else {
				_Tt_arg_ptr sarg = (*sargs)[args.num2];
				result.str_val = sarg->type();
			}
		}
	}
	if (svc_sendreply(transp, (xdrproc_t)tt_xdr_rpc_result,
			  (RPC_ARG_T)&result) == 0) {
		return;
	}
}

//
// Horizontal (inter-ttsession) dispatch
//
void
_tt_rpc_hdispatch(SVCXPRT *transp)
{
	_Tt_s_message_ptr		msg;
	_Tt_qmsg_info_ptr		qm = (_Tt_qmsg_info *)0;
	_Tt_dispatch_reply_args		args;

	msg = new _Tt_s_message();
	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_xdr_message,
			     (char *)&msg)) {
		svcerr_decode(transp);
		return;
	}
	_Tt_msg_trace trace( *msg, TTDR_HDISPATCH );
	(void)msg->dispatch( trace );
	(void)msg->deliver_to_observers_and_handlers( trace );
}

//
// Horizontal (inter-ttsession) update
//
void
_tt_rpc_hupdate_msg(SVCXPRT *transp)
{
	_Tt_s_procid_ptr		sender;
	_Tt_s_update_args		args;

	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_s_xdr_update_args,
			     (char *)&args))
		{
			svcerr_decode(transp);
			return;
		}
	if (! _tt_s_mp->find_proc(args.message->sender(), sender, 0)) {
		return;
	}

	if (   (args.newstate == TT_FAILED)
	       && (args.message->status() == TT_ERR_NO_MATCH)
	       && (args.message->handler().is_null()))
	{
		//
		// No remote session could find a handler.
		// Simulate a rejection, and redeliver the message.
		// _tried prevents re-offers, but any new handler
		// will get a crack at the request, which is nice.
		// _rsessions will be empty, so if we again find no
		// handler, we will perform disposition instead of
		// hdispatch()ing to remote sessions.
		//
		args.message->set_state(TT_SENT);
		args.message->set_status((int)TT_OK);
		_Tt_msg_trace trace( *args.message, TTDR_HUPDATE );
		if (args.message->indoctrinate( trace ) == TT_OK) {
			args.message->change_state( 0, TT_REJECTED, trace );
		}
	} else {
		args.message->set_state(args.newstate);
		// Tracing occurs inside ::add_message()
		sender->add_message(args.message);
	}
}


void
_tt_rpc_msgread_2(SVCXPRT *transp)
{
#ifdef OPT_ADDMSG_DIRECT
	_Tt_procid_ptr		p;
	_Tt_s_procid_ptr	rp;

	if (! _tt_svc_getargs(transp,
			      (xdrproc_t)tt_xdr_procid,
			      (char *)&p)) {
		svcerr_decode(transp);
	}
	if (_tt_s_mp->find_proc(p, rp, 0)) {
		rp->msgread();
	}
#else
	svcerr_noproc(transp);
#endif				// OPT_ADDMSG_DIRECT
}


/* 
 * Called when a sender sends a message to the server for delivery on exit.
 */
void
_tt_rpc_dispatch_on_exit(SVCXPRT *transp)
{
	_Tt_s_message_ptr		msg;
	_Tt_s_procid_ptr		proc;
	_Tt_dispatch_reply_args		args;

	msg = new _Tt_s_message();
	if (!_tt_svc_getargs(transp,
			     (xdrproc_t)tt_xdr_message,
			     (char *)&msg)) {
		svcerr_decode(transp);
		return;
	}

	args.status = TT_OK; 

	if (_tt_s_mp->find_proc(msg->sender(),proc, 0)) {
		(void)proc->add_on_exit_message(msg);
	} else {
		args.status = TT_ERR_PROCID;
	}

	if (!svc_sendreply(transp,
			   (xdrproc_t)tt_xdr_dispatch_reply_args,
			   (RPC_ARG_T)&args)) {
		return;
	}

}
