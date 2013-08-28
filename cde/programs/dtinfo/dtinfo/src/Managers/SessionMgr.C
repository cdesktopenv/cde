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
/*
 * $XConsortium: SessionMgr.C /main/2 1996/09/06 15:42:18 cde-hal $
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

#include <Xm/Protocols.h>
#include <Dt/Session.h>

#define C_WindowSystem
#define L_Other

#define C_EnvMgr
#define C_SessionMgr
#define L_Managers

#include "Prelude.h"
#include "utility/funcs.h"

LONG_LIVED_CC( SessionMgr, session );


void
SaveSession_cb( Widget w,
                XtPointer client_data,
                XtPointer calldata )
{
   ((SessionMgr*)client_data)->set();
}


SessionMgr::SessionMgr() : sid(NULL),
			   session_path(NULL),
			   prior_session(NULL)
{
    // get session id & the place to save special startup info (future)
    DtSessionSavePath( window_system().toplevel(), &session_path, &sid );

    /* Add callback to detect session manager messages */
    XmAddWMProtocolCallback( window_system().toplevel(),
                             XInternAtom(
                                 XtDisplay(window_system().toplevel()),
                                "WM_SAVE_YOURSELF", False ),
                             (XtCallbackProc)SaveSession_cb, this );
}


SessionMgr::~SessionMgr()
{
    // free allocations made by the DtSessionSavePath call
    XtFree( session_path ) ;
    XtFree( sid ) ;

    XtFree( prior_session ) ;
}


void
SessionMgr::file( char *savefile )
{
    prior_session = XtsNewString( savefile );
}


void
SessionMgr::set()
{
    saveState() ;
    setWmCommand() ;
}


void
SessionMgr::saveState()
{
    FILE *fp;

    // ...check if file exists, and remove it if so...
    // this operation should rewrite everything needed.

    if( session_path )
    {
        if ((fp = fopen( session_path, "w" )) != NULL) {
            fprintf(fp, "%s\n%s\n%s", env().lang(),
                                      (char *)env().infolibsArg(),
                                      (char *)env().sectionsArg() );
            fclose(fp);
        }
    }
}


void
SessionMgr::setWmCommand()
{
    char **wm_command;
    int i, j;

    const char ** main_argv = env().arglist() ;
    int     in_cnt          = env().arglistCount() ;

    // insert a -session arg and its value into the arg list
    wm_command = (char **) XtMalloc((in_cnt+2) * sizeof(char*));
    wm_command[0] = XtsNewString( main_argv[0] );
    wm_command[1] = XtsNewString( "-session" );

    if( sid )               wm_command[2] = XtsNewString( sid );
    else if( session_path ) wm_command[2] = XtsNewString( session_path );
    else                    wm_command[2] = XtsNewString( "dtinfo_session" );

#ifdef DEBUG
    printf( "session id = %s  input arg count = %d\n", wm_command[2], in_cnt );
#endif

    for (i = 1, j = 3; i < in_cnt; i++) {
            wm_command[j] = XtsNewString(main_argv[i]);
            j++;
    }
    // actually register the command line with the window system
    XSetCommand( XtDisplay(window_system().toplevel()),
                 XtWindow(window_system().toplevel()),
                 wm_command, j);

    for (i=0; i < j; i++) XtFree(wm_command[i]);  // free local allocations
    XtFree((char*)wm_command);
}


void
SessionMgr::restore()
{
    char *lastSessionPath = NULL;

    if( !prior_session )  return;

    DtSessionRestorePath( window_system().toplevel(),
                          &lastSessionPath, prior_session ) ;

    if (lastSessionPath != NULL) {
        restoreState( lastSessionPath );
        XtFree(lastSessionPath);
    }
}


void
SessionMgr::restoreState( char * path )
{
// right now there is nothing being restored that is not specified
// on the command line...

//    FILE *fp;

//    if ((fp = fopen( path, "r" )) != NULL) {
//        fscanf(fp, "%s", &buffer);
//        fclose(fp);
//    }

    // ... remove old session file here after reloading ...
    // if( prior_session )  remove it
}

