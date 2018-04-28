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
//%%  $XConsortium: ttrcp.C /main/3 1995/10/20 16:37:27 rswiston $ 			 				
/*
 * ttrcp.cc - Link Service/ToolTalk wrapper for rcp(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include <stdlib.h>
#include <locale.h>
#include <util/copyright.h>
#include "rcopier.h"

TT_INSERT_COPYRIGHT

/*
 * main()
 */
void
main(int argc, char **argv)
{
	Tt_status	tterr = TT_OK;
	rcopier	       *ttrcp = new rcopier( argv[0] );

	setlocale( LC_ALL, "" );
	ttrcp->parse_args( argc, argv );

	if (ttrcp->should_rcp()) {
		int err = ttrcp->do_rcp();
		if (err != 0) {
			exit( err );
		}
	}

	tterr = ttrcp->open_tt();
	if (ttrcp->tt_opened()) {
		tterr = ttrcp->do_ttrcp();
	}
	(void)ttrcp->close_tt();

	if (tterr > TT_WRN_LAST) {
		exit(1);
	} else {
		exit(0);
	}
}
