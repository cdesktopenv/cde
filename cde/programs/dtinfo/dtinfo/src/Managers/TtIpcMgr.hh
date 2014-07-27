/*
 * $XConsortium: TtIpcMgr.hh /main/5 1996/08/13 15:49:25 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include <Tt/tt_c.h>
#include <Tt/tttk.h>
#include <X11/Intrinsic.h>

// status flags for local use
#define ID_SUCCESS	1
#define BAD_ARG_FORMAT	2
#define LOAD_ERROR	3
#define MSG_SYNTAX_ERROR 4
#define NOT_FOUND	5
#define NOT_OWNER	6
#define UNKNOWN_MSG	7


class TtIpcMgr : public Long_Lived
{
 public: // functions

    TtIpcMgr() ;
   ~TtIpcMgr() ;

    void	establish_server() ;	// any checkins with peers

    static int	do_infolib( char *locator ) ;	// infolib requested
    static int	do_locator( char *locator, 	// specific section requested
                            Boolean new_window ) ;

    int	do_print(Tt_message msg); // reads list of sections from temp file

    // static functions (to be called from C libraries)
    
    static void receive_tt_msg(	  // get the message from tt
	XtPointer client_data,
		int *fid,
		XtInputId *id ) ;


 protected: // functions

    int	handle( Tt_message msg ) ;	// determine action

 private:

    LONG_LIVED_HH( TtIpcMgr, tt_manager ) ;

    int check_locale( Tt_message msg_in,
		      int locale_arg_pos ) ;

    void notify_quit() ;		// sign off to dtinfo observers

        // static functions (to be called from C libraries)
    static Boolean ipc_init_wp0( XtPointer this_obj ) ;
    static Boolean ipc_init_wp1( XtPointer this_obj ) ;
    static Boolean ipc_print_wp( XtPointer this_obj ) ;
    static Boolean ipc_error_wp( XtPointer this_obj ) ;
    static Boolean receive_tt_msg_wp( XtPointer this_obj ) ;

 private: // data

    char  	*procid ;		// tt id for process
    Tt_status	 tt_state ;
} ;

LONG_LIVED_HH2( TtIpcMgr, tt_manager ) ;

