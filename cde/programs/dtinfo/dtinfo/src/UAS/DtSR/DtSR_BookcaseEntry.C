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
// $XConsortium: DtSR_BookcaseEntry.C /main/9 1996/10/04 12:43:28 cde-hal $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>
#include <string.h>

#include <iostream>
using namespace std;

#include "config.h"
#include "Basic/Error.hh"
#include "Basic/FolioObject.hh"
#include "Basic/List.hh"
#include "Basic/Long_Lived.hh"
#include "Managers/EnvMgr.hh"

#include "UAS_Base.hh"
#include "UAS_List.hh"

#include "DtSR_SearchEngine.hh"
#include "DtSR_BookcaseEntry.hh"
#include "DtSR_Stems.hh"

UAS_Pointer<UAS_List<DtSR_BookcaseSearchEntry> >
	DtSR_BookcaseSearchEntry::f_bcases = NULL;

DtSR_BookcaseSearchEntry::DtSR_BookcaseSearchEntry(
				int dbn, UAS_Pointer<UAS_Common>& bookcase,
				int searchable)
			: DtSR_BookcaseEntry(bookcase, searchable),
			  f_dbname(NULL), f_dbn(dbn), f_keytypes(NULL),
			  f_ktcount(0), f_stems(NULL), uas_bcase(bookcase)
{
    f_dbname = DtSR_SearchEngine::search_engine().db_name(f_dbn);

    if (f_dbname == (const int)0) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) could not instantiate DtSR_BookcaseScopeEntry\n");
	abort();
#endif
    }

    int status;
    if ((status = DtSearchGetKeytypes((char*)*f_dbname,
				     &f_ktcount,
				     &f_keytypes))) {
#ifdef DEBUG
	fprintf(stderr, "(ERROR) DtSearchGetKeytypes failed\n");
	abort();
#endif
    }

#ifdef DEBUG
    int i;
    printf("(DEBUG) ktcount = %d\n", f_ktcount);
    for (i = 0; i < f_ktcount; i++) {
	printf("    kt[%d]=%c,%s\n", i, f_keytypes[i].ktchar, f_keytypes[i].name);
    }
#endif

    bcases().insert_item(UAS_Pointer<DtSR_BookcaseSearchEntry>(this));

    f_stems = new DtSR_Stems(f_dbn);

    // determine the language of this bookcase
    // NOTE: dtsearch should provide language information
    if (! strcmp(env().lang(), "ja_JP.EUC-JP"))
	f_language = DtSrLaJPN;
    else
	f_language = DtSrLaENG;
}

void
DtSR_BookcaseSearchEntry::unreference()
{
    static int nest = 0;
    if (nest++ == 0)
	UAS_Base::unreference();
    nest--;
}

DtSR_BookcaseSearchEntry::~DtSR_BookcaseSearchEntry()
{
#if 0
    static int nest = 0;
    if (nest++ == 0)
	bcases().remove_item(this);
    nest--;
#endif
    bcases().remove_item(this);
}

void
DtSR_BookcaseSearchEntry::search_zones(UAS_SearchZones& search_zones)
{
    if (! bcases().length() > 0) {
#ifdef DEBUG
	fprintf(stderr, "search_zones tried on empty BookcaseEntry list\n");
#endif
	return;
    }

    unsigned int i;
    for (i = 0; i < bcases().length(); i++) {
	DtSR_BookcaseSearchEntry* bce = bcases()[i];
	bce->_search_zones(search_zones);
    }
}

void
DtSR_BookcaseSearchEntry::_search_zones(UAS_SearchZones& search_zones)
{
    int i;
#ifdef DO_SUPPORT_EVERYTHING
    if (search_zones.all()) {
	for (i = 0; i < f_ktcount; i++)
	    f_keytypes[i].is_selected = (f_keytypes[i].ktchar == 'A') ? 1 : 0;
    }
    else {
#endif
	for (i = 0; i < f_ktcount; i++) {
	    bool selected = 0;
	    switch (f_keytypes[i].ktchar) {
		case 'D':	// default
		    selected = search_zones.bodies();
		    break;
		case 'H':	// Head
		    selected = search_zones.titles();
		    break;
		case 'G':	// Graphic
		    selected = search_zones.graphics();
		    break;
		case 'E':	// Example
		    selected = search_zones.examples();
		    break;
		case 'I':	// Index
		    selected = search_zones.indexes();
		    break;
		case 'T':	// Table
		    selected = search_zones.tables();
		    break;
		default:
#ifdef DEBUG
		    fprintf(stderr, "(ERROR) unknown keytype found\n");
		    abort();
#endif
		    break;
	    }

	    f_keytypes[i].is_selected = selected ? 1 : 0;
	}
#ifdef DO_SUPPORT_EVERYTHING
    }
#endif

#ifdef KEYTYPES_DEBUG
    cerr << "(DEBUG) " << (char*)*f_dbname << ' ';
    for (i = 0; i < f_ktcount; i++)
	cerr << f_keytypes[i].ktchar << "="
	     << (int)f_keytypes[i].is_selected << ' ';
    cerr << endl;
#endif
}

UAS_List<DtSR_BookcaseSearchEntry>&
DtSR_BookcaseSearchEntry::bcases()
{
    if (f_bcases == (const int)0)
	f_bcases = new UAS_List<DtSR_BookcaseSearchEntry>();
    return *(UAS_List<DtSR_BookcaseSearchEntry>*)f_bcases;
}

UAS_Pointer<DtSR_Stems>
DtSR_BookcaseSearchEntry::takeover_stems()
{
    UAS_Pointer<DtSR_Stems> stems = f_stems;
    f_stems = new DtSR_Stems(f_dbn);

    return stems;
}
