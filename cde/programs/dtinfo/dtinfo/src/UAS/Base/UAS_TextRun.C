// $XConsortium: UAS_TextRun.cc /main/3 1996/06/11 16:40:55 cde-hal $
#include "UAS_TextRun.hh"

UAS_TextRun::UAS_TextRun (unsigned int off, unsigned int len)
:f_offset(off), f_length(len)
{
}

UAS_TextRun::~UAS_TextRun ()
{
}

UAS_Pointer<UAS_Common>
UAS_TextRun::document()
{
    return NULL;
}

UAS_RangeType
UAS_TextRun::type()
{
    return WithoutTags;
}
