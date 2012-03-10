// $XConsortium: MMDB_Common.hh /main/3 1996/06/11 16:43:33 cde-hal $
# ifndef _MMDB_Common_hh_
# define _MMDB_Common_hh_

# include "MMDB.hh"

template <class T> class UAS_Pointer;
template <class T> class UAS_List;
#if 0
class UAS_BookTab;
#endif

class MMDB_Common {
    public:
	MMDB_Common (MMDB &mmdb, info_base *ib = 0):
	    fMMDB (mmdb),
	    fInfobase (ib) {
	}

	MMDB &mmdb () { return fMMDB; }
	info_base *infobase () { return fInfobase; }
# if 0
	UAS_Pointer<UAS_List<UAS_BookTab> > tabList (MMDB &,
						   info_base *,
						   const oid_t &);
#endif

    private:
	MMDB &fMMDB;
	info_base *fInfobase;
};

#endif
