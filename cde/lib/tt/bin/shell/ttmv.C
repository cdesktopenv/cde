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
//%%  $XConsortium: ttmv.C /main/3 1995/10/20 16:37:18 rswiston $ 			 				
/* @(#)ttmv.C	1.13 93/07/30
 * ttmv.cc - Link Service/ToolTalk wrapper for mv(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include <stdlib.h>
#include <locale.h>
#include <util/copyright.h>
#include "mover.h"

TT_INSERT_COPYRIGHT

#ifdef OPT_PATCH
static char PatchID[] = "Patch Id: 100626_03.";
static int Patch_ID100626_03;
#endif

/*
 * main()
 */
int
main(int argc, char **argv)
{
	Tt_status	tterr = TT_OK;
	mover	       *ttmv = new mover( argv[0] );

	setlocale( LC_ALL , "" );
	ttmv->parse_args( argc, argv );
	tterr = ttmv->open_tt();

	if (ttmv->tt_opened()) {
		tterr = ttmv->do_ttmv();
	}
	if (    ttmv->should_mv()
	        /*
		 * I take the -f option pretty seriously, so I'll move
		 * the actual files even though the ttmv failed.
		 */
	     && ((tterr <= TT_WRN_LAST) || ttmv->force()))
	{
		int err = ttmv->do_mv();
		if (err != 0) {
			(void)ttmv->close_tt();
			exit( err );
		}
	}

	(void)ttmv->close_tt();

	if ((tterr > TT_WRN_LAST) && (! ttmv->force())) {
		exit(1);
	}

	return(0);
}
