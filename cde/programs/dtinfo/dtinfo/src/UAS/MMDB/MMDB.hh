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
// $XConsortium: MMDB.hh /main/6 1996/09/26 16:48:45 cde-hal $
# ifndef _MMDB_hh_
# define _MMDB_hh_

# include "UAS_String.hh"

# include "oliasdb/olias_consts.h"
# include "oliasdb/mmdb.h"
# include "oliasdb/collectionIterator.h"

class MMDB
{
public:
  MMDB(const UAS_String &infolibPath);
  ~MMDB();
  OLIAS_DB *database();
  info_lib *infolib () { return f_infoLib; }
  info_base *infobase (const char *locator);
  MMDB &mmdb()
    { return (*this); }

  UAS_String infoLibPath ();
  UAS_String infoLibUid ();

private:
  int          f_initialized;
  OLIAS_DB	*f_oliasDB;
  info_lib	*f_infoLib;
  UAS_String	fInfoLibPath;
};

#endif
