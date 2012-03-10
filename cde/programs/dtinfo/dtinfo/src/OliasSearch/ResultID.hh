// $XConsortium: ResultID.hh /main/3 1996/06/11 16:29:46 cde-hal $
#ifndef _ResultID_hh_
#define _ResultID_hh_

#include "UAS.hh"

class ResultID: public FolioObject {
    public:
	ResultID (UAS_Pointer<UAS_SearchResults> res);
	~ResultID ();

	UAS_String sq();
	int ndocs ();
	const char * display_as();
	UAS_Pointer<UAS_SearchResults> results();

    private:
	UAS_Pointer<UAS_SearchResults> fResults;
};

#endif
