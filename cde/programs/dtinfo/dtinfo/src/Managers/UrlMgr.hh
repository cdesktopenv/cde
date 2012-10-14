// $XConsortium: UrlMgr.hh /main/3 1996/06/11 16:28:27 cde-hal $


class UrlAgent;
class NodeWindowAgent;

class UrlMgr: public Long_Lived {
    public:
	UrlMgr ();
	~UrlMgr ();

    public:
	void			display (NodeWindowAgent *prefWindow = 0);

    private:
	UrlAgent		*fUrlAgent;

	LONG_LIVED_HH(UrlMgr,url_mgr);
};

LONG_LIVED_HH2(UrlMgr,url_mgr);
