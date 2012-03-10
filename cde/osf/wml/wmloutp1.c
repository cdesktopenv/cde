/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: wmloutp1.c /main/7 1995/07/13 21:04:31 drk $"
#endif
#endif
/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
 * This is the standard output module for creating the UIL compiler
 * .h files.
 */


#include "wml.h"


void wmlOutput ()

{

/*
 * Output the .h files
 */
wmlOutputHFiles ();
if ( wml_err_count > 0 ) return;

/*
 * Output the keyword (token) tables
 */
wmlOutputKeyWordFiles ();
if ( wml_err_count > 0 ) return;

/*
 * Output the .mm files
 */
wmlOutputMmFiles ();
if ( wml_err_count > 0 ) return;

return;

}

