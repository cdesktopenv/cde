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
// $XConsortium: UrlMgr.cc /main/3 1996/06/11 16:28:21 cde-hal $
#define C_UrlAgent
#define L_Agents

#define C_UrlMgr
#define L_Managers

#include <Prelude.h>

LONG_LIVED_CC(UrlMgr,url_mgr);


UrlMgr::UrlMgr () {
    fUrlAgent = NULL;
}

UrlMgr::~UrlMgr () {
    delete fUrlAgent;
}

void
UrlMgr::display (NodeWindowAgent *prefWindow) {
    if (fUrlAgent == NULL)
	fUrlAgent = new UrlAgent ();
    fUrlAgent->display (prefWindow);
}
