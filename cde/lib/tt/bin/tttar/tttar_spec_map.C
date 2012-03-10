//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tttar_spec_map.C /main/3 1995/10/20 17:01:06 rswiston $ 			 				
/*
 * tttar_spec_map.cc - Implements mappings of old specs to new specs.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include "tttar_spec_map.h"

implement_list_of(Lstar_spec_map)
implement_table_of(Lstar_spec_map,old_id,_Tt_string)

/*
 * Lstar_spec_map::Lstar_spec_map
 */
Lstar_spec_map::
Lstar_spec_map()
{
}

/*
 * Lstar_spec_map::~Lstar_spec_map()
 */
Lstar_spec_map::
~Lstar_spec_map()
{
}

/*
 * Lstar_spec_map::print()
 */
void Lstar_spec_map::
print( FILE *fs ) const
{
	this->_old_id->print(fs);
	fprintf( fs, " (" );
	this->_path->print(fs);
	fprintf( fs, " ) -> " );
	this->_new_id->print(fs);
}
