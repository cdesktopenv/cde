// $XConsortium: CatMgr.hh /main/5 1996/06/11 18:33:31 cde-hal $
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#ifndef __CATMGR_HH__
#define __CATMGR_HH__

#include <nl_types.h>

#include "Registration.hh"

#define CATALOG_PREFIX	"dtinfo"

class CatMgr
{
  public:

    CatMgr();
    ~CatMgr();
    static CatMgr& msg_catalog_mgr() { return *f_msg_catalog_mgr; }

    char* catgets(int set, int msg, const char* def = (char*)0);

    static int is_open(nl_catd catd)
			{ return ((catd != (nl_catd)-1) ? 1 : 0); }

    nl_catd catd() { return f_catd; }

  protected:
    
  private:
    static CatMgr* f_msg_catalog_mgr;
    nl_catd f_catd;

    char* f_msg;
    char* f_default;
};

inline CatMgr& msg_catalog_mgr()
{
    return CatMgr::msg_catalog_mgr();
};

#define CATGETS		msg_catalog_mgr().catgets

#endif
