// $XConsortium: UAS_Base.cc /main/3 1996/06/11 16:36:23 cde-hal $
# include <stdio.h>
# include "UAS_Base.hh"

UAS_Base::UAS_Base() {
    fReferenceCount = 0;
}

UAS_Base::~UAS_Base() {
}

void
UAS_Base::reference() {
    fReferenceCount ++;
}

void
UAS_Base::unreference() {
    if (--fReferenceCount == 0) {
	delete this;
    }
}

int
UAS_Base::operator == (const UAS_Base &b) {
    return this == &b;
}
