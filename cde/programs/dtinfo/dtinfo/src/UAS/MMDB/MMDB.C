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
// $TOG: MMDB.C /main/8 1998/04/17 11:42:22 mgreess $
#include <sstream>
using namespace std;
#include "MMDB.hh"
#include "UAS_Exceptions.hh"
#include "UAS_Buffer.hh"
#include "UAS_Common.hh"
#include "UAS_Msgs.hh"
#include "MMDB_Factory.hh"
#include "UAS_Collection.hh"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

MMDB::MMDB(const UAS_String &infolibPath)
: f_initialized (0), fInfoLibPath (infolibPath)
{
  f_oliasDB = &MMDB_Factory::olias_db();
  f_infoLib =  f_oliasDB->openInfoLib((char *) infolibPath, 0,
		MMDB_Factory::genInfolibName());
}

MMDB::~MMDB()
{
    if (f_infoLib) {
	//f_oliasDB->closeInfoLib (f_infoLib->get_info_lib_name());
	f_oliasDB->closeInfoLib (f_infoLib->get_info_lib_uid());
    }
    f_infoLib = 0;
}

UAS_String
MMDB::infoLibPath () {
    return fInfoLibPath;
}

UAS_String
MMDB::infoLibUid()
{
  return UAS_String(f_infoLib->get_info_lib_uid());
}

OLIAS_DB *
MMDB::database()
{
  if (!f_initialized)
    {
      f_initialized = 1;
      info_lib *lib = f_infoLib;
      int bad_count = lib->bad_infobases();
      if (bad_count > 0)
	{
	  UAS_ErrorMsg msg;
	  UAS_Buffer buf(256);
	  const char *x =
		(char*)UAS_String(CATGETS(Set_UAS_MMDB, 1, "The following bookcases are not valid:"));
	  buf.write (x, sizeof(char), strlen(x));
	  for (int i = 1; i <= bad_count; i++)
	    {
	      buf.write ("\n", sizeof(char), 1);
	      x = lib->get_bad_infobase_path(i);
	      buf.write (x, sizeof(char), strlen(x));
	      buf.write ("/", sizeof(char), 1);
	      x = lib->get_bad_infobase_name(i);
	      buf.write (x, sizeof(char), strlen(x));
	    }
	  buf.write ("\0", sizeof(char), 1);
	  msg.fErrorMsg = buf.data();
	  UAS_Common::send_message (msg);
	}
    }
  return (f_oliasDB);
}

info_base *
MMDB::infobase (const char *locator)
{
  info_base *ib = f_infoLib->getInfobaseByComponent((const char *)locator,
		    info_lib::LOC);
  if (ib == NULL)
    throw (CASTEXCEPT Exception());
  else
    return (ib);
}
