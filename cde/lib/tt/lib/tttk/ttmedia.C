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
//%%  $XConsortium: ttmedia.C /main/3 1995/10/23 10:32:41 rswiston $ 			 				
#include "api/c/tt_c.h"
#include "util/tt_gettext.h"
#include "tttk/tttk.h"
#include "tttk/ttdesktop.h"
#include "tttk/tttkmessage.h"
#include "tttk/tttkpattern.h"
#include "tttk/tttkutils.h"
#include "tttk/tttk2free.h"

Tt_status	_tt_ptype_opnum_user_set(const char *ptype, int opnum,
					 void *clientdata);
void *		_tt_ptype_opnum_user(const char *ptype, int opnum);

class _Ttmedia_ptype_info: public _Tt_allocated {
    public:
	_Ttmedia_ptype_info(
		Ttmedia_load_pat_cb	loadCB,
		void		       *clientData
	);

	Ttmedia_load_pat_cb	_loadCB;
	void		       *_clientData;
};

_Ttmedia_ptype_info::_Ttmedia_ptype_info(
	Ttmedia_load_pat_cb	loadCB,
	void		       *clientData)
{
	_loadCB = loadCB;
	_clientData = clientData;
}

Tt_message
ttMediaLoadPatCB(
	Tt_message	msg,
	Tt_pattern	,
	Ttmedia_load_pat_cb callback,
	void	       *clientData,
	Tttk_op		op,
	int		counterfoilArg	= -1,
	int		docNameArg	= -1
)
{
	static const char here[]        = "ttMediaLoadPatCB()";
	unsigned char     *contents    = 0;
	int                len         = 0;
	char              *file        = 0;
	Tt_status          status;
	Tt_status          diagnosis   = TT_OK;
	const char        *diagnosisStr= 0;
	char              *counterfoil = 0;
	char              *docName     = 0;

	if (callback == 0) {
		return msg;
	}
	status = tt_message_arg_bval( msg, 0, &contents, &len );
	if (status != TT_OK) {
		diagnosis = status;
		diagnosisStr = "tt_message_arg_bval( msg, 0 )";
	} else if (len == 0) {
		//
		// contents arg empty; contents are in named file
		//
		file = tt_message_file( msg );
		status = tt_ptr_error( file );
		if (status != TT_OK) {
			file = 0;
			diagnosis = status;
			diagnosisStr = "tt_message_file()";
		} else if (   (file == 0)
                        && (tt_message_arg_mode( msg, 0 ) != TT_OUT)
                        && (op != TTME_INSTANTIATE))
		{
			diagnosis = TT_DESKTOP_ENODATA;
			diagnosisStr = catgets( _ttcatd, 1, 25,
				"empty document and no file" );
		}
	}
	if (counterfoilArg >= 0) {
		counterfoil = tt_message_arg_val( msg, counterfoilArg );
		status = tt_ptr_error( counterfoil );
		if (status != TT_OK) {
			_ttDtPrintStatus( here,
					  catgets( _ttcatd, 1, 26,
						   "cannot get messageID; "
						   "operation may not be "
						   "cancel-able, because "
						   "tt_message_arg_val()"),
					  status );
			counterfoil = 0;  // charge ahead
		}
	}
	if (docNameArg > 0) {
		docName = tt_message_arg_val( msg, docNameArg );
		status = tt_ptr_error( docName );
		if (status != TT_OK) {
			_ttDtPrintStatus( here,
					  catgets( _ttcatd, 1, 27,
						   "cannot get title; document "
						   "will be untitled because "
						   "tt_message_arg_val()" ),
					  status );
			docName = 0;  // charge ahead
		}
	}
	msg = (*callback)( msg, clientData, op, diagnosis, contents,
			   len, file, docName );
	status = tt_ptr_error( msg );
	if ((status == TT_OK) && (msg != 0) && (diagnosis != TT_OK)) {
		tttk_message_fail( msg, diagnosis, diagnosisStr, 1 );
		return (Tt_message)tt_error_pointer( diagnosis );
	} else {
		return msg;
	}
}

