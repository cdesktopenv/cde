/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: DtSR_SearchZones.C /main/6 1996/09/14 09:18:57 barstow $

#include <string.h>

#if defined(_AIX)
#include <strings.h>		// strcasecmp()
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
