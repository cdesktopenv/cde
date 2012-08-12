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
/*%%  $XConsortium: ttdnd.c /main/3 1995/10/23 09:57:30 rswiston $ 			 				 */
/*
 *
 * ttdnd.c
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api/c/tt_c.h"
#include "ttdnd.h"

static Tt_callback_action	_ttdnd_get_contents_callback();
static void 			_ttdnd_get_contents();
static Tt_callback_action	_ttdnd_continue_transfer();
void				_ttdnd_receive();
int				_ttdnd_convert_selection();


#define MAXCHUNK 32768 /* Need some way for user to set this? */	
#define USER_FUNC_KEY 1

/*
 * ttdnd_init does all ToolTalk specific initialization: registers
 * patterns for all valid types.
 * Returns a 
 *
 * TARGETS:  _SUN_TOOLTALK + each data type
 * (we're not using Neil's proposed _SUN_ALTERNATE_TYPES, but it would
 *  be trivial to do so.)
 * +one item for each data type.
 */

int
ttdnd_init(
     int Xid,
     const char **validtypes,
     Ttdnd_send_data_func *sendfunc,
     Ttdnd_input_func **input_func,
     int *fd_ptr)
{
	int	mark = tt_mark();
	const char	*t, **vt;
	int	validtypecount, i;


	if (tt_ptr_error(tt_open())>TT_WRN_LAST) {
		return 0;
	}
	*input_func = _ttdnd_receive;
	*fd_ptr = tt_fd();
	for(vt=validtypes, t=*vt; t; t=*vt++) {
		Tt_pattern p = tt_pattern_create();
		tt_pattern_category_set(p,TT_HANDLE);
		tt_pattern_scope_add(p,TT_SESSION);
		tt_pattern_op_add(p, "oldd_get_contents");
		tt_pattern_state_add(p, TT_SENT);
		tt_pattern_iarg_add(p, TT_IN, "int", Xid); /* 0 */
		tt_pattern_arg_add(p, TT_OUT, t, (char *)0); 	/* 1 */
		tt_pattern_callback_add(p,_ttdnd_get_contents_callback);
		tt_pattern_user_set(p, USER_FUNC_KEY, (void *)sendfunc);
		tt_pattern_register(p);
	}
	/* tt_session_join(tt_X_session()); TO_DO */

	tt_release(mark);
	return 1;
}

/*
 * ttdnd_negotiate_types finds the first type in the target's list which is
 * also in the source's list and returns it.  This could/should be extended
 * to search for registered conversions.
 */
char *
ttdnd_negotiate_types(
     const char **sourcetypes,
     const char **targettypes)
{
	const char **slist, **tlist;
	const char *s, *t;

	tlist = targettypes;
	while (t = *tlist++) {
		slist = sourcetypes;
		while (s = *slist++) {
			if (0==strcmp(s,t)) {
				return (char *)t;
			}
		}
	}
	/*
	 * If no common data type was found, return the target's favorite
	 * data type. ttdnd_start_transfer will then send a oldd_get_contents
	 * message with this type, in the hope that a dynamically-registered
	 * conversion routine will volunteer to handle it.
	 */
	return (char *)targettypes[0];
}

/*
 * ttdnd_start_transfer() is called by the drop target to get the transfer
 * going.
 */
void
ttdnd_start_transfer(
     int sourceXid,
     const char *source_type,
     Ttdnd_receive_data_func *receivefunc)
{
	_ttdnd_get_contents(sourceXid, source_type, receivefunc, 0);
}

/*
 * internal routine to create and send a oldd_get_contents message, 
 * used from ttdnd_start_transfer for the first chunk and
 * _ttdnd_continue_transfer for subsequent chunks.
 */
static void
_ttdnd_get_contents(
     int sourceXid,
     const char *source_type,
     Ttdnd_receive_data_func *receivefunc,
     int chunknumber)
{
	Tt_message m;

	m = tt_message_create();
	tt_message_address_set(m,TT_PROCEDURE);
	tt_message_class_set(m,TT_REQUEST);
	tt_message_scope_set(m,TT_SESSION);
	tt_message_op_set(m,"oldd_get_contents");
	tt_message_iarg_add(m, TT_IN,  "int",       sourceXid);  /* 0 */
	tt_message_arg_add( m, TT_OUT, source_type, (char *)0);	 /* 1 */
	tt_message_iarg_add(m, TT_IN,  "int",       chunknumber);/* 2 */
	tt_message_iarg_add(m, TT_IN,  "int",       MAXCHUNK);   /* 3 */
	tt_message_arg_add( m, TT_OUT, "int",       (char *)0);	 /* 4 */
	tt_message_user_set(m, USER_FUNC_KEY, (void *)receivefunc);
	tt_message_callback_add(m, _ttdnd_continue_transfer);
	tt_message_send(m);
}