Tt_callback_action
_ttMeOpnumCB(
	Tt_message msg,
	Tt_pattern pat
)
{
	static const char here[] = "_ttMeOpnumCB()";

	if (    (tt_message_state( msg ) != TT_SENT)
	     || (tt_message_class( msg ) != TT_REQUEST )
	     || (! _tttk_message_am_handling( msg )))
	{
		return TT_CALLBACK_CONTINUE;
	}
	int opnum = tt_message_opnum( msg );
	Tt_status status = tt_int_error( opnum );
	if ((status != TT_OK) && (opnum != -1)) {
		return TT_CALLBACK_PROCESSED;
	}
	int base = opnum - (opnum % 1000);
	int offset = opnum - base;
	char *ptype = tt_message_handler_ptype( msg );
	_Ttmedia_ptype_info *i = (_Ttmedia_ptype_info *)
		_tt_ptype_opnum_user( ptype, base );
	tt_free( ptype );
	status = tt_ptr_error( i );
	if ((status != TT_OK) || (i == 0)) {
		return TT_CALLBACK_PROCESSED;
	}
	switch (offset) {
	    case 1:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_DISPLAY );
		break;
	    case 2:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_DISPLAY, 1 );
		break;
	    case 3:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_DISPLAY, -1, 1 );
		break;
	    case 4:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_DISPLAY, 1, 2 );
		break;
	    case 101:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_EDIT );
		break;
	    case 102:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_EDIT, 1 );
		break;
	    case 103:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_EDIT, -1, 1 );
		break;
	    case 104:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_EDIT, 1, 2 );
		break;
	    case 201:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_COMPOSE );
		break;
	    case 202:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_COMPOSE, 1 );
		break;
	    case 203:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_COMPOSE, -1, 1 );
		break;
	    case 204:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_COMPOSE, 1, 2 );
		break;
	    case 301:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_MAIL );
		break;
	    case 311:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_MAIL_EDIT );
		break;
	    case 313:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_MAIL_EDIT, -1, 1 );
		break;
	    case 321:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_MAIL_COMPOSE );
		break;
	    case 323:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_MAIL_COMPOSE, -1, 1 );
		break;
	    case 401:
		msg = ttMediaLoadPatCB( msg, pat, i->_loadCB, i->_clientData,
				        TTME_INSTANTIATE );
		break;
	    // Add any new offsets to ttmedia_ptype_declare()
	    default:
		_ttDtPrintInt( here,
			       catgets(_ttcatd, 1, 28, "unknown opnum offset"),
			       offset );
		return TT_CALLBACK_CONTINUE;
	}
	return _ttDtCallbackAction( msg );
}

