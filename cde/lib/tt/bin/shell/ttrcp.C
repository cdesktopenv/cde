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
