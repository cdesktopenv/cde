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
// $XConsortium: CatMgr.hh /main/5 1996/06/11 18:33:31 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __CATMGR_HH__
#define __CATMGR_HH__

#include <nl_types.h>

#include "Registration.hh"

#define CATALOG_PREFIX	"dtinfo"

class CatMgr
{
  public:

    CatMgr();
    ~CatMgr();
    static CatMgr& msg_catalog_mgr() { return *f_msg_catalog_mgr; }

    char* catgets(int set, int msg, const char* def = (char*)0);

    static int is_open(nl_catd catd)
			{ return ((catd != (nl_catd)-1) ? 1 : 0); }

    nl_catd catd() { return f_catd; }

  protected:
    
  private:
    static CatMgr* f_msg_catalog_mgr;
    nl_catd f_catd;

    char* f_msg;
    char* f_default;
};

inline CatMgr& msg_catalog_mgr()
{
    return CatMgr::msg_catalog_mgr();
};

#define CATGETS		msg_catalog_mgr().catgets

#endif
