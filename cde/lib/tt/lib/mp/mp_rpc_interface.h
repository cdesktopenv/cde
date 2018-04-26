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
/*%%  $TOG: mp_rpc_interface.h /main/4 1999/08/30 10:57:58 mgreess $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_rpc_interface.h	1.9 93/07/30
 *
 * mp_rpc_interface.h
 *
 * Constants that must be known by both client and server in the 
 * ttsession RPC interface.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#if !defined(_MP_RPC_INTERFACE_H)
#define _MP_RPC_INTERFACE_H

/*
 * Current ToolTalk RPC protocol version
 *
 * 1	1.0	Classic
 * 2	1.0.1	Asynchronous (i.e. faster) RPCs
 * 3	1.2	TT_OFFER, TT_HANDLE_PUSH, TT_HANDLE_ROTATE, affecting
 *		any RPC that can send ttsession a new message or pattern:
 *			TT_RPC_DISPATCH
 *			TT_RPC_DISPATCH_2
 *			TT_RPC_DISPATCH_WITH_CONTEXT
 *			TT_RPC_DISPATCH_2_WITH_CONTEXT
 *			TT_RPC_ADD_PATTERN
 *			TT_RPC_ADD_PATTERN_WITH_CONTEXT
 */
const int	TT_RPC_VERSION			= 4;
const int	TT_OFFER_RPC_VERSION		= 4;
/*
 * Current ToolTalk XDR protocol version.  See tt_xdr_version.h
 *
 * 1	1.0	Classic; used by default for all on-disk data structures
 * 2	1.0.1	new _Tt_string::xdr(); more efficient _Tt_message::xdr(); etc?
 * 3	1.1	contexts in _Tt_pattern::xdr(), _Tt_signature::xdr();
 *		used in any types database with contextful signatures.
 * 4	1.2	PUSH, ROTATE in _Tt_pattern::xdr(), _Tt_signature::xdr();
 *		_Tt_arg::_matched_type in _Tt_arg::xdr();
 *		used in any types database with push/rotate signatures.
 *		However, we still use version 3 for the wire, because
 *		PUSH/ROTATE only make a difference in the types database.
 */
const int	TT_XDR_VERSION			= 3;
const int	TT_TYPESDB_DEFAULT_XDR_VERSION	= 1;
const int	TT_CONTEXTS_XDR_VERSION		= 3;
const int	TT_PUSH_ROTATE_XDR_VERSION	= 4;

/* 
 * Default timeout for RPC requests (in seconds).
 * Use a very large value to indicate almost infinity.
 * In addition to accommodating a loaded ttsession, it will also
 * facilitate debugging.  
 * ONE HUNDRED MILLION IS THE MAXIMUM PERMITTED VALUE
 * ON SunOS 4.1.  Use ONE MILLION to leave room for possible weird system
 * dependencies.  That will still allow the sender to wait a patient 11.6 days.
 */
#define TT_RPC_TMOUT		1000000

/*
 * RPC procedure numbers
 */
/* 
 * Note that each rpc procedure number cannot be re-used. Adding a new
 * rpc procedure thus means allocating a new number below (and then
 * bumping up TT_RPC_LAST) and adding the dispatch function to the
 * _tt_rpc_dispatch_table in mp_rpc_implement.cc. The only exception
 * to this is TT_RPC_VRFY_SESSION which has an out-of-sequence rpc
 * number (it is chosen to minimize the likelyhood of being implemented
 * by another program since the purpose of it is to identify a tooltalk
 * session.)
 */
typedef enum {
	TT_RPC_NULLPROC		=	0, 
	TT_RPC_DISPATCH		=	1,
	TT_RPC_UPDATE_MSG	=	2,
	TT_RPC_JOIN_FILE	=	3,
	TT_RPC_QUIT_FILE	=	4,
	TT_RPC_JOIN_SESSION	=	5,
	TT_RPC_QUIT_SESSION	=	6,
	TT_RPC_NEXT_MESSAGE	=	7,
	TT_RPC_ADD_PATTERN	=	8,
	TT_RPC_DEL_PATTERN	=	9,
	TT_RPC_DECLARE_PTYPE	=	10,
	TT_RPC_SET_FD_CHANNEL	=	11,
	TT_RPC_SET_PROP		=	12,
	TT_RPC_ADD_PROP		=	13,
	TT_RPC_GET_PROP		=	14,
	TT_RPC_PROP_COUNT	=	15,
	TT_RPC_PROP_NAME	=	16,
	TT_RPC_PROP_NAMES_COUNT	=	17,
	TT_RPC_ALLOC_PROCID_KEY	=	18,
	TT_RPC_CLOSE_PROCID	=	19,
	TT_RPC_OTYPE_DERIVEDS_COUNT =	20,
	TT_RPC_OTYPE_DERIVED	=	21,
	TT_RPC_OTYPE_BASE	=	22,
	TT_RPC_OTYPE_IS_DERIVED	=	23,
	TT_RPC_OTYPE_OSIG_COUNT =	24,
	TT_RPC_OTYPE_HSIG_COUNT =	25,
	TT_RPC_OTYPE_OSIG_OP	=	26,
	TT_RPC_OTYPE_HSIG_OP	=	27,
	TT_RPC_OTYPE_OSIG_ARGS_COUNT =	28,
	TT_RPC_OTYPE_HSIG_ARGS_COUNT =	29,
	TT_RPC_OTYPE_OSIG_ARG_MODE =	30,
	TT_RPC_OTYPE_HSIG_ARG_MODE =	31,
	TT_RPC_OTYPE_OSIG_ARG_TYPE =	32,
	TT_RPC_OTYPE_HSIG_ARG_TYPE =	33,
	TT_RPC_HDISPATCH	=	34,
	TT_RPC_HUPDATE_MSG	=	35,

	/* version 2 numbers */

	TT_RPC_DISPATCH_2	=	36,
	TT_RPC_UPDATE_MSG_2	=	37,
	TT_RPC_MSGREAD_2	=	38,

	/* S493 numbers */

	TT_RPC_DISPATCH_ON_EXIT =	39,
	TT_RPC_UNDECLARE_PTYPE	=	40,
	TT_RPC_EXISTS_PTYPE	=	41,
	TT_RPC_UNBLOCK_PTYPE	=	42,
	TT_RPC_JOIN_CONTEXT	=	43,
	TT_RPC_QUIT_CONTEXT	=	44,
	TT_RPC_DISPATCH_WITH_CONTEXT	=	45,
	TT_RPC_DISPATCH_2_WITH_CONTEXT	=	46,
	TT_RPC_ADD_PATTERN_WITH_CONTEXT	=	47,

	/* S1093 numbers */

	TT_RPC_LOAD_TYPES	=	48,

	/* Add new RPC numbers before here and bump TT_RPC_LAST */
	TT_RPC_LAST		=	49,

	/* This high number is treated specially */
	TT_RPC_VRFY_SESSION	=	400
}  _Tt_rpc_procedure_number;

#endif /* _MP_RPC_INTERFACE_H */
