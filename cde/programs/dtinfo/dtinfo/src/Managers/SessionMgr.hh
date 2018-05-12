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
 * $XConsortium: SessionMgr.hh /main/1 1996/08/13 14:15:27 cde-hal $
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


class SessionMgr : public Long_Lived
{
 public: // functions

    SessionMgr() ;
    virtual ~SessionMgr() ;

    const char  *id() { return sid; } 	// return session id
    void	 file( char *savefile ) ;  // put prior save file name
    void	 restore() ;	// restore any prior session state
    void	 set() ;	// register the current session state

 private:

    void	saveState() ;
    void	setWmCommand() ;
    void	restoreState( char * path ) ;

    LONG_LIVED_HH( SessionMgr, session ) ;

 private: // data

    char	*sid ;		  // session id registered
    char	*session_path ;   // new session id/file path for state
    char	*prior_session ;  // prior session state file name, if any
} ;

LONG_LIVED_HH2( SessionMgr, session ) ;


// class SessionMgrX : public SessionMgr
// {
//   // future: base functions should be pure virtual in most cases, with
//   // per-platform GUI resolution available per ifdef of window system
//   // in a derived class such as this.
// }


