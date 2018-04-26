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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: options_tt.C /main/3 1995/10/20 16:25:48 rswiston $ 			 				
/*
 *
 * options_tt.cc
 *
 * ttdbck option handling routines
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dbck.h"     
#include "options_tt.h"
#include "util/tt_gettext.h"

Dbck_specoptions::
Dbck_specoptions()
{
	_repair_filename_p  = 0;
	_repair_filename = (char *)0;
}


/*
 * Return the getopt valid-options string
 */
char * Dbck_specoptions::
optstring()
{
	return "vhf:k:t:bximpaIF:T:Zd:";
}

int Dbck_specoptions::
set_option(int optchar, const char *optval)
{
	switch (optchar) {
	      case 'a':
		_disp_id_p = _disp_mand_p = _disp_prop_p = 1;
		break;
	      case 'F':
		_repair_filename_p = 1;
		_repair_filename = optval;
		break;
	      default:
		return set_common_option(optchar, optval);
	}
	return 1;
}


void Dbck_specoptions::
print(FILE *f) const
{
	Dbck_options::print(f);
	if (_repair_filename_p) {
		fprintf(f,catgets(_ttcatd, 6, 19,
				  "Repair by setting to file: %s\n"),
			(char *)_repair_filename);
	}	
	fprintf(f,">\n");
}
