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
