// $XConsortium: UAS_TextRun.hh /main/3 1996/06/11 16:41:01 cde-hal $
#ifndef _UAS_TextRun_hh_
#define _UAS_TextRun_hh_

#include "UAS_Base.hh"
#include "UAS_Pointer.hh"
#include "UAS_Common.hh"

enum UAS_RangeType { WithTags, WithoutTags };

class UAS_TextRun : public UAS_Base {
  public:
    UAS_TextRun (unsigned int off, unsigned int len);

    virtual ~UAS_TextRun ();

    UAS_Pointer<UAS_Common> document();

    unsigned int offset () const { return f_offset; }
    unsigned int length () const { return f_length; }

    virtual UAS_RangeType  type();

  private:
    UAS_Pointer<UAS_Common> f_document;
    unsigned int f_offset;
    unsigned int f_length;
};

#endif
