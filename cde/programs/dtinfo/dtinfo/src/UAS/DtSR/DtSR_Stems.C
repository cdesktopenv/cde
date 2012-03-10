// $XConsortium: DtSR_Stems.cc /main/3 1996/06/11 16:41:47 cde-hal $

#include <string.h>
#include "DtSR_Stems.hh"

DtSR_Stems::DtSR_Stems(int dbn) : f_dbn(dbn)
{
    clear();
}

int
DtSR_Stems::clear()
{
    size_t size = DtSrMAX_STEMCOUNT * DtSrMAXWIDTH_HWORD;

    void* ret = memset(f_stems, 0x00, size);
    if (ret != (void*)f_stems) // failed
	return 0;
    else {
	f_stemcount = 0;
	return 1;
    }
}
