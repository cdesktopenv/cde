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
// $XConsortium: UrlMgr.hh /main/3 1996/06/11 16:28:27 cde-hal $


class UrlAgent;
class NodeWindowAgent;

class UrlMgr: public Long_Lived {
    public:
	UrlMgr ();
	~UrlMgr ();

    public:
	void			display (NodeWindowAgent *prefWindow = 0);

    private:
	UrlAgent		*fUrlAgent;

	LONG_LIVED_HH(UrlMgr,url_mgr);
};

LONG_LIVED_HH2(UrlMgr,url_mgr);
