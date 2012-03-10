// $XConsortium: UAS_Path.hh /main/3 1996/06/11 16:38:45 cde-hal $
# ifndef _UAS_Path_hh_
# define _UAS_Path_hh_

# include "UAS_Common.hh"

class UAS_Path: public UAS_Common {
    //
    //  Constructors/destructor
    //
    public:

    //
    //  Methods
    //
    public:
	virtual UAS_Pointer<UAS_Common> first ();
	virtual UAS_Pointer<UAS_Common> last ();
	virtual unsigned int length ();
};

#endif
