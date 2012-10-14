// $XConsortium: UAS_SearchScope.hh /main/5 1996/09/14 13:10:24 cde-hal $
#ifndef _UAS_SearchScope_hh_
#define _UAS_SearchScope_hh_

#include "UAS_Base.hh"
#include "UAS_Sender.hh"
#include "UAS_SearchZones.hh"
#include "UAS_BookcaseEntry.hh"
#include "UAS_Common.hh"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//typedef unsigned char bool;

class UAS_SearchScope;

struct ScopeCreated {
  UAS_SearchScope *f_search_scope;
};

struct ScopeDeleted {
  UAS_SearchScope *f_search_scope;
};

struct ScopeRenamed {
  UAS_SearchScope *f_search_scope;
};

class UAS_SearchScope: public UAS_Base {

  public:

    UAS_SearchScope (const char *name,
                     UAS_PtrList<UAS_BookcaseEntry> &bcases,
                     unsigned int component_mask, bool ro = FALSE);
    ~UAS_SearchScope();

    /* -------- data retrieval methods -------- */

    const char *name() const
      { return (f_name); }
    void set_name (const char *name);

    UAS_SearchZones &search_zones()
      { return f_search_zones; }

    UAS_PtrList<UAS_BookcaseEntry> &bookcases()
      { return f_bcases; }

    void bookcases(UAS_PtrList<UAS_BookcaseEntry> &);

    /* -------- data setting methods -------- */
    const char *name (const char *newname);

    void search_zones (UAS_SearchZones &search_zones)
      { f_search_zones = search_zones; }
  
    /* -------- delete flag -------- */

    bool read_only() const
      { return (f_read_only); }
    void set_read_only()
      { f_read_only = TRUE; }

    bool deleted() const
      { return f_deleted; }
    void deleted (bool b)
      { f_deleted = b; }

    void set_infolib(UAS_Pointer<UAS_Common> il)
      { f_infolib = il; }

    UAS_Pointer<UAS_Common> get_infolib()
      { return f_infolib; }

    STATIC_SENDER_HH (ScopeCreated);
    STATIC_SENDER_HH (ScopeDeleted);
    STATIC_SENDER_HH (ScopeRenamed);

#ifdef DEBUG
    /* -------- for debug only -------- */
    void dump();
#endif

  private:
    char                           *f_name;
    bool                            f_deleted;
    bool                            f_read_only;
    UAS_Pointer<UAS_Common>         f_infolib;
    UAS_SearchZones                 f_search_zones;
    UAS_PtrList<UAS_BookcaseEntry>  f_bcases;
};

#endif
