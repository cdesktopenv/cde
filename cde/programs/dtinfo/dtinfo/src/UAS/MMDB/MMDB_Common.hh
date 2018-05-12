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
// $XConsortium: MMDB_Common.hh /main/3 1996/06/11 16:43:33 cde-hal $
# ifndef _MMDB_Common_hh_
# define _MMDB_Common_hh_

# include "MMDB.hh"

template <class T> class UAS_Pointer;
template <class T> class UAS_List;
#if 0
class UAS_BookTab;
#endif

class MMDB_Common {
    public:
	MMDB_Common (MMDB &mmdb, info_base *ib = 0):
	    fMMDB (mmdb),
	    fInfobase (ib) {
	}

	MMDB &mmdb () { return fMMDB; }
	info_base *infobase () { return fInfobase; }
# if 0
	UAS_Pointer<UAS_List<UAS_BookTab> > tabList (MMDB &,
						   info_base *,
						   const oid_t &);
#endif

    private:
	MMDB &fMMDB;
	info_base *fInfobase;
};

#endif
