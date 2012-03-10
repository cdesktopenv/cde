// $XConsortium: UAS_Collection.cc /main/4 1996/07/10 09:40:43 rcs $
# include "UAS_Exceptions.hh"
# include "UAS_Collection.hh"

# define CLASS UAS_Collection
STATIC_SENDER_CC(UAS_CollectionRetrievedMsg)

UAS_Collection::UAS_Collection (UAS_Pointer<UAS_Common> theRoot):fRoot(theRoot){
}

UAS_Pointer<UAS_Common>
UAS_Collection::root () {
    return fRoot;
}

UAS_RetrievalStatus
UAS_Collection::retrieve (void *client_data) {
    UAS_CollectionRetrievedMsg msg (this);
    send_message (msg, client_data);
    return UAS_RETRIEVED;
}
