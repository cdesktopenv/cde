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
// $XConsortium: MMDB_Factory.hh /main/6 1996/10/04 12:35:04 cde-hal $
#ifndef _MMDB_Factory_hh_
#define _MMDB_Factory_hh_

#include "UAS_Factory.hh"
#include "UAS_PtrList.hh"
#include "MMDB.hh"

class MMDB_URL {
    public:
	MMDB_URL();

    public:
	void locator (const UAS_String &);
	UAS_String locator();
	void installPair (const UAS_String &key, const UAS_String &value);
    public:
	UAS_String fInfoLib;
	UAS_String fInfoBase;
	//
	//  Valid values for fType are:
	//	section
	//	collection
	//	embedded
	//
	UAS_String fType;
	UAS_String fLocator;
};

class MMDB_Factory: public UAS_Factory {
    friend class MMDB;
    public:
	MMDB_Factory ();
	~MMDB_Factory ();
    protected:
	UAS_List<UAS_String> rootLocators ();
	void destroy_root_object (UAS_Pointer<UAS_Common>);
	MMDB *getMMDB (const UAS_String &infoLibPath);
	MMDB *createMMDB (const UAS_String &infoLibPath);
	MMDB *getMMDBFromObject (UAS_Pointer<UAS_Common>);
	void resolveLocator (const UAS_String &, MMDB *&, info_base *&);
	UAS_Pointer<UAS_Common> create_object(const UAS_String& abs_locator);
	UAS_Pointer<UAS_Common> create_relative_object(
		    const UAS_String& rel_locator,
		    UAS_Pointer<UAS_Common> parent
		);
	UAS_Pointer<UAS_EmbeddedObject> create_embedded_object(
		    const UAS_String& rel_locator
		);
	UAS_Pointer<UAS_EmbeddedObject> create_relative_embedded_object(
		    const UAS_String& rel_locator,
		    UAS_Pointer<UAS_Common> parent
		);
	void initializeFactory(UAS_List<UAS_String>&);
	void finalizeFactory();
	static OLIAS_DB &olias_db();
	static UAS_String genInfolibName ();
    private:
	static OLIAS_DB *fOLIASDB;
	UAS_PtrList<MMDB> fMMDBList;
	UAS_PtrList<MMDB> fDeadMMDBList;
};

#endif
