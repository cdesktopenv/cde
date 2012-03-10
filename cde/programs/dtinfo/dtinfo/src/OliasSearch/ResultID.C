// $TOG: ResultID.C /main/7 1997/08/12 12:59:24 samborn $
#define C_ResultID
#define L_OliasSearch
#include <Prelude.h>

ResultID::ResultID (UAS_Pointer<UAS_SearchResults> res): fResults(res) {
}

ResultID::~ResultID () {
}

int
ResultID::ndocs () {
    return fResults->num_docs();
}

UAS_String
ResultID::sq () {
    UAS_String& temp_string =  *(UAS_String*)fResults->query();
    return temp_string;
}

const char *
ResultID::display_as () {
#ifdef __osf__
    UAS_String& temp_string =  *(UAS_String*)fResults->query();
    return (char *) temp_string;
#else
    return (char *)*(UAS_String *) fResults->query();
#endif
}

UAS_Pointer<UAS_SearchResults>
ResultID::results() {
    return fResults;
}
