//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: prop.C /main/3 1995/10/20 16:26:03 rswiston $ 			 				
/*
 *
 * prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#if defined(ultrix)
#include <sys/types.h>
#endif
#include <sys/stat.h>     
#include "prop.h"
#include "ttdbck.h"

// A Prop just holds the name and its list of values.
// We probably have 600 classes that do this by now.

Prop::
Prop(_Tt_string propname)
{
	_name = propname;
	_values = new _Tt_string_list();
}

void Prop::
print(FILE *f) const
{
	fprintf(f,"Prop<");
	_name->print(f);
	fprintf(f,"=<");
	_values->print(_tt_string_print,f);
	fprintf(f,">>\n");
}

_Tt_string
_tt_prop_name(_Tt_object_ptr &o)
{
	return(((Prop *)o.c_pointer())->name());
}
