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
/*
 * $XConsortium: WorkspaceMgr.cc /main/1 1996/08/13 14:15:55 cde-hal $
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

#define C_WindowSystem
#define L_Other

#define C_WorkspaceMgr
#define L_Managers

#include "Prelude.h"

LONG_LIVED_CC( WorkspaceMgr, workspace );

#include <Xm/XmAll.h>
#include <Dt/Wsm.h>


WorkspaceMgr::WorkspaceMgr()
{
    Atom  wsc ;

    current( &wsc ) ;
    f_target = wsc ;

    DtWsmAddCurrentWorkspaceCallback( window_system().toplevel(),
                  (DtWsmWsChangeProc) PrimaryWorkspace_cb, this );
                  
    f_actual = 0;
}


WorkspaceMgr::~WorkspaceMgr()
{
    // XtFree( f_target ) ;
}


Status
WorkspaceMgr::current( Atom *wscur_ap )
{
    Display *display = XtDisplay(window_system().toplevel()) ;
    return DtWsmGetCurrentWorkspace( display,
                                     XDefaultRootWindow(display),
                                     wscur_ap ) ;
}


Status
WorkspaceMgr::current( Widget w,
                       Atom   wst_a )
{
    return DtWsmSetCurrentWorkspace( w, wst_a ) ;
}


Status
WorkspaceMgr::get( Window w,
                   Atom **wslp,
                   unsigned long *wsnum )
{
    Display *display = XtDisplay(window_system().toplevel()) ;
    return DtWsmGetWorkspacesOccupied( display, w, wslp, wsnum );
}


void
WorkspaceMgr::put( Window w,
                   Atom *wslp,
                   int  wsnum )
{
    Display *display = XtDisplay(window_system().toplevel()) ;
    DtWsmSetWorkspacesOccupied( display, w, wslp, wsnum );
}


void
WorkspaceMgr::target( Atom wst_a )
{
    f_target = wst_a ;
}


// void
// WorkspaceMgr::target( Window w )
// {
// }


void
WorkspaceMgr::PrimaryWorkspace_cb( Widget w,
                                   Atom aWorkspace,
                                   XtPointer client_data )
{
    ((WorkspaceMgr*)client_data)->f_actual = aWorkspace ;
}


