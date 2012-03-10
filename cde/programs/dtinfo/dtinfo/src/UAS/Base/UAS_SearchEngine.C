// $XConsortium: UAS_SearchEngine.cc /main/4 1996/06/11 16:39:22 cde-hal $
# include "UAS_Exceptions.hh"
#include "UAS_SearchEngine.hh"

#include "UAS_OQLParser.hh"

#define CLASS UAS_SearchEngine
STATIC_SENDER_CC (UAS_SearchMsg);

UAS_SearchEngine::UAS_SearchEngine () : f_oql_parser(NULL) {
}

UAS_SearchEngine::~UAS_SearchEngine () {
    if (f_oql_parser != NULL) {
	delete f_oql_parser;
	f_oql_parser = NULL;
    }
}

unsigned int
UAS_SearchEngine::avail_caps()
{
    unsigned int caps;

    if (f_oql_parser) {
	caps = f_oql_parser->determine_caps();
    }
    else // let query editor allow everything
	caps = (unsigned int)-1;

    return caps;
}

UAS_Pointer<UAS_SearchResults>
UAS_SearchEngine::search (UAS_String, UAS_SearchScope &, unsigned int) {
    return (UAS_SearchResults *) 0;
}
