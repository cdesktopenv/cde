// $XConsortium: DtSR_SearchZones.hh /main/3 1996/06/11 16:41:42 cde-hal $
#ifndef _DTSR_SEARCHZONES_HH_
#define _DTSR_SEARCHZONES_HH_

#include "UAS_SearchZones.hh"

class DtSR_SearchZones : public UAS_SearchZones
{
  public:
    static uas_zones keytype2zone(const char);
    static uas_zones zonename2zone(const char*);
};

#endif
