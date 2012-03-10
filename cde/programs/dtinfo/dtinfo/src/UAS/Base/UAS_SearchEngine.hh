// $XConsortium: UAS_SearchEngine.hh /main/4 1996/06/11 16:39:27 cde-hal $
#ifndef _UAS_SearchEngine_hh_
#define _UAS_SearchEngine_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_String.hh"
#include "UAS_Common.hh"
#include "UAS_SearchResults.hh"
#include "UAS_SearchScope.hh"
#include "UAS_Msgs.hh"

class UAS_OQLParser;

class UAS_SearchEngine : public UAS_Base {
  public:
    UAS_SearchEngine ();

    virtual ~UAS_SearchEngine ();

    virtual UAS_Pointer<UAS_SearchResults> search (UAS_String,
		   UAS_SearchScope &,
		   // NOTE: Might beef this
		   // up later.
		   unsigned int maxDocs = 2000);

    STATIC_SENDER_HH (UAS_SearchMsg);

    unsigned int avail_caps();

  protected:
    // Each implementation must set f_oql_parser, otherwise avail_caps
    // would fail.
    UAS_OQLParser *f_oql_parser;

};

#endif
