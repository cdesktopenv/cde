// $XConsortium: UAS_SearchResultsEntry.cc /main/3 1996/06/11 16:39:43 cde-hal $
# include "UAS_SearchResultsEntry.hh"

UAS_SearchResultsEntry::UAS_SearchResultsEntry (const char *id,
                                                const char *book,
                                                const char *section,
                                                unsigned int relevance)
{
    f_matches = NULL;
    f_document = NULL;
    f_id = id;
    f_book = book;
    f_section = section;
    f_relevance = relevance;
}

UAS_SearchResultsEntry::~UAS_SearchResultsEntry ()
{
}

UAS_Pointer<UAS_Common>
UAS_SearchResultsEntry::document ()
{
    return NULL;
}


