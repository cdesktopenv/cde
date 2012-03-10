// $XConsortium: UAS_Msgs.hh /main/6 1996/06/11 16:38:23 cde-hal $
# ifndef _UAS_Msgs_hh_
# define _UAS_Msgs_hh_

template <class T> class UAS_Pointer;
class UAS_Common;
class UAS_Collection;

class UAS_StatusMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	char *fText;
};

class UAS_PartialDataMsg {
};

class UAS_DocumentRetrievedMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	UAS_DocumentRetrievedMsg (UAS_Common *doc): fDoc(doc) { }
};

class UAS_CollectionRetrievedMsg {
    public:
	UAS_Pointer<UAS_Collection> fCol;
	UAS_CollectionRetrievedMsg (UAS_Collection *col): fCol(col) {}
};

class UAS_CancelRetrievalMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	UAS_CancelRetrievalMsg (UAS_Pointer<UAS_Common> doc): fDoc(doc) {
	}
};

class UAS_ErrorMsg {
    public:
	UAS_ErrorMsg(): fErrorMsg(0) {}
	const char *fErrorMsg;
};

class UAS_LibraryDestroyedMsg {
    public:
	UAS_LibraryDestroyedMsg (): fLib (0) {
	}
        UAS_LibraryDestroyedMsg (UAS_Pointer<UAS_Common> lib):
            fLib (lib) {
        }
        UAS_Pointer<UAS_Common> fLib;
};

class UAS_SearchMsg {
    public:
        UAS_SearchMsg(char *m) : fSearchMsg(m), fNumBcases(0),
                                 fMaxNumBcases(0), fContFlag(1) {}
        char          *fSearchMsg;
        int            fNumBcases;
        int            fMaxNumBcases;
        unsigned char  fContFlag;
};

# endif
