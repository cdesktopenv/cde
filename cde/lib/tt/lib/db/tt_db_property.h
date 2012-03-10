/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_db_property.h /main/3 1995/10/23 10:04:37 rswiston $ 			 				 */
/*
 * tt_db_property.h - Defines the TT DB server property class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_PROPERTY_H
#define _TT_DB_PROPERTY_H

#include "util/tt_object.h"
#include "util/tt_string.h"

class _Tt_db_property : public _Tt_object {
public:
  _Tt_db_property ();
  ~_Tt_db_property ();
  int is_empty() const;

  _Tt_string          name;
  _Tt_string_list_ptr values;

  void print (FILE *file=stdout)
    {
       fprintf(file, "Property Name: %s\n", ((char *)name ?
					     (char *)name : "(NULL)"));
       
       if (!values.is_null()) {
         fprintf(file, "Number of Values: %d\n", values->count());

	 int i = 0;
	 _Tt_string_list_cursor values_cursor(values);
	 while (values_cursor.next()) {
	   if (*values_cursor != (char*)NULL) 
	        fprintf(file, "Property Value #%d: %s\n", i, (char *)*values_cursor);
           else fprintf(file, "Property Value #%d: NULL\n", i);
	   i++;
	 }
       }
    }
};

#endif // _TT_DB_PROPERTY_H
