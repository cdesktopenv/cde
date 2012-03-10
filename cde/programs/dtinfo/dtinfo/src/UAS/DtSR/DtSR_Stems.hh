// $XConsortium: DtSR_Stems.hh /main/3 1996/06/11 16:41:52 cde-hal $
#ifndef _DTSR_STEMS_HH_
#define _DTSR_STEMS_HH_

extern "C" {
#include "Search.h"
}

#include "UAS_Base.hh"

typedef char Stems[DtSrMAX_STEMCOUNT][DtSrMAXWIDTH_HWORD];

class DtSR_Stems : public UAS_Base
{
  public:
    DtSR_Stems(int dbn);

    Stems& stems() { return f_stems; }
    int& count() { return f_stemcount; }

    int clear();

    int dbn() { return f_dbn; }

  private:
    Stems f_stems;    
    int   f_stemcount;

    int   f_dbn;

};

#endif
