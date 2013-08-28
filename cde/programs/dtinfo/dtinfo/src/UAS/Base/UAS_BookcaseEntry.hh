// $XConsortium: UAS_BookcaseEntry.hh /main/4 1996/06/11 16:36:39 cde-hal $
#ifndef _UAS_BookcaseEntry_hh_
#define _UAS_BookcaseEntry_hh_

#include "UAS_Base.hh"
#include "UAS_Common.hh"
#include "UAS_ObjList.hh"

// //////////////////////////////////////////////////////////////////////
// Class UAS_BookcaseEntry
// //////////////////////////////////////////////////////////////////////

class UAS_BookcaseEntry : public UAS_Base
{

  public:
    UAS_BookcaseEntry (UAS_Pointer<UAS_Common> &bookcase,
                      int searchable = 0);
    virtual ~UAS_BookcaseEntry ();

    void    set_searchable (int searchable) {f_searchable = searchable ; }
    int     searchable()              const {return f_searchable ; }

    const char  *path()           const { return f_path ; }
    const char  *name()           const { return f_name ; } 

    UAS_ObjList<int> &book_list()       { return (f_book_list); }
    void set_book_list (UAS_ObjList<int> &list);

    int base_num()                const { return f_base_num ; }
    void set_base_num           (int n) { f_base_num = n; }

    // for multi-infolib scope
    int infolib_num()             const { return f_infolib_num; }
    void set_infolib_num        (int n) { f_infolib_num = n; }

    UAS_String lid()                    { return f_infolib_id; }
    UAS_String bid()                    { return f_bookcase_id; }

  protected:
    int               f_searchable;   // specifies if bookcase can be searched
    char             *f_name ;        // bookcase name
    char             *f_path ;        // path to search engine index
    int               f_base_num ;    // don't ask
    int               f_infolib_num;  // index into f_infobase_list list
    UAS_ObjList<int>  f_book_list;

    UAS_String        f_infolib_id;   // infolib id where bookcase resides
    UAS_String        f_bookcase_id;  // bookcase id
    UAS_String        f_bookcase_name;// infolib name where bookcase resides
};

#endif
