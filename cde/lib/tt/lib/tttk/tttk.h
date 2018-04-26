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
/* $XConsortium: tttk.h /main/3 1995/10/23 10:33:00 rswiston $ */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */

/*
 * @(#)tttk.h	1.11 93/09/29
 */

#ifndef tttk_h
#define tttk_h

#include <Tt/tt_c.h>
#include <X11/Intrinsic.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
	TTDT_OP_NONE,
	TTDT_CREATED,
	TTDT_DELETED,
	TTDT_DO_COMMAND,
	TTDT_SET_ENVIRONMENT,
	TTDT_GET_ENVIRONMENT,
	TTDT_SET_GEOMETRY,
	TTDT_GET_GEOMETRY,
	TTDT_SET_ICONIFIED,
	TTDT_GET_ICONIFIED,
	TTDT_SET_LOCALE,
	TTDT_GET_LOCALE,
	TTDT_SET_MAPPED,
	TTDT_GET_MAPPED,
	TTDT_MODIFIED,
	TTDT_REVERTED,
	TTDT_GET_MODIFIED,
	TTDT_MOVED,
	TTDT_PAUSE,
	TTDT_RESUME,
	TTDT_QUIT,
	TTDT_RAISE,
	TTDT_LOWER,
	TTDT_SAVE,
	TTDT_REVERT,
	TTDT_SAVED,
	TTDT_SET_SITUATION,
	TTDT_GET_SITUATION,
	TTDT_SIGNAL,
	TTDT_STARTED,
	TTDT_STOPPED,
	TTDT_STATUS,
	TTDT_GET_STATUS,
	TTDT_GET_SYSINFO,
	TTDT_SET_XINFO,
	TTDT_GET_XINFO,
	TTME_ABSTRACT,
	TTME_DEPOSIT,
	TTME_DISPLAY,
	TTME_EDIT,
	TTME_COMPOSE,
	TTME_INTERPRET,
	TTME_PRINT,
	TTME_TRANSLATE,
	TTME_MAIL,
	TTME_MAIL_COMPOSE,
	TTME_MAIL_EDIT,
	TTME_INSTANTIATE,
	TTDT_OP_LAST
} Tttk_op;

/*
 * Standard vtype names
 */
extern const char      *Tttk_integer;
extern const char      *Tttk_string;
extern const char      *Tttk_boolean;
extern const char      *Tttk_file;
extern const char      *Tttk_message_id;
extern const char      *Tttk_title;
extern const char      *Tttk_width;
extern const char      *Tttk_height;
extern const char      *Tttk_xoffset;
extern const char      *Tttk_yoffset;

/*
 **********************************************************************
 *
 * Procid lifecycle
 *
 **********************************************************************
 */
char		       *ttdt_open(
				int	       *tt_fd,
				const char     *toolname,
				const char     *vendor,
				const char     *version,
				int		sendStarted
			);
Tt_status		ttdt_sender_imprint_on(
				const char     *handler,
				Tt_message	commission,
				char	      **display,
				int	       *width,
				int	       *height,
				int	       *xoffset,
				int	       *yoffset,
				XtAppContext	app2run,
				int		ms_timeout
			);
Tt_status		ttdt_close(
				const char     *procid,
				const char     *newprocid,
				int		sendStopped
			);
/*
 **********************************************************************
 *
 * Sessions
 *
 **********************************************************************
 */
typedef Tt_message	(*Ttdt_contract_cb)(
				Tt_message	msg,
				void	       *clientdata,
				Tt_message	contract
			);
Tt_pattern	       *ttdt_session_join(
				const char	       *sessid,
				Ttdt_contract_cb	cb,
				Widget			shell,
				void		       *clientdata,
                                int			join
			);
Tt_status		ttdt_session_quit(
				const char     *sessid,
				Tt_pattern     *sess_pats,
                                int		quit
			);
/*
 **********************************************************************
 *
 * Contracts
 *
 **********************************************************************
 */
Tt_pattern	       *ttdt_message_accept(
				Tt_message	contract,
				Ttdt_contract_cb	cb,
				Widget		shell,
				void	       *clientdata,
				int		accept,
				int		sendStatus
			);
Tt_pattern	       *ttdt_subcontract_manage(
				Tt_message	subcontract,
				Ttdt_contract_cb cb,
				Widget		shell,
				void	       *clientdata
			);
/*
 **********************************************************************
 *
 * Desktop: Files
 *
 **********************************************************************
 */