/*
 * _ttdnd_continue_transfer is called when a reply to an oldd_get_contents
 * message comes in.  It passes the data to the application's receive-data
 * callback, and sends a message for the next chunk if necessary.
 */
static Tt_callback_action
_ttdnd_continue_transfer(m,p)
     Tt_message m;
     Tt_pattern p;
{
	Ttdnd_receive_data_func *receivefunc;
	int sourceXid;
	char *sourcetype;
	int chunknum;
	int chunksize;
	unsigned char *chunkptr;
	int is_last;
	int receivefunc_quits;
	int mark;

	mark = tt_mark();
	receivefunc = (Ttdnd_receive_data_func *)tt_message_user(m, USER_FUNC_KEY);
	sourcetype = tt_message_arg_type(m,1);
	
	switch(tt_message_state(m)) {
	      case TT_HANDLED:
		tt_message_arg_ival(m,0,&sourceXid);
		tt_message_arg_bval(m,1,&chunkptr,&chunksize);
		tt_message_arg_ival(m,2,&chunknum);
		tt_message_arg_ival(m,4,&is_last);
		receivefunc_quits = (*receivefunc)(chunknum, sourcetype,
						   (void *)chunkptr, chunksize,
						   is_last);
		
		if (receivefunc_quits) {
			/* 
			 * Should there be any notification to the sender if
			 * the receiver quits?
			 */
		} else if (is_last) {
			/* Transfer is complete. */
		} else {
			_ttdnd_get_contents(sourceXid, sourcetype,
					    receivefunc, chunknum+1);
		}
		break;
	      case TT_FAILED:	/* Sender requested breakoff */
		(void)(*receivefunc)(-1,sourcetype,0,0,1);
		break;
	      case TT_REJECTED: /* Couldn't rendezvous with sender */
		(void)(*receivefunc)(-1,sourcetype,0,0,1);
	      case TT_SENT:
		/* tt_message_arg_val(m,0,&sourceXid); TO_DO */
		return TT_CALLBACK_CONTINUE;
	      default:		/* Don't care about starts and queues */
		break;
	}
	tt_release(mark);
	return TT_CALLBACK_PROCESSED;
}

/*
 * _ttdnd_get_contents_callback is called by TT whenever a 
 * oldd_get_contents request message arrives. It gets the data
 * from the user's send routine, and fills it into the message.
 */
static Tt_callback_action
_ttdnd_get_contents_callback(m, p)
     Tt_message m; 
     Tt_pattern p;
{
	int xid;
	int chunknumber;
	int maxchunk;
	char *type;
	char *buffer;
	int chunksize;
	int is_last;
	int sendfunc_quits;
	Ttdnd_send_data_func *sendfunc;
	int mark;

	mark = tt_mark();
	
	type = tt_message_arg_type(m,1);
	tt_message_arg_ival(m,2,&chunknumber);
	tt_message_arg_ival(m,3,&maxchunk);
	
	sendfunc = (Ttdnd_send_data_func *)tt_pattern_user(p, USER_FUNC_KEY);
	
	buffer = (char *)malloc(maxchunk);
	if (buffer==0) {
		tt_message_fail(m);
		tt_release(mark);
		return TT_CALLBACK_PROCESSED;
	}
	
	sendfunc_quits = (*sendfunc)(chunknumber, maxchunk, type,
				     buffer, &chunksize, &is_last);
	if (sendfunc_quits) {
		tt_message_fail(m);
	} else {
		tt_message_arg_bval_set(m,1,(unsigned char *)buffer,chunksize);
		tt_message_arg_ival_set(m,4,is_last);
		tt_message_reply(m);
	}
	free(buffer);
	tt_release(mark);
	return TT_CALLBACK_PROCESSED;
}
				  
	
/*
 * This is a simple notifier fd input function to read
 * tt messages whenever the tt filedescriptor has input.
 */
void
_ttdnd_receive()
{
	Tt_message m;
	m = tt_message_receive();
	if (m) tt_message_destroy(m); /* should not be needed, but to be safe. */
}

