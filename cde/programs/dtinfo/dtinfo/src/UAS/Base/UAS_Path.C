// $XConsortium: UAS_Path.cc /main/3 1996/06/11 16:38:39 cde-hal $
# include "UAS_Path.hh"

UAS_Pointer<UAS_Common>
UAS_Path::first () {
    return (UAS_Common *) 0;
}

UAS_Pointer<UAS_Common>
UAS_Path::last () {
    return (UAS_Common *) 0;
}

unsigned int
UAS_Path::length () {
    return 0;
}
