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
