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
// $XConsortium: DtSR_SearchResults.hh /main/4 1996/10/16 14:23:29 cde-hal $
#ifndef _DTSR_SEARCHRESULTS_HH_
#define _DTSR_SEARCHRESULTS_HH_

#include "UAS_SearchResults.hh"
#include "UAS_Collection.hh"
#include "DtSR_SearchResultsEntry.hh"
#include "DtSR_Stems.hh"

template <class T> class UAS_Pointer;
template <class T> class UAS_List;

class DtSR_SearchResultsEntry;

class DtSR_SearchResults : public UAS_SearchResults
{
  public:
    DtSR_SearchResults (UAS_Pointer<UAS_String> query,
			UAS_Pointer<UAS_String> scope_name,
			UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > res,
			int ndocs, UAS_Pointer<DtSR_Stems>,
			UAS_SearchZones zones, int stype);

    ~DtSR_SearchResults() {}

    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > create_results(int, int);

    static void sort(UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >);

    void merge(UAS_Pointer<DtSR_SearchResults> &);

    UAS_Pointer<DtSR_Stems> stems(int dbn);

    UAS_SearchZones &search_zones() { return f_zones; }

    int search_type() { return f_search_type; }

  protected:
    friend class UAS_Pointer<DtSR_SearchResults>;
    virtual void unreference();

  private:

    UAS_List<DtSR_Stems>	f_stems_list;
    UAS_SearchZones		f_zones;

    int				f_search_type;
};

#endif
