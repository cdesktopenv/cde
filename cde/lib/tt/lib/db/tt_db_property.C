//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_property.C /main/3 1995/10/23 10:04:30 rswiston $ 			 				
/* 
 * tt_db_property.cc - Implement the _Tt_db_property class
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */


#include "db/tt_db_property.h"

// Constructors and destructors 

_Tt_db_property::
_Tt_db_property ()
{
	values = new _Tt_string_list;
}

_Tt_db_property::
~_Tt_db_property ()
{
}


// whether a property is empty

int _Tt_db_property::
is_empty() const
// consider a property to be empty if it's values list is empty, or if all
// entries in the values list are NULL content member tt_strings.
{
 if (values->is_empty()) return TRUE;  // empty values list case

 // check the list for all NULL content member tt_strings
 _Tt_string_list_cursor values_cursor(values);
 while (values_cursor.next())  if (*values_cursor != (char*)NULL) return FALSE;

 return TRUE;
} 

   
