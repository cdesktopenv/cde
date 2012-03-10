// $XConsortium: DtSR_SearchZones.C /main/6 1996/09/14 09:18:57 barstow $

#include <string.h>

#if defined(_AIX)
#include <strings.h>		// strcasecmp()
#endif

#if defined(USL)
extern "C" {
   extern char *strcasecmp (const char*, const char *);
}
#endif

#include "DtSR_SearchZones.hh"

DtSR_SearchZones::uas_zones
DtSR_SearchZones::keytype2zone(const char key)
{
    switch (key) {
	case 'D' :
	    return uas_bodies;
	case 'H' :
	    return uas_titles;
	case 'G' :
	    return uas_graphics;
	case 'E' :
	    return uas_examples;
	case 'I' :
	    return uas_indexes;
	case 'T' :
	    return uas_tables;
	case 'A' :
	    return uas_all;
	default  :
	    break;
    }
    return uas_inv;
}

DtSR_SearchZones::uas_zones
DtSR_SearchZones::zonename2zone(const char* name)
{
    if (! strcasecmp(name, "TITLE"))
	return UAS_SearchZones::uas_titles;
    if (! strcasecmp(name, "EXAMPLE"))
	return UAS_SearchZones::uas_examples;
    if (! strcasecmp(name, "INDEX"))
	return UAS_SearchZones::uas_indexes;
    if (! strcasecmp(name, "TABLE"))
	return UAS_SearchZones::uas_tables;
    if (! strcasecmp(name, "GRAPHIC"))
	return UAS_SearchZones::uas_graphics;

    return UAS_SearchZones::uas_inv;
}
