/* $XConsortium: Options.C /main/2 1995/07/17 13:54:17 drk $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "Options.h"
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include <pwd.h>

/**************************************************************
 *
 * Two options are available for dtappgather:
 *
 *	-v	Verbose    - prints the Search Path environment
 *			     variables to standard output.
 *	-r	Retain     - Retain previous contents of the 
 *			     Application Manager
 *
 **************************************************************/

Options::Options
	(
	unsigned int argc,
	char **      argv
	) : flags(0)
{
CString Usage ("Usage: dtappgather [ -v | -r ]");

    if (argc > 1) {
	for (int i = 1; i < argc; i++) {
	    if (strcmp(argv[i],"-v") == 0)
		flags += 1;
	    else if (strcmp(argv[i],"-r") == 0)
		flags += 2;
	    else {
		fprintf(stderr,"%s\n",Usage.data());
		exit (1);
	    }
	}
    }
}