typedef Tt_message	(*Ttdt_file_cb)(
				Tt_message	msg,
				Tttk_op		op,
				char           *pathname,
				void	       *clientdata,
				int		same_euid_egid,
				int		same_procid
			);
Tt_pattern	       *ttdt_file_join(
                                const char     *pathname,
                                Tt_scope	scope,
				int		join,
				Ttdt_file_cb	cb,
				void           *clientdata
			);
Tt_status		ttdt_file_event(
				Tt_message	context,
				Tttk_op		event,
				Tt_pattern     *patterns,
				int		send
			);
Tt_status		ttdt_file_quit(
				Tt_pattern     *patterns,
				int		quit
			);
int			ttdt_Get_Modified(
				Tt_message	context,
				const char     *pathname,
                                Tt_scope	scope,
				XtAppContext	app2run,
				int		ms_timeout
			);
Tt_status		ttdt_Save(
				Tt_message	context,
				const char     *pathname,
                                Tt_scope	scope,
				XtAppContext	app2run,
				int		ms_timeout
			);
Tt_status		ttdt_Revert(
				Tt_message	context,
				const char     *pathname,
                                Tt_scope	scope,
				XtAppContext	app2run,
				int		ms_timeout
			);
Tt_message		ttdt_file_notice(
				Tt_message	context,
				Tttk_op		op,
				Tt_scope	scope,
				const char     *file,
				int		send_and_destroy
			);
Tt_message		ttdt_file_request(
				Tt_message	context,
				Tttk_op		op,
				Tt_scope	scope,
				const char     *file,
				Ttdt_file_cb	cb,
				void           *clientdata,
				int		send
			);
/*
 **********************************************************************
 *
 * Media Exchange
 *
 **********************************************************************
 */
typedef Tt_message	(*Ttmedia_load_pat_cb)(
				Tt_message	msg,
				void           *clientdata,
				Tttk_op		op,
				Tt_status	diagnosis,
				unsigned char  *contents,
				int             len,
				char           *file,
				char           *docname
			);
Tt_status		ttmedia_ptype_declare(
				const char     *ptype,
				int		base_opnum,
				Ttmedia_load_pat_cb	cb,
				void	       *clientdata,
				int		declare
			);
typedef Tt_message	(*Ttmedia_load_msg_cb)(
				Tt_message	msg,
				void           *clientdata,
				Tttk_op		op,
				unsigned char  *contents,
				int             len,
				char           *file
			);
Tt_message		ttmedia_load(
				Tt_message		context,
				Ttmedia_load_msg_cb	cb,
				void		       *clientdata,
				Tttk_op			op,
				const char	       *media_type,
				const unsigned char    *contents,
				int			len,
				const char	       *file,
				const char	       *docname,
				int			send
			);
Tt_status		ttmedia_load_reply(
				Tt_message		contract,
				const unsigned char    *new_contents,
				int             	new_len,
				int			reply_and_destroy
			);
Tt_status		ttmedia_Deposit(
				Tt_message		contract,
				const char	       *buffer_id,
				const char	       *media_type,
				const unsigned char    *new_contents,
				int             	new_len,
				const char 	       *file,
				XtAppContext		app2run,
				int			ms_timeout
			);
/*
 **********************************************************************
 *
 * ToolTalk Toolkit
 *
 **********************************************************************
 */
void			tttk_Xt_input_handler(
				XtPointer		procid,
				int		       *,
				XtInputId	       *
			);
Tt_status		tttk_block_while(
				XtAppContext		app2run,
				const int	       *blocked,
				int			ms_timeout
			);
Tt_message		tttk_message_create(
				Tt_message		context,
				Tt_class		the_class,
				Tt_scope		the_scope,
				const char	       *handler,
				const char	       *op,
				Tt_message_callback	callback
			);
Tt_status		tttk_message_destroy(
				Tt_message		msg
			);
Tt_status		tttk_message_reject(
				Tt_message		msg,
				Tt_status		status,
				const char	       *status_string,
				int			destroy
			);
Tt_status		tttk_message_fail(
				Tt_message		msg,
				Tt_status		status,
				const char		*status_string,
				int			destroy
			);
Tt_status		tttk_message_abandon(
				Tt_message		msg
			);
Tttk_op			tttk_string_op(
				const char     *s
			);
char		       *tttk_op_string(
				Tttk_op		op
			);
#if defined(__cplusplus)
}
#endif

#endif
