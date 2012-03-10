// $XConsortium: UAS_StyleSheet.hh /main/3 1996/06/11 16:40:49 cde-hal $
#ifndef _UAS_StyleSheet_hh_
#define _UAS_StyleSheet_hh_

# include "UAS_Common.hh"

typedef enum {
    SS_UNKNOWN,
    SS_ONLINE,
    SS_HARDCOPY
} StyleSheetType;

class UAS_StyleSheet: public UAS_Common {
    public:
	UAS_StyleSheet (StyleSheetType sstype = SS_ONLINE) {
	    fSSType = sstype;
	}
    public:
	virtual StyleSheetType style_sheet_type () { return fSSType; }
	UAS_ObjectType type() { return UAS_STYLE_SHEET; }

    protected:
	StyleSheetType fSSType;
};

#endif
