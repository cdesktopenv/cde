//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: ttcp.C /main/3 1995/10/20 16:37:11 rswiston $ 			 				
/* @(#)ttcp.C	1.12 93/07/30
 * ttcp.cc - Link Service/ToolTalk wrapper for cp(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include <stdlib.h>
#include <locale.h>
#include <util/copyright.h>
#include <tt_options.h>
#include "copier.h"

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
	copier	       *ttcp = new copier( argv[0] );

	setlocale( LC_ALL, "" );
	ttcp->parse_args( argc, argv );

	if (ttcp->should_cp()) {
		int err = ttcp->do_cp();
		if (err != 0) {
			exit( err );
		}
	}

	tterr = ttcp->open_tt();
	if (ttcp->tt_opened()) {
		tterr = ttcp->do_ttcp();
	}
	(void)ttcp->close_tt();

	if (tterr > TT_WRN_LAST) {
		exit(1);
	} else {
		exit(0);
	}
}
