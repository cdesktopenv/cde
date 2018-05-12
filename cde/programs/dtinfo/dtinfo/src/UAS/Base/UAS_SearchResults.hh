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
// $XConsortium: UAS_SearchResults.hh /main/4 1996/06/11 16:39:38 cde-hal $
#ifndef _UAS_SearchResults_hh_
#define _UAS_SearchResults_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_String.hh"
#include "UAS_List.hh"
#include "UAS_SearchResultsEntry.hh"

class UAS_SearchResults : public UAS_Base {
  public:
    UAS_SearchResults(UAS_Pointer<UAS_String> q,
                      UAS_Pointer<UAS_String> n,
		      UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > res,
                      int ndocs)
	: f_query(q), f_scope_name(n),
	  f_results(*(UAS_List<UAS_SearchResultsEntry>*)res), f_ndocs(ndocs) {}
    UAS_SearchResults(UAS_Pointer<UAS_String> q,
                      UAS_Pointer<UAS_String> n) :
                      f_query(q), f_scope_name(n),
                      f_results(0), f_ndocs(0) {}
    virtual ~UAS_SearchResults () {}

    virtual UAS_Pointer<UAS_String> query () const { return f_query; }
    virtual UAS_Pointer<UAS_String> scope_name () const { return f_scope_name; }
    virtual UAS_List<UAS_SearchResultsEntry> & results ()
	{ return f_results; }
    virtual UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > create_results
        (int, int) { return NULL; }

    int num_docs() const { return f_ndocs; }

  protected:
    UAS_Pointer<UAS_String> f_query;
    UAS_Pointer<UAS_String> f_scope_name;
    UAS_List<UAS_SearchResultsEntry> f_results;
    int f_ndocs;
};

#endif
