// $XConsortium: UAS_Collection.hh /main/4 1996/07/10 09:40:51 rcs $
#ifndef _UAS_Collection_hh_
#define _UAS_Collection_hh_

# include "UAS_Common.hh"

class UAS_Collection: public UAS_Common {
    //
    //  Constructors/destructor
    //
    public:
	UAS_Collection (UAS_Pointer<UAS_Common> theRoot);

    //
    //  Methods.
    //
    public:
	UAS_RetrievalStatus retrieve(void *client_data = NULL);
	virtual UAS_Pointer<UAS_Common> root ();
	STATIC_SENDER_HH(UAS_CollectionRetrievedMsg);

    protected:
	UAS_Pointer<UAS_Common> fRoot;
};

#endif
