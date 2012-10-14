/*
 * $XConsortium: WorkspaceMgr.hh /main/1 1996/08/13 14:16:48 cde-hal $
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

#include <Xm/XmAll.h>
#include <Dt/Wsm.h>


class WorkspaceMgr : public Long_Lived
{
 public: // functions

    WorkspaceMgr() ;
    ~WorkspaceMgr() ;

    Status	current( Atom *wscur_ap ) ; // return current workspace
    Status	current( Widget w,	// add w to ws wst
                         Atom   wst_a ) ;

    Status	get( Window w,		// get window's workspaces
                     Atom **wslp,
                     unsigned long *wsnum ) ;
    void	put( Window win,	// set window's workspaces
                     Atom *wslp,
                     int wsnum ) ;

    Atom	*target() 		// return target workspace
                    { return &f_target ; }
    void	target( Atom wst_a ) ; 	// set target workspace
    // void	target( Window w ) ; 	// add window to target workspace

    static void	PrimaryWorkspace_cb( Widget w,
                                   Atom aWorkspace,
                                   XtPointer client_data ) ;

 private:

    LONG_LIVED_HH( WorkspaceMgr, workspace ) ;

 private: // data

    Atom	f_actual ;		// workspace per change callback
    Atom	f_target ;		// workspace to use for delayed
					//  servicing of request (dynamic)
} ;

LONG_LIVED_HH2( WorkspaceMgr, workspace ) ;


