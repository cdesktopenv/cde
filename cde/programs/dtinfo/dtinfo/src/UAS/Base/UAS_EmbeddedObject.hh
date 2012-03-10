// $XConsortium: UAS_EmbeddedObject.hh /main/3 1996/06/11 16:37:15 cde-hal $
# ifndef _UAS_EmbeddedObject_hh_
# define _UAS_EmbeddedObject_hh_

# include "UAS_Common.hh"

class UAS_EmbeddedObject: public UAS_Common {
    //
    //  Constructors/destructor
    //
    public:

    //
    //  Methods
    //
    public:
	virtual unsigned int width () { return 0; }
	virtual unsigned int height () { return 0; }

	virtual unsigned int llx () { return 0; }
	virtual unsigned int lly () { return 0; }
	virtual unsigned int urx () { return 0; }
	virtual unsigned int ury () { return 0; }
};

# endif
