// $XConsortium: UrlMgr.cc /main/3 1996/06/11 16:28:21 cde-hal $
#define C_UrlAgent
#define L_Agents

#define C_UrlMgr
#define L_Managers

#include <Prelude.h>

LONG_LIVED_CC(UrlMgr,url_mgr);


UrlMgr::UrlMgr () {
    fUrlAgent = NULL;
}

UrlMgr::~UrlMgr () {
    delete fUrlAgent;
}

void
UrlMgr::display (NodeWindowAgent *prefWindow) {
    if (fUrlAgent == NULL)
	fUrlAgent = new UrlAgent ();
    fUrlAgent->display (prefWindow);
}