Tt_status
ttmedia_ptype_declare(
	const char	       *ptype,
	int			baseOpnum,
	Ttmedia_load_pat_cb	loadCB,
	void		       *clientData,
	int			declare
)
{
	Tt_status          status;

/*
 * XXX Join default contexts of current default procid.
 */
	for (int i = 0; i <= 200; i += 100) {
		for (int j = 1; j <= 4; j++) {
			status = tt_ptype_opnum_callback_add( ptype,
					i + j + baseOpnum, _ttMeOpnumCB );
			if (status != TT_OK) {
				return status;
			}
		}
	}
	status = tt_ptype_opnum_callback_add( ptype, 301, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	status = tt_ptype_opnum_callback_add( ptype, 311, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	status = tt_ptype_opnum_callback_add( ptype, 313, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	status = tt_ptype_opnum_callback_add( ptype, 321, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	status = tt_ptype_opnum_callback_add( ptype, 323, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	status = tt_ptype_opnum_callback_add( ptype, 401, _ttMeOpnumCB );
	if (status != TT_OK) return status;
	//
	// Like opnum callbacks themselves, this memory lives as long
	// as the procid.
	//
	_Ttmedia_ptype_info *info =
		new _Ttmedia_ptype_info( loadCB, clientData );
	status = _tt_ptype_opnum_user_set( ptype, baseOpnum, info );
	if (status != TT_OK) {
		return status;
	}
	if (declare) {
		status = tt_ptype_declare( ptype );
		if (status != TT_OK) {
			return status;
		}
	}
	return TT_OK;
}

//
// Parse Display/Edit/Compose reply, or Deposit request, and pass to
// client callback.
//
static Tt_message
_ttMediaLoadMsgCB(
	Tt_message	msg,
	Tt_pattern	,
	void	       *clientCB,
	void	       *clientData
)
{
	char *opname = tt_message_op( msg );
	Tttk_op op = tttk_string_op( opname );
	tt_free( opname );
	int final = _tttk_message_in_final_state( msg );
	switch (tt_message_state( msg )) {
	    case TT_STARTED:
	    case TT_QUEUED:
		return 0;
	    case TT_SENT:
		if (op != TTME_DEPOSIT) {
			// This address space is handler, but is not
			// through handling, so continue.
			return msg;
		}
		break;
	}
	Tt_pattern depositPat = 0;
	if (final) {
		depositPat =
			(Tt_pattern)tt_message_user( msg, _TttkDepositPatKey );
	}
	Ttmedia_load_msg_cb _cb = (Ttmedia_load_msg_cb)clientCB;
	if (_cb != 0) {
		_TttkItem2Free fuse = msg;
		unsigned char     *contents = 0;
		int                len      = 0;
		Tt_status	   status;
		status = tt_message_arg_bval( msg, 0, &contents, &len );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
		char *file = tt_message_file( msg );
		if (tt_ptr_error( file ) != TT_OK) {
			file = 0;
		}
		fuse = (caddr_t)0;
		msg = (*_cb)( msg, clientData, op, contents, len, file );
	}
	if (final) {
		//
		// The reply has now come in, so there can be no
		// more Deposit requests on this buffer.
		//
		tt_pattern_destroy( depositPat );
	}
	return msg;
}

static Tt_pattern
ttmedia_Deposit_pat(
	Tt_category		category,
	const char	       *media_type,
	Tt_message		commission,
	const char	       *buffer_id,
	Ttmedia_load_msg_cb	clientcb,
	void		       *clientdata,
	int			register_it
)
{
	Tt_pattern pat = _ttDtPatternCreate( category, TT_SESSION,
					     register_it, 0, TTME_DEPOSIT,
					     _ttMediaLoadMsgCB, (void *)clientcb,
					     clientdata, 0 );
	Tt_status status = tt_ptr_error( pat );
	if (status != TT_OK) {
		return pat;
	}
	_TttkItem2Free fuse = pat;
	status = tt_pattern_arg_add( pat, TT_IN, media_type, 0 );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	if (buffer_id != 0) {
		status = tt_pattern_arg_add( pat, TT_IN, "bufferID",
					     buffer_id );
		if (status != TT_OK) {
			return (Tt_pattern)tt_error_pointer( status );
		}
	}
	fuse = (caddr_t)0;
	return _ttDesktopPatternFinish( pat, commission, register_it );
}

Tt_message
ttmedia_load(
	Tt_message		context,
	Ttmedia_load_msg_cb	cb,
	void		       *clientData,
	Tttk_op			op,
	const char	       *mediaType,
	const unsigned char    *contents,
	int             	len,
	const char	       *file,
	const char	       *docname,
	int			send
)
{
	Tt_message msg = _ttDtPMessageCreate( context, TT_REQUEST, TT_SESSION,
				0, op, _ttMediaLoadMsgCB, (void *)cb, clientData );
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return msg;
	}
	_TttkItem2Free fuse = msg;
	Tt_mode mode;
	switch (op) {
	    case TTME_DISPLAY:
	    case TTME_MAIL:
	    case TTME_INSTANTIATE:
		mode = TT_IN;
		break;
	    case TTME_COMPOSE:
	    case TTME_MAIL_COMPOSE:
		mode = TT_OUT;
		break;
	    default:
		mode = TT_INOUT;
		break;
	}
	status = tt_message_barg_add( msg, mode, mediaType, contents, len );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	if (file != 0) {
		status = tt_message_file_set( msg, file );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	if (docname != 0) {
		status = tt_message_arg_add( msg, TT_IN, Tttk_title, docname );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	Tt_pattern pat = ttmedia_Deposit_pat( TT_HANDLE, mediaType,
					msg, 0, cb, clientData, 1 );
	status = tt_ptr_error( pat );
	if (status == TT_OK) {
		// Save the pattern so we can destroy it when
		// the message reaches a final state.
		tt_message_user_set( msg, _TttkDepositPatKey, pat );
	}
	if (send) {
		status = tt_message_send( msg );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	fuse = (caddr_t)0;
	return msg;
}

Tt_status
ttmedia_load_reply(
	Tt_message		contract,
	const unsigned char    *newContents,
	int             	newLen,
	int			reply_and_destroy
)
{
	Tt_status status = TT_OK;
	status = tt_message_arg_bval_set( contract, 0, newContents, newLen );
	if (status != TT_OK) {
		return status;
	}
	if (reply_and_destroy) {
		status = tt_message_reply( contract );
		if (status != TT_OK) {
			return status;
		}
		tttk_message_destroy( contract );
	}
	return status;
}

Tt_status
ttmedia_Deposit(
	Tt_message		contract,
	const char	       *buffer_id,
	const char	       *media_type,
	const unsigned char    *new_contents,
	int             	new_len,
	const char 	       *file,
	XtAppContext		app2run,
	int			ms_timeout
)
{
	Tt_status status;
	char *handler = 0;
	if (buffer_id == 0) {
		handler = tt_message_sender( contract );
		status = tt_ptr_error( handler );
		if (status != TT_OK) {
			return status;
		}
	}
	int replyStatus = 1;
	Tt_message msg = _ttDtPMessageCreate( contract, TT_REQUEST, TT_SESSION,
				handler, TTME_DEPOSIT, _ttTkNoteReplyStatus,
				0, &replyStatus );
	tt_free( handler );
	status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return status;
	}
	_TttkItem2Free fuse = msg;
	const char *_media_type = media_type;
	if (media_type == 0) {
		_media_type = tt_message_arg_type( contract, 0 );
	}
	status = tt_message_barg_add( msg, TT_IN, _media_type, new_contents,
				      new_len );
	if (media_type == 0) {
		tt_free( (caddr_t)_media_type );
	}
	if (status != TT_OK) {
		return status;
	}
	if (file != 0) {
		status = tt_message_file_set( msg, file );
		if (status != TT_OK) {
			return status;
		}
	}
	msg = _ttDesktopMessageFinish( msg, contract, 1 );
	status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return status;
	}
	fuse = (caddr_t)0;
	status = tttk_block_while( app2run, &replyStatus, ms_timeout );
	if (status == TT_DESKTOP_ETIMEDOUT) {
		// See comment in ttdt_Get_Modified()
		tttk_message_destroy( msg );
		return status;
	}
	return (Tt_status)-replyStatus;
}
