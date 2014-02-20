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
// $XConsortium: DtSR_SearchResults.C /main/9 1996/10/16 14:23:04 cde-hal $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include "DtSR_SearchEngine.hh"
#include "DtSR_SearchResultsEntry.hh"
#include "DtSR_SearchResults.hh"

DtSR_SearchResults::DtSR_SearchResults
	(UAS_Pointer<UAS_String> query,
	 UAS_Pointer<UAS_String> scope_name,
	 UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > res,
	 int ndocs,
	 UAS_Pointer<DtSR_Stems> stems,
	 UAS_SearchZones zones, int stype)
	: UAS_SearchResults(query, scope_name, res, ndocs), f_zones(zones),
	  f_search_type(stype)
{
    f_stems_list.insert_item(stems);

    UAS_List<UAS_SearchResultsEntry>& list =
			*(UAS_List<UAS_SearchResultsEntry>*)res;

    for (unsigned int i = 0; i < list.length(); i++) {

	UAS_Pointer<UAS_SearchResultsEntry> crude_ptr = list[i];
	UAS_SearchResultsEntry &cptr2 = *crude_ptr;
	UAS_Pointer<DtSR_SearchResults> uas_this = this;
	((DtSR_SearchResultsEntry *)&cptr2)->search_result(uas_this);

    }
}

void
DtSR_SearchResults::sort(UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > res)
{
    UAS_List<UAS_SearchResultsEntry>& Ref_list =
			*(UAS_List<UAS_SearchResultsEntry>*)res;
  
    unsigned int list_length = Ref_list.length();
    for (unsigned int i = 0; i < list_length; i++) {
	for (unsigned int j = i+1; j < list_length; j++) {

#ifdef USL
	    // this line does nothing, but if you remove it
	    // this code will not compile on novell.  go figure.  rCs

	    UAS_Pointer<UAS_SearchResultsEntry> reflisti = Ref_list[i];
#endif

	    if (Ref_list[i]->relevance() < Ref_list[j]->relevance()) {
		UAS_Pointer<UAS_SearchResultsEntry> temp = Ref_list[i];
		Ref_list[i] = Ref_list[j];
		Ref_list[j] = temp;
	    }
	}
    }
}

UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
DtSR_SearchResults::create_results(int index, int nres)
{
    // garbage input
    if (nres == 0 || index >= (int) f_results.length())
	return NULL;

    int n;
    if ((n = index + nres - f_results.length()) > 0)
	nres -= n;

    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> >
	uas_res = new UAS_List<UAS_SearchResultsEntry>();

    for (int i = index; i < nres; i++)
	uas_res->insert_item(f_results[i]);

    return uas_res;
}

void
DtSR_SearchResults::merge(UAS_Pointer<DtSR_SearchResults> & res)
{
    if (res == (const int)0 || res->f_ndocs == 0) // nothing to merge
	return;

    int i;

    if (f_ndocs == 0) { // none is there, just copy all of them

	for (i = 0; i < res->f_ndocs; i++) {

	    f_results.insert_item(res->f_results[i]);

	    UAS_Pointer<UAS_SearchResultsEntry> crude_ptr = res->f_results[i];
	    UAS_SearchResultsEntry &cptr2 = *crude_ptr;	    
	    UAS_Pointer<DtSR_SearchResults> uas_this = this;

	    ((DtSR_SearchResultsEntry*)&cptr2)->search_result(uas_this);

	}

	f_ndocs = res->f_ndocs;
	for (i = 0; i < (int) res->f_stems_list.length(); i++) {
	    f_stems_list.insert_item(res->f_stems_list[i]);
	}
	res = NULL; 
	return;
    }

    int ndocs = f_ndocs;

    UAS_Pointer<DtSR_SearchResults> uas_this = this;

    // both have some
    int position = 0;
    for (i = 0; i < res->f_ndocs; i++, ndocs++) {

	if (! (position < ndocs))
	    break;

	int j;
	for (j = position; j < ndocs; j++) {
	    if (f_results[j]->relevance() > res->f_results[i]->relevance())
		continue;

	    f_results.insert_item(res->f_results[i], j);
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) insert at %d\n", j);
#endif
	    UAS_SearchResultsEntry* crude_ptr =
			(UAS_SearchResultsEntry*)res->f_results[i];
	    ((DtSR_SearchResultsEntry*)crude_ptr)->search_result(uas_this);
	    break;
	}
	if (j == ndocs) { // insert at the tail
	    f_results.insert_item(res->f_results[i]);
#ifdef DEBUG
	    fprintf(stderr, "(DEBUG) insert at the tail\n");
#endif
	    UAS_SearchResultsEntry* crude_ptr =
			(UAS_SearchResultsEntry*)res->f_results[i];
	    ((DtSR_SearchResultsEntry*)crude_ptr)->search_result(uas_this);
	}
	position = ++j;
    }
    for (;i < res->f_ndocs; i++) {
#ifdef DEBUG
	fprintf(stderr, "(DEBUG) append the remaining list\n");
#endif
	f_results.insert_item(res->f_results[i]);
	UAS_SearchResultsEntry* crude_ptr =
		(UAS_SearchResultsEntry*)res->f_results[i];
	((DtSR_SearchResultsEntry*)crude_ptr)->search_result(uas_this);
	ndocs++;
    }

    // merge stems list
    for (i = 0; i < (int) res->f_stems_list.length(); i++) {
	f_stems_list.insert_item(res->f_stems_list[i]);
    }

    f_ndocs += res->f_ndocs;
    res = NULL;
}

UAS_Pointer<DtSR_Stems>
DtSR_SearchResults::stems(int dbn)
{
    int db_count = DtSR_SearchEngine::search_engine().db_count();

    if (dbn < 0 || dbn >= db_count)
	return NULL;

    unsigned int i;
    for (i = 0; i < f_stems_list.length(); i++) {
	if (f_stems_list[i]->dbn() == dbn)
	    break;
    }
    if (i == f_stems_list.length()) // not found
	return NULL;

    return f_stems_list[i];
}

void
DtSR_SearchResults::unreference()
{
    static int nest = 0;
    if (nest++ == 0)
	UAS_Base::unreference();
    nest--;
}
