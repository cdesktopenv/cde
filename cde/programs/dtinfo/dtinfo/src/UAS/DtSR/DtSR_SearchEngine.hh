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
// $XConsortium: DtSR_SearchEngine.hh /main/5 1996/05/29 12:31:30 rcs $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#ifndef _DTSR_SEARCHENGINE_HH_
#define _DTSR_SEARCHENGINE_HH_

#include "UAS_Pointer.hh"
#include "UAS_SearchEngine.hh"

extern "C" {
#include "Search.h"
}

#include "DtSR_Parser.hh"
#include "DtSR_SearchResults.hh"
#include "DtSR_SearchResultsEntry.hh"

class DtSR_SearchEngine : public UAS_SearchEngine
{
  public:
    static DtSR_SearchEngine &search_engine
				(UAS_PtrList<const char> *bcases = NULL);

    UAS_Pointer<UAS_SearchResults> search (UAS_String query,
                                           UAS_SearchScope & scope,
                                           unsigned int maxdocs);

    ~DtSR_SearchEngine();

    UAS_Pointer<UAS_String> db_name(int n);
    char *char_db_name(int n);
    int db_count() { return f_dbcount; }

  protected:
    DtSR_SearchEngine();
    void init(UAS_PtrList<const char> *bcases);

    // NOTE: compress_DtSrResult frees res space
    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
		compress_DtSrResult(DtSrResult*&, long &);

  private:
                                 // enforce as a singleton class
    static DtSR_SearchEngine  *f_search_engine;

    UAS_String f_config_path;

    char	**f_dbnames;
    int		f_dbcount;

    unsigned int f_valid_bc_map;

    static int f_init_count;
};

// The DtSR_SE is responsible for translating the query from OQL
// into the query structure required by the DtSearch engine and does the
// actual search. It then calls the DtSearchResult constructor with the
// cookies required to retrieve results.

#endif
